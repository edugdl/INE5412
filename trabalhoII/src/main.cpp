#include <string>
#include <unistd.h>
#include <iostream>
#include "algorithms.h"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc < 2 || argv[1] <= 0){
        cout << "A entrada deve informar a quantidade de quadros disponíveis na RAM, sendo ele um valor positivo" << endl;
        return 1;
    } else if (isatty(0)) {
        cout << "A entrada deve ser redirecionada para um input\nExemplo: ./solucao 1 < input.txt" << endl;
        return 1;
    }

    int n_frames = atoi(argv[1]);
    vector<int> pages;

    int page;
    int n_pages = 0;
    while (!feof(stdin))
    {
        scanf("%d",&page);
        pages.push_back(page);
        if (page > n_pages)
            n_pages = page;
    }
    pages.pop_back();

    Algorithms *a = new Algorithms(pages, n_frames, n_pages);
    cout << n_frames << " quadros" << endl;
    cout << pages.size() << " refs" << endl;
    cout << "FIFO: " << a->fifo() << " PFs"<< endl;
    cout << "LRU: " << a->lru() << " PFs"<< endl;
    cout << "OPT: " << a->opt() << " PFs"<< endl;
    
    return 0;
}
