#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "population.h"
#include "bool.h"
#include "person.h"

void arg_check(int, char **);

bool arg_validate(int, char **);

void set_topology(const char *);

topology selected_topology;
dimension population_dimension;

int main(int argc, char **argv) {
    srand(time(NULL));

    arg_check(argc, argv);
    set_topology(*(argv + 2));

    coordinates *(*selection_strategy)(population const *, int);
    switch (selected_topology) {
        case FIXED:
            selection_strategy = select_neighbors;
            break;
        case RANDOM:
            selection_strategy = select_randomly;
            break;
        case MIXED:
        default:
            selection_strategy = select_randomly;
    }

    population_dimension = (dimension) {.width = 10, .height = 10};

    int group_size = 5;
    population *pop = new_population(population_dimension);
    init_simulation(pop);
    FILE *out_file = fopen("sim_result.txt", "w");

    int round;
    for (round = 0; round < 1000; ++round) {
        clear_grouping_status(pop);

        person **group = get_group(pop, selection_strategy, group_size);

        int i;
        for (i = 0; i < group_size; ++i) {
            group[i]->in_group = TRUE;
        }

        int cm = collect_money(pop);
        fprintf(out_file, "%d\t%d\r\n",round, cm);
        split_collected_money(pop);

        print_population_profit(pop);
        printf("\n");
        int selected_index = (rand() % group_size - 1) + 1;
        person *person1 = group[selected_index];
        if (person1->profit > group[0]->profit)
            group[0]->contributing_strategy = person1->contributing_strategy;

        free(group);
    }

    fclose(out_file);
    delete_population(pop);

    return 0;
}

void set_topology(const char *s) {
    switch (*s) {
        case 'f':
            selected_topology = FIXED;
            break;
        case 'r':
            selected_topology = RANDOM;
            break;
        case 'm':
        default:
            selected_topology = MIXED;
            break;
    }
}

void arg_check(int argc, char **argv) {
    if (!arg_validate(argc, argv)) {
        printf("Bad command line arguments.\nUsage: program { -t | --topology } <fixed | random | mixed>\nExiting...");
        exit(EXIT_FAILURE);
    }
}

bool arg_validate(int argc, char **argv) {
    if (argc != 3)
        return FALSE;
    if (!(strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--topology") == 0))
        return FALSE;
    if (!(strcmp(argv[2], "fixed") == 0 || strcmp(argv[2], "random") == 0 || strcmp(argv[2], "mixed") == 0))
        return FALSE;
    return TRUE;
}