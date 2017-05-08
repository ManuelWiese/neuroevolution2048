#define POPULATION  150
#define RUNS_PER_NETWORK 1000

#define DELTA_DISJOINT  1.0
#define DELTA_WEIGHTS  0.0
#define DELTA_BIAS 0.0
#define DELTA_TRANSFER 1.0
#define DELTA_RATES	0.0
#define DELTA_THRESHOLD  1.5

#define WEIGHT_RANGE  2.0
#define BIAS_RANGE  2.0

#define CULL_MINIMUM 4
#define STALE_SPECIES  15

#define PERTUBCHANCE  0.90
#define CROSSOVER_CHANCE  0.75

#define LINK_MUTATION_CHANCE  2.0 //2.0
#define NODE_MUTATION_CHANCE  0.5 //0.50
#define BIAS_MUTATION_CHANCE  0.0 //1.00
#define WEIGHT_MUTATION_CHANCE  0.0 //1.00
#define TRANSFER_MUTATION_CHANCE  0.5 //0.5
#define DISABLE_MUTATION_CHANCE  0.4 //0.4
#define ENABLE_MUTATION_CHANCE  0.2 //0.2
#define DELETE_MUTATION_CHANCE  0.1

#define STEPSIZE  0.1

#define MUTATIONRATE_THRESHOLD 0.04
#define MUTATIONRATE_FACTOR 1.01

#define PRECISION 0.01

#define LEARNING_RATE 0.1
#define EPSILON_GREEDY 0.1
#define GAMMA 0.3

#define NAME_PREFIX	"openmpLogSpecNoUp"
#define SAVEPATH  "./save/"
