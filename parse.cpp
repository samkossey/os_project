// Samantha Kossey, Lakshmi Palaparthi
#include "project.hpp"

//for each new line, decides which parsing function is needed
int parse(string line, System* s){
	char first = line.at(0);
	//system config
	if (first == 'C'){
		return parseC(line, s);
	}
	//process arrival
	else if (first == 'A'){
		return parseA(line, s);
	}
	//device request or release
	else if (first == 'Q' || first == 'L'){
		return parseQL(line, s);
	}
	//display
	else if (first == 'D'){
		return parseD(line, s);
	}
	else{
		cout << "Error: Could not find a handle for the event." << endl;
		return -1;
	}
}

//system configuration parsing
int parseC(string line, System* s){
	int size = line.length();
	int i = 0;
	while(i < size){
		if (line.at(i)=='C'){
			i = i+2;
			string ti = "";
			while (i<size && line.at(i) != ' '){
				ti = ti + line.at(i);
				i++;
			}
			s->arrival = atoi(ti.c_str());
			s->curr_ti = atoi(ti.c_str());
			s->lastTime = atoi(ti.c_str());
			i++;
		}
		else if (line.at(i) == 'M'){
			i = i+2;
			string mem = "";
			while (i<size && line.at(i) != ' '){
				mem = mem + line.at(i);
				i++;
			}
			s->tot_mem = atoi(mem.c_str());
			s->a_mem = atoi(mem.c_str());
			i++;
		}
		else if (line.at(i) == 'S'){
			i = i+2;
			string dev = "";
			while (i<size && line.at(i) != ' '){
				dev = dev + line.at(i);
				i++;
			}
			s->tot_dev = atoi(dev.c_str());
			s->a_dev = atoi(dev.c_str());
			i++;
		}
		else if (line.at(i) == 'Q'){
			i = i+2;
			string qu = "";
			while (i<size && line.at(i) != ' '){
				qu = qu + line.at(i);
				i++;
			}
			s->quantum = atoi(qu.c_str());
			i++;
		}

	}
	s->next = N;
	return -1;
}

//display parsing
int parseD(string line, System* s){
	int i = 2;
	int size = line.size();
	string ti = "";
	while (i<size && line.at(i) != ' '){
		ti = ti + line.at(i);
		i++;
	}
	//ensures input times are in order, any times out of order are skipped
	if (atoi(ti.c_str()) < s->lastTime){
		cout << "Error: Skipped event because was not in chronological order in input." << endl;
		return -1;
	}
	s->next = D;
	s->external = atoi(ti.c_str());
	s->lastTime = s->external;
	return s->external;

}

//process arrival parsing
int parseA(string line, System* s){
	Process* pr = new Process();
	pr->waiting = 0;
	int i = 0;
	int size = line.size();
	while(i < size){
		if (line.at(i) == 'A'){
			i = i+2;
			string ti = "";
			while (i<size && line.at(i) != ' '){
				ti = ti + line.at(i);
				i++;
			}
			pr->arrival = atoi(ti.c_str());
			//ensures input times are in order, any times out of order are skipped
			if (pr->arrival < s->lastTime){
				cout << "Error: Skipped event because was not in chronological order in input." << endl;
				delete pr;
				return -1;
			}
			s->external = pr->arrival;
			i++;
		}
		else if (line.at(i) == 'J'){
			i = i+2;
			string n = "";
			while (i<size && line.at(i) != ' '){
				n = n + line.at(i);
				i++;
			}
			pr->num = atoi(n.c_str());
			i++;
		}
		else if (line.at(i) == 'M'){
			i = i+2;
			string m = "";
			while (i<size && line.at(i) != ' '){
				m = m + line.at(i);
				i++;
			}
			pr->memory = atoi(m.c_str());
			i++;
		}
		else if (line.at(i) == 'S'){
			i = i+2;
			string d = "";
			while (i<size && line.at(i) != ' '){
				d = d + line.at(i);
				i++;
			}
			pr->max_dev = atoi(d.c_str());
			pr->remain_dev = pr->max_dev;
			pr->waiting = 0;
			pr->compl_ti = -1;
			pr->curr_dev = 0;
			i++;
		}
		else if (line.at(i) == 'R'){
			i = i+2;
			string r = "";
			while (i<size && line.at(i) != ' '){
				r = r + line.at(i);
				i++;
			}
			pr->tot_run = atoi(r.c_str());
			pr->run_remain = atoi(r.c_str());
			i++;
		}
		else if (line.at(i) == 'P'){
			i = i+2;
			string p = "";
			while (i<size && line.at(i) != ' '){
				p = p + line.at(i);
				i++;
			}
			pr->priority = atoi(p.c_str());
			i++;
		}

	}
	s->process = pr;
	s->next = A;
	s->lastTime = pr->arrival;
	return pr->arrival;
}

//parsing for request/release of devices
int parseQL(string line, System* s){
	Dev dev;
	int i = 0;
	int size = line.length();
	while (i < size){
		if (line.at(i) == 'L'){
			dev.action = 1;
			i = i+2;
			string ti = "";
			while(i < size && line.at(i) != ' '){
				ti = ti + line.at(i);
				i++;
			}
			dev.ti = atoi(ti.c_str());
			//ensures input times are in order, any times out of order are skipped
			if (dev.ti < s->lastTime){
				cout << "Error: Skipped event because was not in chronological order in input." << endl;
				return -1;
			}
			i++;
			s->next = L;
		}
		else if (line.at(i) == 'Q'){
			dev.action = 0;
			i = i + 2;
			string ti = "";
			while(i < size && line.at(i) != ' '){
				ti = ti + line.at(i);
				i++;
			}
			dev.ti = atoi(ti.c_str());
			//ensures input times are in order, any times out of order are skipped
			if (dev.ti < s->lastTime){
				cout << "Error: Skipped event because was not in chronological order in input." << endl;
				return -1;
			}
			i++;
			s->next = Q;
		}
		else if (line.at(i) == 'J'){
			i = i+2;
			string n = "";
			while (i<size && line.at(i) != ' '){
				n = n + line.at(i);
				i++;
			}
			dev.job_num = atoi(n.c_str());
			i++;
		}
		else if (line.at(i) == 'D'){
			i = i+2;
			string d = "";
			while (i<size && line.at(i) != ' '){
				d = d + line.at(i);
				i++;
			}
			dev.num = atoi(d.c_str());
			//if requesting too many devices skip request
			if (dev.num > s->tot_dev){
				cout << "Error: Skipped event because too many devices requested/released." << cout;
				return -1;
			}
			i++;
		}
	}
	s->external = dev.ti;
	s->lastTime = dev.ti;
	s->devreq = dev;
	return dev.ti;
}
