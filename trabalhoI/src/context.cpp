#include "context.h"
#include <random>

using namespace std;

Context::Context() {
    // Gera uma seed de números aleatórios a partir de um dispositivo aleatório
    static random_device rd;  
    // Especifica o intervalo desejado para o valor aleatório
    uniform_int_distribution<long long int> dist(0ULL, 1000ULL);

    SP = dist(rd);
    PC = dist(rd);
    ST = dist(rd);
    for (int i = 0; i < 6; i++)
        REG[i] = dist(rd);
}
//Caso o usuário queira atribuir valores específicos
Context::Context(long long int SP_, long long int PC_, long long int ST_, long long int REG_[6]) : SP(SP_), PC(PC_), ST(ST_){
    for (int i = 0; i < 6; i++)
    {
        REG[i] = REG_[i];
    }
    
}

long long int Context::get_SP() {
    return SP;
}

long long int Context::get_PC() {
    return PC;
}

long long int Context::get_ST() {
    return ST;
}

long long int Context::get_REG(int i) {
    return REG[i];
}

void Context::set_SP(long long int SP_) {
    SP = SP_;
}

void Context::set_PC(long long int PC_) {
    PC = PC_;
}

void Context::set_ST(long long int ST_) {
    ST = ST_;
}

void Context::set_REG(long long int REG_, int i) {
    REG[i] = REG_;
}
