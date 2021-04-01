#pragma once
#include <ilopl/iloopl.h>
#include "order.h"
#include "solution.h"
#include <vector>
#include <chrono>

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<IloIntArray> IntMatrix;

class ATI
{
private:

	Solution* _sol;
	std::vector <uint16_t> _subsequence;
	OrderData* _dat;

	IloModel _model;
	NumVarMatrix _x;
	NumVarMatrix _u;
	

	ATI
	(
		Solution* sol,
		const std::vector <uint16_t> & subsequence,
		OrderData * dat,
		const IloModel& model,
		const NumVarMatrix& x,
		const NumVarMatrix& u
	) :
		_sol(sol),
		_subsequence(subsequence),
		_dat(dat),
		_model(model),
		_x(x),
		_u(u)
	{}

public:


	static ATI* init
	(
		IloEnv& env,
		Solution* sol,
		const std::vector <uint16_t>& subsequence,
		OrderData* dat
	)
	{
		
		IloModel model(env);
		
		NumVarMatrix x(env,subsequence.size());
		NumVarMatrix u(env,subsequence.size());

		initializeVariables(env, x, u, model, dat,subsequence);
		IntMatrix vals(env,subsequence.size());

		initializeVals(env, vals,dat,subsequence);
		
		initializeObjective(env, x, u, model,dat,subsequence);
		
		initializeConstraints(env, x, u, model,dat,sol,subsequence);

		return new ATI(sol,subsequence,dat,model,x,u);
	}

	static size_t getT(OrderData *dat, const std::vector<uint16_t> & subsequence) 
	{
		size_t T(dat->getDb(subsequence.at(0)));
		for (size_t i(1); i < subsequence.size(); ++i)
		{
			if (dat->getDb(subsequence.at(i)) > T)
			{
				T = dat->getDb(subsequence.at(i));
			}
		}
		return T;
	}

	static void initializeVariables(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat, const std::vector<uint16_t> & subsequence);
	static void initializeVals(IloEnv& env, IntMatrix& vals, OrderData* dat, const std::vector<uint16_t>& subsequence);
	static void initializeObjective(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData * dat, const std::vector<uint16_t>& subsequence);
	
	
	static void initializeConstraints(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat, Solution* sol, const std::vector <uint16_t>& subsequence);

	static void addMIPstart(IloEnv & env, IloCplex& cplx, NumVarMatrix& x, NumVarMatrix& u, Solution * sol, const std::vector<uint16_t> & subsequence, OrderData* dat);

	IloModel & getModel()
	{
		return _model;
	}

	NumVarMatrix& getX()
	{
		return _x;
	}

	NumVarMatrix& getU()
	{
		return _u;
	}

	static std::vector<uint16_t> solve(Solution * sol, const std::vector<uint16_t> & subsequence, OrderData* dat)
	{
		IloEnv env;

		ATI* ati = ATI::init(env, sol, subsequence, dat);

	


		IloCplex cplx(ati->getModel());

		cplx.setOut(env.getNullStream());
		//cplx.setParam(IloCplex::Param::Threads, 4);
		cplx.setParam(IloCplex::Param::TimeLimit, 0.5);

		//ATI::addMIPstart(env, cplx, ati->getX(), ati->getU(), sol, subsequence, dat);
		std::vector <uint16_t> bestpermut;


		if (cplx.solve())
		{
			size_t n(dat->getN());
			size_t T(ATI::getT(dat, subsequence));
			for (IloInt t(0); t <= T; ++t)
			{
				for (IloInt j(0); j < subsequence.size(); ++j)
				{	
					if (cplx.getValue(ati->getX()[j][t]) == 1)
					{
						bestpermut.push_back(subsequence.at(j));
					}

				}
			}  
			
			return bestpermut;
		}
		else
		{
			return 	subsequence;
		}

	
	}

};

