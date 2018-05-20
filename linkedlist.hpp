// Samantha Kossey, Lakshmi Palaparthi
#include "project.hpp"

#ifndef linkedlistheader
#define linkedlistheader

//Node for linked list of processes
struct Node{
	Process* proc;
	Node* next;
	Node(Process* p){
		proc = p;
		next = NULL;
	}
	~Node(){
	    
	}
};

//linked list class for processes
class LinkedList{
public:
    Node* head;
    int count;
	LinkedList(){
	    count = 0;
	    head = NULL;
	}
	~LinkedList();
	//for ready queue, hold queue 2
	void putFIFO(Process* p);
	//for hold queue 1
	void putSJF(Process* p);
	//for device queue, puts smallest device request first
	void putDQ(Process* p);
	//removes a job from the linked list
	void removeJob(int jobnum);
	//puts the job numbers of the linked list in a vector
	vector<int> displayList();
    
};

#endif

