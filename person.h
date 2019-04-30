#ifndef PERSON_H
#define PERSON_H

#include "bool.h"
#include "coordinates.h"

typedef struct person person;
struct person {
    bool __contributing_strategy; /* true: contributes, false: keeps money for themselves */
    double __profit;
    bool __in_group;
    coordinates __coordinates;
};

#endif /* PERSON_H */
