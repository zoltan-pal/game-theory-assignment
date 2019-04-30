#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "population.h"
#include "dimension.h"

struct population {
    person **__people;
    dimension __dimension;
};

/*  "PRIVATE" FUNC. DEF. */
person **__a2da(population const *);
void __d2da(population const *);

/* "PUBLIC" FUNC. IMPL. */
population *new_population(dimension _dimension) {
    population *new_population;

    new_population = (population *) malloc(sizeof(population));
    new_population->__dimension = _dimension;
    new_population->__people = __a2da(new_population);
    return new_population;
}

void init_simulation(population const *_population) {
    int row,
        col;

    for (row = 0; row < _population->__dimension.height; ++row) {
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__people[row][col].__contributing_strategy = rand() % 2;
            _population->__people[row][col].__profit = 0;
            _population->__people[row][col].__coordinates.y = row;
            _population->__people[row][col].__coordinates.x = col;
        }
    }
}

void clear_grouping_status(population const *_population) {
    int row,
        col;

    for (row = 0; row < _population->__dimension.height; ++row) {
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__people[row][col].__in_group = FALSE;
        }
    }
}

void print_population_strategy(population const *_population) {
    int row,
        col;

    for (row = 0; row < _population->__dimension.height; ++row) {
        for (col = 0; col < _population->__dimension.width; ++col) {
            if(_population->__people[row][col].__contributing_strategy) {
                printf("[X]");
            } else {
                printf("[ ]");
            }
        }
        printf("\n");
    }
    printf("\n");
}
void print_population_group(population const *_population) {
    int row,
        col;

    for (row = 0; row < _population->__dimension.height; ++row) {
        for (col = 0; col < _population->__dimension.width; ++col) {
            if(_population->__people[row][col].__in_group) {
                printf("[X]");
            } else {
                printf("[ ]");
            }
        }
        printf("\n");
    }
    printf("\n");
}
void print_population_profit(population const *_population) {
    int row,
        col;

    for (row = 0; row < _population->__dimension.height; ++row) {
        for (col = 0; col < _population->__dimension.width; ++col) {
            printf("%.2f\t", _population->__people[row][col].__profit);
        }
        printf("\n");
    }
    printf("\n");
}

void delete_population(population *pop) {
    __d2da(pop);
    free(pop);
}

person **get_group(
        population const *_population,
        person const *_preselected_person,
        coordinates *_selector(person const *, population const *, int),
        int _count) {
    coordinates *selected_coordinates;
    person **selected_persons;
    int i;

    selected_coordinates = _selector(_preselected_person, _population, _count);
    selected_persons = (person **) malloc(_count * sizeof(person *));
    for (i = 0; i < _count; ++i) {
        selected_persons[i] = &_population->__people[selected_coordinates[i].y][selected_coordinates[i].x];
        selected_persons[i]->__in_group = TRUE;
    }

    free(selected_coordinates);
    return selected_persons;
}

coordinates *select_randomly(person const *_preselected_person, population const *_population, int _count) {
    coordinates *selected_coordinates;
    int i,
        x,
        y,
        x_max,
        y_max;

    selected_coordinates = (coordinates *) malloc(_count * sizeof(coordinates));
    x_max = _population->__dimension.width;
    y_max = _population->__dimension.height;

    i = 0;
    while(i < _count) {
        x = rand() % x_max;
        y = rand() % y_max;

        if(_population->__people[y][x].__in_group)
            continue;
                
        selected_coordinates[i].x = x;
        selected_coordinates[i].y = y;
        ++i;
        
    }
    return selected_coordinates;
}

coordinates *select_neighbors(person const *_person_in_center, population const *_population, int _count) {
    coordinates *selected_coordinates = (coordinates *) malloc(_count * sizeof(coordinates));
    coordinates north, east, south, west;
    coordinates center_coordinates;

    center_coordinates = new_coordinates(
        _person_in_center->__coordinates.x,
        _person_in_center->__coordinates.y
    );

    north = center_coordinates.y == 0
            ? new_coordinates(center_coordinates.x, _population->__dimension.height - 1)
            : new_coordinates(center_coordinates.x,  center_coordinates.y - 1);

    east = center_coordinates.x == _population->__dimension.width - 1
           ? new_coordinates(0, center_coordinates.y)
           : new_coordinates(center_coordinates.x + 1, center_coordinates.y);

    south = center_coordinates.y == _population->__dimension.height - 1
            ? new_coordinates(center_coordinates.x, 0)
            : new_coordinates(center_coordinates.x, center_coordinates.y + 1);

    west = center_coordinates.x == 0
           ? new_coordinates(_population->__dimension.width - 1, center_coordinates.y)
           : new_coordinates(center_coordinates.x - 1, center_coordinates.y);

    *(selected_coordinates + 0) = north;
    *(selected_coordinates + 1) = east;
    *(selected_coordinates + 2) = south;
    *(selected_coordinates + 3) = west;

    return selected_coordinates;
}

person *get_random_person(population const *_population) {
    int x,
        y;

    x = rand() % _population->__dimension.width;
    y = rand() % _population->__dimension.height;

    _population->__people[y][x].__in_group = TRUE;
    return &_population->__people[y][x];
}

int collect_contributions(person const *_influencer_candidate, person const **_groupmates, int _groupmate_count) {
    int i,
        total_contrubutions;

    total_contrubutions = 0;
    for (i = 0; i < _groupmate_count; ++i) {
        total_contrubutions += _groupmates[i]->__contributing_strategy;
    }
    return total_contrubutions + _influencer_candidate->__contributing_strategy;
}

void split_contributions(person *_influencer_candidate, person **_groupmates, int _groupmate_count, int contributions, double _multiplication_factor ) {
    double share;
    int i;

    share = contributions / (_groupmate_count + 1.0) * _multiplication_factor;
    _influencer_candidate->__profit += share - _influencer_candidate->__contributing_strategy;
    for (i = 0; i < _groupmate_count; ++i) {
        _groupmates[i]->__profit += share - _groupmates[i]->__contributing_strategy;
    }
}

int get_contrubutor_count(population const *_population) {
    int row,
        col,
        contributor_count = 0;

    for (row = 0; row < _population->__dimension.height; ++row) {
        for (col = 0; col < _population->__dimension.width; ++col) {
            contributor_count += _population->__people[row][col].__contributing_strategy;
        }
    }
    return contributor_count;
}

/* "PRIVATE" FUNC. IMPL. */
person **__a2da(population const *_population) {
    person **new_2d_array;
    int row;

    new_2d_array = (person **) malloc(_population->__dimension.height * sizeof(person *));
    for (row = 0; row < _population->__dimension.height; ++row) {
        *(new_2d_array + row) = (person *) malloc(_population->__dimension.width * sizeof(person));
    }
    return new_2d_array;
}

void __d2da(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        free(*(_population->__people + row));
    }
    free(_population->__people);
}
