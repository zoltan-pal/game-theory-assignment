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

    crowd_dimension = (dimension) {.width = 4, .height = 4};

    srand(time(NULL));

    int group_size = 5;
    crowd *c = new_crowd(crowd_dimension);
    init_sym(c);
    print_people_2d_arr(c);

    person *p = get_group(c, select_neighbors, group_size);
    
    printf("\n");
    print_people_2d_arr(c);

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