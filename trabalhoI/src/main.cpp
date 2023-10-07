#include <iostream>
#include "kernel.h"
#include <iomanip>

using namespace std;

int main() {

    //Para selecionar o algoritmo de escalonamento desejado
    //TODO adicionar verificador para ver se a opção inserida está entre as possíveis
    int algorithm;
    cout << "Escolha o algoritmo de escalonamento ->\n1- FCFS (First Come, First Served)\n2- Shortest Job First\n3- Por prioridade, sem preempcao\n4- Por prioridade, com preempcao por prioridade\n5- Round-Robin com quantum = 2s, sem prioridade\n" << endl;
    cin >> algorithm;
    while (algorithm > 5 || algorithm < 1) {
        cout << "Escolha o algoritmo de escalonamento ->\n1- FCFS (First Come, First Served)\n2- Shortest Job First\n3- Por prioridade, sem preempcao\n4- Por prioridade, com preempcao por prioridade\n5- Round-Robin com quantum = 2s, sem prioridade\n" << endl;
        cin >> algorithm;
    }
    
    cout << endl;

    //Novo objeto Kernel é atribuído ao ponteiro k
    Kernel *k = new Kernel(algorithm);
    k->create_processes();
    vector<vector<State>> result = k->run_processes();

    vector<Process *> kernel_executed_processes = k->get_processes();


    float total_tournaround = 0;
    int tournaround_time = 0;
    int wait_time = 0;
    float n_processes = kernel_executed_processes.size();

    cout << "Tempo de Tournaround e Tempo Médio de Espera por processo:" << endl;

    //Para cada um dos objetos Process do vetor processes, calcula o Turnaround e Tempo de Espera 
    for (Process *process : kernel_executed_processes) {
        //Turnaround = Tempo decorrido desde o momento que o processo ficou pronto até o fim de sua execução (fim - início)
        tournaround_time = process->get_end() - process->get_start();
        total_tournaround += tournaround_time;
        //Tempo Médio de Espera = Soma dos períodos em que o processo estava pronto (fim - início - tempo em execução)
        wait_time = (tournaround_time) - process->get_executed_time();
        cout << "P" << process->get_id() << " Turnaround: " << tournaround_time << " Tempo Medio de Espera: " << wait_time << endl;
    }
    cout << "Tempo medio de tournaround: " << total_tournaround/n_processes << endl; 
    cout << "Trocas de contexto: " << k->get_context_changes() << endl;

    cout << "tempo";
    for (Process* p : k->get_processes()) {
        cout << "  P" << p->get_id();
    }
    cout << endl;

    int time = 0;

    for (vector<State> processes_states : result) {
        cout << setw(2) << setfill('0') << time << "-";
        cout << setw(2) << setfill('0') << time+1;
        for (State s : processes_states){
            if (s == NEW || s == FINISHED) {
                cout << "    ";
            } else if (s == READY) {
                cout << "  --";
            } else {
                cout << "  ##";
            }
        }
        time++;
        cout << endl;
    }

    delete k;
    return 0;
}