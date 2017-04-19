import matplotlib.pyplot as plt
import sys

if len(sys.argv) != 2:
	print("Usage: plot.py NAME")
	quit()

name = sys.argv[1]

colors = ["#A00000", "#FF0000", "#FF8080","#FFC0C0", "#0000A0", "#0000FF", "#8080FF", "#C0C0FF", "#00A000", "#00FF00"]

generationFile = name +"_generation.dat"
generationData = []
with open(generationFile, "r") as f:
	for line in f:
		generationData.append([float(x) for x in line.split()])


transposedGenerationData = list(zip(*generationData))
fig1, ax11 = plt.subplots()
ax11.set_xlabel('Generation')
ax11.set_ylabel('Score')

ax11.plot(transposedGenerationData[0], transposedGenerationData[3], 'r')
ax11.plot(transposedGenerationData[0], transposedGenerationData[1], 'b')
ax11.plot(transposedGenerationData[0], transposedGenerationData[4], 'g')

ax12 = ax11.twinx()
ax12.set_ylabel('Precision')

ax12.plot(transposedGenerationData[0], transposedGenerationData[2], '0.75')

fig1.tight_layout()

tileProbabilityFile = name + "_tileprobability.dat"
tileProbabilityData = []
with open(tileProbabilityFile, "r") as f:
	for line in f:
		tileProbabilityData.append([float(x) for x in line.split()])

transposedTileProbabilityData = list(zip(*tileProbabilityData))

fig2, ax21 = plt.subplots()
ax21.set_xlabel('Generation')
ax21.set_ylabel('Success Rate')

plt.ylim(-0.05, 1.1)

ax21.plot(transposedTileProbabilityData[0], transposedTileProbabilityData[7], colors[0], label='64')
ax21.plot(transposedTileProbabilityData[0], transposedTileProbabilityData[8], colors[1], label='128')
ax21.plot(transposedTileProbabilityData[0], transposedTileProbabilityData[9], colors[2], label='256')
ax21.plot(transposedTileProbabilityData[0], transposedTileProbabilityData[10], colors[3], label='512')
ax21.plot(transposedTileProbabilityData[0], transposedTileProbabilityData[11], colors[4], label='1024')
ax21.plot(transposedTileProbabilityData[0], transposedTileProbabilityData[12], colors[5], label='2048')

ax21.legend(loc=9, ncol=6, mode="expand", borderaxespad=0.)

fig2.tight_layout()

bestGenomeTileProbabilityFile = name + "_bestGenomeTileprobability.dat"
bestGenomeTileProbabilityData = []
with open(bestGenomeTileProbabilityFile, "r") as f:
	for line in f:
		bestGenomeTileProbabilityData.append([float(x) for x in line.split()])

transposedBestGenomeTileProbabilityData = list(zip(*bestGenomeTileProbabilityData))

fig3, ax31 = plt.subplots()
ax31.set_xlabel('Generation')
ax31.set_ylabel('Success Rate')

plt.ylim(-0.05, 1.1)

ax31.plot(transposedBestGenomeTileProbabilityData[0], transposedBestGenomeTileProbabilityData[7], colors[0], label='64')
ax31.plot(transposedBestGenomeTileProbabilityData[0], transposedBestGenomeTileProbabilityData[8], colors[1], label='128')
ax31.plot(transposedBestGenomeTileProbabilityData[0], transposedBestGenomeTileProbabilityData[9], colors[2], label='256')
ax31.plot(transposedBestGenomeTileProbabilityData[0], transposedBestGenomeTileProbabilityData[10], colors[3], label='512')
ax31.plot(transposedBestGenomeTileProbabilityData[0], transposedBestGenomeTileProbabilityData[11], colors[4], label='1024')
ax31.plot(transposedBestGenomeTileProbabilityData[0], transposedBestGenomeTileProbabilityData[12], colors[5], label='2048')

ax31.legend(loc=9, ncol=6, mode="expand", borderaxespad=0.)

fig3.tight_layout()

mutationRatesFile = name + "_mutationrates.dat"
mutationRatesData = []
with open(mutationRatesFile, "r") as f:
	for line in f:
		mutationRatesData.append([float(x) for x in line.split()])

transposedMutationRatesData = list(zip(*mutationRatesData))

fig4, ax41 = plt.subplots()
ax41.set_xlabel('Generation')
ax41.set_ylabel('Mean Mutation Rates')

weightHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[1], marker='.', label='weight')
linkHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[2], marker='o', label='link')
biasHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[3], marker='v', label='bias')
nodeHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[4], marker='^', label='node')
enableHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[5], marker='<', label='enable')
disableHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[6], marker='>', label='disable')
transferHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[7], marker='s', label='transfer')
deleteHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[8], marker='p', label='delete')
stepHandle = ax41.plot(transposedMutationRatesData[0], transposedMutationRatesData[9], marker='*', label='step')


ax41.legend(loc=9, ncol=5, mode="expand", borderaxespad=0.)

fig4.tight_layout()

statsFile = name + "_stats.dat"
statsData = []
with open(statsFile, "r") as f:
	for line in f:
		statsData.append([float(x) for x in line.split()])

transposedStatsData = list(zip(*statsData))

fig5, ax51 = plt.subplots()

plt.ylim(0, max(transposedStatsData[7])+1)

ax51.set_xlabel('Generation')
ax51.set_ylabel('Number of Species')

ax51.plot(transposedStatsData[0], transposedStatsData[7])

fig5.tight_layout()

fig6, ax61 = plt.subplots()

ax62 = ax61.twinx()

ax61.plot(transposedStatsData[0], transposedStatsData[1], color=colors[0], label='total neurons')
ax61.plot(transposedStatsData[0], transposedStatsData[2], color=colors[1], label='active input neurons')
ax61.plot(transposedStatsData[0], transposedStatsData[3], color=colors[2], label='mutable neurons')
ax62.plot(transposedStatsData[0], transposedStatsData[4], color=colors[4], label='total genes')
ax62.plot(transposedStatsData[0], transposedStatsData[5], color=colors[5], label='enabled genes')
ax62.plot(transposedStatsData[0], transposedStatsData[6], color=colors[6], label='disabled genes')

ax61.legend(loc=2)
ax62.legend(loc=3)

plt.show()