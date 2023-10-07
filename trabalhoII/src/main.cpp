#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc < 2){
        cout << "A entrada deve informar a quantidade de quadros disponíveis na RAM" << endl;
        return 1;
    } else if (isatty(0)) {
        cout << "A entrada deve ser redirecionada para um input\nExemplo: ./solucao < input.txt" << endl;
        return 1;
    }

    int frames = atoi(argv[1]);
    vector<int> pages;

    int page;
    int i = 0;
    
    while (!feof(stdin))
    {
        scanf("%d",&page);
        pages.push_back(page);
    }
    
    return 0;
}
