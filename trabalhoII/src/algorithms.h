#include <vector>

using namespace std;

class Algorithms {
    private:
        vector<int> pages;
        int n_frames;
        int n_pages;
    public:
        Algorithms(vector<int> pages_, int n_frames_, int n_pages_);
        ~Algorithms();
        int fifo();
        int lru();
        int opt();
        int find_next_occurrence(int page, vector<int>* opt_pages);
        vector<vector<int>*>* get_all_occurrences(vector<int>* opt_pages);

};