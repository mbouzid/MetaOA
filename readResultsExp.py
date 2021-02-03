import sys
import re
import os

assert (len(sys.argv) > 1), "An argument must be given"

mainDir = sys.argv[1]


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

def list_dir_recursive(path,value):
    """
    Function that receives as a parameter a directory path
    :return list_: dir List 
    """
    directories = []

    # r = root, d = directories, f = files
    for r, d, f in os.walk(path):
        for directory in d:
            if directory.find(value) != -1:
                directories.append(os.path.join(r,directory))
	
    lst = [directory for directory in directories]
    return lst
	

dirs = list_dir_recursive(mainDir,"sol")
print("n\tTau\tR\tinstance\tk\tobj\ttime")
for dir in dirs:
	outs = list_files_recursive(dir)
	for out in outs:
		repo,n,tau,R,instance,k = re.search('(.*)Dataslack_(.*)orders_Tao(.*)R(.*)_(.*)_(.*).sol',out).groups()
		display = n+"\t"+tau+"\t"+R+"\t"+instance+"\t"+k
		with open(out) as f:
			lines = f.readlines()
			for line in lines:
				
				isResult = re.search('objVal=(.*)',line)
				if isResult is not None:
					obj = isResult.groups(0)[0]
					display += "\t"+ obj
					
				isTime = re.search('time=(.*)',line)
				if isTime is not None:
					time = isTime.groups(0)[0]
					display += "\t"+ time
		print(display)