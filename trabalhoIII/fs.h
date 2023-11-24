#ifndef FS_H
#define FS_H
#include <vector>
#include "disk.h"

class INE5412_FS
{
public:
    static const unsigned int FS_MAGIC = 0xf0f03410;
    static const unsigned short int INODES_PER_BLOCK = 128;
    static const unsigned short int POINTERS_PER_INODE = 5;
    static const unsigned short int POINTERS_PER_BLOCK = 1024;
    int* bitmap = nullptr;

    // Superblock (descreve o layout do resto do sistema de arquivos)
    // Cada campo do superblock é um inteiro de 4 bytes
    class fs_superblock {
        public:
            // Número mágico: assinatura do sistema de arquivos
            unsigned int magic;
            // Número total de blocos (igual ao número de blocos no disco)
            int nblocks;
            // Número de blocos que armazenam Inodes
            // ninodeblocks = 10% de nblocks
            int ninodeblocks;
            // Número total de inodos nos Inode Blocks
            int ninodes;
    }; 

    // Inode
    // Cada campo de inodo tem 4 bytes
    class fs_inode {
        public:
            // 1 se o inodo é válido (foi criado), 0 caso inválido
            int isvalid;
            // Tamanho do inodo em bytes
            int size;
            // 5 ponteiros diretos para blocos de dados [ponteiro = número de um bloco que possui dados]
            int direct[POINTERS_PER_INODE];
            // Ponteiro para bloco indireto de dados
            int indirect;
            // 0 = ponteiro de bloco nulo
            // Cada inodo ocupa 32 bytes - 128 inodos por bloco de inodo de 4KB
            // Cada ponteiro é um inteiro de 4 bytes
            // 1024 ponteiros por bloco
    };

    // Cada block do disco vai ser um destes tipos
    union fs_block {
        public:
            fs_superblock super;
            // Array de inodos (Inode Block)
            fs_inode inode[INODES_PER_BLOCK];
            // Array de inteiros
            // Cada pointer é um número correspondente a um bloco de dados
            int pointers[POINTERS_PER_BLOCK];
            // Array de chars
            // Área de dados do disco
            char data[Disk::DISK_BLOCK_SIZE];
    };

public:

    INE5412_FS(Disk *d) {
        disk = d;
    } 

    ~INE5412_FS() {
        delete[] bitmap;
    }

    // Varre o sistema de arquivos montado e reporta como os inodos e blocos estão organizados
    void fs_debug();
    // Cria um novo FS no disco e apaga todos os dados presentes
    int  fs_format();
    // Examina o disco para um FS (se um está presente, lê o superblock, cria bitmap, e prepara o FS para uso)
    int  fs_mount();
    // Cria um novo inodo de comprimento 0 (retorna inúmero positivo em caso de sucesso, 0 caso contrário)
    int  fs_create();
    // Deleta o inodo
    int  fs_delete(int inumber);
    // Retorna o tamanho do do inodo em bytes
    int  fs_getsize(int inumber);
    // Lê dado do inodo (copia "length" bytes do inodo em data, a partir de "offset")
    int  fs_read(int inumber, char *data, int length, int offset);
    // Escreve dado no inodo (copia "length" bytes de data no inodo, a partir de "offset")
    int  fs_write(int inumber, const char *data, int length, int offset);
    // Lê um inode do disco e retorna se existe
    int load_inode(fs_inode *inode, int inumber, int ninodeblocks);
    // Salva um inode no disco
    void save_inode(int inumber, fs_inode *inode);
    // Limpa os ponteiros e os blocos que eles apontam
    void clear_pointers(int npointers, int pointers[]);
    // Lê os blocos que os ponteiros apontam e os salva em data
    int  read_pointers(int length, int* bytes_read, int starting_pointer, int npointers, int *pointers, char *data);
    // Troca o valor armazenado em bitmap[block]
    void change_bitmap(int block);
    // Retorna o valor total em bytes que o inodo ainda consegue armazenar
    int  get_remaining_storage_size(fs_inode inode);
    std::vector<int>  write_in_pointers(int *bytes_written, int length, int starting_block, int npointers, int *pointers, const char *data);
    // Retorna a posição do bloco i no bitmap
    int  bitmap_hash(int i);
    // Retorna a posição do próximo bloco livre
    int  get_next_free_block();
    // Aloca dados nos blocos indiretos
    int  alloc_indirect_block();
private:
    Disk *disk;
};

#endif