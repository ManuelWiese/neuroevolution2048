
#include "pool.h"
#include "genome.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>

pool::pool() {
  currentSpecies = 0;
  currentGenome = 0;
}

pool::pool(unsigned short inputs, unsigned short outputs,
           unsigned short population = POPULATION) {
  runDir = createRunDirectory();
  saveConstants();

  generation = 0;
  innovation = outputs;
  currentSpecies = 0;
  currentGenome = 0;
  maxFitness = std::numeric_limits<double>::min();
  this->population = population;
  this->inputs = inputs;
  this->outputs = outputs;
  targetPrecision = PRECISION;
  deltaThreshold = DELTA_THRESHOLD;

  for (unsigned short i = 0; i < population; i++) {
    addToSpecies(genome::basicGenome(this));
  }
}

pool::~pool() {
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      for (auto const &gen : genom->genes)
        delete gen;
      for (auto const &neur : genom->neurons)
        delete neur;
      delete genom;
    }
    delete spec;
  }
}

std::string pool::getCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t timeNow = std::chrono::system_clock::to_time_t(now);
  std::tm tm = *std::localtime(&timeNow);

  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y%m%d%H%M%S");
  return oss.str();
}

std::filesystem::path pool::createRunDirectory() {
  std::filesystem::path baseDir = "runs";

  if (!std::filesystem::exists(baseDir)) {
    std::filesystem::create_directory(baseDir);
  }

  std::string timestamp = getCurrentTimestamp();
  std::filesystem::path runDir = baseDir / timestamp;

  if (std::filesystem::exists(runDir)) {
    std::cout << "Rundir " << runDir << " already exists! Exiting" << std::endl;
    exit(-1);
  } else {
    std::filesystem::create_directory(runDir);
    std::cout << "Created run directory: " << runDir << std::endl;
  }
  return runDir;
}

void pool::saveConstants() {
  std::ofstream file(runDir / "config.txt");
  if (!file) {
    throw std::runtime_error("Could not open config.txt");
  }

  file << std::fixed << std::setprecision(6);

  file << "POPULATION = " << POPULATION << std::endl
       << "RUNS_PER_NETWORK = " << RUNS_PER_NETWORK << std::endl

       << "DELTA_DISJOINT = " << DELTA_DISJOINT << std::endl
       << "DELTA_WEIGHTS = " << DELTA_WEIGHTS << std::endl
       << "DELTA_BIAS = " << DELTA_BIAS << std::endl
       << "DELTA_TRANSFER = " << DELTA_TRANSFER << std::endl
       << "DELTA_RATES = " << DELTA_RATES << std::endl
       << "DELTA_THRESHOLD = " << DELTA_THRESHOLD << std::endl

       << "WEIGHT_RANGE = " << WEIGHT_RANGE << std::endl
       << "BIAS_RANGE = " << BIAS_RANGE << std::endl

       << "CULL_MINIMUM = " << CULL_MINIMUM << std::endl
       << "STALE_SPECIES = " << STALE_SPECIES << std::endl

       << "PERTUBCHANCE = " << PERTUBCHANCE << std::endl
       << "CROSSOVER_CHANCE = " << CROSSOVER_CHANCE << std::endl

       << "LINK_MUTATION_CHANCE = " << LINK_MUTATION_CHANCE << std::endl
       << "NODE_MUTATION_CHANCE = " << NODE_MUTATION_CHANCE << std::endl
       << "BIAS_MUTATION_CHANCE = " << BIAS_MUTATION_CHANCE << std::endl
       << "WEIGHT_MUTATION_CHANCE = " << WEIGHT_MUTATION_CHANCE << std::endl
       << "TRANSFER_MUTATION_CHANCE = " << TRANSFER_MUTATION_CHANCE << std::endl
       << "DISABLE_MUTATION_CHANCE = " << DISABLE_MUTATION_CHANCE << std::endl
       << "ENABLE_MUTATION_CHANCE = " << ENABLE_MUTATION_CHANCE << std::endl
       << "DELETE_MUTATION_CHANCE = " << DELETE_MUTATION_CHANCE << std::endl

       << "STEPSIZE = " << STEPSIZE << std::endl

       << "MUTATIONRATE_THRESHOLD = " << MUTATIONRATE_THRESHOLD << std::endl
       << "MUTATIONRATE_FACTOR = " << MUTATIONRATE_FACTOR << std::endl

       << "PRECISION = " << PRECISION << std::endl;
}

unsigned int pool::newInnovation() {
  innovation += 1;
  return innovation;
}

static bool compareByFitness(genome *a, genome *b) {
  return a->fitness < b->fitness;
}

void pool::rankGenomes() {
  std::vector<genome *> tmpGenomes;
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      tmpGenomes.push_back(genom);
    }
  }
  std::sort(tmpGenomes.begin(), tmpGenomes.end(), compareByFitness);

  for (unsigned short i = 0; i < tmpGenomes.size(); i++) {
    tmpGenomes[i]->globalRank = i + 1;
  }
}

double pool::getAverageFitness() {
  double sum = 0;
  for (auto const &spec : speciesVector)
    sum += spec->averageFitness;
  return sum / population;
}

static bool compareByFitnessDescending(genome *a, genome *b) {
  return b->fitness < a->fitness;
}

void pool::cullSpecies(bool cutToMin) {
  for (auto const &spec : speciesVector) {
    std::sort(spec->genomes.begin(), spec->genomes.end(),
              compareByFitnessDescending);
    unsigned short remaining;
    if (cutToMin)
      remaining = CULL_MINIMUM;
    else
      remaining = ceil(spec->genomes.size() / 2.0);

    for (unsigned int i = remaining; i < spec->genomes.size(); i++) {
      for (auto const &gen : spec->genomes[i]->genes) {
        delete gen;
      }
      for (auto const &neur : spec->genomes[i]->neurons) {
        delete neur;
      }
      delete spec->genomes[i];
    }
    if (remaining < spec->genomes.size())
      spec->genomes.erase(spec->genomes.begin() + remaining,
                          spec->genomes.end());
  }
}

/*static bool compareSpeciesByTopFitness(species* a, species* b){
    return b->topFitness < a->topFitness;
}*/

void pool::removeStaleSpecies() {
  std::vector<species *> survived;
  for (auto const &spec : speciesVector) {
    std::sort(spec->genomes.begin(), spec->genomes.end(),
              compareByFitnessDescending);
    if (spec->genomes[0]->fitness > spec->topFitness) {
      spec->topFitness = spec->genomes[0]->fitness;
      spec->staleness = 0;
    } else {
      spec->staleness += 1;
    }
    if (spec->staleness < STALE_SPECIES || spec->topFitness >= maxFitness)
      survived.push_back(spec);
    else {
      for (auto const &genom : spec->genomes) {
        for (auto const &gen : genom->genes) {
          delete gen;
        }
        for (auto const &neur : genom->neurons) {
          delete neur;
        }
        delete genom;
      }
      delete spec;
    }
  }
  if (!survived.size()) {
    // Since everything is free, this cannot work anymore
    // std::sort(speciesVector.begin(), speciesVector.end(),
    // compareSpeciesByTopFitness); survived.push_back(speciesVector[0]);
    printf("ERROR: survived.size() == 0");
    exit(-1);
  }
  speciesVector = survived;
}

void pool::removeWeakSpecies() {
  std::vector<species *> survived;
  double sum = getAverageFitness();
  for (auto const &spec : speciesVector) {
    if (floor(spec->averageFitness / sum) >= 1)
      survived.push_back(spec);
    else {
      for (auto const &genom : spec->genomes) {
        for (auto const &gen : genom->genes) {
          delete gen;
        }
        for (auto const &neur : genom->neurons) {
          delete neur;
        }
        delete genom;
      }
      delete spec;
    }
  }
  speciesVector = survived;
}

void pool::addToSpecies(genome *child) {
  for (auto const &spec : speciesVector) {
    if (genome::sameSpecies(spec->genomes[0], child)) {
      spec->genomes.push_back(child);
      return;
    }
  }
  species *newSpecies = new species();
  newSpecies->genomes.push_back(child);
  speciesVector.push_back(newSpecies);
}

void pool::setMaxFitness() {
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      if (genom->fitness > maxFitness)
        maxFitness = genom->fitness;
    }
  }
}

void pool::writeStats() {
  calculateFitnessStats();
  writeGenerationStats();
  writeTileProbabilityStats();
  writeMutationRateStats();
  writeNetworkStats();
  writeFitnessStats();
  std::ofstream fileHandle;

  fileHandle.open(runDir / "species.dat",
                  std::ofstream::out | std::ofstream::app);
  fileHandle << generation << "\t";
  for (auto const &spec : speciesVector)
    fileHandle << spec->speciesNumber << "\t" << spec->genomes.size() << "\t";
  fileHandle << std::endl;
  fileHandle.close();
}

void pool::calculateFitnessStats() {
  currentMeanFitness = 0.0;
  currentMinFitness = std::numeric_limits<double>::max();
  currentMaxFitness = std::numeric_limits<double>::min();

  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      currentMeanFitness += genom->fitness;
      currentMinFitness = std::min(genom->fitness, currentMinFitness);
      currentMaxFitness = std::max(genom->fitness, currentMaxFitness);
    }
  }
  currentMeanFitness /= population;
}

void pool::writeGenerationStats() {
  std::filesystem::path statsFile = runDir / "generation.csv";
  bool fileExists = std::filesystem::exists(statsFile);

  std::ofstream fileHandle(statsFile, std::ofstream::out | std::ofstream::app);

  if (!fileHandle) {
    throw std::runtime_error("Could not open generation.csv");
  }

  if (!fileExists) {
    fileHandle << "generation,meanFitness,targetPrecision,minFitness,maxFitness"
               << std::endl;
  }

  fileHandle << generation << "," << currentMeanFitness << ","
             << targetPrecision << "," << currentMinFitness << ","
             << currentMaxFitness << std::endl;
}

void pool::writeTileProbabilityStats() {
  std::ofstream fileHandle;

  // calculate tile probabilities
  // this is the probability to reach tile with value N.
  // This is calculated for the whole generation(tileProbabilityGeneration)
  // and the best genome in this generation(bestTileProbability).

  std::vector<double> tileProbabilityGeneration(16, 0.0);
  std::vector<double> bestTileProbability;
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      std::vector<double> tileProbability(16, 0.0);
      for (auto const &maxT : genom->maxTile) {
        for (unsigned char tile = 0; tile < 16; ++tile) {
          if (tile > maxT)
            break;
          tileProbability[tile]++;
        }
      }
      if (genom->fitness == currentMaxFitness) {
        bestTileProbability = tileProbability;
        for (auto &tile : bestTileProbability)
          tile /= genom->maxTile.size();
      }

      for (unsigned char tile = 0; tile < 16; ++tile)
        tileProbabilityGeneration[tile] +=
            (tileProbability[tile] / genom->maxTile.size()) / population;
    }
  }

  writeTileProbabilityStatsFile("tileProbability.csv",
                                tileProbabilityGeneration);
  writeTileProbabilityStatsFile("bestGenomeTileProbability.csv",
                                bestTileProbability);
}

void pool::writeTileProbabilityStatsFile(
    std::string filename, std::vector<double> tileProbabilities) {

  std::filesystem::path statsFile = runDir / filename;
  bool fileExists = std::filesystem::exists(statsFile);

  std::ofstream fileHandle(statsFile, std::ofstream::out | std::ofstream::app);

  if (!fileHandle) {
    throw std::runtime_error("Could not open file.");
  }

  if (!fileExists) {
    fileHandle << "generation";
    for (unsigned char tile = 1; tile < 16; ++tile) {
      fileHandle << "," << std::pow(2, tile);
    }
    fileHandle << std::endl;
  }

  fileHandle << generation;
  for (unsigned char tile = 1; tile < 16; ++tile) {
    fileHandle << "," << tileProbabilities[tile];
  }
  fileHandle << std::endl;
}

void pool::writeMutationRateStats() {
  std::filesystem::path statsFile = runDir / "mutationRates.csv";
  bool fileExists = std::filesystem::exists(statsFile);

  std::ofstream fileHandle(statsFile, std::ofstream::out | std::ofstream::app);

  if (!fileHandle) {
    throw std::runtime_error("Could not open mutationRates.csv");
  }

  if (!fileExists) {
    fileHandle << "generation,"
               << "weight,link,bias,node"
	       << "enable,disable,transfer,delete,step"
               << std::endl;
  }

  std::map<std::string, double> meanMutationRates = {
      {"weight", 0.0},   {"link", 0.0},   {"bias", 0.0},
      {"node", 0.0},     {"enable", 0.0}, {"disable", 0.0},
      {"transfer", 0.0}, {"delete", 0.0}, {"step", 0.0}};
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      for (auto const &mutation : genom->mutationRates)
        meanMutationRates[mutation.first] += mutation.second;
    }
  }

  fileHandle << generation << ","
	     << meanMutationRates["weight"] / population << ","
	     << meanMutationRates["link"] / population << ","
             << meanMutationRates["bias"] / population << ","
             << meanMutationRates["node"] / population << ","
             << meanMutationRates["enable"] / population << ","
             << meanMutationRates["disable"] / population << ","
             << meanMutationRates["transfer"] / population << ","
             << meanMutationRates["delete"] / population << ","
             << meanMutationRates["step"] / population
	     << std::endl;
}

void pool::writeNetworkStats() {
  std::filesystem::path statsFile = runDir / "network.csv";
  bool fileExists = std::filesystem::exists(statsFile);

  std::ofstream fileHandle(statsFile, std::ofstream::out | std::ofstream::app);

  if (!fileHandle) {
    throw std::runtime_error("Could not open network.csv");
  }

  if (!fileExists) {
    fileHandle << "generation,"
               << "neurons,activeInputNeurons,mutableNeurons,genes"
	       << "enabledGenes,disabledGenes,species,deltaThreshold"
               << std::endl;
  }

  // write network statistics mean count of: neurons, active neurons, mutable
  // neurons, genes, enabled genes, disabled genes, deleted genes?
  unsigned int neurons = 0, activeInputNeurons = 0, mutableNeurons = 0;
  unsigned int genes = 0, disabledGenes = 0, enabledGenes = 0;
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      genes += genom->genes.size();
      for (auto const &gen : genom->genes) {
        if (gen->enabled)
          enabledGenes++;
        else
          disabledGenes++;
      }
      neurons += genom->neurons.size();
      mutableNeurons += genom->neurons.size() - inputs;
      for (auto const &neur : genom->inputActivated) {
        if (neur)
          activeInputNeurons++;
      }
    }
  }

  fileHandle << generation << "," << neurons / population << ","
             << activeInputNeurons / population << ","
             << mutableNeurons / population << "," << genes / population
             << "," << enabledGenes / population << ","
             << disabledGenes / population << "," << speciesVector.size()
             << "," << deltaThreshold << std::endl;
}

void pool::writeFitnessStats() {
  std::filesystem::path statsFile = runDir / "fitness.csv";
  bool fileExists = std::filesystem::exists(statsFile);

  std::ofstream fileHandle(statsFile, std::ofstream::out | std::ofstream::app);

  if (!fileHandle) {
    throw std::runtime_error("Could not open fitness.csv");
  }

  if (!fileExists) {
    fileHandle << "generation,index,fitness" << std::endl;
  }

  unsigned int index = 0;
  for (auto const &spec : speciesVector)
    for (auto const &genom : spec->genomes) {
      fileHandle << generation << "," << index << "," << genom->fitness
                 << std::endl;
      index++;
    }
}

bool pool::setPrecision() {
  bool increasePrecision = false;
  for (auto const &spec : speciesVector) {
    double mean = 0.0, min = std::numeric_limits<double>::max(), max = 0.0;
    std::vector<double> boundary;
    std::vector<double> fitness;
    for (auto const &genom : spec->genomes) {
      fitness.push_back(genom->fitness);
      if (genom->fitness < min)
        min = genom->fitness;
      if (genom->fitness > max)
        max = genom->fitness;
      mean += genom->fitness;
    }
    boundary.push_back(min * (1 + targetPrecision));
    double upperBoundary = boundary[0];
    while (upperBoundary < max) {
      upperBoundary *= 1 + targetPrecision;
      boundary.push_back(upperBoundary);
    }
    std::vector<unsigned int> freq(boundary.size(), 0);
    std::sort(fitness.begin(), fitness.end());
    std::vector<double>::iterator fitnessIterator = fitness.begin();
    for (unsigned int i = 0; i < boundary.size(); i++) {
      while ((*fitnessIterator) < boundary[i] &&
             fitnessIterator != fitness.end()) {
        freq[i]++;
        fitnessIterator++;
      }
    }
    double entropy = 0;
    for (unsigned int i = 0; i < freq.size(); i++) {
      if (freq[i] != 0)
        entropy -= freq[i] * log(freq[i] * 1.0 / spec->genomes.size()) /
                   spec->genomes.size();
    }
    printf("entropy of species %d: %f %f\n", spec->speciesNumber, entropy,
           log(spec->genomes.size() * 1.0 / CULL_MINIMUM));
    if (!(entropy > log(spec->genomes.size() * 1.0 / CULL_MINIMUM))) {
      increasePrecision = true;
    }
  }
  if (generation < 1)
    return false;
  if (increasePrecision) {
    targetPrecision /= 1.1;
    printf("Rerun with precision %f\n", targetPrecision);
    return true;
  } else {
    printf("Finished this generation\n");
    targetPrecision *= 1.25;
    return false;
  }
}

void pool::checkVariance() {
  double mean = 0.0;
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      mean += genom->fitness / population;
    }
  }

  double variance = 0.0;
  for (auto const &spec : speciesVector) {
    for (auto const &genom : spec->genomes) {
      variance += std::pow(mean - genom->fitness, 2) / population;
    }
  }

  if (sqrt(variance) / mean < MUTATIONRATE_THRESHOLD) {
    for (auto const &spec : speciesVector) {
      for (auto const &genom : spec->genomes) {
        for (auto const &mutation : genom->mutationRates) {
          genom->mutationRates[mutation.first] *= MUTATIONRATE_FACTOR;
        }
      }
    }
  }
  printf("%f\n", sqrt(variance) / mean);
}

void pool::newGeneration() {
  if (setPrecision())
    return;

  // checkVariance();
  setMaxFitness();
  writeStats();
  cullSpecies(false);
  removeStaleSpecies();
  rankGenomes();

  for (auto const &spec : speciesVector)
    spec->calculateAverageFitness();

  // removeWeakSpecies();
  double avgFitness = getAverageFitness();

  std::vector<genome *> children;
  for (auto const &spec : speciesVector) {
    short breed = floor(spec->averageFitness / avgFitness) -
                  std::min((int)spec->genomes.size(), CULL_MINIMUM);
    printf("breed: %d\n", breed);
    for (unsigned short i = 0; i < breed; i++) {
      children.push_back(spec->breedChild());
    }
  }

  cullSpecies(true);
  unsigned short remaining = 0;
  for (auto const &spec : speciesVector) {
    remaining += spec->genomes.size();
  }
  printf("children.size() = %ld\n", children.size());
  printf("remaining after cull: %d\n", remaining);
  printf("speciesVector.size() = %ld\n", speciesVector.size());
  while (children.size() + remaining < population)
    children.push_back(
        speciesVector[(int)(distribution(generator) * speciesVector.size())]
            ->breedChild());

  for (auto const &child : children)
    addToSpecies(child);

  generation += 1;

  // save();
}

void pool::nextGenome() {
  std::vector<genome *> currentGenomes = speciesVector[currentSpecies]->genomes;
  printf("%d  %d  %d  %f  %f  %lu  %f\n", generation, currentSpecies,
         currentGenome, currentGenomes[currentGenome]->fitness,
         currentGenomes[currentGenome]->precision,
         currentGenomes[currentGenome]->scores.size(), maxFitness);
  currentGenome += 1;
  if (currentGenome >= currentGenomes.size()) {
    currentGenome = 0;
    currentSpecies += 1;
    if (currentSpecies >= speciesVector.size()) {
      currentSpecies = 0;
      newGeneration();
    }
  }
}

void pool::save() {
  std::ofstream ofs;
  ofs.open(runDir / "savefile", std::ofstream::out);
  ofs << *this;
  ofs.close();
}

pool pool::load(std::string filename) {
  pool output;
  std::ifstream ifs;
  ifs.open(filename.c_str());
  ifs >> output;
  return output;
}

std::ostream &operator<<(std::ostream &os, const pool &p) {
  os << p.runDir << std::endl;
  os << p.generation << std::endl;
  os << p.innovation << std::endl;
  //    os << p.currentSpecies << std::endl;
  //    os << p.currentGenome << std::endl;
  os << p.maxFitness << std::endl;
  os << p.population << std::endl;
  os << p.inputs << std::endl;
  os << p.outputs << std::endl;
  os << p.targetPrecision << std::endl;

  os << p.speciesVector.size() << std::endl;
  for (auto const &spec : p.speciesVector)
    os << *spec;

  return os;
}

std::istream &operator>>(std::istream &is, pool &p) {
  is >> p.runDir;
  is >> p.generation;
  is >> p.innovation;
  p.currentSpecies = 0;
  p.currentGenome = 0;
  is >> p.maxFitness;
  is >> p.population;
  is >> p.inputs;
  is >> p.outputs;
  is >> p.targetPrecision;
  unsigned int speciesVectorSize;
  is >> speciesVectorSize;
  for (unsigned int i = 0; i < speciesVectorSize; ++i) {
    species *speciesPointer = new species;
    is >> *speciesPointer;
    p.speciesVector.push_back(speciesPointer);
  }
  for (auto const &spec : p.speciesVector)
    for (auto const &genom : spec->genomes)
      genom->poolPointer = &p;
  return is;
}
