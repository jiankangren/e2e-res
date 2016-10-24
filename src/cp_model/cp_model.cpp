#include "cp_model.hpp"

CP_model::CP_model(vector<Base_Transaction*> _base_transactions, Settings* _settings)
	{
		base_transactions = _base_transactions;
		application = new Application(_base_transactions);
		settings = _settings;
		no_resources = application->no_resources();
		budget = IntVarArray(*this, no_resources, 0, Int::Limits::max);
		period = IntVarArray(*this, no_resources, settings->getMin_period(), application->get_min_trans_deadline());
		res_times = IntVarArray(*this, application->get_no_trans(), 0, Int::Limits::max);
		age_delays = IntVarArray(*this, application->get_no_trans(), 0, Int::Limits::max);
		reac_delays = IntVarArray(*this, application->get_no_trans(), 0, Int::Limits::max);
		utilization = IntVarArray(*this, no_resources, 0, 100);	
		total_utilization = IntVar(*this, 0, Int::Limits::max);//100*no_resources);
		std::ostream debug_stream(nullptr); /**< debuging stream, it is printed only in debug mode. */
		std::stringbuf debug_strbuf;
		debug_stream.rdbuf(&debug_strbuf); 
		debug_stream << "\n==========\ndebug log:\n..........\n";
		
		cout << "logging\n" << "nodes=" << no_resources << endl;
		
		/**
		 * For each node:
		 * (1) budgets can not exceed periods
		 * (2) bandwidth should be more than utilization
		 */ 
		 try
		 {
			for(int i=0; i<no_resources; i++)
			{
				rel(*this, budget[i] <= period[i]);  
				rel(*this, utilization[i] >= ceil(application->get_utilization(i) * 100));  
				rel(*this, budget[i]*100 == utilization[i]*period[i]);  
				debug_stream << "utilization on node " << i << " should be mode than = " <<  application->get_utilization(i) * 100 <<  endl;
			}
			rel(*this, total_utilization == sum(utilization));
			Schedulability(*this, budget, period, res_times, age_delays, reac_delays, _base_transactions);
			
			for(int i=0; i<_base_transactions.size(); i++)
			{
				rel(*this, res_times[i] <= _base_transactions[i]->get_deadline());  
				rel(*this, age_delays[i] <= _base_transactions[i]->get_age_delay_deadline());  
				rel(*this, reac_delays[i] <= _base_transactions[i]->get_reaction_delay_deadline());  
			}
			debug_stream << "search type: " << settings->getSearchTypeString() << endl;
			if(settings->getSearchType() == Settings::OPTIMIZE || settings->getSearchType() == Settings::GIST_OPT)
			{
				if(settings->getOptCriterion() == Settings::RES_TIME ||
				   settings->getOptCriterion() == Settings::AGE_DELAY ||
				   settings->getOptCriterion() == Settings::REACTION_DELAY)
				{
					branch(*this, budget, INT_VAR_NONE(), INT_VAL_MAX());
					branch(*this, period, INT_VAR_NONE(), INT_VAL_MIN());
					debug_stream << "branching for RES_TIME, AGE_DELAY or REACTION_DELAY\n";
				}
				if(settings->getOptCriterion() == Settings::UTILIZATION)
				{
					branch(*this, utilization, INT_VAR_NONE(), INT_VAL_MIN());
					branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
					branch(*this, period, INT_VAR_NONE(), INT_VAL_MAX());
					debug_stream << "branching for UTILIZATION\n";
				}
		    }
		    else
		    {
				branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
				branch(*this, period, INT_VAR_NONE(), INT_VAL_MAX());
				debug_stream << "branching for regular search\n";
			}
		}
		catch(std::exception const & e)
		{
			cout << "Could not create the CP model:\n"
			     << e.what() << endl;
			throw e;     
		}
	if(settings->IsDebug())
		cout << debug_strbuf.str();
}
  
CP_model::CP_model(bool share, CP_model& s):
	Space(share, s),
	application(s.application),
	settings(s.settings)
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
	out 	<< 	"utilization: " 			<< 	utilization				<< 	endl;
	out 	<< 	"budget: " 					<< 	budget 					<< 	endl;
	out 	<< 	"period: "	 				<< 	period	 				<< 	endl;
	out 	<< 	"total_utilization: "	 	<< 	total_utilization		<< 	endl;
	out 	<< 	"res_times: "	 	        << 	res_times		        << 	endl;
	out 	<< 	"age_delays: "	 	        << 	age_delays		        << 	endl;
	out 	<< 	"reac_delays: "	 	        << 	reac_delays		        << 	endl;
	out 	<< 	"----------------------------------------" << endl;
  
}

void CP_model::printCSV(std::ostream& out) const
{
	const char sep = ',';
	for(int i=0; i<budget.size();i++)
		out <<	budget[i] 	<< 	sep;
	for(int i=0; i<period.size();i++)
		out << 	period[i] 	<< 	sep;
	out		<< 	endl;
}



