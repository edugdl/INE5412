#include <vector>

using namespace std;

class Reader {
    private:
        vector<int>* pages;
        int n_pages;
    public:
        Reader();
        ~Reader();
        void read_file(int n_frames);
        vector<int>* get_pages();
        int get_n_pages();
};