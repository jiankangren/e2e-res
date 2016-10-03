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
bool Application::schedulability(int budget, int period)
{
	bool schedulabe = false;
	perform_analysis();
	
	return schedulabe;
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

int Application::rbf(Task &task, int time)
{
	int request = 0;
	int current_node = task.get_node();
	int current_priority = task.get_priority();
	int current_id = task.get_id();
	
	for (auto trans : transactions) 
	{
		for (auto trans_task : trans->tasks) 
		{
			if (trans_task->get_node() == current_node)
			{
				if (trans_task->get_id() != current_id)
				{
					if (trans_task->get_priority() <= current_priority)
					{
						request += trans_task->get_load(time);
					}
				}
			}
		}
	}
	request += task.get_execution();
	
	return request;
}

int  Application::calculateTaskResponseTime(Task &task, int t)
{
	Resource res = get_resource(task);
	// find the response time - we loop for every integer value of time from 0 to period
	for (int time = 0; time < task.get_period(); time++)
	{
		// get the sbf for time
		int _sbf = res.sbf(time);

		// compute the rbf for 'time'
		
		int _rbf = rbf(task, time);

		// compare the rbf and sbf and extract the response time
		if (_sbf >= _rbf)
		{
			task.set_response_time(time);
			return 0;
		}
	}
	
}

Resource& Application::get_resource(Task& task)
{
	for(auto res : resources)
	{
		if(res->get_id() == task.get_node())
			return *res;
	}
	cout << "Did not find any resource matching the task node\n";
}
