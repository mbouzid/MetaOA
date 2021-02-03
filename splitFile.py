import sys
import re
import os

assert (len(sys.argv) > 1), "An argument must be given"

out = sys.argv[1]

with open(out) as f:
	k = 0
	lines = f.readlines()
	buffer = ""
	fname = ""
	max = 0
	tau = 0
	R = 0
	for line in lines:
	
		if line.find('Dataslack') != -1:
			print(line)
			tauPrev = tau
			RPrev = R
			n,tau,R,instance = re.search("Dataslack_(.*)orders_Tao(.*)R(.*)_(.*).dat",line).groups(0)
			
			outSol = "Dataslack_"+str(n)+"orders_Tao"+str(tau)+"R"+str(R)+"_"+str(instance)+"_"+str(k)+".out"
			print(outSol)
	
		if line.find('time') != -1:
			buffer += line
			#print(buffer)
			k+=1
			
			
			#outSol = "Dataslack_"+str(n)+"orders_Tao"+str(tau)+"R"+str(R)+"_"+str(instance)+"_"+str(k)+".out"
					
			#print(buffer)
			#output = open(outSol, "w")
			#output.write(buffer)
			#output.close()
					
			buffer = ""
		else:
			buffer += line
	print(k)