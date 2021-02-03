import matplotlib.pyplot as plt
from matplotlib import cm
import pandas as pd
import numpy as np
import sys
import os

assert (len(sys.argv) > 1), "Two arguments must be given"

## instdir informations
instdir = sys.argv[1]

def list_files_recursive(path):
    """
    Function that receives as a parameter a directory path
    :return list_: File List and Its Absolute Paths
    """
    files = []

    # r = root, d = directories, f = files
    for r, d, f in os.walk(path):
        for file in f:
            files.append(os.path.join(r, file))

    lst = [file for file in files]
    return lst


dircsv = list_files_recursive(instdir)	
	
#fig = plt.figure()
#ax1 = fig.add_subplot(111)

cols = ["exchangeRandom","inversion","swap","scramble","randomShuffle","addRandomly","shift"]

n = len(cols)
m = 400
avg = pd.DataFrame(columns=cols)
avg = avg.fillna(0.0)

for col in cols:
	avg[col] = [0.0] * m

for csv in dircsv:
	df = pd.read_csv(csv,delimiter=',',decimal=".")
	#print(df)
	for col in cols:
		for i in range(m):
			avg[col][i] += float(df[col][i])/float(n)
	


print(avg)
ax = avg.plot()
ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.05),ncol=4)
#ax1.legend(loc=2)


	
plt.show()