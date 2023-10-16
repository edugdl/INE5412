#include <iostream>
#include <queue>
#include <list>
#include "algorithms.h"

using namespace std;

Algorithms::Algorithms(vector<int> pages_, int n_frames_, int n_pages_){
    this->pages = pages_;
    this->n_frames = n_frames_;
    this->n_pages = n_pages_;
}

Algorithms::~Algorithms(){}

int Algorithms::fifo() {
    vector<int> fifo_pages = this->pages;
    queue<int> frames;
    vector<int> in_frame(this->n_pages, 0);
    int page_faults = 0;
    
    for (int page : fifo_pages) {
        if (not in_frame[page]){
            if ((int) frames.size() == this->n_frames) {
                in_frame[frames.front()] = 0;
                frames.pop();
            }
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
    vector<int> in_frame(this->n_pages, 0);
    int page_faults = 0;
    
    for (int page : lru_pages) {
        if (not in_frame[page]){
            if ((int) frames.size() == this->n_frames) {
                in_frame[frames.front()] = 0;
                frames.erase(frames.begin());
            }
            frames.push_back(page);
            in_frame[page] = 1;
            page_faults++;
        } else {
            frames.remove(page);
            frames.push_back(page);
        }
    }
    return page_faults;
}


int Algorithms::opt() {
    vector<int>* opt_pages = &this->pages;
    vector<int>* frames = new vector<int>;
    vector<int>* in_frame = new vector<int>(this->n_pages+1, 0);
    int page_faults = 0;
    vector<vector<int>*>* all_occurrences = get_all_occurrences(opt_pages);
    for (int i = 0; i < (int) opt_pages->size(); i++) {
        int current_opt_page = opt_pages->at(i);
        if (!in_frame->at(current_opt_page)) {
            if ((int) frames->size() == this->n_frames) {
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

int Algorithms::find_next_occurrence(int i, vector<int>* occurrences) {
    // int j = 0;
    while (occurrences->size() && occurrences->at(0) <= i)
    {
        // cout << j++ << " " << occurrences.size() << endl;
        occurrences->erase(occurrences->begin());
    }
    if (occurrences->size()) return occurrences->at(0);
    return (int) this->pages.size();
}

vector<vector<int>*>* Algorithms::get_all_occurrences(vector<int>* opt_pages) {
    vector<vector<int>*>* all_occurrences = new vector<vector<int>*>(this->n_pages + 1);
    for (int i = 0; i < (int) opt_pages->size(); i++) {
        int current_page = opt_pages->at(i);
        if (!all_occurrences->at(current_page))
            all_occurrences->at(current_page) = new vector<int>;
        all_occurrences->at(current_page)->push_back(i);

    }
    return all_occurrences;
}