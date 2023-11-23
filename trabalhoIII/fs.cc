#include "fs.h"
#include "math.h"

// Lê os blocos que os ponteiros apontam e os salva em data
int INE5412_FS::read_pointers(int length, int* bytes_read, int starting_block, int starting_index, int npointers, int pointers[], char *data) {
	union fs_block block;
	for (int i = starting_block; i < npointers; i++) {
		if (pointers[i]) {
			disk->read(pointers[i], block.data);
			for (int j = starting_index; j < disk->DISK_BLOCK_SIZE; j++) {
				data[*bytes_read] = block.data[j];
				(*bytes_read) += 1;
				if (*bytes_read == length) return 1;
			}
		}
		starting_index = 0;
	}
	return 0;
}

// Troca o valor armazenado em bitmap[block]
void INE5412_FS::change_bitmap(int block) {
	union fs_block super_block;
	disk->read(0, super_block.data);
	bitmap[block - super_block.super.ninodeblocks - 1] = !bitmap[block - super_block.super.ninodeblocks - 1];
}

// Limpa os ponteiros e os blocos que eles apontam
void INE5412_FS::clear_pointers(int npointers, int pointers[]) {
	union fs_block block;
	for (int i = 0; i < npointers; i++) {
		if (!pointers[i]) continue;
		disk->read(pointers[i], block.data);
		// Zera todos os ponteiros do ponteiro indireto
		for (int j = 0; j < disk->DISK_BLOCK_SIZE; j++) block.data[j] = '0';
		// Sobrescreve as alterações no disco
		disk->write(pointers[i], block.data);
		change_bitmap(pointers[i]);
		pointers[i] = 0;
	}
}

// Lê um inode do disco e retorna se existe
int INE5412_FS::load_inode(fs_inode *inode, int inumber, int ninodeblocks) {
	if (inumber <= 0 || inumber >= INODES_PER_BLOCK*ninodeblocks) return 0;
	inumber--;
	// Calcula em que inode block está o inodo a ser carregado
    int n_inode_block = inumber / (INODES_PER_BLOCK);
    // Calcula em que posição dentro do inode block está o inodo a ser carregado
    int inumber_in_inode_block = inumber % INODES_PER_BLOCK;

	union fs_block inode_block;

	disk->read(n_inode_block + 1, inode_block.data);
	*inode = inode_block.inode[inumber_in_inode_block];
	
	if (!inode->isvalid) return 0;
	return 1;
}

// Salva um inode no disco
void INE5412_FS::save_inode(int inumber, fs_inode *inode) {
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
	super_block.super = {FS_MAGIC, disk->size(), (int)ceil(disk->size()/10), 0};
	// Cria um inodo generico invalido
	fs_inode inode = {0,0,{},0};
	for (int i = 0; i < POINTERS_PER_INODE; i++) inode.direct[i] = 0;

	// Reserva 10% dos blocos para inodos
	for (int i = 1; i <= super_block.super.ninodeblocks*INODES_PER_BLOCK; i++) {
		save_inode(i, &inode);
	}
	
	// Define o data block com 4096 zeros
	for (int i = 0; i < disk->DISK_BLOCK_SIZE; i++) block.data[i] = '0';
	// Copia as informações em "block" (zeros) para todos os blocos de dados do disco
	for (int i = super_block.super.ninodeblocks + 1; i < super_block.super.nblocks; i++) {
		disk->write(i, block.data);
	}
	if (bitmap) delete bitmap;
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
	for (int inumber = 1; inumber <= super_block.super.ninodeblocks * INODES_PER_BLOCK; inumber++) {
		// Pega o inodo atual
		// Se o inodo atual for válido (já tiver sido criado)
		if (!load_inode(&inode, inumber, super_block.super.ninodeblocks)) continue;
		cout << "inode " << inumber << ":\n";
		cout << "    " << "size: " << inode.size << " bytes\n";
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
	// calloc (número de elementos a alocar, tamanho de cada elemento): Aloca dinamicamente um bloco contíguo de memória [Aloca e zera os conteúdos]
	// Ignora os Inode Blocks e o Super Block
	bitmap = (int*) calloc(super_block.super.nblocks - super_block.super.ninodeblocks - 1, sizeof(int));
	for (int i = 1; i <= super_block.super.ninodeblocks * INODES_PER_BLOCK; i++) {
		if (!load_inode(&inode, i, super_block.super.ninodeblocks)) continue;
		for (int j = 0; j < POINTERS_PER_INODE; j++)
			if (inode.direct[j]) bitmap[inode.direct[j]] = 1;
		if (!inode.indirect) continue;
		bitmap[inode.indirect] = 1;
		disk->read(inode.indirect, indirect_block.data);
		for (int j = 0; j < POINTERS_PER_BLOCK; j++) {
			if (indirect_block.pointers[j]) bitmap[indirect_block.pointers[j]] = 1;
		}
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
	for (int inumber = 1; inumber <= super_block.super.ninodeblocks * INODES_PER_BLOCK; inumber++) {
		// Se for inválido (não foi criado ainda)
		if (load_inode(&inode, inumber, super_block.super.ninodeblocks)) continue;
		// Configura o inodo como válido, todos ponteiros diretos e indireto para 0 e tamanho 0
		inode = {1,0,{},0};
		for (int k = 0; k < POINTERS_PER_INODE; k++) inode.direct[k] = 0; 
		save_inode(inumber, &inode);
		super_block.super.ninodes += 1;
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
		// Sobrescreve o bloco indireto no disco, limpando-o
		disk->write(inode.indirect, indirect_block.data);
		change_bitmap(inode.indirect);
	}
	// Limpa os ponteiros diretos
	clear_pointers(POINTERS_PER_INODE, inode.direct);

	// Limpa o resto dos dados
	inode = {0,0,*inode.direct,0};

	// Sobrescreve o inode no disco (limpando-o)
	save_inode(inumber, &inode);
	super_block.super.ninodes -= 1;
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
	if (!load_inode(&inode, inumber, super_block.super.ninodeblocks) || offset < 0 || length <= 0 || offset >= inode.size) return 0;
	// Se o offset + length supera o tamanho do inode, length sera o restante para ler todo o inode
	if (offset + length > inode.size) length = inode.size - offset;

	// Define o bloco inicial da leitura e seu offset
	int starting_block = offset / disk->DISK_BLOCK_SIZE;
	int starting_index = offset % disk->DISK_BLOCK_SIZE;
	int bytes_read = 0;
	// Lê os ponteiros diretos (caso o bloco inicial seja < POINTERS_PER_INODE)
	if (read_pointers(length, &bytes_read, starting_block, starting_index, POINTERS_PER_INODE, inode.direct, data)) return bytes_read;
	if (!inode.indirect) return bytes_read;
	
	// Caso o bloco inicial seja 5, sera o bloco 0 apontado pelo bloco indireto 
	starting_block -= POINTERS_PER_INODE;
	// Caso seja negativo, ambos valores sao 0
	if (starting_block < 0) {
		starting_block = 0;
		starting_index = 0;
	}

	disk->read(inode.indirect, indirect_block.data);
	// Lê os ponteiros indiretos
	read_pointers(length, &bytes_read, starting_block, starting_index, POINTERS_PER_BLOCK, indirect_block.pointers, data);
	return bytes_read;
	
}

int INE5412_FS::fs_write(int inumber, const char *data, int length, int offset)
{
	return 0;
}
