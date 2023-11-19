#include "fs.h"
#include "math.h"

// "Cria" (sobrescreve) um novo FS no disco e apaga todos os dados presentes
int INE5412_FS::fs_format()
{
	// Cria um novo bloco
	union fs_block block;

	// Cria um data block com 4096 zeros
	for (int j = 0; j < disk->size(); j++) block.data[j] = '0';
	
	// Copia as informações em "block" (zeros) para todos os blocos do disco
	for (int i = 0; i < disk->size(); i++) {
		disk->write(i, block.data);
	}

	if (bitmap) {
		delete bitmap;
	}

	// Configura as informações básicas do novo sistema de arquivos
	block.super.magic = FS_MAGIC;
	block.super.nblocks = disk->size();
	// Número de blocos de inodo = 10% do número de blocos
	block.super.ninodeblocks = ceil(disk->size()/10);
	block.super.ninodes = 0;

	disk->write(0, block.data);

	return 1;
}

// Varre o sistema de arquivos montado e reporta como os inodos e blocos estão organizados
void INE5412_FS::fs_debug()
{
	union fs_block block;

	// Lê o superblock
	disk->read(0, block.data);

	cout << "superblock:\n";
	cout << "    " << (block.super.magic == FS_MAGIC ? "magic number is valid\n" : "magic number is invalid!\n");
 	cout << "    " << block.super.nblocks << " blocks\n";
	cout << "    " << block.super.ninodeblocks << " inode blocks\n";
	cout << "    " << block.super.ninodes << " inodes\n";

	union fs_block inode_block;
	union fs_block pointer_block;

	// Reporta a organização dos inode blocks
	for (int i = 1; i <= block.super.ninodeblocks; i++) {
		// Lê o bloco i do disco
		disk->read(i, inode_block.data);
		// Para cada inode dentro do inode block
		for (int j = 0; j < INODES_PER_BLOCK; j++) {
			// Pega o inodo atual
			fs_inode current_inode = inode_block.inode[j];
			// Se o inodo atual for válido (já tiver sido criado)
			if (current_inode.isvalid) {
				cout << "inode " << (i-1)*INODES_PER_BLOCK + j<< ":\n";
				cout << "    " << "size: " << current_inode.size << " bytes\n";
				cout << "    " << "direct blocks:";
				for (int direct_block : current_inode.direct)
					if (direct_block) cout <<  " " << direct_block;
				cout << "\n";
				if (current_inode.indirect) {
					cout << "    " << "indirect block: " << current_inode.indirect << "\n";
					cout << "    " << "indirect data blocks:";
					disk->read(current_inode.indirect, pointer_block.data);
					for (int pointer : pointer_block.pointers)
						if (pointer) cout << " " << pointer;
					cout << "\n";
				}
			}
		}
	}
}

// Examina o disco para um FS (se um está presente, lê o superblock, cria bitmap, e prepara o FS para uso)
int INE5412_FS::fs_mount()
{
	fs_block super_block;

	// Lê o superblock
	disk->read(0, super_block.data);

	// Cria o bitmap
	// calloc (número de elementos a alocar, tamanho de cada elemento): Aloca dinamicamente um bloco contíguo de memória [Aloca e zera os conteúdos]
	// Ignora os Inode Blocks e o Super Block
	bitmap = (int*) calloc(super_block.super.nblocks - super_block.super.ninodeblocks - 1, sizeof(int));

	return 1;
}

// Cria um novo inodo de comprimento 0 (retorna inúmero positivo em caso de sucesso, 0 caso contrário)
int INE5412_FS::fs_create()
{
	fs_block super_block;
	fs_block inode_block;

	// Lê o superblock
	disk->read(0, super_block.data);
	
	for (int i = 1; super_block.super.ninodeblocks; i++) {
		// Lê o bloco i do disco
		disk->read(i, inode_block.data);
		// Para cada inode dentro do inode block
		for (int j = 0; j < INODES_PER_BLOCK; j++) {
			if (i == 1 && !j) continue;
			// Se for inválido (não foi criado ainda)
			if (!inode_block.inode[j].isvalid) {
				// Configura o inodo como válido
				inode_block.inode[j].isvalid = 1;
				// Configura todos os ponteiros para 0 (não apontam para nenhum bloco de dados)
				for (int k = 0; k < POINTERS_PER_INODE; k++) inode_block.inode[j].direct[k] = 0; 
				// Configura o ponteiro indireto para 0 (não aponta para nenhum bloco de dados)
				inode_block.inode[j].indirect = 0;
				// Configura o tamanho do inode como 0
				inode_block.inode[j].size = 0;
				// Sobrescreve o inode block no disco com as novas informações (novo inode)
				disk->write(i, inode_block.data);
				super_block.super.ninodes += 1;
				// Sobrescreve o superblock no disco com as novas informações (novo inode)
				disk->write(0, super_block.data);

				// Retorna o inúmero
				return (i-1)*INODES_PER_BLOCK + j;
			}
		}
	}
	return 0;
}

// Deleta o inodo
int INE5412_FS::fs_delete(int inumber)
{
	fs_block super_block;
	// Lê o superblock
	disk->read(0, super_block.data);

	// Se inúmero for inválido
	if (inumber <= 0 || inumber >= INODES_PER_BLOCK*super_block.super.ninodeblocks) return 0;

	fs_block inode_block;
	fs_block indirect_block;
	fs_block block;
	
	// Calcula em que inode block está o inodo a ser deletado
	int n_inode_block = inumber / (INODES_PER_BLOCK);
	// Calcula em que posição dentro do inode block está o inodo a ser deletado
	int inumber_in_inode_block = inumber % INODES_PER_BLOCK;
	
	// Lê o inode block
	disk->read(1 + n_inode_block, inode_block.data);
	// Se o inodo for inválido (não foi criado) retorna 0 (falha)
	if (!inode_block.inode[inumber_in_inode_block].isvalid) return 0;

	// Itera pelos ponteiros do inodo a ser deletado
	for (int i = 0; i < POINTERS_PER_INODE; i++) {
		if (inode_block.inode[inumber_in_inode_block].direct[i]) {
			// Lê os ponteiros do inodo
			disk->read(inode_block.inode[inumber_in_inode_block].direct[i], block.data);
			// Zera os ponteiros (não apontam mais para nenhum bloco de dados)
			for (int j = 0; j < disk->DISK_BLOCK_SIZE; j++) block.data[j] = '0';
			// Sobrescreve as alterações no disco
			disk->write(indirect_block.pointers[i], block.data);
			// bitmap[inode_block.inode[inumber_in_inode_block].direct[i] - super_block.super.ninodeblocks - 1] = 0;
		}
		inode_block.inode[inumber_in_inode_block].direct[i] = 0;
	}

	inode_block.inode[inumber_in_inode_block].isvalid = 0;
	inode_block.inode[inumber_in_inode_block].size = 0;
	
	if (inode_block.inode[inumber_in_inode_block].indirect) {
		// Lê o ponteiro indireto
		disk->read(inode_block.inode[inumber_in_inode_block].indirect, indirect_block.data);
		for (int i = 0; i < POINTERS_PER_BLOCK; i++) {
			if (indirect_block.pointers[i]) {
				disk->read(indirect_block.pointers[i], block.data);
				// Zera todos os ponteiros do ponteiro indireto
				for (int j = 0; j < disk->DISK_BLOCK_SIZE; j++) block.data[j] = '0';
				// Sobrescreve as alterações no disco
				disk->write(indirect_block.pointers[i], block.data);
				// bitmap[indirect_block.pointers[i] - super_block.super.ninodeblocks - 1] = 0;
			}
		}
	}

	inode_block.inode[inumber_in_inode_block].indirect = 0;
	disk->write(1 + n_inode_block, inode_block.data);
	super_block.super.ninodes -= 1;
	disk->write(0, super_block.data);
	return 1;
}

int INE5412_FS::fs_getsize(int inumber)
{
	fs_block block;
	fs_block inode_block;
	// Lê o superblock
	disk->read(0, block.data);

	// Se inúmero for inválido
	if (inumber <= 0 || inumber >= INODES_PER_BLOCK*block.super.ninodeblocks) return -1;

	// Calcula em que inode block está o inodo a ser deletado
	int n_inode_block = inumber / (INODES_PER_BLOCK);
	// Calcula em que posição dentro do inode block está o inodo a ser deletado
	int inumber_in_inode_block = inumber % INODES_PER_BLOCK;

	// Lê o inode block
	disk->read(1 + n_inode_block, inode_block.data);
	if (inode_block.inode[inumber_in_inode_block].isvalid) return inode_block.inode[inumber_in_inode_block].size;
	return -1;
}

int INE5412_FS::fs_read(int inumber, char *data, int length, int offset)
{
	return 0;
}

int INE5412_FS::fs_write(int inumber, const char *data, int length, int offset)
{
	return 0;
}
