import os
import re
import sys

assert (len(sys.argv) > 2), "Two arguments must be given"

## instdir informations

mod = sys.argv[1]
instdir = sys.argv[2]

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

dats = list_files_recursive(instdir)
nb = len(dats)
i = 1
for dat in dats:
	print(str(i)+"/"+str(nb))
	i+=1
	out = dat.replace(".dat",".out")
	if not os.path.exists(out):
		cmd = "oplrun "+mod+" "+dat+" > "+out
		print(cmd)
		os.system(cmd)

