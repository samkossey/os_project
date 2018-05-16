#include "project.hpp"

int main(void){
	string name;
	string line;
	int req = 0;
	//read file name from cin
	cout << "Input file name" << endl;
	cin >> name;
	ifstream input;
	System* sys = new System();

	//open file
	input.open(name.c_str());
	if (input.is_open()){
		if (getline(input, line)){
			if (line.at(0) != 'C'){
				cerr << "Error: First line is not system specifications\n";
				delete sys;
				return 0;
			}
			else{
				parse(line, sys);
				
				cout << sys->tot_mem << endl;
			}
		}
		sys->internal = -1;
		sys->external = -1;
		bool waitNew = true;
		while (getline(input, line)){
		    while (waitNew){
		        if (sys->internal == -1 && sys->external == -1){
		            waitNew = false;
		        }
		        if (sys->internal == -1 && sys->external != -1){
		        //handle external event
		        handleEvent(sys);
		        waitNew = false;
		        }
		        else if (sys->internal != -1 && sys->external == -1){
		        //handle internal event
		        }
		        else if (sys->internal != -1 && sys->external != -1){
		            if (sys->external < sys->internal){
		            //handle external
		            waitNew = false;
		            }
		            else{
		            //handle internal
		            }
		        }
		    }
			cout << parse(line, sys) << endl;
			waitNew = true;
		}

		input.close();
		delete sys;
	}






	return 0;
}