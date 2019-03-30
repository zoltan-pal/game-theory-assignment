#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crowd.h"
#include "bool.h"
#include "person.h"

void strprint(char *);

void arg_check(int, char **);

bool arg_validate(int, char **);

void set_topology(const char *);

topology selected_topology;
dimension crowd_dimension;

int main(int argc, char **argv) {
    arg_check(argc, argv);
    set_topology(*(argv + 2));

    crowd_dimension = (dimension) {.width = 5, .height = 5};

    srand(time(NULL));

    crowd *c = new_crowd(crowd_dimension);
    foo(c);

    person *p = get_group(c, select_randomly, 5);

    int i;
    for (i = 0; i < 5; ++i) {
        printf("%d\n", (p + i)->gambled_in_last_turn);
    }

    free(p);
    delete_crowd(c);

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
    if (!(strcmp(*(argv + 1), "-t") == 0 || strcmp(*(argv + 1), "--topology") == 0))
        return FALSE;
    if (!(strcmp(*(argv + 2), "fixed") == 0 || strcmp(*(argv + 2), "random") == 0 || strcmp(*(argv + 2), "mixed") == 0))
        return FALSE;
    return TRUE;
}

void strprint(char *s) {
    char *p = s;
    while (*p != '\0')
        printf("%c", *p++);
}