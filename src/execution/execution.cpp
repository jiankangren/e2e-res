/**
 * This class is a simple contraint model execution engine
 * It receives a CP model and a setting object
 */ 
#include <math.h>
#include <iostream>
#include <vector>
#include <boost/math/common_factor.hpp>
#include <cstring>
#include <gecode/gist.hh>
#include "../settings/settings.hpp"
#include "../cp_model/cp_model.hpp"
#include <chrono>
#include <fstream> 

using namespace std;
using namespace Gecode;

template <class CPModelTemplate>
class Execution
{
public:
	~Execution()
	{
		delete geSearchOptions.stop;
	}
	Execution(CP_model* _model, Settings* _settings):
    model(_model),
    settings(_settings)
    {
		geSearchOptions.threads = 0.0;
		if(settings->getTimeout() > 0)
		{
		    Search::TimeStop* stop = new Search::TimeStop(settings->getTimeout()); 
		    geSearchOptions.stop = stop;
		}
	};
	/**
	 * This funtion executes the CP model.
	 * The CP model has to implement the following functions:
	 * (i) 	print()
	 * (ii) printCSV()
	 */ 
	int Execute()
	{
		switch(settings->getSearchType())
		{
			case(Settings::GIST_ALL):
			{
				Gist::Print<CPModelTemplate> p("Print solution");
				Gist::Options options;
				options.inspect.click(&p);
				Gist::dfs(model,options);					
				break;
			}
			case(Settings::GIST_OPT):
			{
				Gist::Print<CPModelTemplate> p("Print solution");
				Gist::Options options;
				options.inspect.click(&p);
				Gist::bab(model,options);					
				break;
			}
			case(Settings::FIRST):
			case(Settings::ALL):
			{
				cout << "DFS engine ...\n";				
				DFS<CPModelTemplate> e(model, geSearchOptions);
				loopSolutions<DFS<CPModelTemplate>>(&e);
				break;
			}
			case(Settings::OPTIMIZE):
			{
				cout << "BAB engine, optimizing ... \n";
				BAB<CPModelTemplate> e(model, geSearchOptions);
				loopSolutions<BAB<CPModelTemplate>>(&e);
				break;
			}
			case(Settings::OPTIMIZE_IT):
			{
				cout << "BAB engine, optimizing iteratively ... \n";
				Search::Cutoff* cut = Search::Cutoff::luby(settings->getLubyScale());
				geSearchOptions.cutoff = cut;
				RBS<BAB,CPModelTemplate> e(model, geSearchOptions);
				loopSolutions<RBS<BAB,CPModelTemplate>>(&e);
				break;
			}
			default:
				cout << "unknown search type !!!" << endl;
				throw CException("");
				break;
		}		
		cout << "Output file name: " << settings->getOutputsPath(".txt") <<	
		" \nreturning: end of execution class" << endl;
		return 1;
	};
	
	
	
	
private:
	CP_model* 			model;										/**< Pointer to the constraint model class. */  
	Settings* 			settings;									/**< pointer to the DSESetting class. */  
	int 				nodes;										/**< Number of nodes. */  
	Search::Options 	geSearchOptions;							/**< Gecode search option object. */  
	ofstream 			out, outCSV;								/**< Output file streams: .txt and .csv. */  
	typedef std::chrono::high_resolution_clock runTimer;			/**< Timer type. */  
	runTimer::time_point t_start, t_endAll;							/**< Timer objects for start and end of experiment. */  
	
	
	/**
	 * Prints the solutions in the ofstreams (out and outCSV)
	 */ 
	template<class SearchEngine> void printSolution(SearchEngine *e, CPModelTemplate* s)
	{
		  cout << nodes << " designes found out of " << e->statistics().node << " nodes so far" << endl;
		  auto durAll 		= t_endAll 		- t_start;
		  auto durAll_ms 	= std::chrono::duration_cast<std::chrono::milliseconds>(durAll).count();
		  out 	<< 	"*** Solution number: " 	<< 	nodes 						
				<< 	", after " 					<< 	durAll_ms 					<< " ms" 	
				<< 	", search nodes: "			<<	e->statistics().node		
				<<	", fail: "					<<	e->statistics().fail
				<<	", propagate: "				<<	e->statistics().propagate
				<<	", depth: "					<<	e->statistics().depth
				<<	", nogoods: "				<<	e->statistics().nogood
				<<	" ***\n";
		  s->print(out); 
		  outCSV << nodes << "," << durAll_ms << ",";
		  s->printCSV(outCSV);	
	};
	
	/**
	 * Loops through the solutions and prints them using the input search engine
	 */ 
	template<class SearchEngine> void loopSolutions(SearchEngine *e)
	{
		nodes = 0;
		out.open(settings->getOutputsPath(".txt"));
		outCSV.open(settings->getOutputsPath(".csv"));
		cout << "start searching for " << settings->getSearchTypeString() << " solutions \n";
		t_start = runTimer::now(); 
		while(CPModelTemplate * s = e->next()) 
		{
			nodes++;
			if(nodes==1)
			{
				if(settings->getSearchType() == Settings::FIRST)
				{
					t_endAll = runTimer::now();
					printSolution(e, s);
					cout << "returning" << endl;
					return;
				}
			}
			t_endAll = runTimer::now();
			printSolution(e, s);
			delete s;
		}
		auto durAll 	= runTimer::now()	- t_start;
		auto durAll_s 	= std::chrono::duration_cast<std::chrono::seconds>(durAll).count();
		out 	<< "===== search ended after: " << durAll_s 					<<	" s"	<<	" =====\n"
				<< 	"search nodes: "			<<	e->statistics().node		
				<<	", fail: "					<<	e->statistics().fail
				<<	", propagate: "				<<	e->statistics().propagate
				<<	", depth: "					<<	e->statistics().depth
				<<	", nogoods: "				<<	e->statistics().nogood
				<<	" ***\n";
		  
		out.close();
		outCSV.close();
	}

};

	

