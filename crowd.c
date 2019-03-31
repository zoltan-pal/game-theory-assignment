#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include "crowd.h"

struct crowd {
    person **__people;
    dimension __dimension;
};

// PRIVATE FUNC. DEF.
person **__a2da(crowd const *);

void __d2da(crowd const *);

// PUBLIC FUNC. IMPL.
crowd *new_crowd(dimension _dimension) {
    crowd *c = (crowd *) malloc(sizeof(crowd));
    c->__dimension = _dimension;
    c->__people = __a2da(c);
    return c;
}

void init_sym(crowd const *_crowd) {
    srand(time(NULL));
    int row;
    for (row = 0; row < _crowd->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _crowd->__dimension.width; ++col) {
            _crowd->__people[row][col].gambled_in_last_turn = FALSE; // rand() % 2;
        }
    }
}

void print_crowd(crowd const *_crowd) {
    int row;
    for (row = 0; row < _crowd->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _crowd->__dimension.width; ++col) {
            printf("%d\t", _crowd->__people[row][col].gambled_in_last_turn);
        }
        printf("\n");
    }
}

void delete_crowd(crowd *c) {
    __d2da(c);
    free(c);
}

person **get_group(crowd const *_crowd, coordinates *_selector(crowd const *, int), int _count) {
    coordinates *selected_coordinates = _selector(_crowd, _count);
    person **selected_persons = (person **) malloc(_count * sizeof(person *));

    int i;
    for (i = 0; i < _count; ++i) {
        selected_persons[i] = &_crowd->__people[selected_coordinates[i].y][selected_coordinates[i].x];
    }

    free(selected_coordinates);
    return selected_persons;
}

coordinates *select_randomly(crowd const *_crowd, int _count) {
    coordinates *selected_coordinates = (coordinates *) malloc(_count * sizeof(coordinates));

    int x_max = _crowd->__dimension.width;
    int y_max = _crowd->__dimension.height;

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

coordinates *select_neighbors(crowd const *_crowd, int _count) {
    coordinates *selected_coordinates = (coordinates *) malloc(_count * sizeof(coordinates));

    coordinates *person_in_center = select_randomly(_crowd, 1);
    *selected_coordinates = *person_in_center;
    coordinates north, east, south, west;

    north = person_in_center->y == 0
            ? (coordinates) {.y = _crowd->__dimension.height - 1, .x = person_in_center->x}
            : (coordinates) {.y = person_in_center->y - 1, .x = person_in_center->x};

    east = person_in_center->x == _crowd->__dimension.width - 1
           ? (coordinates) {.y = person_in_center->y, .x = 0}
           : (coordinates) {.y = person_in_center->y, .x = person_in_center->x + 1};

    south = person_in_center->y == _crowd->__dimension.height - 1
            ? (coordinates) {.y = 0, .x = person_in_center->x}
            : (coordinates) {.y = person_in_center->y + 1, .x = person_in_center->x};

    west = person_in_center->x == 0
           ? (coordinates) {.y = person_in_center->y, .x = _crowd->__dimension.width - 1}
           : (coordinates) {.y = person_in_center->y, .x = person_in_center->x - 1};

    *(selected_coordinates + 1) = north;
    *(selected_coordinates + 2) = east;
    *(selected_coordinates + 3) = south;
    *(selected_coordinates + 4) = west;

    free(person_in_center);
    return selected_coordinates;
}

// PRIVATE FUNC. IMPL.
person **__a2da(crowd const *_crowd) {
    person **new_2d_array;
    new_2d_array = (person **) malloc(_crowd->__dimension.height * sizeof(person *));
    int row;
    for (row = 0; row < _crowd->__dimension.height; ++row) {
        *(new_2d_array + row) = (person *) malloc(_crowd->__dimension.width * sizeof(person));
    }
    return new_2d_array;
}

void __d2da(crowd const *_crowd) {
    int row;
    for (row = 0; row < _crowd->__dimension.height; ++row) {
        free(*(_crowd->__people + row));
    }
    free(_crowd->__people);
}