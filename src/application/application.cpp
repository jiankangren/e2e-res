#include "application.hpp"

using namespace std;

Application::Application(vector<Base_Transaction*> _transactions)
{
	init(_transactions);
};
void Application::init(vector<Base_Transaction*> _transactions)
{
	for (auto trans : _transactions)
	{
		/**
		 * For all base transactions, we create
		 * a fresh transaction for the application
		 */ 
		transactions.push_back(new Transaction(*trans));
		for (auto entity : trans->get_entities())
		{
			set<int> entity_resources = entity->get_resources();
			resources.insert(entity_resources.begin(), entity_resources.end());
		}
	}
}
Application::Application(vector<Base_Transaction*> _transactions, vector<Reservation*> _reservations):
reservations(_reservations)
{
	init(_transactions);
};
Application::~Application()
{	
}
int Application::get_min_trans_deadline()
{
	try
	{
	    if(transactions.empty())	
		{
			cout << "The application does not have any transactions\n";
			throw std::runtime_error("Error: no transaction");
		}			
	}
	catch(runtime_error e)
	{
		cout << "The application does not have any transactions\n";
		throw e;
	}
	int min_deadline = transactions[0]->base_transaction.get_deadline();
	for(unsigned int i=1;i<transactions.size();i++)
	{
		if(min_deadline > transactions[i]->base_transaction.get_deadline())
			min_deadline = transactions[i]->base_transaction.get_deadline();
	}
	return min_deadline;
}
double Application::get_utilization(int n)
{
	double utilization = 0.0;
	for (auto trans : transactions)
		utilization += trans->get_utilization(n);	
	return utilization;
}
bool Application::schedulability()
{
	for (auto trans : transactions) 
	{
		for (auto entity : trans->entities) 
		{
			calculateResponseTime(entity);
			if(entity->get_response_time() > entity->base_entity.get_deadline())
			{
				//cout << *entity << " is not schedulable, res time=" << entity->get_response_time() << endl;
				return false;
			}	
		}
		//cout << "calculating trans res time\n";	
		trans->calculateTransResponseTime();
	}
	/// derive the activation time of tasks and messages in application 
	deriveActivationInstants();

	/// find all timed path for the transactions within application
	findAllTimedPaths();

	/// find all reachable timed path among all possible paths for transactions in application 
	//cout << "finding reachable time paths\n";
	findReachableTimePaths();

	/// find all fisrt timed paths (for reaction computation) in application 
	//cout << "finding first time paths\n";
	findFirstTimePaths();

	/// calculate the age and reaction delays of transaction 't'
	//cout << "calculating other delays\n";
	for (auto trans : transactions) 
	{
		trans->calculateTransAgeDelay();
		trans->calculateTransReacDelay();		
		if(trans->get_response_time() > trans->base_transaction.get_deadline())
		{
			//cout << "trans missing deadline\n";
			return false;
		}	
	}
	return true;
}
std::ostream& operator<< (std::ostream &out, const Application &app)
{
	out << 	"Application contains the following " << app.transactions.size()
		<<	" transactions \n";
	for(unsigned int i=0;i<app.transactions.size();i++)
	{
		out << *app.transactions[i] << endl;
	}
	out << "Application uses the following resources: ";
	for (auto res : app.resources)
		out << res << " ";
 
	out << "\nApplication has the following reservations:\n";
	for (auto res : app.reservations)
		out << *res << " " << endl;
 
	return out;
}
void Application::calculateResponseTime(Entity* entity)
{
	if (entity->base_entity.is_task())
	{
		calculateTaskResponseTime(*entity);
	}
	if (entity->base_entity.is_message())
	{
		calculateMessageResponseTime(*entity);
	}
}
int Application::rbf_task(Task &task, int time)
{
	int request = 0;
	int current_node = task.get_resource_id();
	int current_priority = task.get_priority();
	int current_id = task.get_id();
	
	for (auto trans : transactions) 
	{
		for (auto entity : trans->entities) 
		{
			if (entity->base_entity.is_task() && entity->base_entity.exist_on_resource(current_node))
			{
				if (entity->base_entity.get_id() != current_id)
				{
					if (entity->base_entity.get_priority() <= current_priority)
					{
						request += entity->base_entity.get_load(time);
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
			if (entity->base_entity.is_message())
			{
				Message* msg = dynamic_cast<Message*>(&entity->base_entity);
				for (auto link_b : msg->get_resources())
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

void  Application::calculateTaskResponseTime(Entity &entity)
{
	Task* task = dynamic_cast<Task*>(&entity.base_entity);
	Reservation res;
	try
	{
		res = get_resource(task->get_resource_id());
	}
	catch(std::runtime_error const & e)
	{
		throw e;
	}
	/// find the response time - we loop for every integer value of time from 0 to period
	for (int time = 0; time < task->get_period(); time++)
	{
		/// get the sbf for time
		int _sbf = res.sbf_proc(time);	

		/// compute the rbf for 'time'		
		int _rbf = rbf_task(*task, time);

		/// compare the rbf and sbf and extract the response time
		if (_sbf >= _rbf)
		{
			entity.set_response_time(time);
			break;
		}		
	}	
}

Reservation& Application::get_resource(int resource_id)
{
	for(auto res : reservations)
	{
		if(res->get_resource_id() == resource_id)
			return *res;
	}
	throw runtime_error("Did not find any resource matching the resource_id " + to_string(resource_id));
}
void Application::calculateMessageResponseTime(Entity& entity)
{
	Message* message = dynamic_cast<Message*>(&entity.base_entity);
	int idle = 0;
	/// for each link that the message crosses through
	for(auto link_a: message->get_resources())
	{
		idle = 0;
		for (auto trans : transactions) 
		{
			for (auto ent: trans->entities) 
			{
				if(ent->base_entity.is_message())
				{
					Message* mess = dynamic_cast<Message*>(&ent->base_entity);
					for (auto link_b : mess->get_resources())
					{
						if(mess != message && link_a == link_b && 
						   mess->get_priority() <= message->get_priority() &&
						   mess->is_periodic() == message->is_periodic())
						{
							if(mess->get_execution() > idle)
								idle = mess->get_execution();
						}
					}
				}
			}
		}
		/// find the response time
		for (int time = 0; time < message->get_period(); time++)
		{
			Reservation res;
			try
			{
				res = get_resource(link_a);
			}
			catch(std::runtime_error const & e)
			{
				throw e;
			}
			int sbf = res.sbf_net(time, idle);
			int rbf = rbf_message(*message, time, link_a);
			if (sbf >= rbf)
			{
				entity.add_to_response_time(time);
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
			lcm = LCM(lcm, entity->base_entity.get_period());
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
			while (entity->base_entity.get_period() * fact <= hyper_LCM)
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
				if(tp_entity->base_entity.is_message()==true)
			    {
					forward = 0;
					forward2 = 0;
					alphaL = tp_entity->get_activation_time();
					alphaLL = tp->get_activation_next_entity(tp_entity);
					delta = tp_entity->get_response_time();
					/*
					   cout << "alphaL=" << alphaL
						 << " alphaLL=" << alphaL
						 << " delta=" << alphaL 
						 << " next prio=" <<tp->get_priority_next_entity(tp_entity)
						 << "cuurent prio=" <<  tp_entity->get_priority()
						 << " next_activ_time=" << tp_entity->get_next_activ_time()
						 << " id=" << tp_entity->get_id() << endl;	
						 */ 
					if ((alphaLL >= alphaL) && ((alphaLL >= alphaL + delta) || (tp->get_priority_next_entity(tp_entity) > tp_entity->base_entity.get_priority())))
					{
						forward = 1;
					}
					alphaLNext = tp_entity->get_next_activ_time();
					if (!((alphaLL >= alphaLNext) && ((alphaLL >= alphaLNext + delta) || (tp->get_priority_next_entity(tp_entity) > tp_entity->base_entity.get_priority()))))
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
				//cout << "tp " << tp->id << " is reachable\n";
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
				for(auto tp_b : trans->time_paths)
				{
					if(tp_b->reachable)
					{
						/*cout << "tp_a->get_first_instant()=" << tp_a->get_first_instant()
						     << " tp_b->get_first_instant()=" << tp_b->get_first_instant() 
						     << " tp_b->get_last_instant()=" << tp_b->get_last_instant()
						     << " tp_a->get_last_instant()" << tp_a->get_last_instant()
						     << endl;*/
						if(tp_a->get_first_instant() == tp_b->get_first_instant() &&
							tp_b->get_last_instant() <= tp_a->get_last_instant())
							{
								tp_a->is_first = false;
								tp_b->is_first = true;
							}
						
					}
				}
			}
		}
	}
}

int Application::no_resources()
{
	return resources.size();
}
void Application::set_reservations(vector<Reservation*> _reservations)
{
	reservations = _reservations;
}
int Application::get_response_time(Base_Entity& _entity)
{
	for (auto trans : transactions)
	{
		for (auto entity : trans->entities)
		{
			if(&entity->base_entity == &_entity)
				return entity->get_response_time();
		}
	}
	throw runtime_error("get_response_time did not find the entity");
}
