//
// Created by ASUS on 18.11.2018.
//

#ifndef SCHEDULER_PROCESS_H
#define SCHEDULER_PROCESS_H


#include <vector>
#include <string>

using namespace std;
class process {

public:
    string prname;      //name of process
    int priority;
    int arrtime;        //arrival time of process
    bool operator < (const process &a)const;
    vector<vector<string>> inst;    //2-dimensional array storing instruction names and execution times
    int nextinst=1;     //shows which instruction should be executed next



    int totinsttime=0;         //sum of execution times of instructions of this process


};


#endif //SCHEDULER_PROCESS_H
