#include "solution.h"
#include <numeric>
#include "ATI.h"

// update & repair
void Solution::updateCompletionTimes(OrderData* d)
{
	if (_seq.getSeq().empty())
	{
		
		std::cout << "empty" << std::endl;
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
		
		// slack = _C[prev] - d->getDb()[prev]- d->getP()[prev] + 1;
		
		// if completion time of previous job is before release, update C[k] to its release date 
		if (_C.at(prev) <= d->getR(i))
		{
			_C[i] = d->getR(i) + d->getS(prev, i) + d->getP(i);
			_ST[i] = d->getR(i);
		}
		else	// update completion time with previous completion time of order and processing and setup
		{
			_C[i] = _C.at(prev) + d->getS(prev, i) + d->getP(i);
			_ST[i] = _C.at(prev);
		}

		
		// if completion is greater than deadline => reject order
		// prev is not updated
		if (_C.at(i) > d->getDb(i))
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
		_ST[i] = 0;
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
	size_t N(_seq.getN());


	for (size_t k(0); k < n; ++k)
	{
		_profit[k] = 0.0;
		_tardiness[k] = 0.0;
	}
	
	uint16_t prev(0); 

	for (size_t k(0); k < n ; ++k)
	{
		uint16_t i(_seq.at(k));	// kth order

		_tardiness[k] = (std::max(0, _C.at(i) - d->getD(i)));
		// energy cost
		double energyCost(0.0);

		
		for (uint16_t t(1); t <= _C.at(i)-_ST.at(i); ++t)
		{
			energyCost += d->getEnergyCost(i, _C.at(i) - t);
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

	while (pos1 == pos2)
	{
		pos2 = sol._seq.getRandomPosInSequence();
	}

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
	
	sol.updateCompletionTimes(dat);

	std::vector<uint16_t> rejected;

	uint16_t prev(0);
	for (size_t k(0); k < sol._seq.size(); ++k)
	{
		uint16_t i(sol._seq.at(k)); // get kth order  to be processed

		uint16_t start(std::max(dat->getR(i),sol._C.at(prev)));	  
		
		uint16_t last(dat->getD(i) - dat->getP(i) - dat->getS(prev, i));
		if (k != sol._seq.size()-1)
			last = std::min(dat->getD(i) ,sol._ST.at(sol._seq.at(k + 1))) - dat->getP(i) - dat->getS(prev,i);
	 

		
		double prevEnergyCost(0.0);
		for (uint16_t tp(sol._ST.at(i)); tp <= sol._C.at(i) ; ++tp)
		{
			prevEnergyCost += dat->getEnergyCost(i, tp);
		}
		


		uint16_t bestStartingTime(start);


		for (uint16_t t(start); t <= last; ++t)
		{
			sol._ST[i] = t;
			sol._C[i] = sol._ST.at(i) + dat->getS(prev, i) + dat->getP(i);

			double energyCost(0.0);
			for (uint16_t tp(sol._ST.at(i)); tp <= sol._C.at(i); ++tp)
			{	
				energyCost += dat->getEnergyCost(i,tp);
			}

			if (energyCost < prevEnergyCost)
			{	
			
				prevEnergyCost = energyCost;
				bestStartingTime = t;	
			}
		}

		

		sol._ST[i] = bestStartingTime;
		//std::cout << "ST=" << sol._ST.at(i) << std::endl;
		sol._C[i] = sol._ST.at(i) + dat->getS(prev, i) + dat->getP(i);
		

		// if completion is greater than deadline => reject order
		// prev is not updated
		if (sol._C.at(i) > dat->getDb(i))
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
		sol._ST[i] = 0;
	}


	sol.computeObjective(dat);

}

void Solution::opt(Solution& sol, OrderData* dat)
{
	uint16_t i(Sequence::getRandomOrderInSequence(sol._seq));
	uint16_t j(Sequence::getRandomOrderInSequence(sol._seq));

	while (i == j)
	{
		j = Sequence::getRandomOrderInSequence(sol._seq);
	}

	Solution bis(sol);
	bis._seq.swap(i, j);
	update(bis, dat);

	if (bis.getObjectiveValue() > sol.getObjectiveValue())
	{
		sol._seq.swap(i, j);
		update(sol, dat);
	}

}

void Solution::insertAtBestPos(Solution& sol, OrderData* dat)
{
	// on recupere une tache 
	uint16_t i(Sequence::getRandomOrderInSequence(sol._seq));
	uint16_t r(dat->getR(i));

	// on la supprime de la sequence
	//sol._seq.deleteOrder(i);
	// maj 
	//update(sol, dat);

	std::vector<uint16_t> rejected(sol._seq.getRejectedOrders());

	std::sort(rejected.begin(), rejected.end(), [&dat,&r](const uint16_t & i, const uint16_t& j) 
		{
			return std::abs( r - dat->getR(i) ) <= std::abs( r - dat->getR(j))
			; 
		}
	);

	uint16_t x(rejected.at(0));

	// on va echanger i avec la commande la plus proche en terme de release date
	
	sol._seq.exchange(i, x);

	update(sol, dat);

}

void Solution::RISJ(Solution& sol, OrderData* dat)
{
	size_t pos1(sol._seq.getRandomPosInSequence()), pos2(pos1+1);
	if (pos2 < sol._seq.size())
	{
		sol._seq.swap(sol._seq[pos1], sol._seq[pos2]);
		update(sol, dat);
	}
	
}

void Solution::change(Solution& sol, OrderData* dat)
{
	size_t pos1(sol._seq.getRandomPosInSequence());
	uint16_t i(sol._seq[pos1]);
	sol._seq.deleteOrder(i);
	size_t nextpos (utils::random_num(0, sol._seq.size()));

	
	sol._seq.insertOrderAt(nextpos, i);
	update(sol, dat);
}

void Solution::ATI(Solution& sol, OrderData* dat)
{
	size_t cpt(0);
	std::vector<uint16_t> subsequence; 
	size_t pos(sol._seq.getRandomPosInSequence());
	subsequence.push_back(sol.getSequence().at(pos));
	
	size_t nb(std::min(sol._seq.size(), pos + 4)-pos);
	for (size_t p(pos+1); p < std::min(pos+4,sol.getSequence().size()) ; ++p)
	{
		subsequence.push_back(sol.getSequence().at(p));
	}

	std::vector<uint16_t> bestpermut(ATI::solve(&sol, subsequence, dat));


	size_t k(0);
	for (size_t p(pos); p < pos + nb; ++p)
	{
		sol._seq[p] = bestpermut.at(k);
		++k;
	}


	update(sol, dat);
	
}

void Solution::testMut(Solution& sol, OrderData* dat)
{	
	std::vector<uint16_t> rejected(sol._seq.getRejectedOrders());

	std::vector<uint16_t> accept;
	for (size_t k(0); k < 4; ++k)
	{
		uint16_t i(Sequence::getRandomOrderInSequence(sol._seq));
		accept.push_back(i);
		sol._seq.deleteOrder(i);
	}

	for (uint16_t r : rejected)
	{
		size_t pos(sol._seq.getRandomPosInSequence());
		sol._seq.insertOrderAt(pos, r);
	}
	
	update(sol, dat);

}

Solution Solution::singlePointCrossover(const Solution& p1, const Solution& p2, OrderData * dat)
{
	size_t n1(p1._seq.size()), n2(p2._seq.size());



	size_t pos(utils::random_num(0,std::min(n1,n2)));

	Sequence child(p1._seq.getN());

	// parent 1 from k to pos-1	
	for (size_t k(0); k < pos; ++k)
	{
		child.append(p1.getOrderAt(k));
	}

	// parent 2 from pos to n1-1
	for (size_t k(pos); k < n2; ++k)
	{
		child.append(p2.getOrderAt(k));
	}
	
	return Solution(p1.getOrigin(), child, dat);
}





Solution Solution::testCrossover(const Solution& p1, const Solution& p2, OrderData* dat)
{
	size_t n1(p1._seq.size()), n2(p2._seq.size());
	

	Sequence childseq(p1._seq.getN());

	if (n1 > n2)
	{
		for (size_t k(0); k < n1; ++k)
		{
			
			if (k < n2)
			{
				double randNum((double)rand() / (double)RAND_MAX);
				if (randNum < 0.5)
				{
					childseq.append(p1.getOrderAt(k));
					childseq.append(p2.getOrderAt(k));
				}
				else
				{
					childseq.append(p2.getOrderAt(k));	 
					childseq.append(p1.getOrderAt(k));
				}
			}
			else
			{
			
					childseq.append(p1.getOrderAt(k));

			}
		}

	}
	else
	{
		for (size_t k(0); k < n2; ++k)
		{

			if (k < n1)
			{
				double randNum((double)rand() / (double)RAND_MAX);
				if (randNum < 0.5)
				{
					childseq.append(p2.getOrderAt(k));
					childseq.append(p1.getOrderAt(k));
				}
				else
				{
					childseq.append(p1.getOrderAt(k));
					childseq.append(p2.getOrderAt(k));
				}
			}
			else
			{
				
					childseq.append(p2.getOrderAt(k));

			}
		}
	}


	return Solution(p1.getCurrentOrigin(), childseq, dat);
}

Solution Solution::testCrossover1(const Solution& p1, const Solution& p2, OrderData* dat)
{
	size_t n1(p1._seq.size()), n2(p2._seq.size());


	Sequence childseq(p1._seq.getN());

	if (n1 > n2)
	{
		for (size_t k(0); k < n1; ++k)
		{

			if (k < n2)
			{
				double randNum((double)rand() / (double)RAND_MAX);
				if (randNum < 0.5)
				{
					childseq.append(p1.getOrderAt(k));
					//childseq.append(p2.getOrderAt(k));
				}
				else
				{
					childseq.append(p2.getOrderAt(k));
					//childseq.append(p1.getOrderAt(k));
				}
			}
			else
			{
				childseq.append(p1.getOrderAt(k));
			}
		}

	}
	else
	{
		for (size_t k(0); k < n2; ++k)
		{

			if (k < n1)
			{
				double randNum((double)rand() / (double)RAND_MAX);
				if (randNum < 0.5)
				{
					childseq.append(p2.getOrderAt(k));
					//childseq.append(p1.getOrderAt(k));
				}
				else
				{
					childseq.append(p1.getOrderAt(k));
					//childseq.append(p2.getOrderAt(k));
				}
			}
			else
			{
				childseq.append(p2.getOrderAt(k));
			}
		}
	}


	return Solution(p1.getCurrentOrigin(), childseq, dat);
}


// generate random
Solution Solution::genRandom(size_t origin, OrderData* dat, double pAccepted)
{
	Sequence s(dat->getN());
	s.genRandom(1.0);

	return Solution(origin, s, dat);
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

Solution Solution::genGreedy1(size_t origin, OrderData* dat)
{
	Sequence s(dat->getN());

	std::vector<uint16_t> orders;

	for (uint16_t i(1); i <= dat->getN(); ++i)
	{
		orders.push_back(i);
	}

	std::sort(orders.begin(), orders.end(), [&dat](const uint16_t& i, const uint16_t& j) { return dat->getR(i) <= dat->getR(j); });


	for (uint16_t i : orders)
	{
		s.append(i);
	}


	return Solution(origin,s,dat);
}

Solution Solution::genGreedy2(size_t origin, OrderData* dat)
{
	Sequence s(dat->getN());

	std::vector<uint16_t> L;

	for (uint16_t i(1); i <= dat->getN(); ++i)
	{
		L.push_back(i);
	}

	std::sort(L.begin(), L.end(), [&dat](const uint16_t& i, const uint16_t& j) { return dat->getR(i) <= dat->getR(j); });
	

	// t = rmin
	uint16_t t(dat->getR(*L.begin()));

	std::vector<uint16_t> reject;
	uint16_t prev(0);
	while (not L.empty())
	{
		std::vector<uint16_t> crit;
		for (uint16_t j : L)
		{
			if (dat->getR(j) <= t)
			{
				crit.push_back(j);
			}
		}

		
		// advance
		if (crit.empty())
		{	
			
			uint16_t rmin(std::max(t,dat->getR(L.at(0))));
			
			uint16_t argrmin(L.at(0));
			// find next order 
			for (size_t ii(1); ii < L.size(); ++ii)
			{
				uint16_t i(L.at(ii));
				if (rmin  > std::max(t, dat->getR(i)))
				{
					argrmin = i;
					rmin = std::max(t, dat->getR(i));
				}
			}
			
			t = rmin;
			crit.push_back(argrmin);
		
		}
			 
		std::sort(crit.begin(), crit.end(), [&dat,&t,&prev](const uint16_t& i, const uint16_t & j) {
			return (double) dat->getE(i)/(double)( dat->getP(i) + dat->getS(prev,i))
				>= (double)dat->getE(j) / (double)(dat->getP(j) + dat->getS(prev, j))
				
				; });
		
		
		size_t i(0);
		uint16_t k(crit.at(i));

		if ( t <= dat->getDb(k) )
		{
			s.append(k);
			prev = k;	  
			L.erase(std::find(L.begin(),L.end(),k));
			t += dat->getP(k) + dat->getS(prev,k);
		}
		else
		{
			reject.push_back(k);
			L.erase(std::find(L.begin(), L.end(), k));
			
		}
		

	}

	
	return Solution(origin, s, dat);
}

Solution Solution::genGreedy3(size_t origin, OrderData* dat)
{
	Sequence s(dat->getN());

	std::vector<uint16_t> orders;

	for (uint16_t i(1); i <= dat->getN(); ++i)
	{
		orders.push_back(i);
	}


	std::sort(orders.begin(), orders.end(), [&dat](const uint16_t& i, const uint16_t& j) { return dat->getR(i)*(dat->getDb(i)) <= dat->getR(j)*(dat->getDb(j)); });

	for (uint16_t i : orders)
	{
		s.append(i);
	}

	return Solution(origin, s, dat);
}

Solution Solution::genGreedy4(size_t origin, OrderData* dat)
{
	Sequence s(dat->getN());

	std::vector<uint16_t> orders;
	for (uint16_t i(1); i <= dat->getN(); ++i)
	{
		orders.push_back(i);
	}

	// ATCS rule
	uint16_t t(0);
	size_t l(0);

	std::vector<uint16_t> sq;
	while (not orders.empty())
	{

		std::map <uint16_t,double> ATCS;

		for (uint16_t i : orders)
		{
			ATCS.emplace(i, 0.0);
		}


		double pavg(0);
		for (uint16_t i : orders)
		{
			pavg += dat->getP(i);
		}
		pavg = pavg / (double)orders.size();

		double savg(0.0);
		for (uint16_t i : orders)
		{
			for (uint16_t j : orders)
			{
				savg += dat->getS(i, j);
			}

		}
		savg = savg / (orders.size()*orders.size());





		for (uint16_t i : orders)
		{

			double term1( ((double)dat->getW(i) / (double)dat->getP(i)));

			double term2(-(double)std::max((uint16_t)(dat->getD(i) - dat->getP(i) - t), /*dat->getR(i)*/(uint16_t)0) / (0.9 * pavg));
			double term3(-((double)dat->getS(l, i) / (0.9 * savg)));


			double t1(std::exp(term2)), t2(std::exp(term3));
			
			ATCS[i] = (term1)*t1 * t2;



			std::sort(orders.begin(), orders.end(), [&ATCS](const uint16_t& i, const uint16_t& j)
				{
					return ATCS.at(i) >= ATCS.at(j);
				});
			

		}


		std::vector<double> atcs;
		for (uint16_t i : orders)
		{
			atcs.push_back(ATCS.at(i));
		}

		/*std::discrete_distribution<> d(atcs.begin(), atcs.end());


		static std::random_device srd;
		static std::mt19937 smt(srd());
		smt.seed(161295);

		
		uint16_t i(orders.at(d(smt)));	*/
		uint16_t i(orders.at(0));
		t = std::max(t, dat->getR(l)) + dat->getP(l) + dat->getS(l, i);
		l = i;
		sq.push_back(l);
		
		
		orders.erase(std::find(orders.begin(),orders.end(),l));


	}


	for (uint16_t i : sq)
	{
		s.append(i);
	}

	return Solution(origin, s, dat);
}

Solution Solution::genGreedy(size_t origin, OrderData* dat)
{

	std::vector <double> RLR(dat->getN()+1,0.0);
	Sequence s(dat->getN());

	
	std::vector<uint16_t> orders;

	for (uint16_t i(1); i <= dat->getN(); ++i)
	{
		orders.push_back(i);
	}

	std::sort(orders.begin(), orders.end(), [&dat](const uint16_t& i, const uint16_t& j) { return dat->getDb(i) <=dat->getDb(j); });
	
	uint16_t t(0);
	uint16_t prev(0);
	for (uint16_t i : orders)
	{
		t = std::max(t, dat->getR(i));
		if (t <= dat->getDb(i))	
		{	
			
			s.append(i);  
			t += dat->getS(prev,i) + dat->getP(i);
			prev = i;
		}
		
	}

	
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
