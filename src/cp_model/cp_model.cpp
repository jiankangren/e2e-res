#include "cp_model.hpp"

CP_model::CP_model(vector<Base_Transaction*> _base_transactions, Settings* _settings)
	{
		base_transactions = _base_transactions;
		application = new Application(_base_transactions);
		settings = _settings;
		no_resources = application->no_resources();
		budget = IntVarArray(*this, no_resources, 0, Int::Limits::max);
		period = IntVarArray(*this, no_resources, settings->getMin_period(), application->get_min_trans_deadline());
		
		std::ostream debug_stream(nullptr); /**< debuging stream, it is printed only in debug mode. */
		std::stringbuf debug_strbuf;
		debug_stream.rdbuf(&debug_strbuf); 
		debug_stream << "\n==========\ndebug log:\n..........\n";
		
		cout << "logging\n" << "nodes=" << no_resources << endl;
		IntVarArray utilization(*this, no_resources, 0, 100);		/**< the utilization of resources in percentage. */
		
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
				rel(*this, utilization[i] >= application->get_utilization(i) * 100);  
				rel(*this, budget[i]*100 >= utilization[i]*period[i]);  
				debug_stream << "utilization on node " << i << " should be mode than = " <<  application->get_utilization(i) * 100 <<  endl;
			}
			
			Schedulability(*this, budget, period, _base_transactions);
			
			branch(*this, budget, INT_VAR_NONE(), INT_VAL_MIN());
			branch(*this, period, INT_VAR_NONE(), INT_VAL_MAX());
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
	}
  

Space* CP_model::copy(bool share)
{
	return new CP_model(share, *this);
}
  

void CP_model::print(std::ostream& out) const
{
	out 	<< "----------------------------------------" << endl;
	out 	<< 	"budget: " 					<< 	budget 					<< 	endl;
	out 	<< 	"period: "	 				<< 	period	 				<< 	endl;
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



