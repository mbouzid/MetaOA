#pragma once
#include "solution.h"
#include <random>
#include <functional>


class Island: public std::vector <Solution>
{
	static size_t s_id;

	private:

		size_t _id;

		std::string _name;

		// mutation operator
		std::function< void(Solution&, OrderData *) > _operator;
		
		
		// stats
		double _maxFit;
		double _avgFit;
		

	public:

		Island
		(
			const std::string & name,
			const std::function <void (Solution&,OrderData *)> & op
		):
			std::vector<Solution>(),
			_id(s_id++),
			_name(name),
			_operator(op),
			_maxFit(0.0),
			_avgFit(0.0)
		{}

		Island(const Island& rhs) :
			std::vector<Solution>(rhs),
			_id(rhs._id),
			_name(rhs._name),
			_operator(rhs._operator) ,
			_maxFit(rhs._maxFit),
			_avgFit(rhs._avgFit)
		{}

		const std::string& getName() const
		{
			return _name;
		}

		// randomly populate this island with nb individuals & data
		void populateRandomly(OrderData * d, size_t nb)
		{
			size_t cpt(0);

			
			while (cpt < nb)
			{
				//push_back(Solution::genRandom(_id, d));//push_back(Solution::genEmpty(_id,d));
				
				double randNum((double)rand() / (double)RAND_MAX);
				if (randNum < 0.20)
				{ 
					
					double otherRandNum((double)rand() / (double)RAND_MAX);
					if (otherRandNum < 0.5)
					{
						Solution sol(Solution::genGreedy2(_id, d));
					//	std::cout << sol << std::endl;
						
						Island::const_iterator found(std::find(begin(), end(), sol));
						if (found == end())
						{
							push_back(sol);
							++cpt;
						}
					}
					else
					{
						Solution sol(Solution::genGreedy3(_id, d));
						//std::cout << sol << std::endl;
						Island::const_iterator found(std::find(begin(), end(), sol));
						if (found == end())
						{
							push_back(sol);
							++cpt;
						}
					}
				}
				else 
				{ 	
					
					Solution sol(Solution::genRandom(_id, d));
					//std::cout << sol << std::endl;
					push_back(sol);
					++cpt;
				}	  
				
			}
		}

		void populateWithEmptySeq(OrderData* d, size_t nb)
		{
			size_t cpt(0);
			while (cpt < nb)
			{
				push_back(Solution::genEmpty(_id,d));
				++cpt;
			}
		}


		static void updateStats(Island & island, OrderData* dat);

		// apply operator with probability pm
		static void apply(Island & island, double pm, OrderData * dat);
		static void applyToAll(Island & island, OrderData * dat);


		void updateAvgFitness() ;
		void updateMaxFitness();

		double getMaxFitness() const
		{
			return _maxFit;
		}

		double getAvgFitness() const
		{
			return _avgFit;
		}

		Solution getBestSol() const;

		double getMinFitness() const;


		void print(std::ostream& os)
		{
			for (size_t i(0); i < size(); ++i)
			{
				os << at(i) << std::endl;
			}
		}


		std::pair <Solution, Solution> select() const;


		// replace dr percent of the worst population with child
		void replace(OrderData * dat, double pm, double dr = 0.5);

		~Island()
		{}


		double getAvgAcceptanceRate() const
		{
			if (empty())
				return 0.0;
			else
			{
				double avg(0.0);
				size_t nbIndiv(size());
				
				for (size_t i(0); i < nbIndiv; ++i)
					avg += at(i).getAcceptanceRate();

				return avg / (double)nbIndiv;
			}
		}

		double getBestAcceptanceRate() const
		{
			if (empty())
				return 0.0;
			else
			{
				double best(at(0).getAcceptanceRate());
				size_t nbIndiv(size());
				if (nbIndiv > 1)
				{ 
					
					for (size_t i(1); i < nbIndiv; ++i)
					{
						double accrate(at(i).getAcceptanceRate());
						if (accrate > best)
							best = accrate;
					}
				}
				return best;
			}
		}

		double getAvgServiceLevel() const
		{
			if (empty())
				return 0.0;
			else
			{
				double avg(0.0);
				size_t nbIndiv(size());
				for (size_t i(0); i < nbIndiv; ++i)
					avg += at(i).getServiceLevel();

				return avg / (double)nbIndiv;
			}
		}


		double getBestServiceLevel() const
		{
			if (empty())
				return 0.0;
			else
			{
				double best(at(0).getAcceptanceRate());
				size_t nbIndiv(size());
				if (nbIndiv > 1)
				{
					for (size_t i(1); i < nbIndiv; ++i)
					{
						double srvrate(at(i).getServiceLevel());
						if (srvrate > best)
							best = srvrate;
					}
				}
				return best;
			}
		}

};

std::ostream& operator << (std::ostream&, const Island& );

