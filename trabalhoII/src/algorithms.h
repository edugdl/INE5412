#include <vector>

using namespace std;

class Algorithms {
    private:
        vector<int> pages;
        int n_frames;
    public:
        Algorithms(vector<int> pages_, int n_frames_);
        ~Algorithms();
        int fifo();
        int lru();
        int opt();
        int find_next_occurence(int page, vector<int> opt_pages);

};