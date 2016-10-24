#include "settings.hpp"

using namespace std;

	Settings::Settings(std::string _filePath):
    filePath(_filePath)
    {
		loadDSESettings();
	};
	Settings::~Settings()
    {
	};
	string Settings::getCPModelString() const
	{
		string modelStr = "";
		switch(model)
		{
			case CP  			:	modelStr = "CP";			   		break;
			default				:	cout << "unknown model type !!!"; 	break;
		}
		return modelStr;
	}
	
	int Settings::getCPModel() const
	{
		return model;
	}
	
	string Settings::getSearchTypeString() const
	{
		string searchStr = "";
		switch(search)
		{
			case FIRST  	: 	searchStr = "FIRST";  				break;
			case ALL  		: 	searchStr = "ALL";					break;
			case OPTIMIZE	: 	searchStr = "OPTIMIZE";   			break;
			case OPTIMIZE_IT: 	searchStr = "OPTIMIZE_IT"; 			break;
			case GIST_ALL	: 	searchStr = "GIST_ALL";				break;
			case GIST_OPT	: 	searchStr = "GIST_OPT";				break;
			case PSA		: 	searchStr = "PSA";					break;
			default			:	cout << "unknown search type !!!"; 	break;
		}
		return searchStr;
	}
	
	int Settings::getSearchType() const
	{
		return search;
	}
	
	string Settings::getOptCriterionString() const
	{
		string criterionStr = "";
		switch(criterion)
		{
			case POWER  	:	criterionStr = "POWER";  		break;
			case THROUGHPUT : 	criterionStr = "THROUGHPUT";	break;
			case LATENCY	: 	criterionStr = "LATENCY";   	break;
			case UTILIZATION: 	criterionStr = "UTILIZATION";   break;
			case RES_TIME	: 	criterionStr = "RES_TIME";		break;
			case AGE_DELAY	: 	criterionStr = "AGE_DELAY";		break;
			case REACTION_DELAY:criterionStr = "REACTION_DELAY";break;
			case COST		:   criterionStr = "COST";			break;
			default			: 	criterionStr = "unknown";   	break;	
		}
		return criterionStr;
	}
	
	int Settings::getOptCriterion() const
	{
		return criterion;
	}
	
	string Settings::getInputsPath() const
	{
		return inputsPath;
	}
	
	void Settings::setInputsPath(string _inputsPath)
	{
		inputsPath = _inputsPath;
	}
	
	string Settings::getOutputsPath(string fileType) const
	{
		string outPutsPath = outputsPath;
		outPutsPath += getCPModelString();
		outPutsPath += "_";
		outPutsPath += getSearchTypeString();
		if(search == OPTIMIZE || search == OPTIMIZE_IT) 
		{
			outPutsPath += "_";
			outPutsPath += getOptCriterionString();
		}
		outPutsPath += "_results";
		outPutsPath += fileType;
		return outPutsPath;
	}

	int Settings::loadDSESettings()
	{
		cout << "----- Loading settings.cfg -----" << endl;
		try
		{
			char myArray[filePath.size()+1];///Converting to char array
			strcpy(myArray, filePath.c_str());
			cfg.readFile(myArray);
		}
		catch(const FileIOException &fioex)
		{
			std::cerr << "I/O error while reading file." << std::endl;
			return(EXIT_FAILURE);
		}
		catch(const ParseException &pex)
		{
			std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
					  << " - " << pex.getError() << std::endl;
			return(EXIT_FAILURE);
		}
		/**
		 * Get the model name.
		 */ 
		try
		{
			string modelStr = cfg.lookup("model");
			if(modelStr.compare("CP") == 0)
			{
				model = CP;
			}
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'model' setting in configuration file." << endl;
		}
		/**
		 * Get the luby scale.
		 */ 
		try
		{
			string lubyStr 	= 	cfg.lookup("luby_scale");
			luby_scale 		=	atoi(lubyStr.c_str());
			
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'luby scale' setting in configuration file." << endl;
		}
		/**
		 * Get the search timeout.
		 */ 
		try
		{
			string timeoutStr 	= 	cfg.lookup("timeout");
			timeout		 		=	atoi(timeoutStr.c_str());
			
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'luby scale' setting in configuration file." << endl;
		}
		/** 
		 * Get the optimization criterion.
		 */ 
		try
		{
			string criterionStr = cfg.lookup("criterion");
			criterion = NONE;
			if(criterionStr.compare("POWER") == 0)
			{
				criterion = POWER;
			}
			if(criterionStr.compare("THROUGHPUT") == 0)
			{
				criterion = THROUGHPUT;
			}
			if(criterionStr.compare("LATENCY") == 0)
			{
				criterion = LATENCY;
			}
			if(criterionStr.compare("UTILIZATION") == 0)
			{
				criterion = UTILIZATION;
			}
			if(criterionStr.compare("RES_TIME") == 0)
			{
				criterion = RES_TIME;
			}
			if(criterionStr.compare("AGE_DELAY") == 0)
			{
				criterion = AGE_DELAY;
			}
			if(criterionStr.compare("REACTION_DELAY") == 0)
			{
				criterion = REACTION_DELAY;
			}
			if(criterionStr.compare("COST") == 0)
			{
				criterion = COST;
			}
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'criterion' setting in configuration file." << endl;
		}
		/**
		 *  Get the search name.
		 */ 
		try
		{
			string searchlStr = cfg.lookup("search");
			if(searchlStr.compare("FIRST") == 0)
			{
				search = FIRST;
			}
			else if(searchlStr.compare("ALL") == 0)
			{
				search = ALL;
			}
			else if(searchlStr.compare("OPTIMIZE") == 0)
			{
				search = OPTIMIZE;
			}
			else if(searchlStr.compare("OPTIMIZE_IT") == 0)
			{
				search = OPTIMIZE_IT;
			}
			else if(searchlStr.compare("GIST_ALL") == 0)
			{
				search = GIST_ALL;
			}
			else if(searchlStr.compare("GIST_OPT") == 0)
			{
				search = GIST_OPT;
			}
			else if(searchlStr.compare("PSA") == 0)
			{
				search = PSA;
			}
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'search' setting in configuration file." << endl;
		}
		
		/**
		 *  Get path for input files.
		 */ 
		try
		{
			string inputsStr = cfg.lookup("inputs");
			setInputsPath(inputsStr);
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'inputs' setting in configuration file." << endl;
		}
		/**
		 *  Get path for output files.
		 */ 
		try
		{
			string outputsStr 	= cfg.lookup("outputs");
			outputsPath			= outputsStr;
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'inputs' setting in configuration file." << endl;
		}
		/**
		 *  Get debug mode setting.
		 */ 
		try
		{
			string debugStr = cfg.lookup("debug");
			if(debugStr.compare("TRUE") == 0)
				debug = 1;
			else
				debug = 0;
						
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'inputs' setting in configuration file." << endl;
		}
		/**
		 *  Get debug mode setting.
		 */ 
		try
		{
			string debugStr = cfg.lookup("test");
			if(debugStr.compare("TRUE") == 0)
				test = 1;
			else
				test = 0;
						
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'inputs' setting in configuration file." << endl;
		}

		/**
		 * Get min period
		 */
		try
		{
			string minPeriodStr 	= 	cfg.lookup("min_period");
			min_period 				=	atoi(minPeriodStr.c_str());
			
		}
		catch(const SettingNotFoundException &nfex)
		{
			cerr << "No 'min_period' setting in configuration file." << endl;
		} 
		return 1;
	}
	int Settings::IsDebug()
	{
		return debug;
	}
	int Settings::IsTest()
	{
		return test;
	}
	void Settings::ParseCommandLine(int argc, char ** argv)
	{
		int arg = 1;
		while(arg < argc)
		{ 
			if(argv[arg] == CString("--apps") && arg+1<argc)
			{
				cout << "Overwriting input path" << endl;
				arg++;
				setInputsPath(argv[arg]);
			}
			else if(argv[arg] == CString("--mode") && arg+1<argc)
			{
				cout << "Overwriting search mode" << endl;
				arg++;
				if(strcmp (argv[arg], "gist") == 0)
					search = GIST_ALL;
				else if(strcmp (argv[arg], "gist_opt") == 0)
					search = GIST_OPT;
				else if(strcmp (argv[arg], "psa") == 0)
					search = PSA;	
				else if(strcmp (argv[arg], "solution") == 0)
					search = FIRST;      
				else if(strcmp (argv[arg], "best_solution") == 0)
					search = OPTIMIZE;
			}	
			else if(argv[arg] == CString("--debug") && arg+1<argc)
			{
				cout << "Overwriting debug mode" << endl;
				arg++;
				if(strcmp (argv[arg], "true") == 0)
					debug = 1;
				else
					debug = 0;
			}	
			else if(argv[arg] == CString("--timeout") && arg+1<argc)
			{
				cout << "Overwriting timeout TODO" << endl;
				arg++;				
			}			
			else
			{
				cout << "unknonw command variable\n";
				throw CException("");
			}
			arg++;
		}
	}
	unsigned long int Settings::getLubyScale()
	{
		return luby_scale;
	}
	unsigned long int Settings::getTimeout()
	{
		return timeout;
	}
	int Settings::getMin_period()
	{
		return min_period;
	}
	std::ostream& operator<< (std::ostream &out, const Settings &settings)
	{
		out << "filePath:" 	<< settings.filePath 				<< endl;
		out << "model:" 	<< settings.getCPModelString() 		<< endl;
		out << "search:" 	<< settings.getSearchTypeString() 	<< endl;
		out << "criterion:" << settings.getOptCriterionString() << endl;
		out << "inputsPath:" << settings.getInputsPath() 		<< endl;
		if(settings.test)
			out << "running the test cases\n";
		return out;
	}

