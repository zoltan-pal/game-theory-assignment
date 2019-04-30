/* 
    set yrange [:] reverse
    splot "sim_result.txt" u 1:2:3 with lines
*/

#include <stdio.h>
#include <stdlib.h>

#include "population.h"

/* #define DEBUG */

typedef enum topology {
    FIXED = 0, RANDOM = 1, MIXED = 2
} topology;

void arg_check(int, char **);
bool arg_validate(int, char **);
void set_topology(const char *);

double
get_profit(int, population *, person **, int, person *, coordinates *(*)(person const *, population const *, int));

topology selected_topology;
dimension population_dimension;

int main(int argc, char **argv) {
    coordinates *(*primary_groupmate_selection_strategy)(person const *, population const *, int);
    coordinates *(*secondary_groupmate_selection_strategy)(person const *, population const *, int);
    
    FILE *out_file;
    char out_file_name[24];
    population *simulated_population;
    int groupmate_count,
            multiplication_factor_r;
    
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

    for (multiplication_factor_r = 1; multiplication_factor_r <= 10; ++multiplication_factor_r) {
        int round,
            max_rounds;

        max_rounds = 10000;
        init_simulation(simulated_population);
        for (round = 0; round < max_rounds; ++round) {
            double learning_agent_profit,
                    strategy_role_model_profit;
            int selected_index,
                contributor_count;
            person *learning_agent,
                **learning_agent_groupmates,
                    *strategy_role_model;

            learning_agent = get_random_person(simulated_population);
            learning_agent_groupmates = get_group(simulated_population, learning_agent, primary_groupmate_selection_strategy, groupmate_count);

            /* Choose a group leader/local influencer candidate from primary groupmates. */
            selected_index = (rand() % groupmate_count - 1) + 1;
            strategy_role_model = learning_agent_groupmates[selected_index];

            /* Calculate profits and compare only if they are not following the same strategy  */
            if (learning_agent->__contributing_strategy != strategy_role_model->__contributing_strategy) {
                person **strategy_role_model_groupmates;
                learning_agent_profit = get_profit(groupmate_count, simulated_population, learning_agent_groupmates,
                                                   multiplication_factor_r, learning_agent,
                                                   secondary_groupmate_selection_strategy);

                strategy_role_model_groupmates = get_group(simulated_population, strategy_role_model,
                                                           primary_groupmate_selection_strategy, groupmate_count);

                strategy_role_model_profit = get_profit(groupmate_count, simulated_population,
                                                        strategy_role_model_groupmates, multiplication_factor_r,
                                                        strategy_role_model, secondary_groupmate_selection_strategy);

                if (strategy_role_model_profit > learning_agent_profit)
                    learning_agent->__contributing_strategy = strategy_role_model->__contributing_strategy;
            }
            contributor_count = get_contrubutor_count(simulated_population);

            fprintf(out_file, "%d\t%d\t%d\r\n", multiplication_factor_r, round, contributor_count);

#ifdef DEBUG
            print_population_profit(simulated_population);
            print_population_group(simulated_population);
            print_population_strategy(simulated_population);
#endif
            clear_grouping_status(simulated_population);
            reset_round(simulated_population);
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

double get_profit(int groupmate_count, population *simulated_population, person **group, int multiplication_factor_r,
                  person *p,
                  coordinates *secondary_groupmate_selection_strategy(person const *, population const *, int)) {
    int i, contributions;
    double profit;

    for (i = 0; i < groupmate_count; ++i) {
        person **secondary_groupmates;

        secondary_groupmates = get_group(
                simulated_population,
                group[i],
                secondary_groupmate_selection_strategy,
                groupmate_count
        );
        contributions = collect_contributions(
                group[i],
                (const person **) secondary_groupmates,
                groupmate_count
        );
        split_contributions(
                group[i],
                secondary_groupmates,
                groupmate_count,
                contributions,
                multiplication_factor_r
        );
        free(secondary_groupmates);
    }

    contributions = collect_contributions(p, (const person **) group, groupmate_count);
    split_contributions(
            p,
            group,
            groupmate_count,
            contributions,
            multiplication_factor_r
    );
    clear_grouping_status(simulated_population);
    profit = p->__profit;

    reset_profits(simulated_population);
    return profit;
}