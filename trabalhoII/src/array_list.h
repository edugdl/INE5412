#include <cstdint>

class ArrayList {
    private:
        int* contents;
        int size, max_size;
    
    public:
        ArrayList(int max_size_);
        ~ArrayList();
        void clear();
        void clear(); 
        void push_back(int data); 
        void push_front(int data);  
        void insert(int data, int index); 
        void insert_sorted(int data);  
        int pop(int index); 
        int pop_back();  
        int pop_front();  
        void remove(int data);
        bool full(); 
        bool empty();  
        bool contains(int data); 
        int find(int data);  
        int get_size();  
        int get_max_size() ;  
        int& at(int index);
        int& operator[](int index);

};