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

    coordinates *(*selection_strategy)(person const *, population const *, int);
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

    int groupmate_count = 4;
    population *pop = new_population(population_dimension);
    init_simulation(pop);
    FILE *out_file = fopen("sim_result.txt", "w");

    int multiplication_factor_r = 1;

    int round;
    for (round = 0; round < 1000; ++round) {
        clear_grouping_status(pop);
        int i;
        person *p = get_random_person(pop);
        person **p_groupmates = get_group(pop, p, selection_strategy, groupmate_count);

//        p->__in_group = TRUE;
//        print_population_group(pop);
//
//        for (i = 0; i < 4; ++i) {
//            p_groupmates[i]->__in_group = TRUE;
//        }
//
//        print_population_group(pop);

        for (i = 0; i < groupmate_count; ++i) {
            person **gm_groupmates = get_group(pop, p_groupmates[i], selection_strategy, groupmate_count);
            int contribs = collect_contributions(p_groupmates[i], (const person **) gm_groupmates, groupmate_count);
            split_contributions(
                    p_groupmates[i],
                    gm_groupmates,
                    groupmate_count,
                    contribs,
                    multiplication_factor_r
                    );
//            int j;
//            for (j = 0; j < 4; ++j) {
//                gm_groupmates[j]->__in_group = TRUE;
//            }
//            p_groupmates[i]->__in_group = TRUE;
//            print_population_group(pop);
//            print_population_profit(pop);
            free(gm_groupmates);
        }
//        print_population_group(pop);
        int contribs = collect_contributions(p, (const person **) p_groupmates, groupmate_count);
        split_contributions(
                p,
                p_groupmates,
                groupmate_count,
                contribs,
                multiplication_factor_r
        );

        int selected_index = (rand() % groupmate_count - 1) + 1;
        person *person1 = p_groupmates[selected_index];
        if (person1->__profit > p_groupmates[0]->__profit)
            p_groupmates[0]->__contributing_strategy = person1->__contributing_strategy;

        int contributor_count = get_contrubutor_count(pop);
        fprintf(out_file, "%d\t%d\r\n",round, contributor_count);


//        print_population_profit(pop);
//        printf("\n");

//        clear_grouping_status(pop);
        print_population_strategy(pop);
        free(p_groupmates);
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