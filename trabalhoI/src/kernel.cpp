#include <iostream>
#include "kernel.h"
#include "read_file.cpp"

using namespace std;

//Construtor da classe Kernel (inicializa objetos desta classe)
Kernel::Kernel(int algorithm_){
    time = 0;
    //Novo objeto da classe INE5412 é atribuído a cpu (objeto Kernel tem relação de associação com objetos INE5412)
    cpu = new INE5412();
    context_changes = 1;
    //Seleciona o algoritmo a ser usado com base no input do usuário
    switch (algorithm_) {
    case 1:
        algorithm = new FCFS();
        break;
    case 2:
        algorithm = new SJF();
        break;
    case 3:
        algorithm = new Priority();
        break;
    case 4:
        algorithm = new Priority_with_preemption();
        break;
    case 5:
        algorithm = new Round_robin();
        break;
    }   
}

//Destrutor
Kernel::~Kernel() {
    delete cpu;
    delete algorithm;
    for(Process* p : processes){
        delete p;
    }
}

//Função da classe Kernel para criar processos
void Kernel::create_processes() {

    //Cria objeto da classe File chamado f
    File f;

    //Recebe as informações passadas no arquivo de entrada
    f.read_file();

    int id = 1;
    for (ProcessParams* process_data : f.get_processes()) {
        //Cada elemento de processes_data gera um objeto Process e é colocado no fim do vetor processes
        processes.push_back(new Process(id++, process_data->get_creation_time(), process_data->get_duration(), process_data->get_priority()));
    }
}

vector<Process *> Kernel::get_processes() {
    return processes;
}

//Função da classe Kernel para simular a execução dos processos usando um algoritmo de escalonamento específico
vector<vector<State>> Kernel::run_processes() {
    //Atribui todos os processos como processos novos (NEW)
    vector<Process *> new_processes = processes;
    vector<Process *> ready_processes;
    vector<vector<State>> result;
    vector<State> loop_result;

    while (1) {
        size_t i = 0;

        //Se o tempo de criação do processo for igual ao tempo atual, ele é passado para a lista de processos prontos
        while (i != new_processes.size()) {
            if (new_processes[i]->get_start() == time) {
                ready_processes.push_back(new_processes[i]);
                new_processes[i]->set_current_state(READY);
                new_processes.erase(new_processes.begin()+i);
            } else {
                i++;
            }
        }
        
        //Se ainda nenhum processo tiver começado
        if (new_processes.size() == processes.size()) {
        
        //Se CPU não estiver executando nenhum processo
        } else if (cpu->is_empty()) {
            if(ready_processes[0]->get_current_state() == READY) {
                //Algoritmo de escalonamento escolhe qual processo deve ser executado
                algorithm->schedule(&ready_processes);
                cpu->set_new_process(ready_processes[0]);
                ready_processes[0]->set_current_state(EXECUTING);
            }
        //Se só existir um processo pronto e nenhum processo no estado novo - Condição de término da simulação
        } else if (ready_processes.size() == 1 && !new_processes.size()) {
            //Se o processo atual houver terminado, é marcado como FINISHED e os dados da simulação são printados na tela
            if (cpu->get_current_process()->is_finished()) {
                Process* last_process = cpu->free_current_process();
                last_process->set_current_state(FINISHED);
                last_process->set_end(time);
                loop_result = {};
                for (Process* p : processes) {
                    loop_result.push_back(p->get_current_state());
                }
                result.push_back(loop_result);        
                return result;
            }
        //Se precisa trocar o processo sendo executado
        } else if (algorithm->need_to_change_process(cpu->get_current_process(), ready_processes)) {
            context_changes++;
            //Ponteiro para o processo liberado
            Process* freed_process = cpu->free_current_process();
            freed_process->set_current_time(0);
            //Tira esse processo da lista dos processos prontos
            ready_processes.erase(ready_processes.begin());
            if (freed_process->is_finished()) {
                //Se o processo já estiver finalizado, ele é classificado como FINISHED e seu tempo de fim da execução é atribuído
                freed_process->set_current_state(FINISHED);
                freed_process->set_end(time);
            } else {
                //Se o processo ainda não estiver finalizado, ele é classificado como READY e colocado no fim da lista de prcessos prontos
                freed_process->set_current_state(READY);
                ready_processes.push_back(freed_process);
            }

            //Reorganiza a lista de processos prontos
            algorithm->schedule(&ready_processes);
            if (ready_processes.size()) {
                //Novo processo é o processo no início da fila (escalonado)
                cpu->set_new_process(ready_processes[0]);
                ready_processes[0]->set_current_state(EXECUTING);
            }
        }
        //Aumenta o tempo atual e tempo de execução do processo atual
        cpu->execute();
        // cpu->get_data(); // Caso queira ver o contexto atual da cpu
        loop_result = {};
        for (Process* p : processes) {
            loop_result.push_back(p->get_current_state());
        }
        result.push_back(loop_result);        
        time++;
    }
}

int Kernel::get_context_changes() {
    return context_changes;
}