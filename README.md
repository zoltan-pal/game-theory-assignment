# Public goods game simulation

This project is a simulation for a variant of an iterated transparent public goods game created as an assignment for Game Theory course.

The program generates a text file containing how the number of contrubutors change depending on how high is the multiplication factor during 10000 runs for a population of 1024 subjects, with various group selection modes (fixed, mixed, random).

The results can be loaded into gnuplot and drawn as a graph:

1. In the case of fixed mode:

```sh
./executable -t fixed
gnuplot
```

2. Inside gnuplot console:

```sh
set yrange [:] reverse
splot "sim_result_fixed.txt" u 1:2:3 with lines
```
