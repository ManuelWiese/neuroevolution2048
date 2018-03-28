# neuroevolution2048
## Solving the game 2048 using neuroevolution
A fast implementation of the game 2048, solving it using neuroevolution(close to Neat).
Neurons and links are encoded in genes. A set of genes is called a genome and builds a neuronal network.
Genomes are playing a number of games and the fitness is compared

In branch __multicore__ is a parallel implementation using openmp, it automatically uses the maximal number of threads.
