#include "process.h"


class INE5412 {
    private:
        //Aponta para o processo atualmente em execução
        Process* process;
        //Aponta para o contexto inicial da cpu
        Context* cpu_initial_context;
        //Aponta para o contexto do processo atualmente em execução
        Context* current_context;

    public:
        //Construtores
        INE5412();
        INE5412(long long int SP_, long long int PC_, long long int ST_, long long int REG_[6]);
        ~INE5412();

        void get_data();
        
        Process* get_current_process();
        Process* free_current_process();
        bool is_empty();
        
        void set_new_process(Process* process_);
        void execute();
};