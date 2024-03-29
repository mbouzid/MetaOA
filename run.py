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
		
	dirSol = "Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_sol"
	dirOut = "Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_out"
	
	if not os.path.exists(dirOut):
		os.mkdir(dirOut)
		
	if not os.path.exists(dirSol):
		os.mkdir(dirSol)
		
	for i in range(nbRun):
		out = dirOut+"\\Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_"+str(i)+".out"	
		sol = dirSol+"\\Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_"+str(i)+".sol"	
		if not os.path.exists(out):
			print(sol)
			cmd = "x64\\Release\\MetaOA.exe "+dat+" "+out+ " >> "+ sol
			os.system(cmd)

