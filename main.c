/* 
    set yrange [:] reverse
    splot "sim_result.txt" u 1:2:3 with lines
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "population.h"
#include "bool.h"
#include "person.h"
#include "dimension.h"

#define DEBUG

typedef enum topology {
    FIXED = 0, RANDOM = 1, MIXED = 2
} topology;

void arg_check(int, char **);
bool arg_validate(int, char **);
void set_topology(const char *);

topology selected_topology;
dimension population_dimension;

int main(int argc, char **argv) {
    coordinates *(*primary_groupmate_selection_strategy)(person const *, population const *, int);
    coordinates *(*secondary_groupmate_selection_strategy)(person const *, population const *, int);
    
    FILE *out_file;
    char out_file_name[24];
    population *simulated_population;
    int groupmate_count,
        multiplication_factor_r,
        contributions;
    
    srand(time(NULL));

    arg_check(argc, argv);
    set_topology(*(argv + 2));

    switch (selected_topology) {
        case FIXED:
            primary_groupmate_selection_strategy = select_neighbors;
            secondary_groupmate_selection_strategy = select_neighbors;
            break;
        case RANDOM:
            primary_groupmate_selection_strategy = select_randomly;
            secondary_groupmate_selection_strategy = select_randomly;
            break;
        case MIXED:
        default:
            primary_groupmate_selection_strategy = select_randomly;
            secondary_groupmate_selection_strategy = select_neighbors;
    }

    population_dimension.width = 32;
    population_dimension.height = 32;
    simulated_population = new_population(population_dimension);
    groupmate_count = 4;
    
    strcpy(out_file_name, "sim_result_");
    strcat(out_file_name, *(argv + 2));
    strcat(out_file_name, ".txt");
    out_file = fopen(out_file_name, "w");
    fprintf(out_file, "# X\tY\tZ\r\n");
    
    for(multiplication_factor_r = 1; multiplication_factor_r <= 50; ++multiplication_factor_r) {
        int round,
            max_rounds;

        max_rounds = 10000;
        init_simulation(simulated_population);
        for (round = 0; round < max_rounds; ++round) {
            int i,
                selected_index,
                contributor_count;
            person *learning_agent,
                **learning_agent_groupmates,
                *group_leader_candidate;

            learning_agent = get_random_person(simulated_population);
            learning_agent_groupmates = get_group(simulated_population, learning_agent, primary_groupmate_selection_strategy, groupmate_count);

            /* Learning agent's groupmates play in their own local groups as well. */
            for (i = 0; i < groupmate_count; ++i) {
                person **secondary_groupmates;

                secondary_groupmates = get_group(
                        simulated_population,
                        learning_agent_groupmates[i],
                        secondary_groupmate_selection_strategy,
                        groupmate_count
                );
                contributions = collect_contributions(
                        learning_agent_groupmates[i],
                        (const person **) secondary_groupmates,
                        groupmate_count
                );
                split_contributions(
                        learning_agent_groupmates[i],
                        secondary_groupmates,
                        groupmate_count,
                        contributions,
                        multiplication_factor_r
                );
                free(secondary_groupmates);
            }

            /* Let the agent and their groupmates play in the primary group. */
            contributions = collect_contributions(learning_agent, (const person **) learning_agent_groupmates, groupmate_count);
            split_contributions(
                    learning_agent,
                    learning_agent_groupmates,
                    groupmate_count,
                    contributions,
                    multiplication_factor_r
            );

            /* Choose a group leader/local influencer candidate from primary groupmates. */
            selected_index = (rand() % groupmate_count - 1) + 1;
            group_leader_candidate = learning_agent_groupmates[selected_index];

            if (group_leader_candidate->__profit > learning_agent->__profit)
                learning_agent->__contributing_strategy = group_leader_candidate->__contributing_strategy;

            contributor_count = get_contrubutor_count(simulated_population);
            fprintf(out_file, "%d\t%d\t%d\r\n", multiplication_factor_r, round, contributor_count);

#ifdef DEBUG
            print_population_profit(simulated_population);
            print_population_group(simulated_population);
            print_population_strategy(simulated_population);
#endif
            clear_grouping_status(simulated_population);
            
            free(learning_agent_groupmates);
        }
        fprintf(out_file, "\r\n\r\n");
    }
    fclose(out_file);
    delete_population(simulated_population);

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