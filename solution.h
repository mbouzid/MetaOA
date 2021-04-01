#pragma once

#include "sequence.h"
#include "order.h"
#include "utils.h"
#include <algorithm>

class Solution
{
	private:
		
		// island ID origin 
		size_t _prevOrigin;
		size_t _currentOrigin;

		Sequence _seq;
		std::vector<uint16_t> _C;
		std::vector<uint16_t> _ST;


		double _objVal;

		std::vector <double> _profit;
		std::vector <size_t> _tardiness;

		void updateCompletionTimes(OrderData* d);
		void computeObjective(OrderData* d) ;
		
		// private constructor
		Solution(size_t from, const Sequence & seq, OrderData * d):
			_prevOrigin(from),
			_currentOrigin(from),
			_seq(seq),
			_C(_seq.getN()+1,0),
			_ST(_seq.getN() + 1,0),
			_objVal(0.0),
			_profit(_seq.getN() + 1, 0),
			_tardiness(_seq.getN() + 1, 0)
		{
			// at creation : update Completion times and objective
			updateCompletionTimes(d);
			computeObjective(d);
		}


	public:


		


		Solution(const Solution& rhs) :
			_prevOrigin(rhs._prevOrigin),
			_currentOrigin(rhs._currentOrigin),
			_seq(rhs._seq),
			_C(rhs._C),
			_ST(rhs._ST),
			_objVal(rhs._objVal),
			_profit(rhs._profit),
			_tardiness(rhs._tardiness)
		{
			
		}

		Solution& operator = (const Solution& rhs)
		{	 
			if (&rhs != this)
			{
				_prevOrigin = rhs._prevOrigin;
				_currentOrigin = rhs._currentOrigin;
				_seq = rhs._seq;
				_C = rhs._C;
				_ST = rhs._ST;
				_objVal = rhs._objVal;
				_profit = rhs._profit;
				_tardiness = rhs._tardiness;
			}

			return (*this);
		}

		size_t getCurrentOrigin() const
		{
			return _currentOrigin;
		}

		size_t getOrigin() const
		{
			return _prevOrigin;
		}

		void updateOrigin(size_t id)
		{
			size_t tmp(_currentOrigin);
			_currentOrigin = id;
			_prevOrigin = tmp;		
		}

		size_t getTardiness(size_t k) const
		{
			return _tardiness.at(k);
		}

		double getProfit(size_t k) const
		{
			return _profit.at(k);
		}

		double getObjectiveValue() const
		{
			return _objVal;
		}

		const Sequence& getSequence() const
		{
			return _seq;
		}


		uint16_t getOrderAt(size_t k) const
		{
			return _seq.at(k);
		}

		void printCompletion(std::ostream& os)
		{
			for (size_t i(0); i <= _seq.getN(); ++i)
			{
				os << _C.at(i) << " ";
			}
			os << std::endl;
		}



		
		// completion time of order i
		uint16_t getCompletion(uint16_t i) const
		{
			return _C.at(i);
		}

		const std::vector<uint16_t>& getCompletionTimes() const
		{
			return _C;
		}

		const std::vector<uint16_t>& getStartingTimes() const
		{
			return _ST;
		}


		// completion time of order i
		uint16_t getStartingTime(uint16_t i) const
		{
			return _ST.at(i);
		}


		uint16_t getStartingTime(size_t i) const
		{
			return _ST.at(i);
		}

		/* neighbor moves */

		static void randomShuffle(Solution& sol, OrderData * dat = nullptr);

		static void scramble(Solution& sol, OrderData * dat = nullptr);

		static void inversion(Solution& sol, OrderData *dat = nullptr);

		static void swap(Solution& sol, OrderData * dat = nullptr);

		static void exchange(Solution& sol, OrderData * dat);
		
		static void exchangeRandomly(Solution& sol, OrderData* dat);

		static void add(Solution& sol, OrderData * dat);

		static void addRandomly(Solution& sol, OrderData* dat);

		static void remove(Solution& sol, OrderData * dat);

		static void shift(Solution& sol, OrderData* dat);

		static void opt(Solution& sol, OrderData* dat);

		static void insertAtBestPos(Solution& sol, OrderData* dat);

		static void RISJ(Solution& sol, OrderData* dat);

		static void change(Solution& sol, OrderData* dat);

		static void ATI(Solution& sol, OrderData* dat);

		static void testMut(Solution& sol, OrderData* dat);

		
		// recombinaison 
		static Solution singlePointCrossover(const Solution& p1, const Solution& p2, OrderData* dat);
		static Solution testCrossover(const Solution& p1, const Solution& p2, OrderData* dat);
		static Solution testCrossover1(const Solution& p1, const Solution& p2, OrderData* dat);


		static void update(Solution& sol, OrderData * dat)
		{
			sol.updateCompletionTimes(dat);
			sol.computeObjective(dat);
		}

		static double getObjective(const Solution& sol)
		{
			return sol.getObjectiveValue();
		}


		// generate random
		static Solution genRandom(size_t origin, OrderData* dat, double pAccepted = 1.0);
		static Solution genEmpty(size_t origin, OrderData* dat);


		static double getBestSolFrom(const std::vector <Solution>& sols);

		// generate greedy	
		/* TODO */
		static Solution genGreedy(size_t origin, OrderData* dat);
		static Solution genGreedy1(size_t origin, OrderData* dat);
		// J-schedule
		static Solution genGreedy2(size_t origin, OrderData* dat);
		static Solution genGreedy3(size_t origin, OrderData* dat);
		static Solution genGreedy4(size_t origin, OrderData* dat);

		bool operator == (const Solution& sol) const
		{
			return sol._seq == _seq;
		}

		~Solution()
		{}

		static double getFitness(Solution* sol)
		{
			if (sol == nullptr)
			{
				return 0.0;
		   }
			else
			{
				return sol->_objVal;
			}
		}

		bool operator < (const Solution& rhs) const
		{
			return _objVal < rhs._objVal;
		}

		double getAcceptanceRate() const
		{
			return _seq.getAcceptanceRate();
		}

		double getServiceLevel() const
		{
			if (_seq.empty())
				return 0.0;
			// tells % of order non-tardy

			size_t nonTardy(0);
			size_t nbOrders(_seq.size());
			for (size_t i(0); i < nbOrders; ++i)
			{
				if (_tardiness.at(i) == 0)
					nonTardy += 1;
			}

			return (double)nonTardy / (double)nbOrders;
		}

		uint16_t getTardiestOrder() const
		{
			if (_seq.empty())
				return 0;
			// tells % of order non-tardy

			size_t tardy(0);
			uint16_t tard(_tardiness.at(tardy));
			size_t nbOrders(_seq.size());
			for (size_t i(1); i < nbOrders; ++i)
			{
				if (_tardiness.at(_seq.at(i)) > tard)
				{
					tardy = _seq.at(i);
					tard = _tardiness.at(tardy);
				}
			}

			return tard;
		 }

		void printSeq() const
		{
			std::cout << "0";
			for (uint16_t i : _seq.getSeq())
			{
				std::cout << "->" << i;
			}
			std::cout << std::endl;

		}

		std::string toString() const
		{
			std::string seq("");
			for (uint16_t c : _C)
			{
				seq += std::to_string(c);
			}

			return seq;
		}

		uint16_t getMakespan() const
		{
			uint16_t lastProcOrder(_seq.at(_seq.size() - 1));
			return _C.at(lastProcOrder);
		}

		void deleteOrder(uint16_t i, OrderData * dat)
		{
			_seq.deleteOrder(i);
		
			updateCompletionTimes(dat);
			computeObjective(dat);
		}

		static size_t bp(int ** distM, const Solution& s1, const Solution& s2)
		{
			return Sequence::bp(distM, s1._seq, s2._seq);
		}

};

std::ostream & operator << (std::ostream&, const Solution& );
