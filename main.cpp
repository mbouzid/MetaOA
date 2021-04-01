#include <iostream>
#include "solver.h"
#include <initializer_list>
#include <chrono>
#include <ctime>

int main(int argc, char* argv[])
{
	try
	{
		srand(161295);
		char* datname(argv[1]);
		char* out(argv[2]);
		size_t nbIter(atoi(argv[3]));
		double pm(atof(argv[4]));
		double pc(atof(argv[5]));
		double alpha(atof(argv[6]));
		double beta(atof(argv[7]));
		size_t popsize(atoi(argv[8]));
		double dr(0.5);
		
		OrderData*  dat(OrderData::load(datname));
	
		// 
		int** distM = new int * [dat->getN()+1];
		for (size_t i(0); i < dat->getN()+1; ++i)
		{
			distM[i] = new int[dat->getN()+1];
		}
		for (size_t i(0); i < dat->getN() + 1; ++i)
		{
			for (size_t j(0); j < dat->getN() + 1; ++j)
			{
				distM[i][j] = 0;
			}
		}


		/*Solution sol(Solution::genGreedy4(1, dat));
	
		std::cout << sol << std::endl;
		exit(0); */


		std::vector<Island> islands =
		{
				Island("exchange",Solution::exchange),
				Island("exchangeRandom",Solution::exchangeRandomly),
				Island("inversion",Solution::inversion),
				Island("swap",Solution::swap),
			//	Island("scramble",Solution::scramble),
				Island("add",Solution::add),
				Island("addRandomly",Solution::addRandomly),
				Island("shift",Solution::shift),
				//Island("2opt",Solution::opt),
			//	Island("insertAtBestPos",Solution::insertAtBestPos),
			//	Island("ATI",Solution::ATI),
			//	Island("randomShuffle",Solution::randomShuffle),
				Island("change",Solution::change),
			//	Island("testMut",Solution::testMut),
			//	Island("remove",Solution::remove)
				
		};

		Solver s(dat,distM,islands,popsize,alpha,beta);
		auto start = std::chrono::system_clock::now();
		s.run(nbIter,pm,pc,dr,out);
		auto end = std::chrono::system_clock::now();

		std::chrono::duration<double> elapsed_seconds = end - start;
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);

		std::cout <<  "time=" << elapsed_seconds.count() << std::endl;

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}				  

	return EXIT_SUCCESS;
}