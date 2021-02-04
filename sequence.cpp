#include "sequence.h"
#include "utils.h"
#include <chrono>
#include <algorithm>
#include <sstream>


std::ostream& operator<<(std::ostream& os, const Sequence& rhs)
{

	uint16_t prev(0);
	for (uint16_t i : rhs.getSeq())
	{
		os << "u[" << prev << "][" << i << "]==1;";
		prev = i;
	}

	os << std::endl;
	size_t n(rhs.getN());
	for (size_t k(0); k < n; ++k)
	{
		os << "a[" << k+1 << "]==" << rhs.getAccepted(k) << ";";
	}

	return os;
}

std::vector<uint16_t>::iterator Sequence::insertAt(size_t k, uint16_t i)	 
{
	std::vector<uint16_t>::iterator it(_seq.begin()+k);
	it = _seq.insert(it, i);
	return it;
}

std::vector<uint16_t>::iterator Sequence::removeOrder(uint16_t i)
{
	std::vector<uint16_t>::iterator it(std::find(_seq.begin(), _seq.end(), i));
	if (it != _seq.end())
		it = _seq.erase(it);
	return it;
}

void Sequence::setAccepted(uint16_t i, bool accept)
{
	if (inRange(i))
	{
		size_t k(i - 1);
		_accepted[k] = accept;
	}
	else
	{
		std::ostringstream oss;
		oss << "ERROR: order out of range." << i;
		throw std::exception(oss.str().c_str());
	}
}


// return true or false whether order at pos k is accepted

bool Sequence::getAccepted(size_t k) const
{
	if (inRange(k))
	{
		return _accepted[k];
	}
	else
	{
		throw std::exception("ERROR: index out of range");
	}
}

bool Sequence::isAccepted(uint16_t i) const
{
	if (inRange(i))
	{
		std::vector<uint16_t>::const_iterator found(std::find(_seq.cbegin(), _seq.cend(), i));
		if (found != _seq.cend())
		{
			size_t k(*found - 1);
			return _accepted[k];
		}
		else
		{
			std::ostringstream oss;
			oss << "ERROR: order out of range." << i;
			throw std::exception(oss.str().c_str());
		}
	}
	else
	{
		std::ostringstream oss;
		oss << "ERROR: order out of range." << i;
		throw std::exception(oss.str().c_str());
	}
}


// append order at end

void Sequence::append(uint16_t i)
{
	if (inRange(i))
	{
		if (not inSequence(i))
		{
			_seq.push_back(i);
			setAccepted(i);
		}
	}
	else
	{
		throw std::exception("ERROR: order out of range.");
	}
}

void Sequence::insertOrderAt(size_t k, uint16_t i)
{
	if (not inSequence(i))
	{
		insertAt(k, i);
		setAccepted(i);
	}
}

void Sequence::deleteOrder(uint16_t i)
{
	if (inSequence(i))
	{
		removeOrder(i);

		setAccepted(i, false);
	}

}

void Sequence::swap(uint16_t i, uint16_t j)
{
	if (inSequence(i) and inSequence(j) )
	{
		std::vector<uint16_t>::iterator it_i(std::find(_seq.begin(), _seq.end(), i));
		std::vector<uint16_t>::iterator it_j(std::find(_seq.begin(), _seq.end(), j));
		std::iter_swap(it_i, it_j);
	}
}

void Sequence::scrambleBetween(size_t a, size_t b)
{
	if (inRange(a) and inRange(b))
	{
		
		std::vector<uint16_t>::iterator start (_seq.begin() + a);
		std::vector<uint16_t>::iterator end(_seq.begin() + b);
		
		std::random_device rng;
		std::shuffle(start, end ,rng);
	}
}

void Sequence::inversion(size_t a, size_t b)
{
	std::vector<uint16_t>::iterator start(_seq.begin() + a);
	std::vector<uint16_t>::iterator end (_seq.begin() + b);
	std::reverse(start, end);
}

void Sequence::randomShuffle()
{
	/*thread_local*/ static std::random_device srd; 
	/*thread_local*/ static std::mt19937 smt(srd());
	smt.seed(161295);
	std::shuffle(_seq.begin(), _seq.end(),srd);
}

void Sequence::exchange(uint16_t i, uint16_t x)
{	
	if (inSequence(i) and not inSequence(x))
	{
		// add rejected order at the end
		append(x);

		//std::cout << "it1 " << std::endl;
		// get pos of order i
		std::vector<uint16_t>::iterator it_i(std::find(_seq.begin(), _seq.end(), i));
		//std::cout << "it2 " << std::endl;

		// get pos of newly added order x
		std::vector<uint16_t>::iterator it_x(std::find(_seq.begin(), _seq.end(), x));
		//std::cout << "it3 " << std::endl;

		// swap iterator
		std::iter_swap(it_i, it_x);

		//std::cout << "it4" << std::endl;
		// delete order i
		deleteOrder(i);
	}
}

void Sequence::add(uint16_t x, size_t pos)
{	  
	if (not inSequence(x))
	{
		//std::cout << "test" << std::endl;
		insertOrderAt(pos, x);
	}
}

void Sequence::remove(uint16_t i)
{
	if (inSequence(i))
	{
		removeOrder(i);
	}
}

void Sequence::genRandom(double pAccepted)
{
	
	for (size_t i(1); i <= _n; ++i)
	{ 
		double randNum(rand() / RAND_MAX);
		if (randNum < pAccepted)
		{
			append(i);
		}
	}

	//randomShuffle();

}



uint16_t Sequence::getRandomOrderInSequence(Sequence & rhs) 
{
	if (rhs._seq.empty())
		return 0;

	size_t pos(rand() % rhs._seq.size());
	return rhs._seq[pos];
}

size_t Sequence::getRandomPosInSequence() const
{
	if (_seq.empty())
		return 0;
	else
		return rand() % _seq.size();
}

size_t Sequence::getMostProfitablePosInSequence(uint16_t i, OrderData * dat) const
{
	if (_seq.empty())
		return 0;

	size_t pos(0);	  // standard pos is 0
	
	uint16_t prev(0);
	uint16_t Ccurrent(0);
	uint16_t Cestim(0);

	// compare completion times	
	size_t seqSize(size());
	for (size_t k(0); k < seqSize; ++k)
	{
		uint16_t j(at(k));
		// compute C[j]
		if (Ccurrent < dat->getR(j))
		{
			Ccurrent += dat->getR(j) + dat->getP(j) + dat->getS(prev, j);
		}
		else
		{
			Ccurrent += dat->getP(j) + dat->getS(prev, j);
		}

		// compute C[i]
		if (Cestim < dat->getR(i))
		{
			Cestim += dat->getR(i) + dat->getP(i) + dat->getS(prev, i);
		}
		else
		{
			Cestim += dat->getP(i) + dat->getS(prev, i);
		}


		// compare C[j] & C[i]
		if (Cestim < Ccurrent)
		{
			pos = k;   
			break;
		}
		else // continue
		{
			prev = j;
		}
	}


	return pos;
}

uint16_t Sequence::getRandomOrderRejected() const
{

	std::vector<uint16_t> rejected;
	for (size_t k(0); k < _n; ++k)
	{
		if (not _accepted[k])
			rejected.push_back(k+1);
	}

	if (rejected.empty())
		return 0;
	return rejected.at(rand()%rejected.size());
}

uint16_t Sequence::getMostProfitableRejectedOrder(OrderData* dat) const
{
	std::vector <uint16_t> rejected(getRejectedOrders());
	if (rejected.empty())
		return 0;
	std::sort(rejected.begin(), rejected.end(), [dat](uint16_t i, uint16_t j) 
		{ return (double(dat->getE(i))/double(dat->getP(i))) >= (double(dat->getE(j))/double(dat->getP(j))); });

	return rejected.at(0);
}

uint16_t Sequence::getLeastProfitableOrder(OrderData* dat) const
{
	std::vector <uint16_t> orders(_seq);
	if (orders.empty())
		return 0;

	std::sort(orders.begin(), orders.end(), [dat](uint16_t i, uint16_t j) 
		{ return (double(dat->getE(i)) / double(dat->getP(i))) <= (double(dat->getE(j)) / double(dat->getP(j))); });

	return orders.at(0);
}

std::vector<uint16_t> Sequence::getRejectedOrders() const
{
	std::vector<uint16_t> rejected;
	for (size_t k(0); k < _n; ++k)
	{
		if (not _accepted[k])
			rejected.push_back(k + 1);
	}
	return rejected;
}



