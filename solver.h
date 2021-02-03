#pragma once
#include "solution.h"
#include "island.h"
#include "utils.h"
#include <fstream>
#include <sstream>

#include <map>
#define POPSIZE 50

class Solver
{
	private:

		OrderData* _dat;

		// islands
		std::vector <Island> _islands;

		// importance of the knowledge accumulated during the last migrations(inertia or exploitation)
		double _alpha;
		
		// the amount of noise, which is necessary to explore alternative search space areas by means of individuals
		double _beta;

		// data matrix that evaluates the improvement obtained by individuals of island i when they are processed on island j.
		std::vector <std::vector<double>> _D;
		
		// migration matrix
		std::vector <std::vector <double>> _M;
		
		// improvement
		std::vector<double> _improvement;
		
		void migrationPool(std::map <size_t, std::vector<Solution> > & migrants, size_t i);


	public:

		Solver
		(
			OrderData* dat,
			const std::vector<Island>& islands,
			size_t popsize,
			double alpha = 0.8,
			double beta = 0.01
		) :
			_dat(dat),
			_islands(islands),
			_alpha(alpha),
			_beta(beta),
			_D(),
			_M(),
			_improvement()
		{
			size_t n(_islands.size());
			for (size_t i(0); i < n ; ++i)
			{
				_improvement.push_back(0.0);
				_D.push_back(std::vector<double>());
				_M.push_back(std::vector<double>());
				for (size_t j(0); j < n; ++j)
				{
					_M[i].push_back((double)1 / (double)(_islands.size()));
					_D[i].push_back(0.0);
				}
			}

			for (size_t i(0); i < n; ++i)
			{
				_islands[i].populateRandomly(dat, popsize);
				Island::updateStats(_islands[i],dat);

			}

		
		}



		void run(size_t nbIter, double pm, double pc, double dr, const char* output);

		// get island with best improvement
		std::vector<size_t> bestIsland();

		void printTransition(std::ostream& os) const;

		void migrationPolicy();

		void replacePop(double dr = 0.5);

		void replacePop(size_t island, double pm, double dr = 0.5);

		// applies the operator assigned to this island on every individuals of the population
		void evolve(size_t island, double pm, double pc, double dr = 0.5);

		// a process that modifies the migration matrix according to the data matrix
		// i is the best island
		void update(size_t i);
		
		// a process that modifies D according to information received 
		void learn();	  

		// process computes the feedback information 
		void analyze(size_t i);

};

