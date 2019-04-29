#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "population.h"

struct population {
    person **__people;
    dimension __dimension;
};

// PRIVATE FUNC. DEF.
person **__a2da(population const *);

void __d2da(population const *);

// PUBLIC FUNC. IMPL.
population *new_population(dimension _dimension) {
    population *c = (population *) malloc(sizeof(population));
    c->__dimension = _dimension;
    c->__people = __a2da(c);
    return c;
}

void init_simulation(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__people[row][col].__contributing_strategy = rand() % 2;
            _population->__people[row][col].__profit = 0;
            _population->__people[row][col].__coordinates = (coordinates) { .y = row, .x = col };
        }
    }
}

void clear_grouping_status(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__people[row][col].__in_group = FALSE;
        }
    }
}

void print_population_strategy(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
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
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
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
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            printf("%.2lf\t", _population->__people[row][col].__profit);
        }
        printf("\n");
    }
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

    coordinates *selected_coordinates = _selector(_preselected_person, _population, _count);
    person **selected_persons = (person **) malloc(_count * sizeof(person *));

    int i;
    for (i = 0; i < _count; ++i) {
        selected_persons[i] = &_population->__people[selected_coordinates[i].y][selected_coordinates[i].x];
    }

    free(selected_coordinates);
    return selected_persons;
}

coordinates *select_randomly(person const *_preselected_person, population const *_population, int _count) {
    coordinates *selected_coordinates = (coordinates *) malloc(_count * sizeof(coordinates));

    int x_max = _population->__dimension.width;
    int y_max = _population->__dimension.height;

    int i;
    for (i = 0; i < _count; ++i) {
        int x = rand() % x_max;
        int y = rand() % y_max;

        bool b = FALSE;
        int j;
        for (j = 0; j < i; ++j) {
            if (selected_coordinates[j].x == x && selected_coordinates[j].y == y) { //TODO: add || == preselected.coord
                b = TRUE;
                break;
            }
        }

        if (!b) {
            selected_coordinates[i] = (coordinates) {.x = x, .y = y};
        } else {
            --i;
        }
    }
    return selected_coordinates;
}

coordinates *select_neighbors(person const *_person_in_center, population const *_population, int _count) {
    coordinates *selected_coordinates = (coordinates *) malloc(_count * sizeof(coordinates));

    coordinates conter_coordinates = (coordinates) {
        .y = _person_in_center->__coordinates.y,
        .x = _person_in_center->__coordinates.x
    };

    coordinates north, east, south, west;

    north = conter_coordinates.y == 0
            ? (coordinates) {.y = _population->__dimension.height - 1, .x = conter_coordinates.x}
            : (coordinates) {.y = conter_coordinates.y - 1, .x = conter_coordinates.x};

    east = conter_coordinates.x == _population->__dimension.width - 1
           ? (coordinates) {.y = conter_coordinates.y, .x = 0}
           : (coordinates) {.y = conter_coordinates.y, .x = conter_coordinates.x + 1};

    south = conter_coordinates.y == _population->__dimension.height - 1
            ? (coordinates) {.y = 0, .x = conter_coordinates.x}
            : (coordinates) {.y = conter_coordinates.y + 1, .x = conter_coordinates.x};

    west = conter_coordinates.x == 0
           ? (coordinates) {.y = conter_coordinates.y, .x = _population->__dimension.width - 1}
           : (coordinates) {.y = conter_coordinates.y, .x = conter_coordinates.x - 1};

    *(selected_coordinates + 0) = north;
    *(selected_coordinates + 1) = east;
    *(selected_coordinates + 2) = south;
    *(selected_coordinates + 3) = west;

    return selected_coordinates;
}

person *get_random_person(population const *_population) {
    int x_max = _population->__dimension.width;
    int y_max = _population->__dimension.height;

    int x = rand() % x_max;
    int y = rand() % y_max;

    return &_population->__people[y][x];
}

int collect_contributions(person const *_influencer_candidate, person const **_groupmates, int _groupmate_count) {
    int total_contrubutions = 0;
    int i;
    for (i = 0; i < _groupmate_count; ++i) {
        total_contrubutions += _groupmates[i]->__contributing_strategy;
    }
    return total_contrubutions + _influencer_candidate->__contributing_strategy;
}

void split_contributions(person *_influencer_candidate, person **_groupmates, int _groupmate_count, int contributions, double _multiplication_factor ) {
    double share = contributions / 5.0 * _multiplication_factor;
    _influencer_candidate->__profit += share - _influencer_candidate->__contributing_strategy;
    int i;
    for (i = 0; i < _groupmate_count; ++i) {
        _groupmates[i]->__profit += share - _groupmates[i]->__contributing_strategy;
    }
}

int get_contrubutor_count(population const *_population) {
    int contributor_count = 0;
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            contributor_count += _population->__people[row][col].__contributing_strategy;
        }
    }
    return contributor_count;
}

// PRIVATE FUNC. IMPL.
person **__a2da(population const *_population) {
    person **new_2d_array;
    new_2d_array = (person **) malloc(_population->__dimension.height * sizeof(person *));
    int row;
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
