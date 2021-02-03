import sys
import re
import os

assert (len(sys.argv) > 1), "An argument must be given"

dir = sys.argv[1]


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

outs = list_files_recursive(dir)
n,tau,R,instance = re.search('Dataslack_(.*)orders_Tao(.*)R(.*)_(.*)_out',dir).groups()

objVals = []
names = []
for out in outs:
	
	
	with open(out) as f:
		lines = f.readlines()
		for line in lines:
			if line.find("objVal") != -1:
				objVal = float(line.split('=')[1])
				objVals.append(objVal)
				names.append(out)

imax = objVals.index(max(objVals))

fname = names[imax]

outname = dir+"\\"+"Dataslack_"+str(n)+"orders_Tao"+str(tau)+"R"+str(R)+"_"+str(instance)+"_max.out"
os.system("copy "+fname+" "+outname)
