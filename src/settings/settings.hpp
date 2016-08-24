#pragma once
#include <math.h>
#include <iostream>
#include <vector>
#include <boost/math/common_factor.hpp>
#include <libconfig.h++>
#include <cstring>
#include <exception>
#include <sdf3/base/string/cstring.h> ///Needed for parsing command line
#include <sdf3/base/exception/exception.h>
using namespace std;
using namespace libconfig;

class Settings
{
public:
	
	enum CPModels 		{NONECP, CP};
	enum SearchTypes 	{NONESEARCH, FIRST, ALL, OPTIMIZE, OPTIMIZE_IT, GIST_ALL, GIST_OPT, PSA};
	enum OptCriterion	{NONE, POWER, THROUGHPUT, LATENCY};
	
	Settings(std::string _filePath);
	~Settings();
	
	string 	getCPModelString() 					const;
	int 	getCPModel() 						const;
	
	string 	getSearchTypeString() 				const;
	int 	getSearchType() 					const;
	
	string 	getOptCriterionString() 			const;
	int 	getOptCriterion() 					const;
	
	/**
	 * Builds the name of the output path based on the current settings
	 */ 
	string 	getOutputsPath(string fileType) 	const;
	
	string 	getInputsPath() 					const;
	void 	setInputsPath(string _inputsPath);
	
	/**
	 * Using this function we can overwrite some settings that is by default loaded
	 * from settings.cfg file
	 */ 
	void ParseCommandLine(int argc, char ** argv);
	/**
	 * returs 1 if the mode is debug
	 */ 
	int IsDebug();
	/**
	 * Returns the luby scale
	 */
	unsigned long int getLubyScale(); 	
	/**
	 * Returns the timeout for search
	 */
	unsigned long int getTimeout();  
	friend std::ostream& operator<< (std::ostream &out, const Settings &settings);
	
private:
	
	Config 	cfg;
	enum 	CPModels 		model;
	enum 	SearchTypes 	search;
	enum 	OptCriterion	criterion;
	string 	filePath;
	string 	inputsPath;
	string 	outputsPath;
	int 	debug;
	unsigned long int luby_scale;
	unsigned long int timeout;			/**< Search timeout. */
	int loadDSESettings();

};
