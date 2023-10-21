#include <string>
#include <unistd.h>
#include <iostream>
#include "reader.h"
#include "algorithms.h"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc < 2 || atoi(argv[1]) <= 0){
        cout << "A entrada deve informar a quantidade de quadros disponíveis na RAM, sendo ele um valor positivo" << endl;
        return 1;
    } else if (isatty(0)) {
        cout << "A entrada deve ser redirecionada para um input\nExemplo: ./solucao 1 < input.txt" << endl;
        return 1;
    }

    int n_frames = atoi(argv[1]);

    Reader* r = new Reader();
    r->read_file(n_frames);

    Algorithms *a = new Algorithms(r->get_pages(), n_frames, r->get_n_pages());
    cout << n_frames << " quadros" << endl;
    cout << r->get_pages()->size() << " refs" << endl;
    cout << "FIFO: " << a->fifo() << " PFs"<< endl;
    cout << "LRU: " << a->lru() << " PFs"<< endl;
    cout << "OPT: " << a->opt() << " PFs"<< endl;
    
    return 0;
}
