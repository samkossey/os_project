#include "project.hpp"
#include "linkedlist.hpp"

void deleteJobSys(System* s, Process* p){
	for (int i = 0; i < s->jobs.size(); i++){
		if (s->jobs[i] == p){
			s->jobs.erase(s->jobs.begin() + i);
			break;
		}
	}
	delete p;
	p = NULL;
}

void handleEvent(System* s, bool ext){
	if (!ext){
		cout << "handling round robin" << endl;
		round_robin(s);
		return;
	}
	if (s->next == A){
		if (s->process == NULL){
			cout << "error skip this arrival" << endl;
		}
		else{
			cout << "handling arrival" << endl;
			process_arrival(s->process, s);
		}
	}
	else{
		cout << "different external event" << endl;
	}
}

void process_arrival(Process* p, System* s){
	if (p == NULL){
		return;
	}
	if (s->curr_ti > s->external){
		cout << "bad timing" << endl;
		s->process = NULL;
		return;
	}
	s->curr_ti = s->external;
	s->external = -1;
	if (p->memory <= s->tot_mem && p->max_dev <= s->tot_dev){
		//enough memory/devices in total to keep the job
		s->jobs.push_back(p);
		if (p->memory <= s->a_mem){
			s->a_mem = s->a_mem - p->memory;
			//add process to ready queue/starting running if no one else is
			addToReady(p, s);
				
			s->process = NULL;	
		}
		else{
			//add to SJF or FIFO based on priority
			if (p->priority == 1){
				//add to SJF
				s->hq1->putSJF(p);
				p->state = HQ1;
			}
			else if (p->priority == 2){
				//add to FIFO
				s->hq2->putFIFO(p);
				p->state = HQ2;
			}
		s->process = NULL;
		}
	}
	else{
		s->process = NULL;
	}
}

void request_device(Dev d, System* s, Process* p){
	if (p->state == Running){
		//@TODO fake allocate

		//@TODO If banker's accepts as state you can allocate
		bool accept = false;

		if (accept){
			p->curr_dev = p->curr_dev + d.num;
			s->a_dev = s->a_dev - d.num;
			//@TODO remove from running
			//@TODO call new running
			//@TODO add back to ready queue
			p->state = RQ;
		}
		else{
			//@TODO add to device queue
			//@TODO remove from running
			//@TODO call new running
			p->waiting = d.num;
			p->state = Wait;
		}

	}
	else{
		//@TODO some kind of error
	}
}

void release_device(Dev d, System* s, Process* p){
	if (p->state == Running){
			p->curr_dev = p->curr_dev - d.num;
			s->a_dev = s->a_dev + d.num;
			//@TODO remove from running
			//@TODO call new running
			//@TODO add back to ready queue
			p->state = RQ;

			//@TODO check device queue for safe waiting processes
			//@TODO poss add them to the ready queue
	}
	else{
		//@TODO some kind of error
	}
}

//@TODO 
void addFromHold(System* s){
	Node* temp1 = s->hq1->head;
	Node* temp2 = s->hq2->head;
	while (temp1 != NULL && s->a_mem != 0){
		if (temp1->proc->memory <= s->a_mem){
			s->a_mem = s->a_mem - temp1->proc->memory;
			addToReady(temp1->proc, s);
			//@TODO does remove job delete????
			s->hq1->removeJob(temp1->proc->num);
		}
		temp1 = temp1->next;
	}
	while (temp2 != NULL && s->a_mem != 0){
		if (temp2->proc->memory <= s->a_mem){
			s->a_mem = s->a_mem - temp2->proc->memory;
			addToReady(temp1->proc, s);
			s->hq2->removeJob(temp2->proc->num);
		}
		temp2 = temp2->next;
	}
	
}

void addToReady(Process *p, System* s){
	if (s->rq->count == 0 && s->running == NULL){
		s->running = p;
		p->state = Running;
		if (p->run_remain < s->quantum){
				s->internal = s->curr_ti + p->run_remain;
				p->run_remain = 0;
			}
		else{
				s->internal = s->curr_ti + s->quantum;
				p->run_remain = p->run_remain - s->quantum;
			}
	}
	else{
		s->rq->putFIFO(p);
		p->state = RQ;
	}
}

//@TODO not done
void complete_process(Process* p, System* s){
	//TODO release_device()
	p->state = Complete;
	s->running = NULL;
	//@TODO add to complete list
	s->a_mem = s->a_mem + p->memory;
	s->a_dev = s->a_dev + p->curr_dev;
	p->curr_dev = 0;
	
	addFromHold(s);


}

void round_robin(System* s){
	if (s->curr_ti > s->internal){
		cout << "bad timing round robin" << endl;
		return;
	}
	s->curr_ti = s->internal;
	s->internal = -1;
	
	if (s->running->run_remain == 0){
		complete_process(s->running, s);
	}
	else{
		s->rq->putFIFO(s->running);
		s->running->state = RQ;
		s->running = NULL;
	}
	if (s->rq->head != NULL){
		s->running = s->rq->head->proc;
		s->rq->removeJob(s->rq->head->proc->num);
		if (s->running->run_remain < s->quantum){
			s->internal = s->curr_ti + s->running->run_remain;
			s->running->run_remain = 0;
		}
		else{
			s->internal = s->curr_ti + s->quantum;
			s->running->run_remain = s->running->run_remain - s->quantum;
		}
	}

	//@TODO process state ???
}
/*
bool checkSafe(System* s, int ){
	return false;
}

bool bankers(System* s, Process* p, int req){
	int avail = s->a_dev;
	int need = p->max_dev - p->curr_dev;
	if (req <= need){
		if (req <= avail){
			//int length = length of ready + running + waiting
			//allocate array of that size -- maybe even map job # to needs/allocation
			return checkSafe();
		}
		else{
			//error
		}
	}
	else{
		//error
	}
}

*/