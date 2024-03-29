#pragma once

#include <vector>
#include <iostream>
#include <exception>
#include <string>
#include "order.h"

class Sequence 
{
	private:

		size_t	_n;

		// sequence
		std::vector <uint16_t> _seq;
		
		// accepted 
		std::vector<bool> _accepted;


 		// insert order i at pos k 
		std::vector<uint16_t>::iterator insertAt(size_t k, uint16_t i) ;
		
		// remove order i
		std::vector<uint16_t>::iterator removeOrder(uint16_t i);

		void setAccepted(uint16_t i, bool accept = true);

		bool inRange(size_t k) const
		{
			return (0 <= k) and (k < _n);
		}

		bool inRange(uint16_t i) const
		{
			return (1 <= i) and (i <= _n);
		}

	public:

		Sequence(size_t n) :
			_n(n),
			_seq(),
			_accepted(n,false)
		{
		}


		Sequence(const Sequence& rhs) :
			_n(rhs._n),
			_seq(rhs._seq),
			_accepted(rhs._accepted)
		{
		
		}

		Sequence& operator = (const Sequence& rhs)
		{
			if (this != &rhs)
			{
				_n = rhs._n;
				_seq = rhs._seq; 
				_accepted = rhs._accepted;
			}
			return *this;
		}


		size_t getN() const
		{
			return _n;
		}

		// return true or false whether order at pos k is accepted
		bool getAccepted(size_t k) const;

		bool isAccepted(uint16_t i) const;

		// return true if order i is in sequence, false otherwise
		// complexity: O(n) 
		bool inSequence(uint16_t i) const
		{
			std::vector<uint16_t>::const_iterator found(std::find(_seq.begin(), _seq.end(), i));

			return found != _seq.end();
		}
		
	
		// append order at end
		void append(uint16_t i);
		void insertOrderAt(size_t k, uint16_t i) ;

		void deleteOrder(uint16_t i);

		~Sequence()
		{}

		// swap between order i and j
		void swap(uint16_t i, uint16_t j);

		// random shuffle between pos a and b
		void scrambleBetween(size_t a, size_t b);

		// inverse the sequence between pos a and b
		void inversion(size_t a, size_t b);

		// random shuffle in the whole sequence
		void randomShuffle();

		// exchange order i and rejected order x
		void exchange(uint16_t i, uint16_t x);

		// add rejected order with max revenue at random pos
		void add(uint16_t x, size_t pos);

		// remove order i from sequence
		void remove(uint16_t i);


		/* construct */
		// random gen
		void genRandom(double pAccepted = 0.75);

		static uint16_t getRandomOrderInSequence(Sequence & s);
		size_t getRandomPosInSequence() const;
		
		// get the most profitable pos in sequence without revenue loss
		size_t getMostProfitablePosInSequence(uint16_t i, OrderData * dat) const;

		uint16_t getRandomOrderRejected() const;
		
		uint16_t getMostProfitableRejectedOrder(OrderData * dat) const;
		uint16_t getLeastProfitableOrder(OrderData * dat) const;

		std::vector<uint16_t>  getRejectedOrders() const;

		size_t getPos(uint16_t i) const
		{
			if (inSequence(i))
			{				
				std::vector<uint16_t>::const_iterator found(std::find(_seq.cbegin(), _seq.cend(), i));
				return std::distance(_seq.cbegin(), found);

			}
		}


		bool operator == (const Sequence& rhs) const
		{
			return (_seq == rhs._seq);

		}

		const std::vector<uint16_t>& getSeq() const
		{
			return _seq;
		}

		std::vector<uint16_t>::iterator begin()
		{
			return _seq.begin();
		}
		
		std::vector<uint16_t>::iterator end()
		{
			return _seq.end();
		}

		uint16_t at(size_t i) const
		{
			return _seq.at(i);
		}

		uint16_t& operator [](size_t i)
		{
			return _seq[i];
		}

		size_t size() const
		{
			return _seq.size();
		}


		bool empty() const
		{
			return _seq.empty();
		}

		double getAcceptanceRate() const
		{
			double sum(0.0);
			for (size_t i(0); i <= _n; ++i)
				sum += _accepted[i];

			return sum / (double)_n;
		}

		std::string getStr() const
		{
			std::string str("");
			for (size_t k(0); k < _n; ++k)
			{
				if (_accepted.at(k))
				{
					str += std::to_string(k);
				}
				else
				{
					str += "0";
				}
			}
			return str;
		}



		static size_t bp(int** & distM, const Sequence& s1, const Sequence& s2)
		{
			for (size_t i(0); i < s1.size()-1; ++i)
			{
				distM[s1.at(i)][s1.at(i+1)] = 1;
				
			}

			size_t cpt(0);
			for (size_t i(0); i < s2.size()-1 ; ++i)
			{
				if (distM[s2.at(i)][s2.at(i+1)] != 1)
				{
					cpt += 1;
				}
				
			}

			for (size_t i(0); i < s1.size()-1; ++i)
			{
				distM[s1.at(i)][s1.at(i+1)] = 0;
				
			}


		
			return cpt;
		}

};


std::ostream& operator <<(std::ostream&, const Sequence&);