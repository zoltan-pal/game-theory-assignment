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
    crowd *c = (crowd *)malloc(sizeof(crowd));
    c->__dimension = _dimension;
    c->__people = __a2da(c);
    return c;
}

person *get_person(crowd const *_crowd, coordinates _coordinates) {
    return &_crowd->__people[_coordinates.x][_coordinates.y];
}

void foo(crowd const* _crowd) {
    srand(time(NULL));
    int row;
    for (row = 0; row < _crowd->__dimension.height; ++row) {
        int col;
        for (col = 0; col < _crowd->__dimension.width; ++col) {
            _crowd->__people[row][col].gambled_in_last_turn = rand() % 2;
        }
    }

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

person *get_group_of_five(crowd const *_crowd, person const *_person, void s() ) {
    return _person;
}

void select_randomly() {

}

void select_neighbors() {

}


// PRIVATE FUNC. IMPL.
person **__a2da(crowd const *_crowd) {
    person **new_2d_array;
    new_2d_array = (person **) malloc(_crowd->__dimension.height * sizeof(person *));
    int row;
    for (row = 0; row < _crowd->__dimension.height; ++row)
    {
        *(new_2d_array + row) = (person *) malloc(_crowd->__dimension.width * sizeof(person));
    }
    return new_2d_array;
}

void __d2da(crowd const *_crowd) {
    int row;
    for (row = 0; row < _crowd->__dimension.height; ++row)
    {
        free(*(_crowd->__people + row));
    }
    free(_crowd->__people);
}