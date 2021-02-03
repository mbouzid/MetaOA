import sys
import re
import os

assert (len(sys.argv) > 1), "An argument must be given"

dir = sys.argv[1]


def list_dir_recursive(path):
    """
    Function that receives as a parameter a directory path
    :return list_: Dir List and Its Absolute Paths
    """
    dirs = []

    # r = root, d = directories, f = files
    for r, d, f in os.walk(path):
        for dir in d:
            dirs.append(os.path.join(r, dir))

    lst = [dir for dir in dirs]
    return lst
	
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
	
outs = list_dir_recursive(dir)
for out in outs:
	files = list_files_recursive(out)
	for file in files:
		if file.find('max') != -1:
			os.system("copy "+file+" ..")