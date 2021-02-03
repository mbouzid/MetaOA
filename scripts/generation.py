import numpy as np
import math
import sys



n = 20
R = [0.1,0.5,0.9]
Tau = [0.1,0.5]
nb = 11
nbInst = len(R)*len(Tau)


def write_vec(v,name):
	s = name + " = ["
	s += str(v[0])
	for i in range(1,len(v)):
		s += "," + str(v[i])
	s += "];\n"
	return(s)

def write_mat(m,n,name):
	s = name + " = ["
	for i in range(n):
		s += "[" + str(m[i][0])
		for j in range(1,len(m[i])):
			s +="," + str(m[i][j]) 
		s += "]\n"
	s += "];\n"
	return(s)
	
def maxi(s,n,i):
	m = []
	for j in range(n):
		m.append(s[j][i])
	return max(m)


_ti = "ti"
_disj = "disj"

modes = [_disj,_ti]

dummies = {_ti: {0}, _disj :{0,n+1} }
	
nbval = {}
nbval[_ti] = n+1
nbval[_disj] = n+2

outputname = {}
for tau in Tau:
	for r in R:
	
		for k in range(1,nb):
			
			prefix = "Dataslack_"+str(n)+"orders_Tao"+str(int(tau*10))+"R"+str(int(r*10))+"_"+str(k)
			
			for mode in modes:
				outputname[mode] = prefix+"_"+mode+".dat"

			
			# init
			rd 		= {}
			p 		= {}
			d 		= {}
			db 		= {}
			e 		= {}
			w 		= {}
			Omega 	= {}
			s 		= {}
			
			for mode in modes:
			
				rd[mode] 	= np.array([0] * nbval[mode])
				p[mode] 	= np.array([0] * nbval[mode])
				d[mode] 	= np.array([0] * nbval[mode])
				db[mode] 	= np.array([0] * nbval[mode])
				e[mode] 	= np.array([0] * nbval[mode])
				w[mode] 	= np.array([0.0] * nbval[mode])
				Omega[mode] = [0] * nbval[mode]
				s[mode] 	= []
			
			# processing times
			p[_ti] = np.random.randint(1,20,nbval[_ti])
			
			for i in range(nbval[_ti]):
				p[_disj][i] = p[_ti][i]
			
			# dummy order has a duration of 0 
			for mode in modes:
				p[mode][0] = 0

			pT = sum(p[_ti])		
			
			
			# revenus
			e[_ti] = np.random.randint(1,20,nbval[_ti])
			for i in range(nbval[_ti]):
				e[_disj][i] = e[_ti][i]
			
			# dummy order generates no revenues
			for mode in modes:
				e[mode][0] = 0
				
			# setup
			for i in range(nbval[_ti]):
				s_i = np.random.randint(1,10,nbval[_ti])
				
				# setup is zero between order i and it self 
				s_i[i] = 0
				# setup is zero between any order i and dummy order
				s_i[0] = 0
				s[_ti].append(s_i)
				
				s_i_disj = [0]*nbval[_disj]
				for k in range(nbval[_ti]):
					s_i_disj[k] = s_i[k]
					
				s[_disj].append(s_i_disj)
				
			s[_disj].append(np.array([0]*nbval[_disj]))	
		
			# release date
			
			rd[_ti] = np.random.randint(0,tau*pT,nbval[_ti])
			rd[_disj] = np.array([0]*nbval[_disj])
			for i in range(nbval[_ti]):
				rd[_disj][i] = rd[_ti][i]

			for mode in modes:
				rd[mode][0] = 0
			
			# power consumption
			for i in range(1,nbval[_ti]):
				Omega[_ti][i] = (np.random.randint(1,e[_ti][i]+1))/2
				Omega[_ti][i] = math.ceil(Omega[_ti][i])
				Omega[_disj][i] = Omega[_ti][i]
			del Omega[_ti][0]
			
			lb = pT*(1-tau-(r/2)) 
			ub = pT*(1-tau+(r/2)) 
			slack = np.random.randint(lb,ub)
			
			for i in range(1,nbval[_ti]):
				d[_ti][i] = rd[_ti][i] + maxi(s[_ti],nbval[_ti],i) + max(slack,p[_ti][i])
				db[_ti][i] = d[_ti][i] + math.ceil(r*p[_ti][i])
				#print("db="+str(db[_ti][i]))
				#print("d="+str(d[_ti][i]))
				w[_ti][i] = e[_ti][i]/(db[_ti][i]-d[_ti][i])
				
				d[_disj][i] = d[_ti][i]
				db[_disj][i] = db[_ti][i]
				w[_disj][i] = w[_ti][i]
					
			db[_disj][nbval[_disj]-1] = max(db[_ti])
			d[_disj][nbval[_disj]-1] = max(d[_ti])

			#ti
			for mode in modes:
				output = open(outputname[mode],"w")
				output.write("n="+str(n)+";\n")
				output.write(write_vec(rd[mode],"r"))
				output.write(write_vec(p[mode],"p"))
				output.write(write_vec(d[mode],"d"))
				output.write(write_vec(db[mode],"db"))
				output.write(write_vec(e[mode],"e"))
				output.write(write_vec(w[mode],"w"))
				output.write(write_vec(Omega[mode],"Omega"))
				output.write(write_mat(s[mode],nbval[mode],"s"))

				toufile = 'TOU.txt'
				with open(toufile) as f:
					for line in f:
						output.write(line)
			
				output.close()
	