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


typedef struct population population;

population *new_population(dimension);

void init_sym(population const *);

void print_population(population const *);
void print_population_group(population const *);
void print_population_profit(population const *);

void start_new_round(population const *);

person **get_group(population const *, coordinates *(*)(population const *, int), int count);

coordinates *select_randomly(population const *, int);

coordinates *select_neighbors(population const *, int);

//void collect_money(population *);
//
//void split_collected_money(population *);

void delete_population(population *);



int collect_money(person **, int);

void split_collected_money(int, person **, int);


#endif //CROWD_H
