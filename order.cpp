#include "order.h"
#include "utils.h"
#include <fstream>
#include <string>
#include <sstream>



std::ostream& operator<<(std::ostream& os, const OrderData& o)
{

	os << "n=" << o.getN() << std::endl;
	
	os << "r=";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getR(i) << " ";
	os << std::endl;
	os << "p=";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getP(i) << " ";
	os << std::endl;
	os << "d= ";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getD(i) << " ";
	os << std::endl;
	os << "db= ";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getDb(i) << " ";
	os << std::endl;
	os << "e= ";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getE(i) << " ";
	os << std::endl;
	os << "Omega= ";
	for (size_t i(0); i < o.getN(); ++i)
		os << o.getOmega(i) << " ";
	os << std::endl;

	os << "s= ";
	for (size_t i(0); i <= o.getN(); ++i)
	{
		for (size_t j(0); j <= o.getN(); ++j)
		{
			os << o.getS(i,j) << " ";
		}
		os << std::endl;
	}

	os << "m=" << o.getM() << std::endl;
	os << "h=" << o.getH() << std::endl;
	
	os << "b= ";
	for (size_t k(0); k < o.getM(); ++k)
	{
		os << o.getB(k) << " ";
	}
	os << std::endl;

	os << "g= ";
	for (size_t k(0); k < o.getH(); ++k)
	{
		os << o.getG(k) << " ";
	}
	os << std::endl;
	os << "w= ";
	for (size_t i(0); i <= o.getN(); ++i)
		os << o.getW(i) << " ";
	os << std::endl;

	os << "EC= ";
	for (size_t k(0); k < o.getM(); ++k)
	{
		os << o.getEC(k) << " ";
	}
	os << std::endl;
	os << "q= ";
	for (size_t k(0); k < o.getH(); ++k)
	{
		os << o.getQ(k) << " ";
	}
	os << std::endl;

	os << "Tax=" << o.getTax() << std::endl;


	return os << "";
}

OrderData* OrderData::load(const char* datname)
{

	std::ifstream f(datname);
	std::string line;

	size_t n(0), m(0), h(0);
	double Tax(0.0);
	uint16_t* r = nullptr, * p = nullptr, * d = nullptr, * db = nullptr, * e = nullptr, * Omega =nullptr , **s = nullptr , *g =nullptr, *b = nullptr;
	double* w =nullptr, *q =nullptr, * EC =nullptr;


	while (std::getline(f, line))
	{
		if (not (line[0] =='/') and not (line[1] == '/'))
		{ 
			std::stringstream iss(line);
			std::string paramName, values;
		
			std::getline(iss, paramName, '=');
			utils::trim(paramName);

			std::getline(iss, values, ';');
			std::vector<std::string> tokens(utils::tokenize(values, { '[',',',']' }));

			utils::cleanVector(tokens);

			switch (utils::resolveOption(paramName))
			{
				case  utils::OptionsParams::OrderNb:
				{
					n = atoi(values.c_str());
					break;
				}

				case utils::OptionsParams::ReleaseDate:
				{
					r = (uint16_t*)calloc(n + 1, sizeof(uint16_t));

					for (size_t i(0); i <= n; ++i)
					{
						r[i] = atoi(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::ProcessingTime:
				{
					p = (uint16_t*)calloc(n + 1, sizeof(uint16_t));

					for (size_t i(0); i <= n; ++i)
					{
						p[i] = atoi(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::DueDate:
				{
					d = (uint16_t*)calloc(n + 1, sizeof(uint16_t));

					for (size_t i(0); i <= n; ++i)
					{
						d[i] = atoi(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::Deadline:
				{
					db = (uint16_t*)calloc(n + 1, sizeof(uint16_t));

					for (size_t i(0); i <= n; ++i)
					{
						db[i] = atoi(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::PowerConsumption:
				{

					Omega = (uint16_t*)calloc(n, sizeof(uint16_t));

					for (size_t i(0); i < n; ++i)
					{
						Omega[i] = atoi(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::Weight:
				{

					w = (double*)calloc(n + 1, sizeof(double));

					for (size_t i(0); i <= n; ++i)
					{
						w[i] = atof(tokens.at(i).c_str());

					}

					break;
				}

				case utils::OptionsParams::ElecCost:
				{

					EC = (double*)calloc(m, sizeof(double));

					for (size_t i(0); i < m; ++i)
					{
						EC[i] = atof(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::Revenue:
				{
					e = (uint16_t*)calloc(n + 1, sizeof(uint16_t));

					for (size_t i(0); i <= n; ++i)
					{
						e[i] = atoi(tokens.at(i).c_str());

					}

					break;
				}

				case utils::OptionsParams::SetupTimes:
				{

					s = (uint16_t**)calloc(n + 1, sizeof(uint16_t*));
					size_t k(0);
					while (k <= n)
					{
						std::stringstream iss(line);

						std::getline(iss, values, ';');
						std::vector<std::string> tokens(utils::tokenize(values, { '[',',',']' }));

					

						s[k] = (uint16_t*)calloc(n + 1, sizeof(uint16_t));

						for (size_t i(0); i <= n; ++i)
						{
							// TODO (dirty)
							if (k == 0)
							{
								s[k][i] = atoi(tokens.at(i+1).c_str());
							}
							else
							{
								s[k][i] = atoi(tokens.at(i).c_str());
							}
						}

						++k;
						std::getline(f, line); // next line
					}

					break;
				}

				case utils::OptionsParams::CO2Intervals:
				{
					g = (uint16_t*)calloc(h, sizeof(uint16_t));

					for (size_t i(0); i < h; ++i)
					{
						g[i] = atoi(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::TOUIntervals:
				{
					b = (uint16_t*)calloc(m, sizeof(uint16_t));

					for (size_t i(0); i < m; ++i)
					{
						b[i] = atoi(tokens.at(i).c_str());
					}

					break;
				}

				case utils::OptionsParams::Tax:
				{
					Tax = atof(values.c_str());
					break;
				}

				case utils::OptionsParams::TOUIntervalNb:
				{
					m = atoi(values.c_str());

					break;
				}

				case utils::OptionsParams::CO2IntervalNb:
				{
					h = atoi(values.c_str());

					break;
				}

				case utils::OptionsParams::CO2Emissions:
				{
					q = (double*)calloc(h, sizeof(double));

					for (size_t i(0); i < h; ++i)
					{
						q[i] = atof(tokens.at(i).c_str());
					}
					break;
				}

				case utils::OptionsParams::Invalid:
				{
					throw("ERROR: invalid parameter.");
					break;
				}

			}
		}



	}

	f.close();

	return new OrderData(n,r,p,d,db,e,Omega,s,m,h,b,g,w,EC,q,Tax);
}
