// Samantha Kossey, Lakshmi Palaparthi
#include "project.hpp"

//main function
int main(void){
	string name;
	string line;
	int req = 0;
	//read file name from cin, example.txt
	cout << "Input file name" << endl;
	cin >> name;
	ifstream input;
	//allocate for new system
	System* sys = new System();
	sys->filename = name;
	
	//open file
	input.open(name.c_str());
	if (input.is_open()){
		cout << "Opened input file successfully." << endl;
		if (getline(input, line)){
			//first line must be system specs if not just exit immediately
			if (line.at(0) != 'C'){
				cerr << "Error: First line is not system specifications\n";
				delete sys;
				return 0;
			}
			else{
				parse(line, sys);
			}
		}
		//internal and external keep track of the next internal/external
		//event times in the system
		sys->internal = -1;
		sys->external = -1;
		//waitNew is to wait for a new external event
		bool waitNew = true;
		while (getline(input, line)){
			parse(line, sys);
			waitNew = true;
		    while (waitNew){
		        if (sys->internal == -1 && sys->external == -1){
		        	//no new events, get new external by reading the next line of input
		            waitNew = false;
		        }
		        if (sys->internal == -1 && sys->external != -1){
		        //no new internal event so handle external event
		        handleEvent(sys, true);
		        //get new external
		        waitNew = false;
		        }
		        else if (sys->internal != -1 && sys->external == -1){
		        //no external event/skipped prev external event due to error
		        //get new external event
		        waitNew = false;
		        }
		        else if (sys->internal != -1 && sys->external != -1){
		        	//if there is an external and an internal event
		            if (sys->external < sys->internal){
		            //handle external
		            handleEvent(sys, true);
		            //get new external
		            waitNew = false;
		            }
		            else{
		            //handle internal
		            handleEvent(sys, false);
		            }
		        }
		    }
		}
		//close input file
		input.close();
		cout << "Closed file successfully." << endl;
		//deallocate system
		delete sys;
	}
	else{
		cout << "Could not find input file. Try again." << endl;
	}

	return 0;
}