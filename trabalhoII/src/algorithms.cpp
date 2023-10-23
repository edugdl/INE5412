#include <iostream>
#include <queue>
#include <list>
#include "algorithms.h"

using namespace std;

// Construtor
Algorithms::Algorithms(vector<int>* pages_, int n_frames_, int n_pages_){
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
    vector<int>* fifo_pages = this->pages;
    queue<int> frames;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    vector<int> in_frame(this->n_pages, 0);
    int page_faults = 0;
    for (int i = 0; i < (int) fifo_pages->size(); i++) {
        int page = fifo_pages->at(i);
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
    vector<int>* lru_pages = this->pages;    
    list<int> frames;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    vector<int> in_frame(this->n_pages, 0);
    int page_faults = 0;
    
    for (int i = 0; i < (int) lru_pages->size(); i++) {
        int page = lru_pages->at(i);
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
    vector<int>* opt_pages = this->pages;
    vector<int>* frames = new vector<int>;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    vector<int>* in_frame = new vector<int>(this->n_pages+1, 0);
    int page_faults = 0;
    // Matriz de ocorrências
    deque<deque<int>*>* all_occurrences = get_all_occurrences(opt_pages);
    int worst_occurrence;
    int index_worst_occurrence;
    int current_occurrence;
    deque<int>* current_occurrences;
    // Percorre todas as ocorrências (opt_pages)
    for (int i = 0; i < (int) opt_pages->size(); i++) {
        worst_occurrence = -1;
        int current_opt_page = opt_pages->at(i);
        // Se a página atual não estiver mapeada para a memória
        if (!in_frame->at(current_opt_page)) {
            // Se todos os frames estiverem ocupados
            if ((int) frames->size() == this->n_frames) {
                /* 
                Worst Ocurrence é a ocorrência que mais vai demorar para acontecer
                (é a página que será retirada de frames)
                -----------------------------------------------------
                Pega a próxima ocorrência da primeira página em frames
                e supõe que ela é a pior ocorrência 
                */
                // Testa todas as outras páginas em frames para ver se alguém
                // tem uma ocorrência pior (vai demorar mais para ser chamado)
                for (int j = 0; j < (int) frames->size(); j++) {
                    current_occurrences = all_occurrences->at(frames->at(j));
                    if (current_occurrences->size()) {
                        current_occurrence = all_occurrences->at(frames->at(j))->at(0);
                        if (current_occurrence > worst_occurrence) {
                            worst_occurrence = current_occurrence;
                            index_worst_occurrence = j;
                        }
                    } else {
                        index_worst_occurrence = j;
                        break;
                    }
                }
                // Página com pior ocorrência deixa de estar mapeada na memória e é retirada da lista frames
                in_frame->at(frames->at(index_worst_occurrence)) = 0;
                frames->at(index_worst_occurrence) = current_opt_page;
            } else {
                frames->push_back(current_opt_page);
            }
            in_frame->at(current_opt_page) = 1;
            page_faults++;
        }
        deque<int>* current_occurrences = all_occurrences->at(current_opt_page);
        current_occurrences->pop_front();
    }
    
    delete frames;
    delete in_frame;

    for (int i = 0; i < (int)all_occurrences->size(); i++) {
        delete all_occurrences->at(i);
    }
    delete all_occurrences;

    return page_faults;
}

// Retorna matriz de ocorrências
// Cada linha na matriz (índice) representa o ID de uma página
// Os valores nessa linha (vetor) indicam os instantes em que essa página é requisitada
deque<deque<int>*>* Algorithms::get_all_occurrences(vector<int>* opt_pages) {
    deque<deque<int>*>* all_occurrences = new deque<deque<int>*>(this->n_pages + 1);

    for (int i = 0; i < (int) opt_pages->size(); i++) {
        // Pega a página localizada na posição i (requisitada no instante i)
        int current_page = opt_pages->at(i);
        if (!all_occurrences->at(current_page))
            all_occurrences->at(current_page) = new deque<int>;
        // Coloca o instante no vetor correspondente a página requisitada
        all_occurrences->at(current_page)->push_back(i);

    }
    return all_occurrences;
}