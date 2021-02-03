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

print("n\tTau\tR\tinstance\tk\tobj\ttime")
for out in outs:
	repo,n,tau,R,instance,k = re.search('(.*)Dataslack_(.*)orders_Tao(.*)R(.*)_(.*)_(.*).sol',out).groups()
	display = n+"\t"+tau+"\t"+R+"\t"+instance+"\t"+k
	with open(out) as f:
		lines = f.readlines()
		for line in lines:
			
			isResult = re.search('obj=(.*)',line)
			if isResult is not None:
				obj = isResult.groups(0)[0]
				display += "\t"+ obj
				
			isTime = re.search('time=(.*)',line)
			if isTime is not None:
				time = isTime.groups(0)[0]
				display += "\t"+ time
	print(display)