#include "fs.h"
#include "math.h"

int INE5412_FS::fs_format()
{
	union fs_block block;
	for (int j = 0; j < disk->size(); j++) block.data[j] = '0';
	
	for (int i = 0; i < disk->size(); i++) {
		disk->write(i, block.data);
	}

	if (bitmap) {
		delete bitmap;
	}

	block.super.magic = FS_MAGIC;
	block.super.nblocks = disk->size();
	block.super.ninodeblocks = ceil(disk->size()/10);
	block.super.ninodes = 0;

	disk->write(0, block.data);

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

	union fs_block inode_block;
	union fs_block pointer_block;

	for (int i = 1; i <= block.super.ninodeblocks; i++) {
		disk->read(i, inode_block.data);
		for (int j = 0; j < INODES_PER_BLOCK; j++) {
			fs_inode current_inode = inode_block.inode[j];
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

int INE5412_FS::fs_mount()
{
	fs_block super_block;

	disk->read(0, super_block.data);

	bitmap = (int*) calloc(super_block.super.nblocks - super_block.super.ninodeblocks - 1, sizeof(int));

	return 1;
}

int INE5412_FS::fs_create()
{
	fs_block super_block;
	fs_block inode_block;

	disk->read(0, super_block.data);
	for (int i = 1; super_block.super.ninodeblocks; i++) {
		disk->read(i, inode_block.data);
		for (int j = 0; j < INODES_PER_BLOCK; j++) {
			if (i == 1 and !j) continue;
			if (!inode_block.inode[j].isvalid) {
				inode_block.inode[j].isvalid = 1;
				for (int k = 0; k < POINTERS_PER_INODE; k++) inode_block.inode[j].direct[k] = 0; 
				inode_block.inode[j].indirect = 0;
				inode_block.inode[j].size = 0;
				disk->write(i, inode_block.data);
				super_block.super.ninodes += 1;
				disk->write(0, super_block.data);
				return (i-1)*INODES_PER_BLOCK + j;
			}
		}
	}
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
