#include "queue.h"
#include <cstdint>  // std::size_t
#include <stdexcept>  // C++ Exceptions

Queue::Queue(size_t max_size_) {
    max_size = max_size_;
    contents = new int[max_size];
    clear();
};

Queue::~Queue() {
    delete [] contents;
}

void Queue::enqueue(int data) {
    if (full()) {
        throw std::out_of_range("Fila Cheia");
    } else {
        end = (end + 1) % max_size;
        contents[end] = data;
        size++;
    }
}

int Queue::dequeue() {
    if (empty()) {
        throw std::out_of_range("Fila Vazia");
    } else {
        int removed_element = contents[begin];
        begin++;
        if (begin >= get_max_size()) {
            begin = 0;
        }
        size--;
        return removed_element;
    }
}

int Queue::back() {
    if (empty()) {
        throw std::out_of_range("Fila Vazia");
    } else {
        return contents[end];
    }
}

void Queue::clear() {
    begin = -1;
    end = -1;
    size = 0;
}

int Queue::get_max_size() {
    return max_size;
}

int Queue::get_size() {
    return size;
}

bool Queue::empty() {
    return (size == 0);
}

bool Queue::full() {
    return (size == max_size);
}