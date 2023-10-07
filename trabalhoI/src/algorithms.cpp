#include "algorithms.h"
#include <algorithm>
#include <iostream>

using namespace std;

//Função que indica se precisa ou não trocar o processo em execução (caso o processo atual tenha terminado)
bool General_algorithm::need_to_change_process(Process* current_process, vector<Process *> _) {
    if (current_process->is_finished()) return true;
    return false;
}

void General_algorithm::schedule(vector<Process *>* ready_processes) {
    return;
}

General_algorithm::~General_algorithm(){}

void SJF::schedule(vector<Process *>* ready_processes) {
    //Ordena pelo tempo de duração dos processos prontos
    sort(ready_processes->begin(), ready_processes->end(), [](Process* a, Process* b) {
        return a->get_duration() < b->get_duration();
    });
}

void Priority::schedule(vector<Process *>* ready_processes) {
    //Ordena pela prioridade dos processos prontos
    sort(ready_processes->begin(), ready_processes->end(), [](Process* a, Process* b) {
        return a->get_priority() > b->get_priority();
    });
}

//Se a prioridade de um processo for maior que a do processo sendo executado, deve ocorrer preempção
bool Priority_with_preemption::need_to_change_process(Process* current_process, vector<Process *> ready_processes) {
    for (Process* p : ready_processes)
        if (p->get_priority() > current_process->get_priority()) return true;
    //Se não precisar trocar pela prioridade, verifica se n precisa trocar pelo tempo (processo finalizou)
    return General_algorithm::need_to_change_process(current_process, ready_processes);
}

void Priority_with_preemption::schedule(vector<Process *>* ready_processes) {
    //Ordena pela prioridade dos processos prontos
    sort(ready_processes->begin(), ready_processes->end(), [](Process* a, Process* b) {
        return a->get_priority() > b->get_priority();
    });
}

bool Round_robin::need_to_change_process(Process* current_process, vector<Process *> ready_processes) {
    //Se o processo sendo executado já estiver há dois segundos em execução, ocorre preempção
    if (current_process->get_current_time() == 2) return true;
    return General_algorithm::need_to_change_process(current_process, ready_processes);
}
