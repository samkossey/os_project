#include "project.hpp"
#include "linkedlist.hpp"

void deleteJobSys(System* s, Process* p){
	vector<Process*> j = s->jobs;
	for (int i = 0; i < j.size(); i++){
		if (j[i] == p){
			j.erase(j.begin() + i);
			break;
		}
	}
	delete p;
	p = NULL;
}

void handleEvent(System* s){
	if (s->next == A){
		if (s->process == NULL){
			cout << "error skip this arrival" << endl;
		}
		else{
			cout << "handling it" << endl;
			process_arrival(s->process, s);
		}
	}
	else{
		cout << "different external event" << endl;
	}
}

void process_arrival(Process* p, System* s){
	if (p->memory <= s->tot_mem && p->max_dev <= s->tot_dev){
		if (p->memory <= s->a_mem){
			//@TODO add process to ready queue
			s->rq->putFIFO(p);
			s->a_mem = s->a_mem - p->memory;
			p->state = RQ;
		}
		else{
			//@TODO add to SJF or FIFO based on priority
			if (p->priority == 1){
				//@TODO add to SJF
				s->hq1->putSJF(p);
				p->state = HQ1;
			}
			else if (p->priority == 2){
				//@TODO add to FIFO
				s->hq2->putFIFO(p);
				p->state = HQ2;
			}
		}
	s->process = NULL;
	}
	else{
		s->process = NULL;
		deleteJobSys(s, p);
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

void complete_process(Process* p, System* s){
	//TODO release_device()
	p->state = Complete;
	//@TODO remove from running
	//@TODO add to complete list
	s->a_mem = s->a_mem + p->memory;
	//@TODO check for processes in HQ1 then HQ2


}

void round_robin(System* s){
	//@TODO if running process has time remaining 0, call complete_process

	//@TODO back to ready queue if not time remaining 0

	//@TODO get new from ready queue, should this be a separate function?

	//@TODO process state 
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