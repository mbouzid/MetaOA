#include "solution.h"
#include <numeric>

// update & repair
void Solution::updateCompletionTimes(OrderData* d)
{
	if (_seq.getSeq().empty())
	{
		size_t n(_seq.getN());
		for (size_t k(0); k < n; ++k)
		{
			_C[k] = 0;
			_ST[k] = 0;
		}

		return;
	}

	std::vector<uint16_t> rejected;

	uint16_t prev(0);
	for (size_t k(0); k < _seq.size(); ++k)
	{
		uint16_t i(_seq.at(k)); // get kth order  to be processed
		
								
		// add random number to starting time?
		
		// a quel moment commencer sans rejeter ?
		// slack = _C[prev] - d->getDb()[prev]- d->getP()[prev] + 1;
		
		// if completion time of previous job is before release, update C[k] to its release date 
		if (_C[prev] <= d->getR(i))
		{
			_C[i] = d->getR(i) + d->getS(prev, i) + d->getP(i);
			_ST[i] = d->getR(i);
		}
		else	// update completion time with previous completion time of order and processing and setup
		{
			_C[i] = _C[prev] + d->getS(prev, i) + d->getP(i);
			_ST[i] = _C[prev];
		}

		
		// if completion is greater than deadline => reject order
		// prev is not updated
		if (_C[i] > d->getDb(i))
		{
			_C[i] = 0;
			_ST[i] = 0;
			rejected.push_back(i);

		}
		else
		{
			prev = i;
		}

	} // end for

	// remove rejected order from sequence
	for (uint16_t i : rejected)
	{
		_seq.deleteOrder(i);
		_C[i] = 0;
	}


}


void Solution::computeObjective(OrderData* d) 
{
	if (_seq.getSeq().empty())
	{
		_objVal = 0.0;
		return;
	}

	
	size_t n(_seq.size());

	for (size_t k(0); k < n; ++k)
	{
		_profit.push_back(0.0);
		_tardiness.push_back(0.0);
	}
	
	uint16_t prev(0); 

	for (size_t k(0); k < n; ++k)
	{
		uint16_t i(_seq.at(k));	// kth order

		_tardiness[k] = (std::max(0, _C[i] - d->getD(i)));
		// energy cost
		double energyCost(0.0);

		
		for (uint16_t t(1); t <= _C[i]-_ST[i]; ++t)
		{
			energyCost += d->getEnergyCost(i, _C[i] - t);
		}


		_profit[k] = d->getE(i) - (d->getW(i) * _tardiness.at(k)) - energyCost;
		
		prev = i;


	}

	double sum(0.0);
	for (size_t k(0); k < n; ++k)
	{
		sum += _profit.at(k);
	}

	_objVal = sum;

}

void Solution::randomShuffle(Solution& sol, OrderData* dat)
{
	sol._seq.randomShuffle();
}

void Solution::scramble(Solution& sol, OrderData* dat)
{
	size_t pos1(sol._seq.getRandomPosInSequence()), pos2(sol._seq.getRandomPosInSequence());

	
	// pos1 < pos2
	size_t tmp(0);
	if (pos1 > pos2)
	{
		tmp = pos2;
		pos2 = pos1;
		pos1 = tmp;
	}

	sol._seq.scrambleBetween(pos1, pos2);


	update(sol, dat);
}

void Solution::inversion(Solution& sol, OrderData* dat)
{
	size_t pos1(sol._seq.getRandomPosInSequence()), pos2(sol._seq.getRandomPosInSequence());


	// pos1 < pos2
	size_t tmp(0);
	if (pos1 > pos2)
	{
		tmp = pos2;
		pos2 = pos1;
		pos1 = tmp;
	}

	sol._seq.inversion(pos1, pos2);
	update(sol, dat);
}

void Solution::swap(Solution& sol, OrderData* dat)
{
	uint16_t i(Sequence::getRandomOrderInSequence(sol._seq)), j(Sequence::getRandomOrderInSequence(sol._seq));

	sol._seq.swap(i, j);

	update(sol, dat);
}

void Solution::exchangeRandomly(Solution& sol, OrderData* dat)
{
	uint16_t i(Sequence::getRandomOrderInSequence(sol._seq)), x(sol._seq.getRandomOrderRejected());
	
	if (i == 0 or x == 0)
		return;

	sol._seq.exchange(i, x);

	update(sol, dat);
}

void Solution::exchange(Solution& sol, OrderData* dat)
{

	uint16_t i(sol._seq.getLeastProfitableOrder(dat)), x(sol._seq.getMostProfitableRejectedOrder(dat));
	
	if (i == 0 or x == 0)
		return;

//	std::cout << "i=" << i << ", x=" << x << std::endl;
	sol._seq.exchange(i, x);

	update(sol, dat);
}



void Solution::add(Solution& sol, OrderData* dat)
{
	uint16_t  x(sol._seq.getMostProfitableRejectedOrder(dat));
	size_t pos(sol._seq.getRandomPosInSequence());

	if (x == 0 )
		return;

	sol._seq.add(x, pos);

	update(sol, dat);
}

void Solution::addRandomly(Solution& sol, OrderData* dat)
{
	/*if (not sol.getSequence().empty())
	{	*/
		uint16_t  x(sol._seq.getRandomOrderRejected());
		size_t pos(sol._seq.getRandomPosInSequence());

		if (x == 0)
		{
			return;
		}

		sol._seq.add(x, pos);
		update(sol, dat);
	//}
}

void Solution::remove(Solution& sol, OrderData* dat)
{
	uint16_t  i(Sequence::getRandomOrderInSequence(sol._seq));
	if (i != 0)
	{
		sol._seq.remove(i);
		update(sol, dat);
	}
}

void Solution::shift(Solution& sol, OrderData* dat)
{
	
	if (sol._seq.getSeq().empty())
	{
		size_t n(sol._seq.getN());
		for (size_t k(0); k < n; ++k)
		{
			sol._C[k] = 0;
			sol._ST[k] = 0;
		}

		sol.computeObjective(dat);
		return;

	}

	sol.updateCompletionTimes(dat);

	std::vector<uint16_t> rejected;

	uint16_t prev(0);
	for (size_t k(0); k < sol._seq.size(); ++k)
	{
		uint16_t i(sol._seq.at(k)); // get kth order  to be processed

		
	
		uint16_t start(std::max(dat->getR(i) + 1,sol._C[prev]+1));	  
		
		uint16_t last(dat->getD(i) - dat->getP(i));
		if (k != sol._seq.size()-1)
			last = std::min((int)dat->getD(i)-dat->getP(i),sol._C[sol._seq.at(k + 1)] - dat->getP(i) - dat->getS(prev,i)+1);
	

		double prevEnergyCost(0.0);
		for (uint16_t tp(1); tp <= sol._C[i] - sol._ST[i]; ++tp)
		{
			prevEnergyCost += dat->getEnergyCost(i, sol._C[i] - tp);
		}

		uint16_t bestStartingTime(start);

		for (uint16_t t(start); t <= last; ++t)
		{
			sol._ST[i] = t;
			sol._C[i] = sol._ST[i] + dat->getS(prev, i) + dat->getP(i);

			double energyCost(0.0);
			for (uint16_t tp(1); tp <= sol._C[i] - sol._ST[i]; ++tp)
			{	
				energyCost += dat->getEnergyCost(i, sol._C[i] - tp);
			}

			if (energyCost < prevEnergyCost)
			{
				prevEnergyCost = energyCost;
				bestStartingTime = t;
			}
			
		}

		sol._ST[i] = bestStartingTime;
		sol._C[i] = sol._ST[i] + dat->getS(prev, i) + dat->getP(i);


		// if completion is greater than deadline => reject order
		// prev is not updated
		if (sol._C[i] > dat->getDb(i))
		{
			sol._C[i] = 0;
			sol._ST[i] = 0;
			rejected.push_back(i);

		}
		else
		{
			prev = i;
		}

	} // end for

	// remove rejected order from sequence
	for (uint16_t i : rejected)
	{
		sol._seq.deleteOrder(i);
		sol._C[i] = 0;
	}


	sol.computeObjective(dat);

}

Solution Solution::singlePointCrossover(const Solution& p1, const Solution& p2, OrderData * dat)
{
	size_t n1(p1._seq.size()), n2(p2._seq.size());
	size_t pos(utils::random_num(0,std::min(n1,n2)));

	Sequence child(p1._seq.getN());
	if (n1 > n2)
	{
		 
		// parent 2 from k to pos-1
		for (size_t k(0); k < pos; ++k)
		{
			child.append(p2.getOrderAt(k));
		}

		// parent 1 from pos to n1-1
		for (size_t k(pos); k < n1; ++k)
		{
			child.append(p1.getOrderAt(k));
		}
	}
	else
	{
		// parent 1 from k to pos-1
		for (size_t k(0); k < pos; ++k)
		{
			child.append(p1.getOrderAt(k));
		}

		// parent 2 from pos to n1-1
		for (size_t k(pos); k < n1; ++k)
		{
			child.append(p2.getOrderAt(k));
		}

	}
	
	Solution elNino(p1.getOrigin(),child, dat);
	update(elNino, dat);

	return elNino;
}



// generate random
Solution Solution::genRandom(size_t origin, OrderData* dat, double pAccepted)
{
	Sequence s(dat->getN());
	s.genRandom(pAccepted);
	s.randomShuffle();
	
	Solution sol(origin, s, dat);
	update(sol, dat);
	return sol;
}

Solution Solution::genEmpty(size_t origin, OrderData* dat)
{
	Sequence s(dat->getN());
	return Solution(origin,s,dat);
}

double Solution::getBestSolFrom(const std::vector<Solution>& sols)
{
	if (sols.empty())
		return 0.0;
	std::vector<double> objs;

	for (const Solution& s : sols)
	{
		objs.push_back(s.getObjectiveValue());
	}



	return *std::max_element(objs.begin(),objs.end());
}

// generate greedy	
/* TODO */

Solution Solution::genGreedy(size_t origin, OrderData* dat)
{
	//RLR1_i = ei / (pi + saverage, i),
	//where saverage_i =(s0, i + s1, i + y + sn, i) / (n + 1).
	double* RLR = (double*)calloc(dat->getN() + 1, sizeof(double));
	Sequence s(dat->getN());



	for (uint16_t i(1); i <= dat->getN(); ++i)
	{

		// compute revenue load ratio
		double savg_i(0.0);

		for (size_t j(0); j <= dat->getN(); ++j)
		{
			savg_i += dat->getS(j, i);
		}

		savg_i = savg_i / double(dat->getN() + 1);


		RLR[i] = dat->getE(i) / (dat->getP(i) + savg_i);
		if (RLR[i] > 0.0)
			s.append(i);  // add order with positive RLR

	}

	// sort sequence by increasing RLR
	std::sort(s.begin(), s.end(), [&RLR](const uint16_t& i, const uint16_t& j) { return RLR[i] <= RLR[j]; });



	free(RLR);

	return Solution(origin, s, dat);
}

std::ostream& operator<<(std::ostream& os, const Solution& sol)
{
	os << sol.getSequence() << std::endl;

	for (uint16_t i : sol.getSequence().getSeq())
	{
		os << "C[" << i << "]==" << sol.getCompletion(i) << ";";
	}

	os << std::endl;

	for (uint16_t i : sol.getSequence().getSeq())
	{
		os << "ST[" << i << "]==" << sol.getStartingTime(i) << ";";
	}

	os << std::endl;

	os << "obj=" << sol.getObjectiveValue() << std::endl;
	
	return os;
}
