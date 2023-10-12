#include "array_list.h"
#include <cstdint>
#include <stdexcept>  // C++ Exceptions

ArrayList::ArrayList(int max_size) {
    size = 0;
    max_size = max_size;
    contents = new int[max_size];
};

ArrayList::~ArrayList() {
    delete [] contents;
};

void ArrayList::clear() {
    size = 0;
};

void ArrayList::push_back(int data) {
    if (full()) {
        throw std::out_of_range("Lista Cheia");
    } else {
        contents[size] = data;
        size++;
    }
}

void ArrayList::push_front(int data) {
    if (full()) throw std::out_of_range("Lista Cheia");
    for (int i = (size); i > 0; i--) {
        contents[i] = contents[i-1];
    }
    contents[0] = data;
    size++;
}

void ArrayList::insert(int data, int index) {
    if (full()) throw std::out_of_range("Lista Cheia");
    if (index >= get_size() || index < 0) throw std::out_of_range("Index Inválido");
    for (int i = (size); i > (index); i--) {
        contents[i] = contents[i-1];
    }
    contents[index] = data;
    size++;
}

void ArrayList::insert_sorted(int data) {
    if (full()) throw std::out_of_range("Lista Cheia");
    bool inserted = false;
    for (int i = 0; i < (size); i++) {
        if (contents[i] > data) {
            insert(data, i);
            inserted = true;
            break;
        }
    }
    if (!inserted) {
        push_back(data);
    }
}

int ArrayList::pop(int index) {
    if (empty()) throw std::out_of_range("Lista Vazia");
    if (index >= get_size() || index < 0) throw std::out_of_range("Index Inválido");
    int var = contents[index];
    for (int i = index; i < (size) - 1; i++) {
        contents[i] = contents[i+1];
    }
    size--;
    return var;
}

int ArrayList::pop_back() {
    if (empty()) throw std::out_of_range("Lista Vazia");
    int var = contents[size-1];
    size--;
    return var;
}

int ArrayList::pop_front() {
    if (empty()) throw std::out_of_range("Lista Vazia");
    int var = contents[0];
    for (int i = 1; i < (size); i++) {
        contents[i-1] = contents[i];
    }
    size--;
    return var;
}

void ArrayList::remove(int data) {
    for (int i = 0; i < (size); i++) {
        if (contents[i] == data) {
            pop(i);
            break;
        }
    }
}

bool ArrayList::full() {
    return size == max_size;
}

bool ArrayList::empty() {
    return size == 0;
}

bool ArrayList::contains(int data) {
    for (int i = 0; i < (size); i++) {
        if (contents[i] == data) {
            return true;
        }
    }
    return false;
}

int ArrayList::find(int data) {
    for (int i = 0; i < size; i++) {
        if (contents[i] == data) {
            return i;
        }
    }
    return get_size();
}


int ArrayList::get_size() {
    return size;
}

int ArrayList::get_max_size() {
    return max_size;
}

int& ArrayList::at(int index) {
    if (index >= get_size() || index < 0) throw std::out_of_range("Index Inválido");
    return contents[index];
}

int& ArrayList::operator[](int index) {
    if (index >= get_size() || index < 0) throw std::out_of_range("Index Inválido");
    return contents[index];
}

int& ArrayList::at(int index) {
    if (index >= get_size() || index < 0) throw std::out_of_range("Index Inválido");
    return contents[index];
}

int& ArrayList::operator[](int index) {
    if (index >= get_size() || index < 0) throw std::out_of_range("Index Inválido");
    return contents[index];
}
