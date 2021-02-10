import os
import re
import sys
import math

assert (len(sys.argv) > 1), "An arguments must be given"

## instdir informations

dir = sys.argv[1]
nbRun = 10

#Initial population	alpha	beta	pm	pc

optExp =[100,1000,0.85,0.1,0.7,0.9]


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

k = 1
popsize = optExp[0]
nbIter = optExp[1]
alpha = optExp[2]
beta = optExp[3]
pm = optExp[4]
pc = optExp[5]
	
dirExp = "optExp\\"
if not os.path.exists(dirExp):
	os.mkdir(dirExp)
		
for dat in dats:
	print(dat)
	n,tau,R,instance = re.search("Dataslack_(.*)orders_Tao(.*)R(.*)_(.*).dat",dat).groups(0)
			
	dirSol = dirExp+"Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_sol\\"
	dirOut = dirExp+"Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_out\\"
		
	if not os.path.exists(dirOut):
		os.mkdir(dirOut)
			
	if not os.path.exists(dirSol):
		os.mkdir(dirSol)
		
	for i in range(nbRun):
		out = dirOut+"Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_"+str(i)+".out"	
		sol = dirSol+"Dataslack_"+n+"orders_Tao"+tau+"R"+R+"_"+instance+"_"+str(i)+".sol"	
		if not os.path.exists(out):
			print(sol)
			cmd = "x64\\Release\\MetaOA.exe "+dat+" "+out+" "+str(nbIter)+" "+str(pm)+ " "+str(pc)+" "+str(alpha)+" "+str(beta)+" "+str(popsize)+" >> "+ sol
			#print(cmd)
			os.system(cmd)
