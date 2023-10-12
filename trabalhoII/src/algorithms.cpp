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
            if (frames.size() == this->n_frames) {
                in_frame[frames.front()] = 0;
                frames.pop();
                page_faults++;
            }
            frames.push(page);
            in_frame[page] = 1;
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
            if (frames.size() == this->n_frames) {
                in_frame[frames.front()] = 0;
                frames.erase(frames.begin());
                page_faults++;
            }
            frames.push_back(page);
            in_frame[page] = 1;
        } else {
            frames.remove(page);
            frames.push_back(page);
        }
    }
    return page_faults;
}


//TODO
int Algorithms::opt() {
    // find next occurrence (from now on) of every page on frames
    // the page who got the most distant next occurrence will be substituted
    return 0;
}