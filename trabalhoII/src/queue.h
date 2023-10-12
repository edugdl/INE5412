#include <cstdint>  // std::size_t
#include <stdexcept>  // C++ Exceptions

class Queue {
    private:
        int begin, end, size, max_size;
        int* contents;

    public:
        // Construtor com parâmetro
        Queue(size_t max_size);
        ~Queue();

        // Mudar caso não sejam inteiros
        void enqueue(int data);
        int dequeue();
        int back();
        void clear();
        int get_size();
        int get_max_size();
        bool empty();
        bool full();
};