#include "process.h"
#include <random>
using namespace std;

//Construtor
Process::Process(int id_, int start_, int duration_, int priority_){
    id = id_;
    start = start_;
    duration = duration_;
    priority = priority_;
    current_state = NEW;
    executed_time = 0;
    current_time = 0;
    end = 0;

    context = new Context();

}

//Destrutor
Process::~Process() {
    delete context;
}

int Process::get_id() {
    return id;
}

int Process::get_start() {
    return start;
}

int Process::get_end() {
    return end;
}

int Process::get_duration() {
    return duration;
}

int Process::get_priority() {
    return priority;
}

State Process::get_current_state() {
    return current_state;
}

int Process::get_executed_time() {
    return executed_time;
}

bool Process::is_finished()  {
    return executed_time == duration;
}

int Process::get_current_time() {
    return current_time;
}

Context* Process::get_context() {
    return context;
}

void Process::set_id(int id_) {
    id = id_;
}

void Process::set_start(int start_) {
    start = start_;
}

void Process::set_end(int end_) {
    end = end_;
}

void Process::set_duration(int duration_) {
    duration = duration_;
}

void Process::set_priority(int priority_) {
    priority = priority_;
}

void Process::set_current_state(State current_state_) {
    current_state = current_state_;
}

void Process::set_executed_time(int executed_time_) {
    executed_time = executed_time_;
}

void Process::set_current_time(int current_time_) {
    current_time = current_time_;
}

//Faz as alterações necessárias no contexto do processo
void Process::execute() {
    executed_time++;
    current_time++;
}