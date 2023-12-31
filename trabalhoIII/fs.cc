#include "fs.h"
#include "math.h"

// Retorna a posição do bloco i no bitmap
int INE5412_FS::bitmap_hash(int i) {
	union fs_block super_block;

	disk->read(0, super_block.data);

	// Índice do bloco no disco - número de blocos de inodo - 1 (superblock) = posição do bloco i no bitmap
	return i - super_block.super.ninodeblocks - 1;
}

// Aloca dados nos blocos indiretos
int INE5412_FS::alloc_indirect_block() {
	union fs_block indirect_block;

	int next_free_block = get_next_free_block();

	// Retorna 0 se não há mais blocos livres
	if (!next_free_block) return 0;
	// Configura o próximo bloco livre como ocupado no bitmap
	change_bitmap(next_free_block);
	// Configura todos os ponteiros do bloco indireto como 0 ("cria" o bloco indireto)
	for (int i = 0; i < POINTERS_PER_BLOCK; i++) indirect_block.pointers[i] = 0;
	disk->write(next_free_block, indirect_block.data);
	return next_free_block;
}

// Retorna o valor total em bytes que o inodo ainda consegue armazenar
int INE5412_FS::get_remaining_storage_size(fs_inode *inode) {
	int free_blocks = 0;
	union fs_block indirect_block;
	// Percorre os ponteiros diretos e indiretos, incrementando free_blocks quando o ponteiro
	// não aponta para um bloco de dados, ou seja, 0.
	for (int i = 0; i < POINTERS_PER_INODE; i++)
		if (!inode->direct[i]) free_blocks++;
	
	disk->read(inode->indirect, indirect_block.data);	
	for (int i = 0; i < POINTERS_PER_BLOCK; i++)
		if (!indirect_block.pointers[i]) free_blocks++;

	return free_blocks * disk->DISK_BLOCK_SIZE;
}

// Escreve "data" nos ponteiros (salva no disco)
std::vector<int> INE5412_FS::write_in_pointers(int *bytes_written, int length, int starting_block, int npointers, int *pointers, const char *data) {
	union fs_block block;

	// Vetor que contém em que blocos foram escritos dados (para onde os ponteiros diretos/indiretos apontam)
	std::vector<int> written_blocks;

	// Zera todo o bloco onde serão escritos os dados
	for (int i = 0; i < disk->DISK_BLOCK_SIZE; i++) block.data[i] = ' ';

	int next_free_block;

	for (int i = starting_block; i < npointers; i++) {
		// Se o ponteiro já estiver sendo usado, pule
		if (pointers[i]) continue;
		next_free_block = get_next_free_block();
		// Se não há mais blocos livres para escrever, retorna os blocos em que escreveu
		if (!next_free_block) return written_blocks;
		
		// Dentro do next_free_block
		for (int j = 0; j < disk->DISK_BLOCK_SIZE; j++) {
			// Escreve o byte de "data" no bloco
			block.data[j] = data[*bytes_written];
			*bytes_written += 1;
			// Se já escreveu tudo
			if (*bytes_written == length) {
				// Passa pro disco
				disk->write(next_free_block, block.data);
				// Configura o bloco em que escreveu como ocupado
				change_bitmap(next_free_block);
				// Adiciona o ponteiro utilizado e o bloco para o qual ele aponta
				// Índice par: Ponteiro
				// Índice ímpar: Bloco
				written_blocks.push_back(i);
				written_blocks.push_back(next_free_block);
				return written_blocks;
			}
		}
		// Passa pro disco
		disk->write(next_free_block, block.data);
		// Configura o bloco em que escreveu como ocupado
		change_bitmap(next_free_block);
		// Adiciona o ponteiro utilizado e o bloco para o qual ele aponta
		// Índice par: Ponteiro
		// Índice ímpar: Bloco
		written_blocks.push_back(i);
		written_blocks.push_back(next_free_block);
	}
	return written_blocks;
}

// Retorna a posição do próximo bloco livre
int INE5412_FS::get_next_free_block() {
	union fs_block super_block;

	disk->read(0, super_block.data);

	// Começa a iteração após os blocos de inodos
	for (int i = super_block.super.ninodeblocks + 1; i < super_block.super.nblocks; i++)
		// Retorna i se o bloco i estiver livre
		if (!bitmap[bitmap_hash(i)]) return i;
	// Se não houverem blocos livres
	return 0;
}

// Lê os blocos que os ponteiros apontam e os salva em data
int INE5412_FS::read_pointers(int length, int* bytes_read, int starting_pointer, int npointers, int *pointers, char *data) {
	union fs_block block;

	for (int i = starting_pointer; i < npointers; i++) {
		// Se o ponteiro não estiver sendo usado, pule
		if (!pointers[i]) continue;
		disk->read(pointers[i], block.data);

		for (int j = 0; j < disk->DISK_BLOCK_SIZE; j++) {
			// Salva o byte do bloco em "data"
			data[*bytes_read] = block.data[j];
			(*bytes_read) += 1;
			if (*bytes_read == length) return 1;
		}
	}
	return 0;
}

// Troca o valor armazenado em bitmap[block]
void INE5412_FS::change_bitmap(int block) {
	union fs_block super_block;

	disk->read(0, super_block.data);

	bitmap[bitmap_hash(block)] = !bitmap[bitmap_hash(block)];
}

// Limpa os ponteiros e os blocos que eles apontam
void INE5412_FS::clear_pointers(int npointers, int pointers[]) {
	union fs_block block;

	for (int i = 0; i < npointers; i++) {
		// Se o ponteiro não estiver sendo usado, pule
		if (!pointers[i]) continue;
		disk->read(pointers[i], block.data);
		// Zera todos os ponteiros do ponteiro indireto
		for (int j = 0; j < disk->DISK_BLOCK_SIZE; j++) block.data[j] = ' ';
		// Sobrescreve as alterações no disco
		disk->write(pointers[i], block.data);
		// Libera o bloco que estava sendo apontado pelo ponteiro
		change_bitmap(pointers[i]);
	}
}

// Lê um inode do disco e retorna se existe
int INE5412_FS::load_inode(fs_inode *inode, int inumber, int ninodeblocks) {
	// Verifica se o inúmero é válido
	if (inumber <= 0 || inumber > INODES_PER_BLOCK*ninodeblocks) return 0;

	// Decrementa o inúmero para pegar o índice
	inumber--;

	// Calcula em que inode block está o inodo a ser carregado
    int n_inode_block = inumber / (INODES_PER_BLOCK);
    // Calcula em que posição dentro do inode block está o inodo a ser carregado
    int inumber_in_inode_block = inumber % INODES_PER_BLOCK;

	union fs_block inode_block;

	disk->read(n_inode_block + 1, inode_block.data);
	*inode = inode_block.inode[inumber_in_inode_block];
	
	// Se não for válido
	if (!inode->isvalid) return 0;

	return 1;
}

// Salva um inode no disco
void INE5412_FS::save_inode(int inumber, fs_inode *inode) {
	// Decrementa o inodo para pegar o índice
	inumber--;
	// Calcula em que inode block está o inodo a ser carregado
    int n_inode_block = inumber / (INODES_PER_BLOCK);
    // Calcula em que posição dentro do inode block está o inodo a ser carregado
    int inumber_in_inode_block = inumber % INODES_PER_BLOCK;

	union fs_block inode_block;

	disk->read(n_inode_block + 1, inode_block.data);
	inode_block.inode[inumber_in_inode_block] = *inode;
	disk->write(n_inode_block + 1, inode_block.data);
}

// "Cria" (sobrescreve) um novo FS no disco e apaga todos os dados presentes
int INE5412_FS::fs_format()
{
	// Define o super bloco
	union fs_block super_block;
	// Cria um bloco
	union fs_block block;

	disk->read(0, super_block.data);
	// Verifica se ja existe um sistema de arquivos
	if (super_block.super.magic == FS_MAGIC) return 0;

	// Configura as informações básicas do novo sistema de arquivos
	super_block.super = {FS_MAGIC, disk->size(), (int)ceil(disk->size()/10), INODES_PER_BLOCK*(int) ceil(disk->size()/10)};
	// Cria um inodo generico invalido
	fs_inode inode = {0,0,{},0};
	for (int i = 0; i < POINTERS_PER_INODE; i++) inode.direct[i] = 0;

	// Reserva 10% dos blocos para inodos
	for (int i = 1; i <= super_block.super.ninodeblocks*INODES_PER_BLOCK; i++) {
		save_inode(i, &inode);
	}
	
	// Define o data block com 4096 ' '
	for (int i = 0; i < disk->DISK_BLOCK_SIZE; i++) block.data[i] = ' ';
	// Copia as informações em "block" (' ') para todos os blocos de dados do disco
	for (int i = super_block.super.ninodeblocks + 1; i < super_block.super.nblocks; i++) {
		disk->write(i, block.data);
	}
	if (bitmap) delete[] bitmap;
	disk->write(0, super_block.data);
	return 1;
}

// Varre o sistema de arquivos montado e reporta como os inodos e blocos estão organizados
void INE5412_FS::fs_debug()
{
	union fs_block super_block;

	// Lê o superblock
	disk->read(0, super_block.data);

	cout << "superblock:\n";
	cout << "    " << (super_block.super.magic == FS_MAGIC ? "magic number is valid\n" : "magic number is invalid!\n");
 	cout << "    " << super_block.super.nblocks << " blocks\n";
	cout << "    " << super_block.super.ninodeblocks << " inode blocks\n";
	cout << "    " << super_block.super.ninodes << " inodes\n";

	union fs_block pointer_block;
	fs_inode inode;

	// Reporta a organização dos inode blocks
	for (int inumber = 1; inumber <= super_block.super.ninodes; inumber++) {
		// Pega o inodo atual
		// Se o inodo atual for válido (já tiver sido criado)
		if (!load_inode(&inode, inumber, super_block.super.ninodeblocks)) continue;
		cout << "inode " << inumber << ":\n";
		cout << "    " << "size: " << fs_getsize(inumber) << " bytes\n";
		string direct_blocks_str = "";
		for (int direct_block : inode.direct)
			if (direct_block) direct_blocks_str +=  (" " + to_string(direct_block));
		if (direct_blocks_str != "")
			cout << "    " << "direct blocks:" << direct_blocks_str << "\n";
		if (inode.indirect) {
			cout << "    " << "indirect block: " << inode.indirect << "\n";
			cout << "    " << "indirect data blocks:";
			disk->read(inode.indirect, pointer_block.data);
			for (int pointer : pointer_block.pointers)
				if (pointer) cout << " " << pointer;
			cout << "\n";
		}
	}
}

// Examina o disco para um FS (se um está presente, lê o superblock, cria bitmap, e prepara o FS para uso)
int INE5412_FS::fs_mount()
{
	union fs_block super_block;
	union fs_block indirect_block;
	fs_inode inode;
	// Lê o superblock
	disk->read(0, super_block.data);

	if (super_block.super.magic != FS_MAGIC) return 0;

	// Cria o bitmap
	// Ignora os Inode Blocks e o Super Block
	bitmap = new int[super_block.super.nblocks - super_block.super.ninodeblocks - 1];
	for (int i = 0; i < super_block.super.nblocks  - super_block.super.ninodeblocks - 1; i++) bitmap[i] = 0;
	for (int i = 1; i <= super_block.super.ninodes; i++) {
		if (!load_inode(&inode, i, super_block.super.ninodeblocks)) continue;
		for (int j = 0; j < POINTERS_PER_INODE; j++)
			if (inode.direct[j]) change_bitmap(inode.direct[j]);
		if (!inode.indirect) continue;
		change_bitmap(inode.indirect);
		disk->read(inode.indirect, indirect_block.data);
		for (int j = 0; j < POINTERS_PER_BLOCK; j++)
			if (indirect_block.pointers[j]) change_bitmap(indirect_block.pointers[j]);
	}

	return 1;
}

// Cria um novo inodo de comprimento 0 (retorna inúmero positivo em caso de sucesso, 0 caso contrário)
int INE5412_FS::fs_create()
{
	if (!bitmap) return 0;
	union fs_block super_block;
	fs_inode inode;

	// Lê o superblock
	disk->read(0, super_block.data);
	// Para cada inode 
	for (int inumber = 1; inumber <= super_block.super.ninodes; inumber++) {
		// Se for inválido (não foi criado ainda)
		if (load_inode(&inode, inumber, super_block.super.ninodeblocks)) continue;
		// Configura o inodo como válido, todos ponteiros diretos e indireto para 0 e tamanho 0
		inode = {1,0,{},0};
		for (int k = 0; k < POINTERS_PER_INODE; k++) inode.direct[k] = 0; 
		save_inode(inumber, &inode);
		// Sobrescreve o superblock no disco com as novas informações (novo inode)
		disk->write(0, super_block.data);

		// Retorna o inúmero
		return inumber;
	}
	return 0;
}

// Deleta o inodo
int INE5412_FS::fs_delete(int inumber)
{
	if (!bitmap) return 0;
	union fs_block super_block;
	// Lê o superblock
	disk->read(0, super_block.data);

	union fs_block indirect_block;
	fs_inode inode;

	if (!load_inode(&inode, inumber, super_block.super.ninodeblocks)) return 0;

	// Se houver ponteiro indireto
	if (inode.indirect) {
		// Lê o bloco de ponteiros
		disk->read(inode.indirect, indirect_block.data);
		// Limpa os ponteiros
		clear_pointers(POINTERS_PER_BLOCK, indirect_block.pointers);
		for (int i = 0; i < disk->DISK_BLOCK_SIZE; i++) indirect_block.data[i] = ' ';
		// Sobrescreve o bloco indireto no disco, limpando-o
		disk->write(inode.indirect, indirect_block.data);
		change_bitmap(inode.indirect);
	}
	// Limpa os ponteiros diretos
	clear_pointers(POINTERS_PER_INODE, inode.direct);
	for (int i = 0; i < POINTERS_PER_INODE; i++) inode.direct[i] = 0;
	// Limpa o resto dos dados
	inode = {0,0,*inode.direct,0};

	// Sobrescreve o inode no disco (limpando-o)
	save_inode(inumber, &inode);
	// Sobrescreve o superblock no disco com as novas informações (remove o inode)
	disk->write(0, super_block.data);
	return 1;
}

int INE5412_FS::fs_getsize(int inumber)
{
	fs_block block;
	fs_inode inode;

	// Lê o superblock
	disk->read(0, block.data);

	if (!load_inode(&inode, inumber, block.super.ninodeblocks)) return -1;

	return inode.size;
}

// Lê dado do inodo (copia "length" bytes do inodo em data, a partir de "offset")
int INE5412_FS::fs_read(int inumber, char *data, int length, int offset)
{
	if (!bitmap) return 0;
	union fs_block super_block;
	union fs_block indirect_block;
	// Lê o super bloco
	disk->read(0, super_block.data);
	
	fs_inode inode;
	// Verifica erros (offset incorreto, length, etc...) e retorna 0
	if (!load_inode(&inode, inumber, super_block.super.ninodeblocks) || offset < 0 || length <= 0 || offset >= fs_getsize(inumber)) return 0;
	// Se o offset + length supera o tamanho do inode, length sera o restante para ler todo o inode
	if (offset + length > fs_getsize(inumber)) length = fs_getsize(inumber) - offset;

	// Define o bloco inicial da leitura e seu offset
	int starting_pointer = offset / disk->DISK_BLOCK_SIZE;
	int bytes_read = 0;

	// Lê os ponteiros diretos (caso o bloco inicial seja < POINTERS_PER_INODE)
	if (read_pointers(length, &bytes_read, starting_pointer, POINTERS_PER_INODE, inode.direct, data)) return bytes_read;
	if (!inode.indirect) return bytes_read;
	
	// Caso o bloco inicial seja 5, sera o bloco 0 apontado pelo bloco indireto 
	starting_pointer -= POINTERS_PER_INODE;
	// Caso seja negativo, ambos valores sao 0
	if (starting_pointer < 0) starting_pointer = 0;

	disk->read(inode.indirect, indirect_block.data);
	// Lê os ponteiros indiretos
	read_pointers(length, &bytes_read, starting_pointer, POINTERS_PER_BLOCK, indirect_block.pointers, data);
	return bytes_read;
	
}

// Escreve dado no inodo (copia "length" bytes de data no inodo, a partir de "offset")
int INE5412_FS::fs_write(int inumber, const char *data, int length, int offset)
{
	// Se o disco não foi montado
	if (!bitmap) return 0;

	union fs_block super_block;
	union fs_block indirect_block;

	// Lê o super bloco
	disk->read(0, super_block.data);
	
	fs_inode inode;

	// Verifica erros (offset incorreto, length, etc...) e retorna 0
	if (!load_inode(&inode, inumber, super_block.super.ninodeblocks) || offset < 0 || length <= 0) return 0;
	int remaining_storage_size = get_remaining_storage_size(&inode);
	if (!remaining_storage_size) return 0;
	// Se não tiver espaço para escrever tudo, a nova length é apenas o que couber
	if (length > remaining_storage_size) length = remaining_storage_size;

	// Define o bloco inicial da escrita
	int starting_pointer = offset / disk->DISK_BLOCK_SIZE;
	int bytes_written = 0;

	// Em que blocos escreveu "data" (apontados pelos ponteiros diretos)
	std::vector<int> written_blocks = write_in_pointers(&bytes_written, length, starting_pointer, POINTERS_PER_INODE, inode.direct, data);

	// i: Ponteiros em "written_blocks"
	// inode.direct[written_blocks[i]] = written_blocks[i+1]
	// Ponteiro direto (índice i)     || bloco (índice i+1)
	for (int i = 0; i < (int) written_blocks.size(); i += 2) inode.direct[written_blocks[i]] = written_blocks[i+1];

	inode.size += bytes_written;
	// Salva as mudanças do inodo
	save_inode(inumber, &inode);
	
	// Se já escreveu tudo que precisava
	if (bytes_written == length) return bytes_written; 
	// Se não escreveu tudo ainda e o ponteiro indireto ainda não foi criado
	if (!inode.indirect) inode.indirect = alloc_indirect_block();
	
	// Caso o bloco inicial seja 5, será o bloco 0 apontado pelo bloco indireto 
	starting_pointer -= POINTERS_PER_INODE;
	// Caso seja negativo, ambos valores são 0
	if (starting_pointer < 0) starting_pointer = 0;
	
	// Lê os ponteiros indiretos
	disk->read(inode.indirect, indirect_block.data);
	
	// Em que blocos escreveu "data" (apontados pelos ponteiros indiretos)
	written_blocks = write_in_pointers(&bytes_written, length, starting_pointer, POINTERS_PER_BLOCK, indirect_block.pointers, data);
	
	// i: Ponteiros em "written_blocks"
	// inode.direct[written_blocks[i]] = written_blocks[i+1]
	// Ponteiro direto (índice i)     || bloco (índice i+1)
	for (int i = 0; i < (int) written_blocks.size(); i+=2) indirect_block.pointers[written_blocks[i]] = written_blocks[i+1];
	
	inode.size += bytes_written;
	// Salva as mudanças do inodo
	save_inode(inumber, &inode);

	// Salva as mudanças no disco
	disk->write(inode.indirect, indirect_block.data);
	
	return bytes_written;
}
