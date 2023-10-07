 //Classe abstrata
using namespace std;

class Context{
    private:
        //Variáveis de 64 bits
        //Stack Pointer
        long long int SP;
        //Program Counter
        long long int PC;
        //Status
        long long int ST;
        //Array de Registradores
        long long int REG[6];
    
    public:
        //Construtores
        Context();
        Context(long long int SP_, long long int PC_, long long int ST_, long long int REG_[6]);

        long long int get_SP();
        long long int get_PC();
        long long int get_ST();
        long long int get_REG(int i);

        void set_SP(long long int SP_);
        void set_PC(long long int PC_);
        void set_ST(long long int ST_);
        void set_REG(long long int REG_, int i);

};