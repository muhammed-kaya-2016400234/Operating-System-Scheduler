#include <iostream>


#include <fstream>
#include <sstream>
#include <iterator>
#include "process.h"
#include <map>
#include <algorithm>
using namespace std;

vector<vector<string>> loadprocesses(string codefile);
vector<process> definition(string deffile);
string arrangeoutput(vector<process> a,int time);



int main() {



    vector<process> procvec=definition("definition.txt");   //procvec stores all processes in the order they arrive(using
                                                            //"definition" function which parses "definition.txt" )


    vector<string> output;        //"output" stores the status of the ready queue in specific times(when entry or exit happens)
                                    //for example:   150:HEAD-P1[3]-P2[1]-TAIL

    int procsize=procvec.size();   //total number of processes we will consider



    map<string,int> progendtime;  //stores finishing times of processes by their name(to calculate
                                  // turnaround and waiting times)



    output.push_back("0:HEAD--TAIL");  //this is the first element of "output".It shows the situation when
                                       // time is zero and queue is empty


    ofstream outfile;
    outfile.open("output.txt");         //opens the file where the output will be written



    vector<process> ready;         //this is the ready queue
    int begin=procvec[0].arrtime;   //"begin" is the instant time.It will be changed when specific events(like an
    // execution of an instruction  ) occur

    int end;                       //end will store the arrival time of the next element to be considered




    //This is the main loop.At each ieration,(1)we will execute instructions of processes in ready queue until a new
    //process arrives.Execution means that we will increase the instant time (begin) by execution times of atomic
    //instructions.
    // (2)When "begin" becomes higher than the arrival time of the next process("int end"),we have to stop the
    // current process.
    // (3)Then we have to push this new process to ready queue  and continue with instructions at the next iteration
    //of this "for loop".
    for(int i=0;i<procsize;i++){





        ready.push_back(procvec[i]);        //push the new process to ready queue
        //if "begin" becomes higher than arrival times of multiple new processes,we should push them all
        //to ready queue at the same moment.This is provided by this while loop
        while((i<procsize-1)&&procvec[i+1].arrtime<=begin){
            ready.push_back(procvec[i+1]);

            i++;


        }

        //sort the ready queue again(with the new processes) according to priorities.
        //stable sorting provides first come first serve between processes
        //with equal priorities
        stable_sort(ready.begin(),ready.end());

        //Since new processes are pushed to ready queue,we have to output the new status of the ready queue
        //"arrangeoutput" function puts the elements of ready queue into this form : 150:HEAD-P1[3]-P2[1]-TAIL
        output.push_back(arrangeoutput(ready,begin));


        bool lastiteration=false;  //"lastiteration=true" implies that no new proceses will be added to ready queue

        if(i!=procsize-1) {
            end = procvec[i + 1].arrtime;       //initialize "end" with the arrival time of the next arriving process
                                                //if there is a next one(if i!=procsize-1)

        }else{
            lastiteration=true;

        }



        bool quit=true;         //below while loop will execute the instructions until it is time for a new process
                                //to arrive."quit=false" implies that a new process should be pushed to queue and
                                //current instructions must be stopped(we should exit while loop)



        //At each iteration of while loop we will take first process of the ready queue and execute its instructions
        //as long as "begin" is less than "end".If all instructions of this process could be executed in this time
        //interval,we will pop this process from queue and continue with the instructions of next process in the queue
        while(quit){

            process * temp=&ready[0];       //take first process of reay queue



            bool pop=false;   //"pop=true" implies we will pop the first element if all instructions of process are done


            //this loop iterates over the instructions of the currently executing process(beginning with the first
            // not executed instruction of the process)
            //inst is an attribute of processes.It is a 2-dimensional array where every row is an instruction.First
            //column is the instruction name and second column is the execution time of instruction.
            for(vector<vector<string> >::iterator iter=temp->inst.begin()+temp->nextinst-1;iter!=temp->inst.end()&&quit;iter++){


                begin+=stoi(iter->at(1));  //increase the instant time "begin" by the execution time of the execued
                                            //instruction(used stoi since executin time is stored as string in "inst")

                temp->nextinst++;           //"nextinst" is an attribute of processes.It shows which instruction of
                                            //this process should be executed next


                if(iter==temp->inst.end()-1){    //if the currently considered instruction is the last instr. of process
                                                 //we should pop this process from queue (pop=true)
                    pop=true;

                    progendtime[temp->prname]=begin;   //store end time of process to calculate turnaround later


                }


                if(begin>=end&&!lastiteration){   //if begin>=end (if it is time for a new process to enter ready queue)
                                                  //stop the current process(exit while loop by assigning "quit=false").
                    //Note that:if new process does not become the first element of the
                    //queue,current process will not actually stop.It will just seem like it did.
                    //Also,if this is the last iteration(if there will be no new processes arriving)
                    //we should not quit until ready queue is empty
                    quit=false;



                }


            }

            if(pop) {       //If the process is completed(decided above)

                ready.erase(ready.begin());         //pop the first element of queue

                if(begin<end||lastiteration){       //since an element is popped we will show it in output.
                                                    //If statement makes sure that we wont print two outputs
                                                    //if at the same time a new process enters queue.

                    output.push_back(arrangeoutput(ready,begin));
                }

            }


            //If ready queue empty there are 2 options.
            //1-It will stay empty for some time
            //2-A new process will immediately enter the queue

            if(ready.empty()){

                quit=false;  //if queue is empty,there are no instructions to execute.So we quit while loop.

                if(begin<end) {         //option 1
                    begin = end;  //Since we increase instant time "begin" according to execution times of instructions
                                //and there is a "no instruction" time interval in option 1 ,we should assign the value
                                  //of the arrival time of next process("end") to "begin".Otherwise, it would be as if
                                // the new process is immediately added to ready queue.

                }

            }


        }




    }



    //this loop writes the elements of output vector to output file.For example:
    //0:HEAD--TAIL
    //20:HEAD-P1[1]-TAIL
    //150:HEAD-P1[3]-P2[1]-TAIL
    for(int i=0;i<output.size();i++){

        outfile<<output[i]<<endl;
    }
    outfile<<endl;



    //this for loop computes sum of instruction times for every process and stores it in their "totinsttime" attribute.
    //We use this value to calculate waiting time.(Waiting time=turnaround-totinsttime)
    for(vector<process>::iterator it=procvec.begin();it!=procvec.end();++it){
        for(vector<vector<string> >::iterator iter=it->inst.begin();iter!=it->inst.end();++iter){


            it->totinsttime+=stoi(iter->at(1));


        }



        int turnaround=progendtime[it->prname]-it->arrtime;
        int waiting=turnaround-it->totinsttime;

        //print the turnaround and waiting times to output file
        outfile<<"Turnaround time for "+it->prname+" = "+ to_string(turnaround)+" ms"<<endl;
        outfile<<"Waiting time for "+it->prname+" = "+to_string(waiting)<<endl;




    }






    outfile.close();








    return 0;


}


//this function parses a code file and returns a vector containing instruction information of the code
vector<vector<string>> loadprocesses(string codefile){
    process a;
    string line;
    ifstream inpfile;
    inpfile.open(codefile);
    vector <vector<string>> instructions;
    while(getline(inpfile,line)){
        vector<string> temp;
        istringstream iss(line);
        for(string a; iss >> a; )
            temp.push_back(a);


        instructions.push_back(temp);

    }


    return instructions;


}


//this function parses the definition file and returns a vector storing all the processes(with their attributes assigned)
vector<process> definition(string deffile){
    vector<process> processes;
    string line;
    ifstream inpfile;
    inpfile.open(deffile);


    while(getline(inpfile,line)){


        process a;
        vector<string> tokens;
        istringstream iss(line);
        copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(tokens));

        a.prname=tokens[0];
        a.priority=stoi(tokens[1]);
        a.arrtime=stoi(tokens[3]);
        a.inst=loadprocesses("./"+tokens[2]+".txt");

        processes.push_back(a);


    }

    return processes;
}

//this function puts the instant status of the ready queue in output format
//for example:    150:HEAD-P1[3]-P2[1]-TAIL
string arrangeoutput(vector<process> a,int time){
    string ret;
    ret=to_string(time);
    ret+=":HEAD-";
    if(!a.empty()) {
        for (vector<process>::iterator it = a.begin(); it != a.end(); ++it) {
            string k =to_string( it->nextinst );
            ret += it->prname + "[" + k + "]";
            if (it != a.end()-1) {
                ret += "-";
            }


        }
    }
    ret+="-TAIL";


    return ret;

}

