#include "cp_model.hpp"

CP_model::CP_model(vector<Base_Transaction*> _base_transactions, Settings* _settings)
	{
		std::stringbuf debug_strbuf;
		try
		 {
			base_transactions = _base_transactions;
			application = new Application(_base_transactions);
			settings = _settings;
			no_resources = application->get_max_resource_id()+1;///Assuming resource ids start from 0
			budget = IntVarArray(*this, no_resources, 0, Int::Limits::max);
			period = IntVarArray(*this, no_resources, settings->getMin_period(), application->get_shortest_element_period());
			res_times = IntVarArray(*this, application->get_no_trans(), 0, Int::Limits::max);
			age_delays = IntVarArray(*this, application->get_no_trans(), 0, Int::Limits::max);
			reac_delays = IntVarArray(*this, application->get_no_trans(), 0, Int::Limits::max);
			utilization = IntVarArray(*this, no_resources, 0, 100);	
			total_utilization = IntVar(*this, 0, Int::Limits::max);//100*no_resources);
			std::ostream debug_stream(nullptr); /**< debuging stream, it is printed only in debug mode. */
			debug_stream.rdbuf(&debug_strbuf); 
			debug_stream << "\n==========\ndebug log:\n..........\n";
			
			cout << "logging\n" << "number of resources=" << no_resources 
			     << " cpu respurces: " << application->get_max_cpu_resource_id()<< endl;
		 
			IntVar period_resolution(*this, settings->getMin_period(), settings->getMin_period());
			IntVar residual(*this, 0, 0);
			/**
			 * For each resource:
			 * (1) budgets can not exceed periods
			 * (2) bandwidth should be more than utilization
			 */ 	
			for(int i=0; i<no_resources; i++)
			{
				rel(*this, budget[i] <= period[i]);  
				rel(*this, utilization[i] >= ceil(application->get_utilization(i) * 100));  
				if(application->get_utilization(i) == 0)
				{
				    rel(*this, (utilization[i] == 0));  				
				    rel(*this, (budget[i] == 0));  				
				    rel(*this, (period[i] == period[i].min()));  
				}
				rel(*this, budget[i]*100 == utilization[i]*period[i]);  
				mod(*this, period[i], period_resolution, residual);				
				debug_stream << "utilization on node " << i << " should be more than = " <<  application->get_utilization(i) * 100 <<  endl;
			}
			rel(*this, total_utilization == sum(utilization));
			rel(*this, total_utilization <= settings->getMax_utilzation());
			Schedulability(*this, utilization, budget, period, res_times, age_delays, reac_delays, _base_transactions);
			
			for(int i=0; i<_base_transactions.size(); i++)
			{
				rel(*this, res_times[i] <= _base_transactions[i]->get_deadline());  
				rel(*this, age_delays[i] <= _base_transactions[i]->get_age_delay_deadline());  
				rel(*this, reac_delays[i] <= _base_transactions[i]->get_reaction_delay_deadline());  
			}
			debug_stream << "search type: " << settings->getSearchTypeString() << endl;
			if(settings->getSearchType() == Settings::OPTIMIZE ||
			   settings->getSearchType() == Settings::OPTIMIZE_IT ||
			   settings->getSearchType() == Settings::GIST_OPT)
			{
				if(settings->getOptCriterion() == Settings::RES_TIME ||
				   settings->getOptCriterion() == Settings::AGE_DELAY ||
				   settings->getOptCriterion() == Settings::REACTION_DELAY)
				{
					branch(*this, total_utilization, INT_VAL_MAX());
					//Rnd r(1U);
					//branch(*this, total_utilization, INT_VAL_RND(r));
					branch(*this, utilization, INT_VAR_NONE(), INT_VAL_MED());
					branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
					branch(*this, period, INT_VAR_NONE(), INT_VAL_MIN());
					debug_stream << "branching for RES_TIME, AGE_DELAY or REACTION_DELAY\n";
				}
				if(settings->getOptCriterion() == Settings::UTILIZATION)
				{
					branch(*this, total_utilization, INT_VAL_MIN());
					branch(*this, utilization, INT_VAR_NONE(), INT_VAL_MIN());
					branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
					branch(*this, period, INT_VAR_NONE(), INT_VAL_MAX());
					debug_stream << "branching for UTILIZATION\n";
				}
				if(settings->getOptCriterion() == Settings::COST)
				{
					branch(*this, total_utilization, INT_VAL_MAX());
					branch(*this, utilization, INT_VAR_NONE(), INT_VAL_MED());
					branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
					branch(*this, period, INT_VAR_NONE(), INT_VAL_MAX());
					debug_stream << "branching for COST\n";
				}
				if(settings->getOptCriterion() == Settings::OVERHEAD)
				{
					branch(*this, period, INT_VAR_NONE(), INT_VAL_MIN());
					branch(*this, total_utilization, INT_VAL_MAX());
					branch(*this, utilization, INT_VAR_NONE(), INT_VAL_MED());
					branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
					debug_stream << "branching for OVERHEAD \n";
				}
		    }
		    else
		    {
				branch(*this, total_utilization, INT_VAL_MIN());
				branch(*this, utilization, INT_VAR_NONE(), INT_VAL_MED());
				branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
				branch(*this, period, INT_VAR_NONE(), INT_VAL_MAX());
				debug_stream << "branching for regular search\n";
			}
		}
		catch(std::exception const & e)
		{
			cout << "Could not create the CP model:\n"
			     << e.what() << endl;
			cout << debug_strbuf.str();     
			throw e;     
		}
	if(settings->IsDebug())
		cout << debug_strbuf.str();
}
  
CP_model::CP_model(bool share, CP_model& s):
	Space(share, s),
	application(s.application),
	settings(s.settings),
	base_transactions(s.base_transactions)
	{
		budget.update(*this, share, s.budget);
		period.update(*this, share, s.period);
		utilization.update(*this, share, s.utilization);
		total_utilization.update(*this, share, s.total_utilization);
		res_times.update(*this, share, s.res_times);
		age_delays.update(*this, share, s.age_delays);
		reac_delays.update(*this, share, s.reac_delays);
	}
  

Space* CP_model::copy(bool share)
{
	return new CP_model(share, *this);
}
  

void CP_model::print(std::ostream& out) const
{
	out 	<< "----------------------------------------" << endl;
	out 	<< 	"total_utilization: "	 	<< 	total_utilization		<< 	endl;
	out 	<< 	"utilization: " 			<< 	utilization				<< 	endl;
	out 	<< 	"budget: " 					<< 	budget 					<< 	endl;
	out 	<< 	"period: "	 				<< 	period	 				<< 	endl;
	out 	<< 	"res_times: "	 	        << 	res_times		        << 	endl;
	out 	<< 	"age_delays: "	 	        << 	age_delays		        << 	endl;
	out 	<< 	"reac_delays: "	 	        << 	reac_delays		        << 	endl;
	//out 	<< 	"max cost: "		 	        << 	total_utilization.max()*base_transactions[0]->get_deadline()+res_times[0].max()		        << 	endl;
	out 	<< 	"----------------------------------------" << endl;
  
}

void CP_model::printCSV(std::ostream& out) const
{
	const char sep = ',';
	out 	<< 	total_utilization	<< 	sep;
	
	for(int i=0; i<utilization.size();i++)
		out <<	utilization[i] 	<< 	sep;
	
	for(int i=0; i<res_times.size();i++)
		out <<	res_times[i] 	<< 	sep;
	
	for(int i=0; i<age_delays.size();i++)
		out <<	age_delays[i] 	<< 	sep;
	
	for(int i=0; i<reac_delays.size();i++)
		out <<	reac_delays[i] 	<< 	sep;
	        
	for(int i=0; i<budget.size();i++)
		out <<	budget[i] 	<< 	sep;
	for(int i=0; i<period.size();i++)
		out << 	period[i] 	<< 	sep;
	out		<< 	endl;
}

CP_model::~CP_model()
{
	//delete application;
}
