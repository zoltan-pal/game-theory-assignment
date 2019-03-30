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

void init_sym(crowd const *);

void print_people_2d_arr(crowd const *);

person *get_group(crowd const *, coordinates *(*)(crowd const *, int), int count);

coordinates *select_randomly(crowd const *, int);

coordinates *select_neighbors(crowd const *, int);

void delete_crowd(crowd *);

#endif //CROWD_H
