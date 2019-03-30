#ifndef PERSON_H
#define PERSON_H
#include "bool.h"

typedef struct person person;
struct person {
    bool gambled_in_last_turn;
    double profit;
};


#endif PERSON_H
