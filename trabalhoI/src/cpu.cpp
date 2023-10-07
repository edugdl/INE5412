#include "cpu.h"
#include <iostream>
#include <random>

using namespace std;


INE5412::INE5412(){
    process = nullptr;
    current_context = new Context();
    cpu_initial_context = current_context;
}

//Destrutor
INE5412::~INE5412() {
    delete cpu_initial_context;
}

//Construtor da classe INE5412 que permite a criação de um objeto INE5412 com um contexto específico
INE5412::INE5412(long long int SP_, long long int PC_, long long int ST_, long long int REG_[6]) {
    current_context = new Context(SP_, PC_, ST_, REG_);
    cpu_initial_context = current_context;
    process = nullptr;
}

void INE5412::get_data() {
    
    cout << "\nPC: " << current_context->get_PC() << endl;
    cout << "SP: " << current_context->get_SP() << endl;
    cout << "ST: " << current_context->get_ST() << endl;
    for (int i = 0; i < 6; ++i) {
        cout << "REG[" << i << "]: " << current_context->get_REG(i) << endl;
    }
}

Process* INE5412::get_current_process() {
    return process;
}

bool INE5412::is_empty() {
    return !process;
}

//Escolhe o processo para ser executado pela CPU
void INE5412::set_new_process(Process* process_) {
    process = process_;
    current_context = process->get_context();
}

//Libera o processo que está atualmente em execução e retorna um ponteiro para este processo
Process * INE5412::free_current_process() {
    Process *freed_process = process;
    process = nullptr;
    return freed_process;
}

void INE5412::execute() {
    if (!process) return;
    process->execute();
    current_context->set_PC(current_context->get_PC() + 4);
    current_context->set_SP(current_context->get_SP() + 1);
    current_context->set_ST(current_context->get_ST() + 8);
    for (int i = 0; i < 6; i++)
        current_context->set_REG(current_context->get_REG(i) + i,i);
}