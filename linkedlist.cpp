#include "project.hpp"
#include "linkedlist.hpp"

LinkedList::~LinkedList(){
    Node* temp = this->head;
    while (temp != NULL){
        Node* old = temp;
        temp = temp->next;
        delete old;
        old = NULL;
    }
}

void LinkedList::putFIFO(Process* p){
    Node *n = new Node(p);
    if (count == 0){
        this->head = n;
        this->count = 1;
    }
    else{
        Node* temp = this->head;
        while (temp->next != NULL){
            temp = temp->next;
        }
        temp->next = n;
        count = count + 1;
    }
}

void LinkedList::putSJF(Process* p){
    Node *n = new Node(p);
    int run = p->tot_run;
    if (count == 0){
        this->head = n;
        this->count = 1;
    }
    else if (this->head->proc->tot_run > run){
        n->next = this->head;
        this->head = n; 
    }
    else{
        Node* temp = this->head;
        while (temp->next != NULL && (temp->next->proc->tot_run <= run)){
            temp = temp->next;
        }
        if (temp->next == NULL){
            temp->next = n;
        }
        else{
            Node* saveOld = temp->next;
            temp->next = n;
            n->next = saveOld;
        }
        this->count ++;
    }
}

void LinkedList::removeJob(int jobnum){
    Node* temp = this->head;
    if (temp->proc->num == jobnum){
        Node* old = temp;
        this->head = old->next;
        delete old;
        old = NULL;
        this->count--;
    }
    else{
    while(temp->next != NULL && temp->next->proc->num != jobnum){
        temp = temp->next;
    }
    if (temp->next == NULL){
        //not found
    }
    else{
        Node* old = temp->next;
        temp->next = temp->next->next;
        delete old;
        old = NULL;
        this->count --;
        
    }
    }
}


// class FIFO_Queue{
// public:
//     Node* head;
//     int count;
// 	FIFO_Queue(){
// 	    count = 0;
// 	    head = NULL;
// 	}
// 	void put(const Process& p)

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
    
// };

// int main(void){
//     LinkedList* l = new LinkedList();
//     Process* p = new Process();
//     Process* p2 = new Process();
//     Process* p3 = new Process();
    
//     p3->tot_run = 2;
//     p3->num = 3;
//     p->num = 1;
//     p2->num = 2;
//     p->tot_run = 2;
//     p2->tot_run = 2;
//     l->putSJF(p);
//     l->putSJF(p2);
//     l->putSJF(p3);
//     l->removeJob(2);
//     l->removeJob(3);
//     l->removeJob(1);
//     delete p;
//     delete p2;
//     delete p3;
    
//     delete l;
//     return 0;
// }
