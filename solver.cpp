#include "solver.h"
#include "utils.h"


void Solver::run(size_t nbIter, double pm, double pc, double dr, const char* output)
{

	size_t cpt(0);
	size_t n(_islands.size());
	std::ofstream f(output);	 
	
	if (not _islands.empty())
	{
		f << "iteration,";
		std::vector<Island>::const_iterator it(_islands.begin());
		f << (*it).getName();
		while (++it != _islands.end())
		{
			f << "," << (*it).getName();
		}
		f << std::endl;
	}

	while (cpt < nbIter)
	{
		std::ostringstream oss;

		//oss << cpt;

		for (int i(0); i < n; ++i)
		{
			if (not _islands.at(i).empty())
			{
				evolve(i, pm,pc, dr);
			} // end if empty

		oss << "," << _islands.at(i);
		  
			

		} // end for

		oss << std::endl;

		for (size_t i(0); i < n; ++i)
		{
			update(i);
		}

		migrationPolicy();

	
		for (size_t i(0); i < n; ++i)
		{
			analyze(i);
		}

		

		f << oss.str();
		++cpt;
	}
	f.close();

	// get best sol
	Solution* bestSol (nullptr);

	size_t j(0);
	while (j < n)
	{
		if (not _islands.at(j).empty())
		{
			bestSol = new Solution(_islands.at(j).getBestSol());
			break;
		}

		++j;
	}

	for (size_t i(0); i < n; ++i)
	{
		if (not _islands.at(i).empty())
		{
			if (_islands.at(i).getBestSol().getObjectiveValue() > bestSol->getObjectiveValue())
			{
				delete bestSol;
				bestSol = new Solution(_islands.at(i).getBestSol());
			}
		}
	}
	
	
	if (bestSol != nullptr)
	{
		std::cout << *bestSol << std::endl;
		std::cout << "objVal=" << bestSol->getObjectiveValue() << std::endl;

		delete bestSol;
	}

}



void Solver::migrationPool(std::map <size_t, std::vector<Solution> > & migrants, size_t i)
{
	
	std::vector<Solution>::iterator it(_islands[i].begin());
	
	std::vector<Solution> toRemove;

	while (it != _islands[i].end())
	{
		// choice with transition matrix
		
		double randNum(rand() / RAND_MAX);
	//	std::discrete_distribution<> d(_M.at(i).begin(), _M.at(i).end());

		size_t n(_islands.size());
		size_t destination(i);
		for (size_t j(0); j < n; ++j)
		{
			if ((i!=j) && (randNum < _M.at(i).at(j)))
			{
				destination = j;
		
			}
		}
		
		if (destination != i)
		{
			
			Solution tmp(*it);	 
			migrants[destination].push_back(tmp);
			toRemove.push_back(tmp);

		}
		++it;
	}

	for (size_t k(0); k < toRemove.size(); ++k)
	{	
		// erase migrants from island
		std::vector<Solution>::iterator found(std::find(_islands[i].begin(), _islands[i].end(), toRemove.at(k)));
		if (found != _islands[i].end())
		{
			_islands[i].erase(found);
		}
	} 

}




std::vector<size_t> Solver::bestIsland()
{
	std::vector <size_t> bestIslands;
	double maxElt(*std::max_element(_improvement.begin(), _improvement.end()));
	
	size_t n(_islands.size());
	for(size_t i(0); i < n; ++i)
	{
		if (_improvement.at(i) == maxElt)
		{
			bestIslands.push_back(i);
		}
	}
	return bestIslands;
}

void Solver::printTransition(std::ostream& os) const
{
	size_t n(_islands.size());
	for (size_t i(0); i < n; ++i)
	{
		for (size_t j(0); j < n; ++j)
		{
			os << _M.at(i).at(j) << " ";
		}
		os << std::endl;
	}
	os << std::endl;
}

void Solver::migrationPolicy()
{
	std::map <size_t, std::vector<Solution> > migrants;
	for (size_t i(0); i < _islands.size(); ++i)
	{
		migrants.emplace(i, std::vector<Solution>());

	}

	for (size_t i(0); i < _islands.size(); ++i)
	{
		migrationPool(migrants, i);
	}

	for (const std::pair<size_t, std::vector<Solution>>& p : migrants)
	{
		size_t destination(p.first);
		for (Solution sol : p.second)
		{  
			sol.updateOrigin(destination);
			_islands[destination].push_back(sol);
			
		}
	}

}

void Solver::replacePop(double dr)
{
	for (size_t i(0); i < _islands.size(); ++i)
	{
		if (_islands.at(i).size() > 1)
		{
			_islands[i].replace(_dat, dr);
		}
	}

}

void Solver::replacePop(size_t island, double pm, double dr)
{
	if (_islands.at(island).size() > 1)
	{
		_islands[island].replace(_dat, pm, dr);
	}
}

void Solver::evolve(size_t island, double pm, double pc, double dr)
{
	//double fitBefore(_islands.at(island).getMaxFitness());
	
	
	double randomNum((double)rand()/ (double)RAND_MAX);
	if (randomNum < pc)
	{
		replacePop(island, pm, dr);
	}

	Island::updateStats(_islands[island], _dat);
	//double fitAfter(_islands.at(island).getMaxFitness());
	//_improvement[island] = (fitAfter - fitBefore);
}

void Solver::update(size_t island)
{
	size_t n(_islands.size());

	// intermediate reward vector
	std::vector<double> R(n);
	for (size_t k(0); k < n; ++k)
	{

		std::vector <size_t> B(utils::argmax(_D[island]));
		

		if (utils::belongs(B, k))
		{	
		
			R[k] = (double)1 / (double)B.size(); 
		}
		else
		{
			R[k] = 0.0;
		}

	}

	std::vector <double> N(n);
	for (size_t i(0); i < n; ++i)
	{
		N[i] = utils::random_num(1,1000);
	}



	utils::normalize(N);
//	utils::normalize(R);


	for (size_t k(0); k < n; ++k)
	{
		_M[island][k] = (1 - _beta) * ((_alpha*_M.at(island).at(k)) + (1-_alpha)*R.at(k) ) + _beta * N.at(k);

	}

	for (size_t j(0); j < n; ++j)
	{ 
		utils::normalize(_M[j]);
	}

}

void Solver::learn()
{
	// The learning process is very simple here since we just keep the last performance (time window of size 1),

}

void Solver::analyze(size_t i)
{
	size_t n(_islands.size());

	// fitness of solutions in island i that come from island k
	std::vector<std::vector<double>> fit(n);
	for (const Solution& s : _islands.at(i))
	{
			
		for (size_t k(0); k < n; ++k)
		{  
			//std::cout << "k=" << k << std::endl;
			// check if origin of sol s in island i is island k 
				if (s.getOrigin()==k)
				{
					fit[k].push_back(s.getObjectiveValue());
					//fit[k].push_back(s.getServiceLevel()+s.getAcceptanceRate());
				}
			}

	}

	for (size_t k(0); k < n; ++k)
	{
		if (fit.at(i).empty())
			_D[k][i] = 0.0;
		else
		{
			
			_D[k][i] = *std::max_element(fit.at(i).cbegin(), fit.at(i).cend());
		}
	}



}
