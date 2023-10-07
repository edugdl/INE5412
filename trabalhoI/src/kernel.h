#include <vector>
#include "algorithms.h"

using namespace std;

class Kernel {
    private:
        //Vetor de ponteiros para objetos da classe Process
        vector<Process *> processes;
        //Ponteiro para cpu (objeto da classe INE5412)
        INE5412* cpu;
        int time;
        General_algorithm* algorithm;
        int context_changes;

    public:
        Kernel(int algorithm_);
        ~Kernel();
        void create_processes();

        int get_context_changes();
        vector<vector<State>> run_processes();
        vector<Process *> get_processes();

};