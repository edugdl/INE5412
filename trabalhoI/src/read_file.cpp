#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class ProcessParams
{
public:
	ProcessParams(int c, int d, int p) { 
		creation_time = c;
		duration = d;
		priority = p;
	}

	friend ostream &operator<<(ostream& os, const ProcessParams& p) {
		os << "Creation time = " << p.creation_time << " duration = " << p.duration << " priority = " << p.priority << endl;
		return os;
	}

	int get_creation_time() {
		return creation_time;
	}

	int get_duration() {
		return duration;
	}

	int get_priority() {
		return priority;
	}
	
private:	
	int creation_time;
	int duration; //seconds
	int priority;
};

class File
{

public:
	File() {
		myfile.open("entrada.txt");
		if (!myfile.is_open()) {
			cout << "Erro ao abrir o arquivo!\n";
		}
	}
	
	void read_file() {
	
		int a, b, c;
		
		if (!myfile.is_open()) {
			cout << "Arquivo não está aberto!" << endl;
		}
		
		while (myfile >> a >> b >> c) {
			cout << a << b << c << endl;
			ProcessParams *p = new ProcessParams(a, b, c);
			processes.push_back(p);
		}

		cout << "Quantidade de processos lidos do arquivo: " << processes.size() << endl;
		myfile.close();
	}

	void print_processes_params() {
		vector<ProcessParams *>::iterator iter;

		for(iter = processes.begin(); iter < processes.end(); iter++) {
			ProcessParams *p = *iter;
			cout << *p;
		}
	}

	~File() {
		for(size_t i = 0; i < processes.size() ; i++) {
			ProcessParams *p = processes[i];
			delete p;
		}
	}

	vector<ProcessParams *> get_processes() {
		return processes;
	}

private:
	ifstream myfile; 
	vector<ProcessParams *> processes;
};