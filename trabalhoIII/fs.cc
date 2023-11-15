#include "fs.h"

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
	block.super.nblocks = 0;
	block.super.ninodeblocks = 0;
	block.super.ninodes = 0;

	disk->write(0, block.data);

	return 1;
}

void INE5412_FS::fs_debug()
{

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
