#ifndef POPULATION_H
#define POPULATION_H

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

void init_simulation(population const *);

void print_population_strategy(population const *);
void print_population_group(population const *);
void print_population_profit(population const *);

void clear_grouping_status(population const *);

person **get_group(population const *, coordinates *(*)(population const *, int), int count);

coordinates *select_randomly(population const *, int);

coordinates *select_neighbors(population const *, int);

int collect_money(population *);

void split_collected_money(population *);

void delete_population(population *);

#endif //POPULATION_H
