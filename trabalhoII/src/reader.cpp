#include "reader.h"
#include <iostream>

// Construtor
Reader::Reader() {
    this->pages = new vector<int>;
    this->n_pages = 0;
}

Reader::~Reader() {
    delete this->pages;
}

// Le o arquivo
void Reader::read_file(int n_frames) {
    // Pages recebe todas as ocorrências das páginas
    int page;

    // Enquanto não for o fim do arquivo ou ler um não inteiro, le a ocorrência da página
    while (!feof(stdin) && scanf("%d", &page) == 1) {
        // Adiciona a página a lista
        pages->push_back(page);
        // Define a quantidade de páginas pelo valor da maior
        if (page > n_pages)
            n_pages = page;
    }
}

vector<int>* Reader::get_pages(){
    return this->pages;
}

int Reader::get_n_pages(){
    return this->n_pages + 1;
}