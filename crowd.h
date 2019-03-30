#ifndef CROWD_H
#define CROWD_H

#include "person.h"

typedef enum topology {
    FIXED = 0, RANDOM = 1, MIXED = 2
} topology;

typedef struct dimension {
    int width, height;
} dimension;

typedef struct coordinates {
    int x, y;
} coordinates;




typedef struct crowd crowd;

crowd *new_crowd(dimension);
person *get_person(crowd const *, coordinates);
void foo(crowd const *);
person *get_group_of_five(crowd const *, person const *, void (*selection_strategy)());
void select_randomly();
void select_neighbors();
void delete_crowd(crowd *);

#endif //CROWD_H
