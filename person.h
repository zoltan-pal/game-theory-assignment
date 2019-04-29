#ifndef PERSON_H
#define PERSON_H

#include "bool.h"

typedef struct person person;
struct person {
    bool contributed_last_round;
    bool contribute_next_round;
    double profit;
    bool in_group;
};


#endif //PERSON_H
