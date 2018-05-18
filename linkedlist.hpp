#include "project.hpp"

#ifndef linkedlistheader
#define linkedlistheader
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

class LinkedList{
public:
    Node* head;
    int count;
	LinkedList(){
	    count = 0;
	    head = NULL;
	}
	~LinkedList();
	void putFIFO(Process* p);
	void putSJF(Process* p);
	void putDQ(Process* p);
	void removeJob(int jobnum);
	vector<int> displayList();

// 	{
// 		Node* t = tail;
// 		head= new Node(p);
// 		if(head == 0)
// 			head = tail;
// 		else
// 			t-> next = tail;
// 			count++;
// }

// Process get(){
// 	Process p = head-> proc;
// 	Link t = head-> next;
// 	delete head;
// 	head = t;
// 	if(head==0)
// 		tail = 0;
// 	return p;
// }
    
};

#endif

