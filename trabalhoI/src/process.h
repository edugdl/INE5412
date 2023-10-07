#include "context.h"

//Define os possíveis estados que um processo pode ter
enum State {NEW, READY, EXECUTING, FINISHED};

class Process {
    private:
        int id, start, end, duration, priority, executed_time, current_time;
        State current_state;
        Context* context;

    public:
        Process(int id_, int start_, int duration_, int priority_);
        ~Process();

        int get_id();
        int get_start();
        int get_end();
        int get_duration();
        int get_priority();
        State get_current_state();
        int get_executed_time();
        bool is_finished();
        int get_current_time();
        Context* get_context();

        void set_id(int id_);
        void set_start(int start_);
        void set_end(int end_);
        void set_duration(int duration_);
        void set_priority(int priority_);
        void set_current_state(State current_state_);
        void set_executed_time(int executed_time_);
        void set_current_time(int current_time_);
        void execute();
};