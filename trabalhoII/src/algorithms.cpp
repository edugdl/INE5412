#include <iostream>
#include <list>
#include "algorithms.h"

using namespace std;

// Construtor
Algorithms::Algorithms(vector<int>* pages_, int n_frames_, int n_pages_){
    // Vetor de requisições das páginas
    this->page_references = pages_;
    // Número de frames
    this->n_frames = n_frames_;
    // Número de páginas
    this->n_pages = n_pages_;
}

//Destrutor
Algorithms::~Algorithms(){}

int Algorithms::fifo() {
    // Vetor das ocorrências
    vector<int>* fifo_page_references = this->page_references;
    // Vetor dos frames
    queue<int> frames;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    int in_frame[this->n_pages];
    fill(in_frame, in_frame + this->n_pages, 0);
    int page_faults = 0;
    int page;
    for (int i = 0; i < (int) fifo_page_references->size(); i++) {
        page = fifo_page_references->at(i);
        // Se não está nos frames
        if (!in_frame[page]){
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
    // Vetor das ocorrências
    vector<int>* lru_page_references  = this->page_references;
    // Vetor dos frames
    list<int> frames;
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    int in_frame[this->n_pages];
    fill(in_frame, in_frame + this->n_pages, 0);
    int page_faults = 0;
    int page;
    
    for (int i = 0; i < (int) lru_page_references->size(); i++) {
        // Se não está nos frames
        if (!in_frame[page]){
            // Se todos os frames estiverem ocupados
            if ((int) frames.size() == this->n_frames) {
                // Página no fim da fila deixa de estar mapeada na memória e é retirada da lista frames
                in_frame[frames.back()] = 0;
                frames.pop_back();
            }
            // Nova página é inserida no início da lista frames
            frames.push_front(page);
            in_frame[page] = 1;
            page_faults++;
        } else {
            // Caso já esteja mapeada, passa para o início da lista
            frames.remove(page);
            frames.push_front(page);
        }
    }
    return page_faults;
    return page_faults;
}


int Algorithms::opt() {
    // Vetor das ocorrências
    vector<int>* opt_page_references = this->page_references;
    // Vetor dos frames
    int frames[this->n_frames];
    // Vetor para saber se uma página está mapeada na memória (0: Não || 1: Sim)
    int in_frame[this->n_pages];
    fill(in_frame, in_frame + this->n_pages, 0);
    // Matriz de ocorrências
    vector<queue<int>*>* all_occurrences = get_all_occurrences(opt_page_references);
    // ocorrência verificada
    queue<int>* current_occurrences;
    int page_faults = 0;
    int worst_occurrence;
    int index_worst_occurrence;
    int current_occurrence;
    int current_opt_page;
    int used_frames = 0;
    // Percorre todas as ocorrências (opt_page_references)
    for (int i = 0; i < (int) opt_page_references->size(); i++) {
        worst_occurrence = -1;
        current_opt_page = opt_page_references->at(i);
        // Se a página atual não estiver mapeada para a memória
        if (!in_frame[current_opt_page]) {
            // Se todos os frames estiverem ocupados
            if (used_frames == this->n_frames) {
                /* 
                Worst Ocurrence é a ocorrência que mais vai demorar para acontecer
                (é a página que será retirada de frames)
                -----------------------------------------------------
                Testa todas as páginas presentes nos frames, verificando qual
                vai demorar mais para ser chamada
                */
                for (int j = 0; j < used_frames; j++) {
                    current_occurrences = all_occurrences->at(frames[j]);
                    if (current_occurrences->size()) {
                        current_occurrence = all_occurrences->at(frames[j])->front();
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
                in_frame[frames[index_worst_occurrence]] = 0;
                frames[index_worst_occurrence] = current_opt_page;
            } else {
                // Insere na lista de frames
                frames[used_frames] = current_opt_page;
                used_frames++;
            }
            in_frame[current_opt_page] = 1;
            page_faults++;
        }
        // Retira a ocorrência atual da lista respectiva da página
        all_occurrences->at(current_opt_page)->pop();
    }
    
    for (int i = 0; i < this->n_pages; i++) {
        delete all_occurrences->at(i);
    }
    delete all_occurrences;

    return page_faults;
}

// Retorna matriz de ocorrências
// Cada linha na matriz (índice) representa o ID de uma página
// Os valores nessa linha (vetor) indicam os instantes em que essa página é requisitada
vector<queue<int>*>* Algorithms::get_all_occurrences(vector<int>* opt_page_references) {
    vector<queue<int>*>* all_occurrences = new vector<queue<int>*>(this->n_pages + 1);
    for (int j = 0; j < this->n_pages; j++)
        all_occurrences->at(j) = new queue<int>;
    

    for (int i = 0; i < (int) opt_page_references->size(); i++) {
        all_occurrences->at(opt_page_references->at(i))->push(i);
    }
    return all_occurrences;
}