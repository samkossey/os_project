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
	else if(s->next == D){
		//display(s);
		cout << "will handle display" << endl;
	}
	else if(s->next == Q){
		request_device(s->devreq, s);
		cout << "handle dev req" << endl;
	}
	else{
		cout << "different external event" << endl;
	}
}

void display(System* s){
	if (s->curr_ti > s->external){
		cout << "bad timing" << endl;
		return;
	}
	s->curr_ti = s->external;
	s->external = -1;
	
	ofstream myfile;
	string filename;
	filename = s->filename + "_"+ to_string(s->curr_ti) + ".json";
	myfile.open (filename);
	myfile << "Writing this to a file.\n";
	myfile.close();
	
	
}

void process_arrival(Process* p, System* s){
	if (p == NULL){
		s->external = -1;
		return;
	}
	if (s->curr_ti > s->external){
		cout << "bad timing" << endl;
		delete s->process;
		s->process = NULL;
		s->external = -1;
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
		delete s->process;
		s->process = NULL;
	}
}

void request_device(Dev d, System* s){
	if (s->curr_ti > s->external){
		cout << "bad timing" << endl;
		s->process = NULL;
		s->external = -1;
		return;
	}
	s->curr_ti = s->external;
	s->external = -1;
	Process* p = s->running;
	if (p != NULL && d.num == p->num){
		//@TODO fake allocate
		int avail = s->a_dev;
		int alloc = d.num;
		if (d.num > s->tot_dev){
			cout << "error requested too many devices" << endl;
			//@TODO interrupt quantum
			interruptQuant(s);
			p->state = RQ;
			addToReady(p,s);
			return;
		}
		interruptQuant(s);
		if (d.num > s->a_dev){
			//@TODO add to dev queue
			p->waiting = d.num;
			p->state = Wait;
			s->dq->putDQ(p);
			round_robin(s);
			return;
		}
		else{
		//@TODO If banker's accepts as state you can allocate
		bool accept = false;
		p->curr_dev = p->curr_dev + d.num;
		p->remain_dev = p->remain_dev - d.num;
		s->a_dev = s->a_dev - d.num;
		accept = bankers(s, p);
		if (accept){
			//@TODO someone else needs to start if no one is running
			addToReady(p, s);
			return;
		}
		else{
			p->curr_dev = p->curr_dev - d.num;
			p->remain_dev = p->remain_dev + d.num;
			s->a_dev = s->a_dev + d.num;
			
			p->waiting = d.num;
			p->state = Wait;
			s->dq->putDQ(p);
			round_robin(s);
			return;
		}
		}
	}
	else{
		//@TODO some kind of error
		cout << "Couldn't request because wasn't running" << endl;
	}
}

void interruptQuant(System* s){
	if (s->running == NULL){
		return;
	}
	else{
		s->running->run_remain = s->running->run_remain + (s->running->endQuant - s->curr_ti);
		s->running->endQuant = s->curr_ti;
		s->running = NULL;
	}
	s->internal = s->curr_ti;
}

void release_device(Dev d, System* s, Process* p){
	if (p->state == Running){
			p->curr_dev = p->curr_dev - d.num;
			s->a_dev = s->a_dev + d.num;
			addToReady(p, s);
			//@TODO added back to ready queue before or after other waiting processes
			//@TODO check device queue for safe waiting processes
			//@TODO poss add them to the ready queue
	}
	else{
		//@TODO some kind of error
		cout << "Couldn't release because wasn't running" << endl;
	}
}

//@TODO 
void addFromHold(System* s){
	Node* temp1 = s->hq1->head;
	Node* temp2 = s->hq2->head;
	while (temp1 != NULL && s->a_mem != 0){
		if (temp1->proc->memory <= s->a_mem){
			s->a_mem = s->a_mem - temp1->proc->memory;
			toReadyFromComplete(temp1->proc, s);
			Node* newn = temp1->next;
			//@TODO does remove job delete????
			s->hq1->removeJob(temp1->proc->num);
			temp1 = newn;
		}
		else{
		temp1 = temp1->next;
		}
	}
	while (temp2 != NULL && s->a_mem != 0){
		if (temp2->proc->memory <= s->a_mem){
			s->a_mem = s->a_mem - temp2->proc->memory;
			toReadyFromComplete(temp2->proc, s);
			Node* newn = temp2->next;
			s->hq2->removeJob(temp2->proc->num);
			temp2 = newn;
		}
		else{
		temp2 = temp2->next;
		}
	}
	
}

void addToReady(Process *p, System* s){
	if (s->rq->count == 0 && s->running == NULL){
		s->running = p;
		p->state = Running;
		if (p->run_remain < s->quantum){
				s->internal = s->curr_ti + p->run_remain;
				p->run_remain = 0;
				p->endQuant = s->internal;
			}
		else{
				s->internal = s->curr_ti + s->quantum;
				p->run_remain = p->run_remain - s->quantum;
				p->endQuant = s->internal;
				
			}
	}
	else{
		s->rq->putFIFO(p);
		p->state = RQ;
		if (s->running == NULL){
			s->internal = s->curr_ti;
			round_robin(s);
		}
	}
}

void toReadyFromComplete(Process* p, System* s){
	if (s->rq->count == 0 && s->running == NULL){
		s->running = p;
		p->state = Running;
		if (p->run_remain < s->quantum){
				s->internal = s->curr_ti + p->run_remain;
				p->run_remain = 0;
				p->endQuant = s->internal;
			}
		else{
				s->internal = s->curr_ti + s->quantum;
				p->run_remain = p->run_remain - s->quantum;
				p->endQuant = s->internal;
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
	p->compl_ti = s->curr_ti;
	//@TODO add from device queue first
	addFromHold(s);


}

void round_robin(System* s){
	if (s->curr_ti > s->internal){
		cout << "bad timing round robin" << endl;
		return;
	}
	s->curr_ti = s->internal;
	s->internal = -1;
	
	if (s->running == NULL){
		
	}
	else{
	if (s->running->run_remain == 0){
		complete_process(s->running, s);
	}
	else{
		s->rq->putFIFO(s->running);
		s->running->state = RQ;
		s->running = NULL;
	}
	}
	if (s->rq->head != NULL){
		s->running = s->rq->head->proc;
		s->rq->removeJob(s->rq->head->proc->num);
		s->running->state = Running;
		if (s->running->run_remain < s->quantum){
			s->internal = s->curr_ti + s->running->run_remain;
			s->running->endQuant = s->internal;
			s->running->run_remain = 0;
		}
		else{
			s->internal = s->curr_ti + s->quantum;
			s->running->endQuant = s->internal;
			s->running->run_remain = s->running->run_remain - s->quantum;
		}
	}
	else{
		s->internal = -1;
	}

	//@TODO process state ???
}

bool checkSafe(Banker* b, int avail, int length){
	bool alldone = false;
	while (!alldone){
		alldone = true;
	for (int i = 0; i < length; i++){
		if (!(b[i].finished)){
			if (b[i].need <= avail){
				alldone = false;
				avail = avail + b[i].alloc;
				b[i].finished = true;
			}
		}
	}
	}
	for (int i = 0; i < length; i++){
		if (!(b[i].finished)){
			return false;
		}
	}
	return true;
}

bool bankers(System* s, Process *p){
	int length = s->rq->count + s->dq->count + 1;
	Banker* bank = new Banker[length];
	int index = 0;
	Node* rq = s->rq->head;
	while (rq != NULL){
		bank[index].finished = false;
		bank[index].need = rq->proc->remain_dev;
		bank[index].num = rq->proc->num;
		bank[index].alloc = rq->proc->curr_dev;
		rq = rq->next;
		index ++;
	}
	Node* dq = s->dq->head;
	while (dq != NULL){
		bank[index].finished = false;
		bank[index].need = dq->proc->remain_dev;
		bank[index].num = dq->proc->num;
		bank[index].alloc = dq->proc->curr_dev;
		dq = dq->next;
		index ++;
	}
	bank[index].finished = false;
	bank[index].need = p->remain_dev;
	bank[index].num = p->num;
	bank[index].alloc = p->curr_dev;
	
	int avail = s->a_dev;
	
	bool safe = checkSafe(bank, avail, length);
	delete [] bank;
	return safe;

}

