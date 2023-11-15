#include "fs.h"

int INE5412_FS::fs_format()
{
	union fs_block super_block;
	union fs_block inode_block;
	union fs_block block;
	
	disk->read(0, super_block.data);
	for (int i = 1; i <= super_block.super.ninodeblocks; i++) {
		disk->read(i, inode_block.data);
		for (int j = 0; j < INODES_PER_BLOCK; j++) {
			inode_block.inode[j].isvalid = 0;
			for (int k = 0; k < POINTERS_PER_INODE; k++) inode_block.inode[j].direct[k] = 0;
			inode_block.inode[j].indirect = 0;
			inode_block.inode[j].size = 0; // Verificar se essa linha funciona corretamente
		}
		disk->write(i, inode_block.data);
	}

	for (int i = super_block.super.ninodeblocks + 1; i < super_block.super.nblocks; i++) {
		disk->read(i, block.data);
		for (long unsigned j = 0; j < sizeof(block.data); j++) block.data[j] = '0';
		disk->write(i, block.data);
	}

	super_block.super.ninodes = 0;

	return 1;
}

void INE5412_FS::fs_debug()
{
	union fs_block block;

	disk->read(0, block.data);

	cout << "superblock:\n";
	cout << "    " << (block.super.magic == FS_MAGIC ? "magic number is valid\n" : "magic number is invalid!\n");
 	cout << "    " << block.super.nblocks << " blocks\n";
	cout << "    " << block.super.ninodeblocks << " inode blocks\n";
	cout << "    " << block.super.ninodes << " inodes\n";
}

int INE5412_FS::fs_mount()
{
	return 0;
}

int INE5412_FS::fs_create()
{
	return 0;
}

int INE5412_FS::fs_delete(int inumber)
{
	return 0;
}

int INE5412_FS::fs_getsize(int inumber)
{
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
