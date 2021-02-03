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


for out in outs:
	n,tau,R,instance = re.search('Dataslack_(.*)orders_Tao(.*)R(.*)_(.*).out',out).groups()
	
	dirout = "Dataslack_"+str(n)+"orders_Tao"+str(tau)+"R"+str(R)+"_"+str(instance)+"_out"
	
	if not os.path.exists(dirout):
		os.mkdir(dirout)
		
	with open(out) as f:
		k = 0
		lines = f.readlines()
		buffer = ""
		max = 0
		for line in lines:
			if line.find('time') != -1:
				buffer += line
				#print(buffer)
				k+=1
					
				outSol = dirout+"\\"+"Dataslack_"+str(n)+"orders_Tao"+str(tau)+"R"+str(R)+"_"+str(instance)+"_"+str(k)+".out"
					
					
				output = open(outSol, "w")
				output.write(buffer)
				output.close()
					
				buffer = ""
			else:
				buffer += line
		print(k)