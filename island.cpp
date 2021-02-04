#include "island.h"
#include "utils.h"

size_t Island::s_id = 0;

// apply operator with probability pm
void Island::updateStats(Island & island, OrderData* dat)
{
	island.updateAvgFitness();
	island.updateMaxFitness();
}

void Island::apply(Island & island, double pm, OrderData * dat)
{
	
	/*thread_local*/ static std::random_device srd;  
	/*thread_local*/ static std::mt19937 smt(srd());
	smt.seed(161295);
	std::bernoulli_distribution d(pm);
	size_t n(island.size());

	for (size_t i(0); i<n ; ++i)
	{
		if (d(srd))
		{
			island._operator(island[i],dat);
		}
	}


}

void Island::applyToAll(Island& island, OrderData* dat)
{
	size_t n(island.size());
	for (size_t i(0); i < n; ++i)
	{
		island._operator(island[i], dat);
	}
}

void Island::updateAvgFitness() 
{
	double avg(0.0);
	for (size_t i(0); i < size(); ++i)
	{
		avg += Solution::getObjective(at(i));
	}

	_avgFit = (avg) / double(size());
}

void Island::updateMaxFitness()
{
	std::vector <double> objs;
	size_t n(size());
	for (size_t i(0); i < n; ++i)
		objs.push_back(Solution::getObjective(at(i)));

	if (not objs.empty())
	{
		_maxFit = *std::max_element(objs.begin(), objs.end());
	}
}

Solution Island::getBestSol() const
{
	size_t n(size());
	size_t imax(0);
	for (size_t i(1); i < n; ++i)
	{
		if (Solution::getObjective(at(i)) > Solution::getObjective(at(imax)))
		{
			imax = i;
		}
	}
	return at(imax);

}

double Island::getMinFitness() const
{
	std::vector <double> objs;
	size_t n(size());
	for (size_t i(0); i < n; ++i)
		objs.push_back(Solution::getObjective(at(i)));

	if (objs.empty())
		return 0.0;
	else
		return *std::min_element(objs.begin(), objs.end());
}

std::pair<Solution, Solution> Island::select() const
{
	
	size_t p1(rand()%(size())), p2(rand()% size());
	if (p1 == p2)
	{
		p2 = rand() % size();
	}

	
	size_t select1(0);
	if (at(p1).getObjectiveValue() > at(p2).getObjectiveValue())
	{
		select1 = p1;
	}
	else
	{
		select1 = p2;
	}

	/*size_t p3(rand() % (size())), p4(rand() % size());
	if (p3 == p4)
	{
		p4 = rand() % size();
	}


	size_t select2(1);
	if (at(p3).getObjectiveValue() > at(p4).getObjectiveValue())
	{
		select1 = p3;
	}
	else
	{
		select1 = p4;
	} */


	return std::make_pair(getBestSol(), at(select1));
}

void Island::replace(OrderData* dat, double pm, double dr)
{
	std::pair <Solution, Solution> bestSols(select());


	// sort pop by worst sol
	std::sort(begin(), end(), [](const Solution& s1, const Solution& s2) { return s1.getObjectiveValue() <= s2.getObjectiveValue(); });

	// remove worst solution
	size_t nbIndiv(size());

	size_t nb(dr*nbIndiv);
	size_t reste(nbIndiv % nb);

	for (size_t k(0); k < nb + reste; ++k)
	{
		// erase worst sol
		std::vector<Solution>::iterator found(std::find(begin(),end(), this->operator[](k)));
		if (found != end())
		{
			erase(found);
		}
	}

	size_t nbChilds(dr * nbIndiv);
	for (size_t k(0); k < nbChilds+reste; ++k)
	{
		Solution child(Solution::singlePointCrossover(bestSols.first, bestSols.second, dat));
		
		double randNum((double)rand()/ (double)RAND_MAX);
		if (randNum < pm)
		{
			_operator(child, dat);
		}
		push_back(child);
	}

}

std::ostream& operator<<(std::ostream& os, const Island& island)
{
	return os << island.size(); /*<< "," <<island.getMaxFitness(); */ //<< "," << island.getAvgFitness();
}
