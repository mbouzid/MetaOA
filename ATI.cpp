#include "ATI.h"

void ATI::initializeVariables(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat)
{
	size_t T(dat->getT());
	size_t n(dat->getN());

	// x_it
	for (IloInt i(0); i <= n; ++i)
	{
		x[i] = IloNumVarArray(env, T + 1);
		u[i] = IloNumVarArray(env, n + 1);




		for (IloInt t(0); t <= T; ++t)
		{
			std::ostringstream oss;
			oss << "x#" << i << "#" << t;
			std::string repr(oss.str());

			x[i][t] = IloNumVar(env, 0, 1, ILOBOOL, repr.c_str());

		}

		for (IloInt j(0); j <= n; ++j)
		{
			std::ostringstream oss;
			oss << "u#" << i << "#" << j;
			std::string repr(oss.str());
			u[i][j] = IloNumVar(env, IloFalse, IloTrue, ILOBOOL, repr.c_str());
		}

	}


}

void ATI::initializeVals(IloEnv& env, IntMatrix& vals, OrderData * dat)
{
	size_t T(dat->getT());
	size_t n(dat->getN());
	for (IloInt i(0); i <= n; ++i)
	{
		vals[i] = IloIntArray(env, T + 1);
		for (IloInt t(0); t <= T; ++t)
		{
			vals[i][t] = IloInt(0);
		}
	}

}

void ATI::initializeObjective(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat)
{
	IloInt n(dat->getN());
	IloInt T(dat->getT());;


	IloExpr obj(env);
	for (IloInt j(1); j <= n; ++j)
	{
		IloExpr profit(env);
		for (IloInt t(dat->getR(j)); t <= dat->getDb(j) - dat->getP(j) + 1; ++t)
		{
			IloNum energyCostProcessing(0.0);
			for (IloInt tp(0); tp < dat->getP(j); ++tp)
			{
				energyCostProcessing += dat->getEnergyCost(j,t + tp);

			}

			IloExpr	energyCostSetup(env);
			for (IloInt i(0); i <= n; ++i)
			{
				if (i != j)
				{
					IloNum cost(0.0);
					for (IloInt tp(1); tp <= dat->getS(i,j); ++tp)
					{
						if (t - tp >= dat->getR(j))
						{
							cost += dat->getEnergyCost(j,t - tp);
						}

					}

					energyCostSetup += u[i][j] * cost;
				}
			}


			profit += x[j][t] * (dat->getF(j,t + dat->getP(j) - 1) - energyCostProcessing - energyCostSetup);

		}

		obj += profit;

	}


	model.add(IloMaximize(env, obj));


}

void ATI::initializeConstraints(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat, Solution* sol, const std::vector <uint16_t>& subsequence)
{
	size_t T(dat->getT());
	size_t n(dat->getN());

	// capacity	 
	for (IloInt t(0); t <= T; ++t)
	{
		IloExpr capacity(env);
		for (IloInt j(1); j <= n; ++j)
		{
			capacity += x[j][t];
		}
		model.add(capacity <= 1);
	}

	// capacity 1
	for (IloInt j(1); j <= n; ++j)
	{
		IloExpr sum(env);
		for (IloInt t(dat->getR(j)); t <= dat->getDb(j) - dat->getP(j) + 1; ++t)
		{
			sum += x[j][t];
		}
		model.add(sum <= 1);
	}


	// def1	& def2
	for (IloInt j(1); j <= n; ++j)
	{
		IloExpr sum1(env);
		for (IloInt t(0); t < dat->getR(j); ++t)
		{
			sum1 += x[j][t];
		}
		model.add(sum1 == 0);

		IloExpr sum2(env);
		for (IloInt t((dat->getDb(j) - dat->getP(j) + 1) + 1); t <= T; ++t)
		{
			sum1 += x[j][t];
		}
		model.add(sum1 == 0);

	}

	model.add(x[0][0] == 1);

	IloExpr sumZ(env);

	for (IloInt t(1); t <= T; ++t)
	{
		sumZ += x[0][t];
	}

	model.add(sumZ == 0);

	// prec
	for (IloInt i(0); i <= n; ++i)
	{
		for (IloInt j(1); j <= n; ++j)
		{
			if (i != j)
			{
				IloExpr sumA(env);
				for (IloInt t(dat->getR(i)); t <= dat->getDb(i) - dat->getP(i) + 1; ++t)
				{
					sumA += (t)*x[i][t];
				}
				sumA += ((dat->getS(i,j) + dat->getP(i)) * u[i][j]) - (dat->getDb(i)) * (1 - u[i][j]);


				IloExpr sumB(env);
				for (IloInt t(dat->getR(j) + dat->getS(i,j) + 1); t <= dat->getDb(j) - dat->getP(j) + 1; t++)
				{
					sumB += t * x[j][t];
				}

				IloConstraint prec(sumA <= sumB);
				std::ostringstream oss;
				oss << "prec#" << i << "#" << j;
				std::string name(oss.str());

				prec.setName(name.c_str());
				model.add(prec);

			}
		}
	}

	for (IloInt i(0); i <= n; ++i)
	{
		IloExpr sum1(env);
		for (IloInt j(1); j <= n; ++j)
		{
			if (i != j)
			{
				sum1 += u[i][j];
			}
		}

		IloExpr sum2(env);
		for (IloInt t(dat->getR(i)); t <= dat->getDb(i) - dat->getP(i) + 1; ++t)
		{
			sum2 += x[i][t];
		}

		model.add(sum1 <= sum2);
	}


	// pred
	for (IloInt i(1); i <= n; ++i)
	{
		IloExpr sum1(env);
		for (IloInt j(0); j <= n; ++j)
		{
			if (i != j)
			{
				sum1 += u[j][i];
			}
		}

		IloExpr sum2(env);
		for (IloInt t(dat->getR(i)); t <= dat->getDb(i) - dat->getP(i) + 1; ++t)
		{
			sum2 += x[i][t];
		}

		model.add(sum1 == sum2);
	}



}


