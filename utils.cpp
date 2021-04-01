#include "utils.h"
#include <fstream>
#include <algorithm>
#include <numeric>


int utils::hammingDist(const std::string & str1, const std::string & str2)
{
	return str1.compare(str2);
}

size_t utils::levenshtein_distance(int** dist, const std::string& str1, const std::string& str2)
{

	for (size_t i = 1, im = 0; i < str2.size(); ++i, ++im)
	{
		for (size_t j = 1, jn = 0; j < str1.size(); ++j, ++jn)
		{
			if (str1.at(jn) == str2.at(im))
			{
					dist[i][j] = dist[i - 1][j - 1];
			}
			else
			{
				dist[i][j] = std::min(dist[i - 1][j] + 1, /* A deletion. */
					std::min(dist[i][j - 1] + 1, /* An insertion. */
							dist[i - 1][j - 1] + 1)); /* A substitution. */
			}
		}
	}

		size_t r = dist[str1.size()-1][str2.size() - 1];


		return r;
}


std::vector<std::string> utils::tokenize(const std::string& str, const std::string & delims)
{

	std::vector<std::string> out;
	size_t beg, pos = 0;
	while ((beg = str.find_first_not_of(delims, pos)) != std::string::npos)
	{
		pos = str.find_first_of(delims, beg + 1);
		out.push_back(str.substr(beg, pos - beg) );
	}

	return out;
}

std::string& utils::ltrim(std::string& str, const std::string& chars )
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

std::string& utils::rtrim(std::string& str, const std::string& chars )
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

std::string& utils::trim(std::string& str, const std::string& chars)
{
	return ltrim(rtrim(str, chars), chars);
}


utils::OptionsParams utils::resolveOption(const std::string input)
{
	if (input == "n")
		return OptionsParams::OrderNb;
	if (input == "r")
		return 	OptionsParams::ReleaseDate;
	if (input == "p")
		return OptionsParams::ProcessingTime;
	if (input == "d")
		return OptionsParams::DueDate;
	if (input == "db")
		return OptionsParams::Deadline;
	if (input == "Omega")
		return OptionsParams::PowerConsumption;
	if (input == "w")
		return OptionsParams::Weight;
	if (input == "EC")
		return OptionsParams::ElecCost;
	if (input == "e")
		return OptionsParams::Revenue;
	if (input == "s")
		return OptionsParams::SetupTimes;
	if (input == "g")
		return OptionsParams::CO2Intervals;
	if (input == "b")
		return OptionsParams::TOUIntervals;
	if (input == "Tax")
		return OptionsParams::Tax;
	if (input == "m")
		return OptionsParams::TOUIntervalNb;
	if (input == "h")
		return OptionsParams::CO2IntervalNb;
	if (input == "q")
		return OptionsParams::CO2Emissions;
	else
		return OptionsParams::Invalid;
}



void utils::cleanVector(std::vector<std::string>& v)
{	
	
	auto is_whitespace = [] (const std::string & str)
	{
		return std::all_of(
		begin(str), end(str), [](unsigned char c) { return std::isspace(c); });
	};

	for (std::vector<std::string>::iterator it =  v.begin(); it != v.end() && is_whitespace(*it); )
	{
		it = v.erase(it);
	}
}

double utils::normalize(double x, double minval, double maxval)
{
	return ((x - minval) / (maxval-minval));
}
#include <iostream>

uint16_t utils::random_num(uint16_t a, uint16_t b)
{
	if (a == b && (a == 0))
		return 0;
	if (a == b)
		return a;
	return (rand() % (b - a)) + a;
}




std::vector<size_t> utils::argmax(std::vector<double> D_i)
{
	std::vector<size_t> argMax;

	double maxElt(*std::max_element(D_i.begin(), D_i.end()));
	
	size_t n(D_i.size());
	for(size_t i(0); i < n;++i)
	{
		if (D_i.at(i) >= maxElt)
			argMax.push_back(i);
	}
	return argMax;
}

bool utils::belongs(const std::vector<size_t> & indices, size_t index)
{
	if (indices.empty())
		return false;
	else
	{
		std::vector<size_t>::const_iterator found(std::find(indices.cbegin(), indices.cend(), index));
		return (found != indices.cend());
	}
}

template<class T>
std::ostream&  utils::print(std::ostream& os, T* array, size_t n)
{
	os << "{";
	if (n >= 1)
	{
		os << array[0];
		for (size_t i(1); i < n; ++i)
		{
			os << "," << array[i];
		}
	}
	os << "}";
	return os;
}

#include <iostream>

void utils::normalize(std::vector <std::vector <double> >& v)
{
	size_t n(v.size());
	for (size_t i(0); i < n; ++i)
	{  
		double sum (0.0);
		for (size_t j(0); j < n; ++j)
		{	  
			sum += v[i][j];
		}
		for (size_t j(0); j < n; ++j)
		{
			v[i][j] = v.at(i).at(j) / sum;
		}
	}
}

void utils::normalize(std::vector <double>& v)
{
	double sum(std::accumulate(v.begin(), v.end(), 0.0));

	size_t n(v.size());
	for (size_t i(0); i < n; ++i)
	{
		v[i]= v.at(i) / sum;
	
	}
}

