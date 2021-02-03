import sys
import re
import os

assert (len(sys.argv) > 1), "An argument must be given"

## resdir informations
resdir = sys.argv[1]

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

res = list_files_recursive(resdir)


print("n\tTao\tR\tobj\ttime\tgap\tseq")
for filename in res:
	isOutFile = re.search('Dataslack_(.*)orders_Tao(.*)R(.*)_(.*).out',filename)
	if isOutFile is not None:
		n,Tao,R,instance = re.search('Dataslack_(.*)orders_Tao(.*)R(.*)_(.*).out',filename).groups()

		with open(filename) as f:
			lines = f.readlines()
			for line in lines:
				findRes = re.search("RESULTS(.*)",line)
				if findRes is not None:
					res = findRes.group(0).split(',')
					fracTao = float(Tao)/10
					fracR = float(R)/10
					print(n+"\t"+str(fracTao)+"\t"+str(fracR)+"\t"+res[1]+"\t"+res[2]+"\t"+res[3],end='')

				findSeq = re.search("SEQUENCE(.*)",line)
				if findSeq is not None:
					res = findSeq.group(0).split(',')
					print("\t"+res[1])