// Samantha Kossey, Lakshmi Palaparthi
#include "project.hpp"
#include "linkedlist.hpp"

//linked list destructor (destroys each node)
LinkedList::~LinkedList(){
    Node* temp = this->head;
    while (temp != NULL){
        Node* old = temp;
        temp = temp->next;
        delete old;
        old = NULL;
    }
}

//adds new process to list in FIFO order
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

//puts new job in list in SJF order
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

//puts new job in device queue in shortest request first order
void LinkedList::putDQ(Process* p){
    Node *n = new Node(p);
    int w = p->waiting;
    if (count == 0){
        this->head = n;
        this->count = 1;
    }
    else if (this->head->proc->tot_run > w){
        n->next = this->head;
        this->head = n; 
    }
    else{
        Node* temp = this->head;
        while (temp->next != NULL && (temp->next->proc->tot_run <= w)){
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

//removes job from list/deletes its corresponding node
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
        cout << "Error: Tried to remove a job number that is not in the list." << cout;
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


