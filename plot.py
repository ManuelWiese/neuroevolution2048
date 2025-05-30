import argparse
import csv
import pathlib
import random

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def main():
        arguments = parse_arguments()

        plot_generation_statistics(arguments.run_dir)

        plot_tile_probabilities(
                arguments.run_dir,
                "Mean tile probabilities",
                "tileProbability.csv"
        )

        plot_tile_probabilities(
                arguments.run_dir,
                "Best genome tile probabilities",
                "bestGenomeTileProbability.csv"
        )

        plot_mutation_rates(arguments.run_dir)
        plot_network_stats(arguments.run_dir)
        plot_fitness_heatmap(arguments.run_dir)

        plt.show()


def parse_arguments():
        parser = argparse.ArgumentParser()
        parser.add_argument("run_dir", type=pathlib.Path)
        arguments = parser.parse_args()

        return arguments


def plot_generation_statistics(run_dir: pathlib.Path):
        data = pd.read_csv(run_dir / "generation.csv")
        fig, ax1 = plt.subplots()

        ax1.set_title("Fitness and Precision")

        ax1.set_xlabel("Generation")
        ax1.set_ylabel("Score")

        ax1.plot(data["generation"], data["minFitness"], color="r", label="minFitness")
        ax1.plot(data["generation"], data["maxFitness"], color="g", label="maxFitness")
        ax1.plot(data["generation"], data["meanFitness"], color="b", label="meanFitness")

        ax2 = ax1.twinx()
        ax2.set_ylabel("Precision")

        ax2.plot(data["generation"], data["targetPrecision"], color='0.75', label="targetPrecision")

        ax1.legend()
        ax2.legend()
        fig.tight_layout()


def plot_tile_probabilities(
                run_dir: pathlib.Path,
                title: str,
                stats_file: str
):
        colors = [
                "#A00000",
                "#FF0000",
                "#FF8080",
                "#FFC0C0",
                "#0000A0",
                "#0000FF",
                "#8080FF",
                "#C0C0FF",
                "#00A000",
                "#00FF00"
        ]

        data = pd.read_csv(run_dir / stats_file)

        fig, ax1 = plt.subplots()

        ax1.set_title(title)

        ax1.set_xlabel("Generation")
        ax1.set_ylabel("Probability to reach tile")

        ax1.set_ylim(-0.05, 1.05)

        for index, tile in enumerate(("64", "128", "256", "512", "1024", "2048", "4096")):
                ax1.plot(data["generation"], data[tile], colors[index], label=tile)

        ax1.legend(loc=9, ncol=6, mode="expand", borderaxespad=0.)
        fig.tight_layout()


def plot_mutation_rates(run_dir: pathlib.Path):
        data = pd.read_csv(run_dir / "mutationRates.csv")
        fig, ax1 = plt.subplots()

        ax1.set_title("Mutation Rates")

        ax1.set_xlabel("Generation")
        ax1.set_ylabel("Mean Mutation Rates")

        markers = {
                "weight": ".",
                "link": "o",
                "bias": "v",
                "node": "^",
                "enable": "<",
                "disable": ">",
                "transfer": "s",
                "delete": "p",
                "step": "*"
        }

        for label, marker in markers.items():
                ax1.plot(data["generation"], data[label], marker=marker, label=label)

        ax1.legend(loc=9, ncol=5, mode="expand", borderaxespad=0.)
        fig.tight_layout()


def plot_network_stats(run_dir: pathlib.Path):
        data = pd.read_csv(run_dir / "network.csv")
        fig, ax1 = plt.subplots()

        ax1.set_title("Network Statistics")

        ax1.set_xlabel("Generation")
        ax1.set_ylabel("Count")

        labels = (
                "neurons",
                "activeInputNeurons",
                "mutableNeurons",
                "genes",
                "enabledGenes",
                "disabledGenes",
                "species"
        )
        for label in labels:
                ax1.plot(data["generation"], data[label], label=label)

        ax1.legend(loc=2)
        fig.tight_layout()


def plot_fitness_heatmap(run_dir: pathlib.Path):
        data = pd.read_csv(run_dir / "fitness.csv")

        X = data["generation"]
        Y = data["fitness"]

        hist, xedges, yedges = np.histogram2d(X, Y, bins=(max(data["generation"])+1, 150))
        extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]

        fig, ax1 = plt.subplots()

        ax1.set_title("Fitness Heatmap")
        ax1.set_xlabel("Generation")
        ax1.set_ylabel("Fitness")
        
        ax1.imshow(hist.T, extent=extent, origin="lower", aspect="auto", interpolation="none")
        

if __name__ == "__main__":
        main()


# name = arguments.base_name

# colors = [
#         "#A00000",
#         "#FF0000",
#         "#FF8080",
#         "#FFC0C0",
#         "#0000A0",
#         "#0000FF",
#         "#8080FF",
#         "#C0C0FF",
#         "#00A000",
#         "#00FF00"
# ]

# randomColors = [(random.random(), random.random(), random.random()) for i in range(1024)]

# speciesFile = name + "_species.dat"
# speciesData = []
# with open(speciesFile, "r") as f:
# 	for line in f:
# 		speciesData.append([float(x) for x in line.split()])

# fig7, ax71 = plt.subplots()

# for generation in range(len(speciesData)):
# 	species = 1
# 	populationSum = 0
# 	while species < len(speciesData[generation]):
# 		color = randomColors[int(speciesData[generation][species])%1024]
# 		population = speciesData[generation][species+1]
# 		x = [generation, generation+1]
# 		y1 = [populationSum, populationSum]
# 		y2 = [populationSum+population, populationSum+population]
# 		ax71.fill_between(x, y1, y2, facecolor=color, lw=0.0)
# 		#plt.plot()
# 		species += 2
# 		populationSum += population

# plt.show()
