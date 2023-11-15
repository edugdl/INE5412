#include "fs.h"

int INE5412_FS::fs_format()
{
	return 0;
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
