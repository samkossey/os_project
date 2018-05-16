#include <iostream>
#include <fstream>
#include "string.h"
#include <cstring>
#include <cstdlib>
#include <vector>

using namespace std;

#ifndef projectheader
#define projectheader

enum CurrState {Running, Submit, HQ1, HQ2, RQ, Wait, Complete};


enum NextExt {A,Q,L,D,N};

struct Process{
	int num;
	int arrival;
	int tot_run;
	int run_remain;
	CurrState state;
	int memory;
	int max_dev;
	int curr_dev;
	int priority;
	int waiting;

};

struct Dev{
	int num;
	int job_num;
	int action; //0 request, 1 release
	int ti;
};

#include "linkedlist.hpp"
struct System{
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
	Dev devreq;
	vector<Process*> jobs;
	LinkedList* rq;
	LinkedList* hq1;
	LinkedList* hq2;
	
	System(){
		rq = new LinkedList();
		hq1 = new LinkedList();
		hq2 = new LinkedList();
	}
	~System(){
		delete rq;
		delete hq1;
		delete hq2;
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
void handleEvent(System* s);
void process_arrival(Process* p, System* s);

#endif