import matplotlib.pyplot as plt
from matplotlib import cm
import pandas as pd
import numpy as np
import sys
import os

assert (len(sys.argv) > 1), "Two arguments must be given"

csv = sys.argv[1]

cols = ["exchange","exchangeRandom","inversion","swap","scramble","randomShuffle","add","addRandomly","shift","remove"]

n = len(cols)
m = 400

df = pd.read_csv(csv,delimiter=',',decimal=".")

ax = df.plot()
ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.05),ncol=4)


	
plt.show()