//
// Created by ASUS on 18.11.2018.
//

#include "process.h"

//this operator is used in sorting.
bool process::operator< (const process &other) const{
    return this->priority < other.priority;
}