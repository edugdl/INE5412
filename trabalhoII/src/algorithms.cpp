#include "algorithms.h"
#include <queue>
#include <list>

using namespace std;

Algorithms::Algorithms(vector<int> pages_, int n_frames_){
    this->pages = pages_;
    this->n_frames = n_frames_;
}

Algorithms::~Algorithms(){}

int Algorithms::fifo() {
    vector<int> fifo_pages = this->pages;
    queue<int> frames;
    vector<int> in_frame(fifo_pages.size(), 0);
    int page_faults = 0;
    for (int page : fifo_pages) {
        if (not in_frame.at(page)){
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
    vector<int> in_frame(lru_pages.size(), 0);
    int page_faults = 0;
    for (int page : lru_pages) {
        if (not in_frame.at(page)){
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
    vector<int> opt_pages = this->pages;
    vector<int> frames;
    vector<int> in_frame(opt_pages.size(), 0);
    int page_faults = 0;
    while (opt_pages.size()) {
        if (not in_frame.at(opt_pages[0])){
            if ((int) frames.size() == this->n_frames) {
                int worst_occurence = find_next_occurence(frames[0], opt_pages);
                int worst_frame = 0;
                for (int i = 1; i < (int) frames.size(); i ++) {
                    int current_occurence = find_next_occurence(frames[i], opt_pages);
                    if (current_occurence > worst_occurence) {
                        worst_occurence = current_occurence;
                        worst_frame = i;
                    }
                }
                in_frame[frames[worst_frame]] = 0;
                frames[worst_frame] = opt_pages[0];
                in_frame[frames[worst_frame]] = 1;
            } else {
                frames.push_back(opt_pages[0]);
                in_frame[frames[frames.size()-1]] = 1;
            }
            page_faults++;
        }
        opt_pages.erase(opt_pages.begin());
    }
    
    return page_faults;
}

int Algorithms::find_next_occurence(int page, vector<int> opt_pages) {
    size_t i = 1;
    do {
        if (opt_pages[i] == page) return i;
        i++;
    } while (i < opt_pages.size());
    
    return i;
}