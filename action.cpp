// Samantha Kossey, Lakshmi Palaparthi
#include "project.hpp"
#include "linkedlist.hpp"

//@TODO add errors printed

//deletes job from vector of all jobs in the system
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

//decides how to handle the upcoming event
void handleEvent(System* s, bool ext){
	if (!ext){
		//internal event aka quantum interrupt and round robin
		round_robin(s);
		return;
	}
	//process arrival
	if (s->next == A){
		process_arrival(s->process, s);
	}
	//display
	else if(s->next == D){
		display(s);
		s->external = -1;
	}
	//device request
	else if(s->next == Q){
		request_device(s->devreq, s);
	}
	//device release
	else if(s->next == L){
		release_device(s->devreq, s);
	}
}

//displays current state in std out and in json file named by input file
//and the curr time
void display(System* s){
	//double checks timing
	if (s->curr_ti > s->external){
		return;
	}
	s->curr_ti = s->external;
	s->external = -1;
	
	ofstream myfile;
	string filename;
	filename = s->filename + "_"+ ".json";
	myfile.open (filename.c_str());
	
	//ready queue
	myfile << "{\"readyq\": [";
	Node* r = s->rq->head;
	string rq = print_queue(r);
	myfile << rq;
	
	//current time
	myfile << "], \"current_time\": ";
	myfile << s->curr_ti;
	myfile << ", ";
	
	//total memory
	myfile << "\"total_memory\": ";
	myfile << s->tot_mem;
	myfile << ", ";
	
	//avail memory
	myfile << "\"available_memory\": ";
	myfile << s->a_mem;
	myfile << ", ";
	
	//total devices
	myfile << "\"total_devices\": ";
	myfile << s->tot_dev;
	myfile << ", ";
	
	//turnaround and weighted turnaround if applicable
	if (s->curr_ti == 9999){
		string turnt = turnaround_sys(s);
		myfile << turnt;
	}
	
	//running
	if (s->running != NULL){
		myfile << "\"running\": ";
		myfile << s->running->num;
		myfile << ", ";
	}
	
	//submit queue
	myfile << "\"submitq\": [], ";
	
	//hold queue 2
	myfile << "\"holdq2\": [";
	Node* h2 = s->hq2->head;
	string hq2 = print_queue(h2);
	myfile << hq2;
	
	//jobs
	myfile << "], \"job\": [";
	string jobs = print_job(s);
	myfile << jobs;
	
	//hold queue 1
	Node* h1 = s->hq1->head;
	string hq1 = print_queue(h1);
	myfile << "\"holdq1\": [";
	myfile << hq1;
	
	//available devices
	myfile << "], \"available_devices\": ";
	myfile << s->a_dev;
	
	//quantum
	myfile << ", \"quantum\": ";
	myfile << s->quantum;
	
	//complete queue
	myfile << ", \"completeq\": [";
	Node* cq = s->complete->head;
	string comp_q = print_queue(cq);
	myfile << comp_q + "], ";
	
	//wait queue
	myfile << ", \"waitq\": [";
	Node* wq = s->dq->head;
	string wait_q = print_queue(wq);
	myfile << wait_q + "]}";
	myfile.close();
}

string turnaround_sys(System* s){
	string result = "";
	int tot_turn = 0;
	double overall_turn = 0;
	double weighted = 0;
	string w = "";
	for (int i = 0; i < s->jobs.size(); i++){
		tot_turn = (s->jobs[i]->compl_ti - s->jobs[i]->arrival);
		overall_turn = overall_turn + tot_turn;
		if (s->jobs[i]->tot_run != 0){
		weighted = weighted + (tot_turn / s->jobs[i]->tot_run);
		}
		else{
			w = " \"weighted_turnaround\": Error, ";
		}
	}
	stringstream tt;
	stringstream wt;
	if (s->jobs.size() != 0){
		weighted = weighted / s->jobs.size();
		overall_turn = overall_turn / s->jobs.size();
		if (w == ""){
			wt >> weighted;
			w = " \"weighted_turnaround\": " + wt.str() + ", ";
		}
		tt >> overall_turn;
		result = "\"turnaround\": " + tt.str() + ", " + w;
	}
	else{
		result = "\"turnaround\": Error, \"weighted_turnaround\": Error, ";
	}
	return result;
}

//print jobs
string print_job(System* s){
	string str = "";
	for (int i = 0; i < s->jobs.size(); i++){
		//arrival
		stringstream arr_ti;
		arr_ti << s->jobs[i]->arrival;
		str = str + "{\"arrival_time\": " + arr_ti.str() + ", ";
		//job id
		str = str + "\"id\": ";
		stringstream id;
		id << s->jobs[i]->num;
		//devices allocated
		if (s->jobs[i]->state == Wait || s->jobs[i]->state == Running 
		|| s->jobs[i]->state == RQ){
		str = str + ", \"devices_allocated\": ";
		stringstream deva;
		deva << s->jobs[i]->curr_dev;
		str = str + deva.str();
		}
		//remaining run time
		str = str + id.str() + ", \"remaining_time\": ";
		stringstream rem;
		if (s->running == s->jobs[i]){
			int remaining_run = s->jobs[i]->run_remain + (s->jobs[i]->endQuant - s->curr_ti);
			rem << remaining_run;
		}
		else{
		rem << s->jobs[i]->run_remain;
		}
		str = str + rem.str();
		//complete time if applicable
		if (s->jobs[i]->state == Complete){
			str = str + ", \"completion_time\": ";
			stringstream comp;
			comp << s->jobs[i]->compl_ti;
			str = str + comp.str();
			str = str + ", \"turnaround\": ";
			stringstream tt;
			int turntime = s->jobs[i]->compl_ti - s->jobs[i]->arrival;
			tt << turntime;
			str = str + tt.str();
			str = str + ", \"weighted_turnaround\": ";
			double weighted = turntime/s->jobs[i]->tot_run;
			stringstream wt;
			wt << weighted;
			str = str + wt.str();
		}
		
		str = str + "}";
		if (i+1 != s->jobs.size()){
			str = str + ", ";
		}
	}
	str = str + "], ";
	return str;
}

//prints job numbers from queue
string print_queue(Node* n){
	string result = "";
	if (n != NULL){
		stringstream pn;
		pn << n->proc->num;
		result = result + pn.str();
		n = n->next;
	}
	while (n != NULL){
		result = result + ", ";
		stringstream pn2;
		pn2 << n->proc->num;
		result = result + pn2.str();
		n = n->next;
	}
	return result;
}

//process arrival, p is process to add
void process_arrival(Process* p, System* s){
	//if no process is available, error skip this event
	if (p == NULL){
		s->external = -1;
		return;
	}
	//double check timing
	if (s->curr_ti > s->external){
		delete s->process;
		s->process = NULL;
		s->external = -1;
		return;
	}
	//set the time to the time of this event
	s->curr_ti = s->external;
	//to get new external event
	s->external = -1;
	if (p->memory <= s->tot_mem && p->max_dev <= s->tot_dev){
		//enough memory/devices in total to keep the job
		//add job to master vector for the system
		s->jobs.push_back(p);
		
		//check if there is enough memory currently
		if (p->memory <= s->a_mem){
			s->a_mem = s->a_mem - p->memory;
			//add process to ready queue/starting running if no one else is
			addToReady(p, s);
				
			s->process = NULL;	
		}
		else{
			//add to SJF
			if (p->priority == 1){
				//add to SJF
				s->hq1->putSJF(p);
				p->state = HQ1;
			}
			//add to FIFO
			else if (p->priority == 2){
				//add to FIFO
				s->hq2->putFIFO(p);
				p->state = HQ2;
			}
		s->process = NULL;
		}
	}
	//doesn't allow process to arrive if too big or too many devices needed
	else{
		delete s->process;
		s->process = NULL;
	}
}

//check the device wait queue to see if any can join the ready queue
void checkWaitQ(System* s){
	Node* temp = s->dq->head;
	while (temp != NULL){
		//wait queue is shortest request first so once the requests get
		//to big you can stop
		if (temp->proc->waiting > s->a_dev){
			break;
		}
		else{
			bool accept = false;
			
			//pretend to allocate
			temp->proc->curr_dev = temp->proc->curr_dev + temp->proc->waiting;
			temp->proc->remain_dev = temp->proc->remain_dev - temp->proc->waiting;
			s->a_dev = s->a_dev - temp->proc->waiting;
			//see if bankers accepts it as safe
			accept = bankers(s,NULL);
			//if safe
			if (accept){
				temp->proc->waiting = 0;
				Node* newn = temp->next;
				//put on the ready queue
				toReadyFromComplete(temp->proc, s);
				//remove from the device queue
				s->dq->removeJob(temp->proc->num);
				temp = newn;
			}
			else{
				//if not safe put the devices allocated back
				temp->proc->curr_dev = temp->proc->curr_dev - temp->proc->waiting;
				temp->proc->remain_dev = temp->proc->remain_dev + temp->proc->waiting;
				s->a_dev = s->a_dev + temp->proc->waiting;
				temp = temp->next;
			}
		}
	}
}

//request a device, d is the data for the device request
void request_device(Dev d, System* s){
	//double check timing
	if (s->curr_ti > s->external){
		s->external = -1;
		return;
	}
	//make current time the event time
	s->curr_ti = s->external;
	s->external = -1;
	
	Process* p = s->running;
	//make sure the process is running
	if (p != NULL && d.job_num == p->num){
		int avail = s->a_dev;
		int alloc = d.num;
		//make sure request is less than tot number of devices
		//should have already been filtered out by parseQL
		if (d.num > s->tot_dev){
			return;
		}
		
		//stops the current running process, updates internal time
		interruptQuant(s);
		
		//check if there are enough available devices for the request
		if (d.num > s->a_dev){
			//add to dev queue
			p->waiting = d.num;
			p->state = Wait;
			s->dq->putDQ(p);
			//let new process run
			round_robin(s);
			return;
		}
		else{
		//If banker's accepts as state you can allocate
		bool accept = false;
		
		//pretend to allocate
		p->curr_dev = p->curr_dev + d.num;
		p->remain_dev = p->remain_dev - d.num;
		s->a_dev = s->a_dev - d.num;
		accept = bankers(s, p);
		if (accept){
			//add to ready queue and accept request
			//let next process in RQ run
			addToReady(p, s);
			return;
		}
		else{
			//if bankers decided unsafe deallocate
			//and add to device queue
			//let next process in RQ run
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
		//is skipped because the process wasn't running
	}
}

//stops the current running process, updates the internal time, updates the 
//remaining run time for the interrupted process
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

//releases devices
void release_device(Dev d, System* s){
	//double check timing
	if (s->curr_ti > s->external){
		s->external = -1;
		return;
	}
	//update current time to event time
	s->curr_ti = s->external;
	s->external = -1;
	
	Process* p = s->running;
	//make sure the correct process is running and that 
	//it doesn't try to release more devices than it has
	if (p != NULL && (p->num == d.job_num)&& (d.num <= p->curr_dev)){
			p->curr_dev = p->curr_dev - d.num;
			s->a_dev = s->a_dev + d.num;
			//stop the current process from running
			interruptQuant(s);
			p->state = RQ;
			
			//add the process back to the RQ
			toReadyFromComplete(p,s);
			//see if any jobs in the device wait queue can 
			//safely rejoin the ready queue now that devices have been released
			checkWaitQ(s);
			//let the next process in the RQ run
			round_robin(s);
	}
	else{
		//skipped because process wasn't running or wanted it to release more
		//devices than it had allocated
	}
}

//see if there is enough memory to add any of the processes from the hold queues
//check 1 and then 2
void addFromHold(System* s){
	Node* temp1 = s->hq1->head;
	Node* temp2 = s->hq2->head;
	while (temp1 != NULL && s->a_mem != 0){
		if (temp1->proc->memory <= s->a_mem){
			s->a_mem = s->a_mem - temp1->proc->memory;
			//if there's enough memory add it to the ready queue
			toReadyFromComplete(temp1->proc, s);
			Node* newn = temp1->next;
			//remove it from the hold queue 1
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
			//if there's enough memory add it to the ready queue
			toReadyFromComplete(temp2->proc, s);
			Node* newn = temp2->next;
			//remove it from the hold queue 2
			s->hq2->removeJob(temp2->proc->num);
			temp2 = newn;
		}
		else{
		temp2 = temp2->next;
		}
	}
	
}

//adds process p to ready queue, and calls round robin
//if RQ is empty, skips straight to running
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

//adds process p to the ready queue
void toReadyFromComplete(Process* p, System* s){
		s->rq->putFIFO(p);
		p->state = RQ;
}

//@TODO not done
//when process completes, add to complete queue,
//release devices/memory, check the device wait queue,
//check the hold queue 1 then 2
void complete_process(Process* p, System* s){
	p->state = Complete;
	s->running = NULL;
	s->complete->putFIFO(p);
	s->a_mem = s->a_mem + p->memory;
	s->a_dev = s->a_dev + p->curr_dev;
	p->curr_dev = 0;
	p->compl_ti = s->curr_ti;
	checkWaitQ(s);
	addFromHold(s);
}

void round_robin(System* s){
	//double check timing
	if (s->curr_ti > s->internal){
		return;
	}
	
	//update the current time to the event time
	s->curr_ti = s->internal;
	s->internal = -1;
	
	
	if (s->running == NULL){
		
	}
	//if someone is currently running, complete it
	//or put it back in the ready queue
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
	
	//check for new job to run off ready queue
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

}

//check if the current state post allocation is safe
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

//creates a list for the safety check to understand
//process p is the running process if one exists
bool bankers(System* s, Process *p){
	int onebit = 1;
	if (p == NULL){
		onebit = 0;
	}
	//safety check using all processes in memory
	int length = s->rq->count + s->dq->count + onebit;
	Banker bank[length];
	int index = 0;
	
	//add all the processes in memory
	//RQ, DQ, and the current running process
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
	if (p != NULL){
	bank[index].finished = false;
	bank[index].need = p->remain_dev;
	bank[index].num = p->num;
	bank[index].alloc = p->curr_dev;
	}
	int avail = s->a_dev;
	
	//check the safety of the allocation made before bankers was called
	bool safe = checkSafe(bank, avail, length);
	return safe;

}

