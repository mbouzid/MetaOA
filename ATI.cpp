#include "ATI.h"

void ATI::initializeVariables(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat, const std::vector<uint16_t>& subsequence)
{

	size_t T(ATI::getT(dat,subsequence));


	size_t n(dat->getN());

	// x_it
	for (IloInt i(0); i < subsequence.size(); ++i)
	{
		x[i] = IloNumVarArray(env, T + 1);
		u[i] = IloNumVarArray(env, subsequence.size());




		for (IloInt t(0); t <= T; ++t)
		{
			std::ostringstream oss;
			oss << "x#" << i << "#" << t;
			std::string repr(oss.str());

			x[i][t] = IloNumVar(env, 0, 1, ILOBOOL, repr.c_str());

		}

		for (IloInt j(0); j < subsequence.size(); ++j)
		{
			std::ostringstream oss;
			oss << "u#" << i << "#" << j;
			std::string repr(oss.str());
			u[i][j] = IloNumVar(env, IloFalse, IloTrue, ILOBOOL, repr.c_str());
		}

	}


}

void ATI::initializeVals(IloEnv& env, IntMatrix& vals, OrderData * dat, const std::vector<uint16_t>& subsequence)
{
	size_t T(ATI::getT(dat, subsequence));
	
	for (IloInt i(0); i < subsequence.size(); ++i)
	{
		vals[i] = IloIntArray(env, T + 1);
		for (IloInt t(0); t <= T; ++t)
		{
			vals[i][t] = IloInt(0);
		}
	}

}

void ATI::initializeObjective(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat, const std::vector<uint16_t>& subsequence)
{
	IloInt n(dat->getN());
	size_t T(ATI::getT(dat, subsequence));


	IloExpr obj(env);
	for (IloInt i(0); i < subsequence.size(); ++i)
	{
		IloInt j(subsequence.at(i));
		IloExpr profit(env);
		for (IloInt t(dat->getR(j)); t <= dat->getDb(j) - dat->getP(j) + 1; ++t)
		{
			IloNum energyCostProcessing(0.0);
			for (IloInt tp(0); tp < dat->getP(j); ++tp)
			{
				energyCostProcessing += dat->getEnergyCost(j,t + tp);

			}

			IloExpr	energyCostSetup(env);
			for (IloInt k(0); k < subsequence.size(); ++k)
			{
				if (k != i)
				{
					IloNum cost(0.0);
					for (IloInt tp(1); tp <= dat->getS(subsequence.at(k),j); ++tp)
					{
						if (t - tp >= dat->getR(j))
						{
							cost += dat->getEnergyCost(j,t - tp);
						}

					}

					energyCostSetup += u[k][i] * cost;
				}
			}


			profit += x[i][t] * (dat->getF(j,t + dat->getP(j) - 1) - energyCostProcessing - energyCostSetup);

		}

		obj += profit;

	}


	model.add(IloMaximize(env, obj));


}

void ATI::initializeConstraints(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat, Solution* sol, const std::vector <uint16_t>& subsequence)
{
	size_t T(ATI::getT(dat, subsequence));
	size_t n(dat->getN());

	// capacity	 
	for (IloInt t(0); t <= T; ++t)
	{
		IloExpr capacity(env);
		for (IloInt j(0); j < subsequence.size(); ++j)
		{
			capacity += x[j][t];
		}
		model.add(capacity <= 1);
	}

	// capacity 1
	for (IloInt i(0); i < subsequence.size(); ++i)
	{
		IloInt j(subsequence.at(i));
		IloExpr sum(env);
		for (IloInt t(dat->getR(j)); t <= dat->getDb(j) - dat->getP(j) + 1; ++t)
		{
			sum += x[i][t];
		}
		model.add(sum <= 1);
	}


	// def1	& def2
	for (IloInt i(0); i < subsequence.size(); ++i)
	{
		IloInt j(subsequence.at(i));
		IloExpr sum1(env);
		for (IloInt t(0); t < dat->getR(j); ++t)
		{
			sum1 += x[i][t];
		}
		model.add(sum1 == 0);

		IloExpr sum2(env);
		for (IloInt t((dat->getDb(j) - dat->getP(j) + 1) + 1); t <= T; ++t)
		{
			sum1 += x[i][t];
		}
		model.add(sum1 == 0);

	}

	/*model.add(x[0][0] == 1);

	IloExpr sumZ(env);

	for (IloInt t(1); t <= T; ++t)
	{
		sumZ += x[0][t];
	}

	model.add(sumZ == 0);*/

	// prec
	for (IloInt k(0); k < subsequence.size(); ++k)
	{
		IloInt i(subsequence.at(k));
		for (IloInt l(0); l < subsequence.size(); ++l)
		{
			IloInt j(subsequence.at(k));
			if (i != j)
			{
				IloExpr sumA(env);
				for (IloInt t(dat->getR(i)); t <= dat->getDb(i) - dat->getP(i) + 1; ++t)
				{
					sumA += (t)*x[k][t];
				}
				sumA += ((dat->getS(i,j) + dat->getP(i)) * u[k][l]) - (dat->getDb(i)) * (1 - u[k][l]);


				IloExpr sumB(env);
				for (IloInt t(dat->getR(j) + dat->getS(i,j) + 1); t <= dat->getDb(j) - dat->getP(j) + 1; t++)
				{
					sumB += t * x[l][t];
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

	for (IloInt i(0); i < subsequence.size(); ++i)
	{
		IloExpr sum1(env);
		for (IloInt j(1); j < subsequence.size(); ++j)
		{
			if (i != j)
			{
				sum1 += u[i][j];
			}
		}

		IloExpr sum2(env);
		for (IloInt t(dat->getR(i)); t <= dat->getDb(subsequence.at(i)) - dat->getP(subsequence.at(i)) + 1; ++t)
		{
			sum2 += x[i][t];
		}

		model.add(sum1 <= sum2);
	}


	// pred
	for (IloInt i(1); i < subsequence.size(); ++i)
	{
		IloExpr sum1(env);
		for (IloInt j(0); j < subsequence.size(); ++j)
		{
			if (i != j)
			{
				sum1 += u[j][i];
			}
		}

		IloExpr sum2(env);
		for (IloInt t(dat->getR(i)); t <= dat->getDb(subsequence.at(i)) - dat->getP(subsequence.at(i)) + 1; ++t)
		{
			sum2 += x[i][t];
		}

		model.add(sum1 == sum2);
	}



}

void ATI::addMIPstart(IloEnv & env, IloCplex& cplx, NumVarMatrix& x, NumVarMatrix& u, Solution* sol, const std::vector<uint16_t>& subsequence, OrderData * dat)
{
	IloNumVarArray vars(env);
	IloNumArray values(env);

	size_t n(dat->getN());
	size_t T(ATI::getT(dat, subsequence));
	size_t prev(0);
	size_t k(0);
	for (uint16_t j : sol->getSequence().getSeq())
	{
		/*std::vector<uint16_t>::const_iterator found(std::find(subsequence.cbegin(), subsequence.cend(), j));
		if (found == subsequence.cend())
		{ */
			IloInt t(sol->getStartingTime(k) + dat->getS(prev, j) + 1);
			vars.add(x[j][t]);
			values.add(IloInt(1));
			std::cout << "j=" << j << ",t=" << t << std::endl;	 
			prev = j;
			++k;
		//}
	}

	prev = 0;
	for (uint16_t j : sol->getSequence().getSeq())
	{
		/*std::vector<uint16_t>::const_iterator found(std::find(subsequence.cbegin(), subsequence.cend(), j));
		if (found == subsequence.cend())
		{ */
			vars.add(u[prev][j]);
			values.add(IloInt(1));
			prev = j;
		//}
	}


	cplx.addMIPStart(vars, values);

}


