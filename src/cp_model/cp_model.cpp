#include "cp_model.hpp"

CP_model::CP_model(vector<Transaction*> _transactions, Settings* _settings, int _nodes):
	transactions(_transactions),
	settings(_settings),
	budget(*this, _nodes, 0, _nodes), 
	period(*this, _nodes, 0, _nodes)
	{
		std::ostream debug_stream(nullptr); /**< debuging stream, it is printed only in debug mode. */
		std::stringbuf debug_strbuf;
		debug_stream.rdbuf(&debug_strbuf); 
		debug_stream << "\n==========\ndebug log:\n..........\n";
		
		
	if(settings->IsDebug())
		cout << debug_strbuf.str();
}
  
CP_model::CP_model(bool share, CP_model& s):
	Space(share, s),
	transactions(s.transactions),
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



