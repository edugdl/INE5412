#include <iostream>
#include <queue>
#include <list>
#include "algorithms.h"

using namespace std;

// Construtor
Algorithms::Algorithms(vector<int> pages_, int n_frames_, int n_pages_){
    // Vetor de requisições das páginas
    this->pages = pages_;
    // Número de frames
    this->n_frames = n_frames_;
    // Número de páginas
    this->n_pages = n_pages_;
}

//Destrutor
Algorithms::~Algorithms(){}

int Algorithms::fifo() {
    vector<int> fifo_pages = this->pages;
    queue<int> frames;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    vector<int> in_frame(this->n_pages, 0);
    int page_faults = 0;
    
    for (int page : fifo_pages) {
        if (not in_frame[page]){
            // Se todos os frames estiverem ocupados
            if ((int) frames.size() == this->n_frames) {
                // Página no início da fila deixa de estar mapeada na memória e é retirada da lista frames
                in_frame[frames.front()] = 0;
                frames.pop();
            }
            // Nova página é inserida no final da lista frames
            frames.push(page);
            in_frame[page] = 1;
            page_faults++;
        }
    }

    return page_faults;
}

int Algorithms::lru() {
    vector<int> lru_pages = this->pages;    
    list<int> frames;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    vector<int> in_frame(this->n_pages, 0);
    int page_faults = 0;
    
    for (int page : lru_pages) {
        if (not in_frame[page]){
            // Se todos os frames estiverem ocupados
            if ((int) frames.size() == this->n_frames) {
                // Página no início da fila deixa de estar mapeada na memória e é retirada da lista frames
                in_frame[frames.front()] = 0;
                frames.erase(frames.begin());
            }
            // Nova página é inserida no final da lista frames
            frames.push_back(page);
            in_frame[page] = 1;
            page_faults++;
        } else {
            // Caso já esteja mapeada, passa para o fim da lista
            frames.remove(page);
            frames.push_back(page);
        }
    }
    return page_faults;
}


int Algorithms::opt() {
    vector<int>* opt_pages = &this->pages;
    vector<int>* frames = new vector<int>;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    vector<int>* in_frame = new vector<int>(this->n_pages+1, 0);
    int page_faults = 0;
    // Matriz de ocorrências
    vector<vector<int>*>* all_occurrences = get_all_occurrences(opt_pages);

    // Percorre todas as ocorrências (opt_pages)
    for (int i = 0; i < (int) opt_pages->size(); i++) {
        int current_opt_page = opt_pages->at(i);
        // Se a página atual não estiver mapeada para a memória
        if (!in_frame->at(current_opt_page)) {
            // Se todos os frames estiverem ocupados
            if ((int) frames->size() == this->n_frames) {
                // Pega a próxima ocorrência da primeira página em frames ?????
                int worst_occurrence = find_next_occurrence(i, all_occurrences->at(frames->at(0)));
                int index_worst_occurrence = 0;
                for (int j = 1; j < (int) frames->size(); j++) {
                    int current_occurrence = find_next_occurrence(i, all_occurrences->at(frames->at(j)));
                    if (current_occurrence > worst_occurrence) {
                        worst_occurrence = current_occurrence;
                        index_worst_occurrence = j;
                    }
                }
                in_frame->at(frames->at(index_worst_occurrence)) = 0;
                frames->at(index_worst_occurrence) = current_opt_page;
            } else {
                frames->push_back(current_opt_page);
            }
            in_frame->at(current_opt_page) = 1;
            page_faults++;
        }
    }
    
    delete opt_pages;
    delete frames;
    delete in_frame;

    for (int i = 0; i < (int)all_occurrences->size(); i++) {
        delete all_occurrences->at(i);
    }
    delete all_occurrences;

    return page_faults;
}

// Retorna o instante da próxima ocorrência de uma página
int Algorithms::find_next_occurrence(int i, vector<int>* occurrences) {
    // int j = 0;
    // Enquanto a lista de ocorrências não estiver vazia e 
    // i for maior ou igual ao primeiro elemento da lista de ocorrências
    while (occurrences->size() && occurrences->at(0) <= i)
    {
        // Retira o primeiro elemento
        occurrences->erase(occurrences->begin());
    }
    if (occurrences->size()) return occurrences->at(0);
    return (int) this->pages.size();
}

// Retorna matriz de ocorrências
// Cada linha na matriz (índice) representa o ID de uma página
// Os valores nessa linha (vetor) indicam os instantes em que essa página é requisitada
vector<vector<int>*>* Algorithms::get_all_occurrences(vector<int>* opt_pages) {
    vector<vector<int>*>* all_occurrences = new vector<vector<int>*>(this->n_pages + 1);

    for (int i = 0; i < (int) opt_pages->size(); i++) {
        // Pega a página localizada na posição i (requisitada no instante i)
        int current_page = opt_pages->at(i);
        if (!all_occurrences->at(current_page))
            all_occurrences->at(current_page) = new vector<int>;
        // Coloca o instante no vetor correspondente a página requisitada
        all_occurrences->at(current_page)->push_back(i);

    }
    return all_occurrences;
}