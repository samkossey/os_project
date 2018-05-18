#include <iostream>
#include <fstream>
#include "string.h"
#include <cstring>
#include <cstdlib>
#include <vector>

using namespace std;

#ifndef projectheader
#define projectheader

enum CurrState {New, Running, Submit, HQ1, HQ2, RQ, Wait, Complete};


enum NextExt {A,Q,L,D,N,I};

struct Process{
	int num;
	int arrival;
	int tot_run;
	int run_remain;
	CurrState state;
	int endQuant;
	int memory;
	int max_dev;
	int curr_dev;
	int remain_dev;
	int compl_ti;
	int priority;
	int waiting; //how many devices it is waiting for

};

struct Banker{
	int num;
	int need;
	int alloc;
	bool finished;
	// Banker(int n, int need, int a){
	// 	num = n;
	// 	need_dev = need;
	// 	alloc = a;
	// 	finished = false;
	// }
	// ~Banker(){
		
	// }
};

struct Dev{
	int num;
	int job_num;
	int action; //0 request, 1 release
	int ti;
};

#include "linkedlist.hpp"
struct System{
	string filename;
	int tot_mem;
	int a_mem;
	int tot_dev;
	int a_dev;
	int curr_ti;
	int quantum;
	int arrival;
	int internal;
	int external;
	int lastTime;
	NextExt next;
	Process* process;
	Process* running;
	Dev devreq;
	vector<Process*> jobs;
	LinkedList* rq;
	LinkedList* hq1;
	LinkedList* hq2;
	LinkedList* dq;
	
	System(){
		rq = new LinkedList();
		hq1 = new LinkedList();
		hq2 = new LinkedList();
		dq = new LinkedList();
		running = NULL;
	}
	~System(){
		delete rq;
		delete hq1;
		delete hq2;
		delete dq;
		for (int i = 0; i<this->jobs.size(); i++){
		    delete this->jobs[i];
		}
	}
};

int parse(string line, System* s);
int parseC(string line, System* s);
int parseD(string line, System* s);
int parseA(string line, System* s);
int parseQL(string line, System* s);
void deleteJobSys(System* s, Process* p);
void handleEvent(System* s, bool ext);
void process_arrival(Process* p, System* s);
void round_robin(System *s);
void addToReady(Process *p, System *s);
void toReadyFromComplete(Process* p, System* s);
void display(System* s);
void interruptQuant(System* s);
bool bankers(System*s, Process* p);
bool checkSafe(Banker* b, int avail, int length);
void request_device(Dev d, System* s);
void release_device(Dev d, System* s);

#endif