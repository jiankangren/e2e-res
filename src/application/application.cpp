#include "application.hpp"

using namespace std;

Application::Application(vector<Transaction*> _transactions):
transactions(_transactions){};

int Application::get_min_trans_deadline()
{
	if(transactions.size() == 0)
	{
		cout << "The application does not have any transactions\n";
		throw std::runtime_error("Error: no transaction");
	}
	int min_deadline = transactions[0]->deadline;
	for(unsigned int i=1;i<transactions.size();i++)
	{
		if(min_deadline > transactions[1]->deadline)
			min_deadline = transactions[1]->deadline;
	}
	return min_deadline;
}
double Application::get_utilization(int n)
{
	double utilization = 0.0;
	for(unsigned int i=1;i<transactions.size();i++)
	{
		utilization += transactions[i]->get_utilization(n);
	}
	
	return utilization;
}
std::ostream& operator<< (std::ostream &out, const Application &app)
{
	out << 	"Application contains the following " << app.transactions.size()
		<<	" transactions \n";
	for(unsigned int i=0;i<app.transactions.size();i++)
	{
		out << *app.transactions[i] << endl;
	}
 
	return out;
}
