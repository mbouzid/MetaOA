import os
import re
import sys
import math

assert (len(sys.argv) > 1), "An arguments must be given"

## instdir informations

dir = sys.argv[1]
nbRun = 10

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

dats = list_files_recursive(dir)
nb = len(dats)


for dat in dats:
	print(dat)
	n,tau,R,instance = re.search("Dataslack_(.*)orders_Tao(.*)R(.*)_(.*).dat",dat).groups(0)
		
	out = "Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+".out"
	
	if not os.path.exists(out):
		for i in range(nbRun):
			
			cmd = "x64\\Release\\MetaOA.exe "+dat+">> "+out
			#print(out)
			os.system(cmd)

