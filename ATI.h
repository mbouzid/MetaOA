#pragma once
#include <ilopl/iloopl.h>
#include "order.h"
#include "solution.h"
#include <vector>

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
		
		NumVarMatrix x(env);
		NumVarMatrix u(env);

		initializeVariables(env, x, u, model, dat);
		IntMatrix vals(env);

		initializeVals(env, vals,dat);

		initializeObjective(env, x, u, model,dat);
		initializeConstraints(env, x, u, model,dat);


		return new ATI(sol,subsequence,dat,model,x,u);
	}


	static void initializeVariables(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData * dat);
	static void initializeVals(IloEnv& env, IntMatrix &vals, OrderData * dat);
	static void initializeObjective(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData * dat);
	
	
	static void initializeConstraints(IloEnv& env, NumVarMatrix& x, NumVarMatrix& u, IloModel& model, OrderData* dat, Solution* sol, const std::vector <uint16_t>& subsequence);


	void solve(IloEnv & env)
	{
		IloCplex cplx(_model);

		if (cplx.solve())
		{

		}
		else
		{
			exit(EXIT_FAILURE);
		}

	}

};

