#ifndef POPULATION_H
#define POPULATION_H

#include "person.h"
#include "dimension.h"

typedef struct population population;

population *new_population(dimension);

void init_simulation(population const *);

void print_population_strategy(population const *);
void print_population_group(population const *);
void print_population_profit(population const *);

void clear_grouping_status(population const *);

person **get_group(population const *, person const *, coordinates *(*)(person const *, population const *, int), int count);

person *get_random_person(population const *);

coordinates *select_randomly(person const *, population const *, int);

coordinates *select_neighbors(person const *, population const *, int);

int collect_contributions(person const *, person const **, int);

void split_contributions(person *, person **, int, int, double);

int get_contrubutor_count(population const *);

void delete_population(population *);

#endif /* POPULATION_H */
