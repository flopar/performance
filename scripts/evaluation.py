#!/usr/bin/python

import csv
import sys
import matplotlib.pyplot as plt
import numpy as np


#---------------------------------------------------------------------------------------#
# This function calculates the x-Positions where the bars will be placed for one xValue #
#---------------------------------------------------------------------------------------#
def findXValuePos(valuesProMeasurement, xValue, barWidth):
	# We need to differentiate between odd/even number of bars pro measurement
	EvenOrOdd = 0
	if valuesProMeasurement%2 == 0:
		EvenOrOdd = barWidth/2

	# Define the start value
	xValue -= int(valuesProMeasurement/2)*barWidth
	xValue += EvenOrOdd

	# Calculate the positions for one measurement
	xPositions = [xValue+value for value in np.arange(0, valuesProMeasurement+barWidth, barWidth)]
	return xPositions

def createBarDiagramm(xValues=[], yValues=[], barLegend=[], name="", xLabel=None, yLabel=None):
	barWidth = 0.9/len(yValues)
	numberOfBars = len(yValues)
	if numberOfBars != len(barLegend):
		raise ValueError("List of values and list of Legend-Names must have the same size")
	# We set the max size to 9, because often we use it for automatical measurement and the difference between each workload is 10.
	# this way we ensure that we will always have a space of 1 between the bars of each measurement
	if numberOfBars > 9:
		raise ValueError("Too many values for one measurement")
	
	posList = [findXValuePos(numberOfBars, value, barWidth) for value in np.arange(len(xValues))]
	# Sort the pairs. The list will look like [[x-n], ..., [x-1], [x], [x+1], ..., [x+n]]. These will be the x-Positions of the bars
	#                                           "n" cannot have a higher value than 4 because numberOfBars cannot be higher than 9
	sortedPosList = [[x[a] for x in posList] for a in range(numberOfBars)]
	
	# Plot the figure
	fig, ax = plt.subplots()
	barsList = [ax.bar(value, bars, width=barWidth, label=barLegend[barsCounter]) for posCounter, value in enumerate(sortedPosList) for barsCounter, bars in enumerate(yValues) if posCounter == barsCounter]
	ax.legend()
	ax.set_xticks(np.arange(len(xValues)))
	ax.set_xticklabels(xValues)
	fig.tight_layout()
	if xLabel != None:
		plt.xlabel(xLabel)
	if yLabel != None:
		plt.ylabel(yLabel)
	fig.suptitle(name)
	plt.savefig(name)


#-------------------#
#       MAIN        #
#-------------------#

csvData = []
repetitions = int(sys.argv[1])
filePath = str(sys.argv[2])
fileName = filePath
meanValue = 0
statisticMean = []
repetitionMean = []

with open(filePath, newline ='') as csvfile:
	spamreader = csv.reader(csvfile, delimiter =',')
	for row in spamreader:
		csvData.append(row)

csvDataLen = len(csvData)
labels = csvData[0]
csvEntryLen = len(csvData[1])

# Calculate the measurement's  mean values 
#for times in range(1, csvDataLen, repetitions):
#	for statistics in range(csvEntryLen):
#		for value in range(repetitions):
#			meanValue = meanValue + int(csvData[times+value][statistics])
#		meanValue = meanValue / repetitions
#		statisticMean.append(int(meanValue))
#		meanValue = 0
#	repetitionMean.append(statisticMean[:])
#	statisticMean.clear()

#repetitionMeanLen = len(repetitionMean)

# Sort the elements
#sortedEntry = []
#sortedList = []
#dictionary = {}
#for numberOfStatistics in range(csvEntryLen):
#	for numberOfValues in range(repetitionMeanLen):
#		sortedEntry.append(repetitionMean[numberOfValues][numberOfStatistics])
#	sortedList.append(sortedEntry[:])
#	dictionary[labels[numberOfStatistics]] = sortedEntry[:]
#	sortedEntry.clear()

# Check filepath
#separator = ""
#if filePath.find("/") != -1: 
	# Unix path
#	separator = "/"
#elif filePath.find("\\") != 1:
	# Windows path
#	separator = "\\"
#else: 
	# File is in the same directory with the executable
#	pass

# Get device name from path
#if separator == "":
#	fileName = fileName.rstrip(".csv")
#else:
#	split = filePath.split(separator)
#	fileName = split[len(split)-1].rstrip(".csv")


# Create Diagramms
# Plot the Package Diagramm
#createBarDiagramm(xValues=dictionary["Set Workload"], yValues=[dictionary["Seen Packages"], dictionary["Received Packages"], dictionary["Lost Packages"], dictionary["Resent Packages"]], barLegend=["Seen Packages", "Received Packages", "Lost Packages", "Resent Packages"], name=fileName+"-PackageDiagramm", xLabel="Workload %", yLabel="Packages")

# Plot Resent Package Diagram
#plt.figure(2)
#plt.plot(dictionary["Set Workload"], dictionary["Resent Packages"])
#plt.xlabel("8-Threads-Workload %")
#plt.ylabel("Packages")
#plt.title(fileName+"-Resent Package Diagramm")
#plt.savefig(fileName+"-ResentPackageDiagramm.png") 

# Plot Resend Commands Diagram
#plt.figure(3)
#plt.plot(dictionary["Set Workload"], dictionary["Resend Commands"])
#plt.xlabel("8-Threads-Workload %")
#plt.ylabel("Commands")
#plt.title(fileName+"-Resend Commands Diagramm")
#plt.savefig(fileName+"-ResendCommandDiagramm.png")

# Plot Images/FrameID Diagramm
#createBarDiagramm(xValues=dictionary["Set Workload"], yValues=[dictionary["Processed Images"], dictionary["Frame ID"]], barLegend=["Processed Images", "Frame ID"], name=fileName+"-ProcessedImages_FrameID_Diagramm", xLabel="8-Threads-Workload %", yLabel="Images/FrameID")


# Plot Block Diagramm
#createBarDiagramm(xValues=dictionary["Set Workload"], yValues=[dictionary["Blocks discarded for any reason"], dictionary["Blocks discarded - no buffer available"], dictionary["Blocks skipped"]], barLegend=["Blocks discarded for any reason", "Blocks discarded - no buffer available", "blocks skipped"], name=fileName+"-BlockDiagramm", xLabel="8-Threads-Workload %", yLabel="Blocks")

# Plot Buffer Diagramm
#createBarDiagramm(xValues=dictionary["Set Workload"], yValues=[dictionary["Incomplete Buffer"], dictionary["Underrun Buffers"]], barLegend=["Incomplete Buffers", "Underrung Buffers"], name=fileName+"-BufferDiagramm", xLabel="8-Threads-Workload %", yLabel="Buffers")

# Plot Average Workload Diagramm
#createBarDiagramm(xValues=dictionary["Set Workload"], yValues=[dictionary["Average CPU Workload"], dictionary["Average Process Workload"]], barLegend=["Average CPU Workload", "Average Process Workload"], name=fileName+"-AverageWorkloadDiagramm", xLabel="8-Threads-Workload %", yLabel="Workload")
