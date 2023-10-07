 #include "cpu.h"
#include <vector>

using namespace std;

//Classe abstrata
class General_algorithm {
    public:
        virtual void schedule(vector<Process *>* ready_processes);
        virtual bool need_to_change_process(Process* current_process, vector<Process *> _);
        virtual ~General_algorithm();
};

//Classes dos algoritmos de escalonamento, classes filhas de General_algorithm
class FCFS : public General_algorithm {};

class SJF : public General_algorithm {
    public:
        void schedule(vector<Process *>* ready_processes);
};

class Priority : public General_algorithm {
    public:
        void schedule(vector<Process *>* ready_processes);
};

class Priority_with_preemption : public General_algorithm {
    public:
        void schedule(vector<Process *>* ready_processes);
        bool need_to_change_process(Process* current_process, vector<Process *> processes);
};

class Round_robin : public General_algorithm {
    public:
        bool need_to_change_process(Process* current_process, vector<Process *> ready_processes);
};