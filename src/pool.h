#ifndef POOL_H
#define POOL_H

#include "species.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class pool {
public:
  std::filesystem::path runDir;
  std::vector<species *> speciesVector;
  unsigned short generation;
  unsigned int innovation;
  unsigned short currentSpecies;
  unsigned short currentGenome;
  double maxFitness;
  unsigned short population;
  unsigned short inputs;
  unsigned short outputs;
  double targetPrecision;
  double deltaThreshold;

  double currentMeanFitness;
  double currentMinFitness;
  double currentMaxFitness;

  pool();
  pool(unsigned short inputs, unsigned short outputs,
       unsigned short population);
  ~pool();

  std::string getCurrentTimestamp();
  std::filesystem::path createRunDirectory();
  void saveConstants();

  unsigned int newInnovation();
  void rankGenomes();
  double getAverageFitness();
  void cullSpecies(bool cutToMin);
  void removeStaleSpecies();
  void removeWeakSpecies();
  void addToSpecies(genome *child);
  void setMaxFitness();
  void writeStats();
  void calculateFitnessStats();
  void writeGenerationStats();
  void writeTileProbabilityStats();
  void writeTileProbabilityStatsFile(std::string filename,
                                     std::vector<double> tileProbabilities);
  void writeMutationRateStats();
  void writeNetworkStats();
  void newGeneration();
  bool setPrecision();
  void checkVariance();
  void nextGenome();
  void save();
  static pool load(std::string filename);

  friend std::ostream &operator<<(std::ostream &os, const pool &p);
  friend std::istream &operator>>(std::istream &is, pool &p);
};

#endif // POOL_H
