// Samantha Kossey, Lakshmi Palaparthi
#include <iostream>
#include <fstream>
#include "string.h"
#include <cstring>
#include <cstdlib>
#include <vector>

using namespace std;

#ifndef projectheader
#define projectheader

//possible job states
enum CurrState {New, Running, HQ1, HQ2, RQ, Wait, Complete};

//possible next external event arrival, request, release, display, new
enum NextExt {A,Q,L,D,N};

//jobs that arrive are saved in the process struct
struct Process{
	int num; //job number
	int arrival; //time job arrives
	int tot_run; //total run time
	int run_remain; //run time remaining
	CurrState state; //state of process
	int endQuant; //time its quantum will end if running
	int memory; //memory required
	int max_dev; //total # of devices
	int curr_dev; //current allocated devices
	int remain_dev; //how many devices it still needs
	int compl_ti; //time the process finished running
	int priority; //priority 1 or 2 determines which hold queue
	int waiting; //how many devices it is waiting for

};

//structures to check for a safe state after pretending to allocate devices
struct Banker{
	int num; //associated job number
	int need; //how many the process still needs
	int alloc; //how many the process has been allocated
	bool finished; //whether or not the process finishes
};

//used to store device request/releases from the input file
struct Dev{
	int num; //number of devices required
	int job_num; //associated job number
	int action; //0 request, 1 release
	int ti; //time req/release was made
};

#include "linkedlist.hpp"
//overall structure for the system
struct System{
	string filename; //name of input file
	int tot_mem; //total memory in the system
	int a_mem; //current available memory in the system
	int tot_dev; //total number of devices in the system
	int a_dev; //current available devices
	int curr_ti; //current system time
	int quantum; //length of RR quantum
	int arrival; //time the system is initialized at
	int internal; //time of next internal event
	int external; //time of next external event
	int lastTime; //last time read from input file (for error checking)
	NextExt next; //next external event that will happen
	Process* process; //saves a process arrival here
	Process* running; //saves a running process here
	Dev devreq; //saves a device request/release here
	vector<Process*> jobs; //all the valid jobs that arrive
	LinkedList* rq; //ready queue FIFO RR
	LinkedList* hq1; //hold queue SJF
	LinkedList* hq2; //hold queue 2 FIFO
	LinkedList* dq; //device wait queue smallest request first
	LinkedList* complete; //complete queue FIFO
	
	//constructor
	System(){
		rq = new LinkedList();
		hq1 = new LinkedList();
		hq2 = new LinkedList();
		dq = new LinkedList();
		complete = new LinkedList();
		running = NULL;
		process = NULL;
	}
	
	//destructor
	~System(){
		delete rq;
		delete hq1;
		delete hq2;
		delete dq;
		delete complete;
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