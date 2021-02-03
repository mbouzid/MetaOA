/*********************************************
 * OPL 12.9.0.0 Model
 * Author: bouzidma
 * Creation Date: 31 janv. 2020 at 15:51:09
 *********************************************/
int n = ...;


/* parametres */
// due dates
int d[0..n] = ...;
// deadlines
int db[0..n] = ...;
// horizon
int dbMax = max(i in 1..n) db[i];
int pMax = sum(i in 1..n) (db[i]==dbMax)*i; 
int H = dbMax + pMax;
// profits
int e[0..n] = ...;
// processing time
int p[0..n] = ...;
// release date
int r[0..n] = ...;
//pénalité de retard sur la commande i
float w[0..n] = ...;
//  setup time entre les commandes i et j 
int s[0..n][0..n] = ...;
// convenient
int pp[i in 0..n][j in 0..n] = (( i == j && i != 0)?1:p[i]);
// profit
float f[j in 1..n][t in 0..H] = (t>=r[j])&&(t<=db[j]-p[j])?(e[j] - w[j]*maxl((t+p[j])-d[j],0) ):0;

/// chen
// nombre de périodes de tarrifs TOU
int m = ...;
// nombre de périodes d'emission de CO2 par jour
int h = ...;
// début de la période k d'émission CO2 
int g[0..h-1] = ...;
// emission CO2/kWh durant la période k
float q[1..h] = ...;
// taxe sur l'emission CO2 /kg
float Tax = ...;
// début de la période k de TOU
int b[0..m-1] = ...;
// cout de l'électricité sur la période k
float EC[1..m] = ...;
// puissance (kW) de consommation de la commande i
float Omega[1..n] = ...;

// cout TOU & emission CO2 a l'instant t pour la commande j
float c[j in 0..n][t in 0..H] = (t>H) ? 0 : ((j == 0) ? 0 :

(1/60)*Omega[j]*sum(k in 1..m-1)(((t>=b[k-1])&&(t<b[k]))*EC[k])
  +(1/60)*Tax*Omega[j]*sum(l in 1..h-1) (((t>=g[l-1])&&(t<g[l]))*q[l]));

/* variable de decision*/
//dvar boolean x[i in 0..n][j in 0..n][t in 0..H];

dvar boolean x[i in 0..n][j in 0..n][t in 0..H] in 0..1;
dvar float+ R[i in 0..n];
// cost

float temp;
execute{
	//cplex.simdisplay=0;
	cplex.tilim = 3600;
	//cplex.mipdisplay=0;
	var before = new Date();
	temp = before.getTime();
}

maximize sum(i in 1..n) R[i];

subject to 
{
	C1:
	{
		sum(j in 1..n) sum( t in r[j]+s[0][j]..H) (x[0][j][t]) == 1	;
	}
	
	
	C2: forall(j in 1..n)
	{
		sum(i in 0..n: (i!=j))(sum(t in (r[i]+p[i]+s[i][j])..(db[j]-p[j]): (t-s[i][j])>=r[j])(x[i][j][t])) <= 1 ;
	}
	
	C3: forall(i in 1..n, t in 0..H)
	{
		((sum(j in 0..n: ((t-pp[j][i]-s[j][i])>=r[j]) && ((t+p[i])<=db[i]) && ((t-s[j][i])>=r[i])) (x[j][i][t])) 
		- 
		(sum(j in 0..n: (j!=i) && ((t+p[i])>=r[j]) && ((t+p[i]+s[i][j]+p[j])<=db[j]))( x[i][j][t+p[i]+s[i][j]])) 
		-
		(sum(j in 0..n: (j==i) && ((t+1)>=r[j]) && ((t+1+p[j])<=H)) (x[i][j][t+1])) )
		== 0 ;	
	}
	
	
	C41:forall(i in 0..n, j in 0..n, t in 0..H: (i!=j) && t < (r[i]+p[i]+s[i][j])  )
	{
		x[i][j][t] == 0;
	}
	
	C42: forall(i in 0..n, j in 0..n, t in 0..H: (i!=j) && t > (db[j]-p[j]) )
	{
		x[i][j][t] == 0;	
	}
	
	C43: forall(i in 0..n, j in 0..n, t in 0..H: (i!=j) && ((t - s[i][j]) < r[j]))
	{
		x[i][j][t] == 0;			
	}
	
	
	C5: forall(i in 1..n, t in 0..H: t < r[i]+1)
	{
		x[i][i][t] == 0;
	};

		
	C51: forall(i in 1..n, t in 0..H: t > H-p[i])
	{
		x[i][i][t] == 0;
	};
	
	
	C6: forall(i in 1..n)
	{	
		R[i] <=	sum(j in 0..n: j!=i)  
			
			sum(t in r[j]+p[j]+s[j][i]..db[i]-p[i]: t-s[j][i]>=r[i]) 
			 x[j][i][t]*
			(
				f[i][t] - 
				(sum(l in 1..s[j][i])(c[i][t-l])
				+ sum(l in 1..p[i])(c[i][t+l]))
			);
	}
	

};

{int} seq;
execute
{
	var after = new Date();
	write("RESULTS,");
    write(cplex.getObjValue());
    write(",",(after.getTime()-temp)/1000);
    writeln(",",cplex.getMIPRelativeGap()); 
	
	for(var t=0; t<=H; t++)
	{
		for(var i=0;i<=n;++i)
		{	
			for(var j=0;j<=n;++j)
			{
				if (j!=i)
				{		
			
					if (x[i][j][t]==1)
					{
						seq.add(j)					
					}
	  			}						
			}			
		}	
	};
	
	write("SEQUENCE,0")
	for(var k=0;k<=seq.size-1;k++)
		write("->"+Opl.item(seq,k))
	writeln("")
	
}

