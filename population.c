#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "population.h"

struct population {
    person **__people;
    dimension __dimension;
    int __collected_money;
    double __multiplication_factor;
};

// PRIVATE FUNC. DEF.
person **__a2da(population const *);

void __d2da(population const *);

// PUBLIC FUNC. IMPL.
population *new_population(dimension _dimension) {
    population *c = (population *) malloc(sizeof(population));
    c->__dimension = _dimension;
    c->__people = __a2da(c);
    c->__multiplication_factor = 9;
    return c;
}

void init_simulation(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__people[row][col].contributing_strategy = rand() % 2;
            _population->__people[row][col].profit = 0;
        }
    }
}

void clear_grouping_status(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__people[row][col].in_group = FALSE;
        }
    }
}

void print_population_strategy(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            printf("%d\t", _population->__people[row][col].contributing_strategy);
        }
        printf("\n");
    }
}
void print_population_group(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            printf("%d\t", _population->__people[row][col].in_group);
        }
        printf("\n");
    }
}
void print_population_profit(population const *_population) {
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            printf("%.2lf\t", _population->__people[row][col].profit);
        }
        printf("\n");
    }
}

void delete_population(population *pop) {
    __d2da(pop);
    free(pop);
}

person **get_group(population const *_population, coordinates *_selector(population const *, int), int _count) {
    coordinates *selected_coordinates = _selector(_population, _count);
    person **selected_persons = (person **) malloc(_count * sizeof(person *));

    int i;
    for (i = 0; i < _count; ++i) {
        selected_persons[i] = &_population->__people[selected_coordinates[i].y][selected_coordinates[i].x];
    }

    free(selected_coordinates);
    return selected_persons;
}

coordinates *select_randomly(population const *_population, int _count) {
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
            if (selected_coordinates[j].x == x && selected_coordinates[j].y == y) {
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

coordinates *select_neighbors(population const *_population, int _count) {
    coordinates *selected_coordinates = (coordinates *) malloc(_count * sizeof(coordinates));

    coordinates *person_in_center = select_randomly(_population, 1);
    *selected_coordinates = *person_in_center;
    coordinates north, east, south, west;

    north = person_in_center->y == 0
            ? (coordinates) {.y = _population->__dimension.height - 1, .x = person_in_center->x}
            : (coordinates) {.y = person_in_center->y - 1, .x = person_in_center->x};

    east = person_in_center->x == _population->__dimension.width - 1
           ? (coordinates) {.y = person_in_center->y, .x = 0}
           : (coordinates) {.y = person_in_center->y, .x = person_in_center->x + 1};

    south = person_in_center->y == _population->__dimension.height - 1
            ? (coordinates) {.y = 0, .x = person_in_center->x}
            : (coordinates) {.y = person_in_center->y + 1, .x = person_in_center->x};

    west = person_in_center->x == 0
           ? (coordinates) {.y = person_in_center->y, .x = _population->__dimension.width - 1}
           : (coordinates) {.y = person_in_center->y, .x = person_in_center->x - 1};

    *(selected_coordinates + 1) = north;
    *(selected_coordinates + 2) = east;
    *(selected_coordinates + 3) = south;
    *(selected_coordinates + 4) = west;

    free(person_in_center);
    return selected_coordinates;
}

int collect_money(population *_population) {
    _population->__collected_money = 0;
    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__collected_money += _population->__people[row][col].contributing_strategy;
        }
    }
    return _population->__collected_money;
}

void split_collected_money(population *_population) {
    double prize_by_person =
            _population->__multiplication_factor
            * _population->__collected_money
            / (double)(_population->__dimension.width * _population->__dimension.height);


    _population->__collected_money = 0;

    int row;
    for (row = 0; row < _population->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _population->__dimension.width; ++col) {
            _population->__people[row][col].profit += prize_by_person - _population->__people[row][col].contributing_strategy;
//            printf("profit: %lf\n", _population->__people[row][col].profit);
        }
    }
//    printf("PBP: %lf\n", prize_by_person);
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
