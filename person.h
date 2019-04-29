#ifndef PERSON_H
#define PERSON_H

#include "bool.h"

typedef struct person person;
struct person {
    bool contributing_strategy; // true: contributes, false: keeps money for themselves
    double profit;
    bool in_group;
};

#endif //PERSON_H
