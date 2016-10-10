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

int Application::rbf_task(Task &task, int time)
{
	int request = 0;
	int current_node = task.get_node();
	int current_priority = task.get_priority();
	int current_id = task.get_id();
	
	for (auto trans : transactions) 
	{
		for (auto entity : trans->entities) 
		{
			if (entity->is_task() && entity->get_node() == current_node)
			{
				if (entity->get_id() != current_id)
				{
					if (entity->get_priority() <= current_priority)
					{
						request += entity->get_load(time);
					}
				}
			}
		}
	}
	request += task.get_execution();
	
	return request;
}

int Application::rbf_message(Message &message, int time, int link)
{
	int rbf = 0;
	int interfTerm = 0;
	int blockTerm = 0;
	for (auto trans : transactions) 
	{
		for (auto entity : trans->entities) 
		{
			if (entity->is_message())
			{
				Message* msg = dynamic_cast<Message*>(entity);
				for (auto link_b : msg->links)
				{
					if (link == link_b)
					{
						if (msg != &message && msg->get_priority() <= message.get_priority() &&
							msg->is_periodic() == message.is_periodic())
						{					
							int interfDiv = time / msg->get_period();
							interfTerm = interfTerm + ((int)floor(interfDiv) * msg->get_execution());							
						}
						else if (msg != &message && msg->get_priority() > message.get_priority() &&
							msg->is_periodic() == message.is_periodic())
						{					
							if (msg->get_execution() > blockTerm)
							{
								blockTerm = msg->get_execution();
							}							
						}						
						
					}
				}
			}
		}
	}

	rbf = message.get_execution() + interfTerm + blockTerm;
	return rbf;
}

void  Application::calculateTaskResponseTime(Task &task, int t)
{
	Resource res = get_resource(task);
	/// find the response time - we loop for every integer value of time from 0 to period
	for (int time = 0; time < task.get_period(); time++)
	{
		/// get the sbf for time
		int _sbf = res.sbf_proc(time);

		/// compute the rbf for 'time'		
		int _rbf = rbf_task(task, time);

		/// compare the rbf and sbf and extract the response time
		if (_sbf >= _rbf)
		{
			task.set_response_time(time);
		}
	}	
}

Resource& Application::get_resource(RunTime_Entity& entity)
{
	for(auto res : resources)
	{
		if(res->get_id() == entity.get_node())
			return *res;
	}
	cout << "Did not find any resource matching the task node\n";
}
Resource& Application::get_resource(int link)
{
	for(auto res : resources)
	{
		if(res->get_id() == link)
			return *res;
	}
	cout << "Did not find any resource matching the task node\n";
}
void Application::calculateMessageResponseTime(Message& message)
{
	int idle = 0;
	// for each link that the message crosses through
	for(auto link_a: message.links)
	{
		idle = 0;
		for (auto trans : transactions) 
		{
			for (auto entity: trans->entities) 
			{
				if(entity->is_message())
				{
					Message* mess = dynamic_cast<Message*>(entity);
					for (auto link_b : mess->links)
					{
						if(mess != &message && link_a == link_b && 
						   mess->get_priority() <= message.get_priority() &&
						   mess->is_periodic() == message.is_periodic())
						{
							if(mess->get_execution() > idle)
								idle = mess->get_execution();
						}
					}
				}
			}
		}
		// find the response time
		for (int time = 0; time < message.get_period(); time++)
		{
			Resource res = get_resource(link_a);
			int sbf = res.sbf_net(time, idle);
			int rbf = rbf_message(message, time, link_a);
			if (sbf >= rbf)
			{
				message.add_to_response_time(time);
				break;
			}
		}
		
	}
}
int Application::get_app_LCM()
{
	// compute the LCM for tasks and messages in application a
	int lcm = 1;
	for (auto trans : transactions) 
	{
		for (auto entity: trans->entities) 
		{
			lcm = LCM(lcm, entity->get_period());
		}
	}
	return lcm;
}

int Application::LCM(int x, int y)
{
	int a, b, t, gcd, lcm;
	a = x;
	b = y;

	while (b != 0)
	{
		t = b;
		b = a % b;
		a = t;
	}
	gcd = a;
	lcm = (x * y) / gcd;
	return lcm;
}

void Application::deriveActivationInstants()
{
	int hyper_LCM = 2 * get_app_LCM();
	for (auto trans : transactions) 
	{
		for (auto entity: trans->entities) 
		{
			int fact = 0;
			while (entity->get_period() * fact <= hyper_LCM)
			{
				entity->add_instace(fact);
				fact++;
			}
		}
	}
}
void Application::findAllTimedPaths()
{
	for (auto trans : transactions) 
	{
		trans->findAllTimedPaths();
	}
}
void Application::findReachableTimePaths()
{
	int forward;
	int forward2;
	int valid = 0;
	int alphaL, alphaLL, delta, alphaLNext;

	for (auto trans : transactions) 
	{
		for(auto tp : trans->time_paths)
		{
			///Loop through all messages
			for (auto tp_entity : tp->tp_entities)
			{
			    if(tp_entity->is_message())
			    {
					forward = 0;
					forward2 = 0;
					alphaL = tp_entity->get_activation_time();
					alphaLL = tp->get_activation_next_entity(tp_entity);
					delta = tp_entity->get_response_time();
					if ((alphaLL >= alphaL) && ((alphaLL >= alphaL + delta) || (tp->get_priority_next_entity(tp_entity) > tp_entity->get_priority())))
					{
						forward = 1;
					}
					alphaLNext = tp_entity->get_next_activ_time();
					if (!((alphaLL >= alphaLNext) && ((alphaLL >= alphaLNext + delta) || (tp->get_priority_next_entity(tp_entity) > tp_entity->get_priority()))))
					{
						forward2 = 1;
					}
					if ((forward == 1) && (forward2 == 1))
					{
						valid = 1;
					}
					else
					{
						valid = 0;
					}
					if (valid == 0)
					{
						tp->reachable = false;
						break;
					}
				}
			}
			if (valid == 1)
			{
				tp->reachable = true;
			}				
		}
	}
}

void Application::findFirstTimePaths()
{

	for (auto trans : transactions) 
	{
		for(auto tp_a : trans->time_paths)
		{
			if(tp_a->reachable)
			{
				TimePath* first_tp = tp_a;			
				for(auto tp_b : trans->time_paths)
				{
					if(tp_b->reachable)
					{
						if(first_tp->get_first_instant() == tp_b->get_first_instant() &&
							tp_b->get_last_instant() <= first_tp->get_last_instant())
							{
								first_tp->is_first = false;
								tp_b->is_first = true;
							}
						
					}
				}
			}
		}
	}
}

