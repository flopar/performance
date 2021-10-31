#!/home/element/programms/anaconda/bin/python3.8

import csv
import sys
import matplotlib.pyplot as plt
import numpy as np



def makeDiagram(elements : list):
    csvData = np.array(elements)

    # Plot the figure
    fig, ax = plt.subplots()
        
    x_values = range(len(csvData[0][1:]))  
     
    for x in csvData:
        plt.plot(x_values, x[1:], label=x[0])
    ax.legend()
    fig.tight_layout()
    plt.axis([0, 65, 0, 32])
    print("saving diagram")
    plt.savefig("Stats.png")



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



if __name__ == '__main__':
    csvData = []
    filePath = str(sys.argv[1])
    
    # Read the csv file
    with open(filePath, newline = '') as csvfile:
        spamreader = csv.reader(csvfile, delimiter =',')
        for row in spamreader:
            csvData.append(row)
    print(csvData)
    makeDiagram(csvData) 
    


