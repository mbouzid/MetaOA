#pragma once
#include <vector>
#include <string>
#include <random>
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

namespace utils
{  
	enum OptionsParams
	{
		Invalid,
		OrderNb,
		ReleaseDate,
		ProcessingTime,
		DueDate,
		Deadline,
		PowerConsumption,
		Weight,
		ElecCost,
		Revenue,
		SetupTimes,
		CO2Intervals,
		TOUIntervals,
		Tax,
		TOUIntervalNb,
		CO2IntervalNb,
		CO2Emissions
	};



	std::vector <std::string> tokenize(const std::string& str, const std::string & delims);

	// martinbroadhurst.com
	std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");
	
	
	OptionsParams resolveOption(const std::string input);

	template<class T> std::ostream &  print(std::ostream & os, T* array, size_t n);


	void cleanVector(std::vector <std::string>& v);


	void normalize(std::vector <std::vector <double> >& v);
	void normalize(std::vector <double>& v);


	double normalize(double x, double minval, double maxval);


	uint16_t random_num(uint16_t a, uint16_t b);



	std::vector<size_t> argmax(std::vector<double> D_i);

	bool belongs(const std::vector <size_t>& indices, size_t index);

};



