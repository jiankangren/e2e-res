#include "transaction.hpp"

using namespace std;

Transaction::Transaction(Base_Transaction& _base_transaction):
base_transaction(_base_transaction),
response_time(0),
reaction_delay(0),
age_delay(0)
{
	/**
	 * We create a fresh vector of entities using the
	 * base entities from the base_transaction.
	 */ 
	for (auto entity : base_transaction.get_entities())
	{
		entities.push_back(new Entity(*entity));
	}
};
Transaction::~Transaction()
{
	for (auto entity : entities)
		delete entity;
	
	for (auto path : time_paths)
		delete path;
};
Base_Transaction::Base_Transaction(int _id, vector<int> _entity_ids, vector<Base_Entity*> _entities, int _deadline, int _age_deadline, int _reaction_deadline):
id(_id),
entity_ids(_entity_ids),
deadline(_deadline),
age_delay_deadline(_age_deadline),
reaction_delay_deadline(_reaction_deadline)
{
	Build_transaction(_entities);
};

Base_Transaction::Base_Transaction(vector<char*> elements, vector<char*> values):
id(0)
{
	for(unsigned int i=0;i< elements.size();i++)
	{
		try 
		{			
			if(strcmp(elements[i], "task") == 0)
				entity_ids.push_back(atoi(values[i]));	
			if(strcmp(elements[i], "message") == 0)
				entity_ids.push_back(atoi(values[i]));		
			if(strcmp(elements[i], "id") == 0)
				id = atoi(values[i]);				
			if(strcmp(elements[i], "deadline") == 0)
				deadline = atoi(values[i]);					
			if(strcmp(elements[i], "age_delay_deadline") == 0)
				age_delay_deadline = atoi(values[i]);				
			if(strcmp(elements[i], "reaction_delay_deadline") == 0)
				reaction_delay_deadline = atoi(values[i]);						
		}				
		catch(std::exception const & e)
		{
			 cout << "reading transactions xml file error : " << e.what() << endl;
		}
	}
};
void Base_Transaction::Build_transaction(vector<Base_Entity*> all_entities)
{
	if(entity_ids.size() == 0)
	{
		cout << "The transaction is empty \n";
		return;
	}
	for (auto id : entity_ids)
	{
		bool entity_found = false;
		for(auto entity : all_entities)
		{
			if(entity->get_id() == id)
			{
				entities.push_back(entity);
				entity_found = true;
				break;
			}
		}
		if(!entity_found)
		{
			cout << "error: entity " << id << " is not defined\n";
			return;
		}
	}
}
double Transaction::get_utilization(int n)
{
	double utilization = 0.0;
	for (auto base_entity : base_transaction.get_entities())
	{
		if(base_entity->exist_on_resource(n))
		{
			utilization += base_entity->get_utilization();
		}
	}
	return utilization;
}
int Transaction::get_response_time()
{
	return response_time;
}
std::ostream& operator<< (std::ostream &out, const Transaction &trans)
{
	out << " Transaction: [" 		<< trans.base_transaction.get_id() 		<< "]: ";
	for(unsigned int i=0;i<trans.base_transaction.get_entity_ids().size();i++)
	{
		out << trans.base_transaction.get_entity_ids()[i];
		if(i+1 < trans.base_transaction.get_entity_ids().size())
			out << " -> ";
	}	
	out << " response time: " << trans.response_time 
	    << " deadline("       << trans.base_transaction.get_deadline() << ")"
	    << " age_delay="      << trans.age_delay
	    << " deadline("       << trans.base_transaction.get_age_delay_deadline() << ")"
	    << " reaction_delay=" << trans.reaction_delay
	    << " deadline("       << trans.base_transaction.get_reaction_delay_deadline() << ")"; 
	return out;
}

void Transaction::calculateTransResponseTime()
{
	/// we sum the resonse times of tasks and messages in a transaction
	for (auto entity: entities) 
	{
		response_time += entity->get_response_time();
	}	
}
void Transaction::add_time_path(TimePath* _time_path)
{
	time_paths.push_back(_time_path);
	_time_path->id = time_paths.size();
}

void Transaction::findAllTimedPaths()
{
	int no_time_paths = 1;
	for (auto entity: entities) 
	{
		no_time_paths *= entity->get_instanc_size();	
	}
	/// we loop for the total number of timed paths
	//cout << "no_time_paths=" << no_time_paths << endl;
	for (int p = 0; p < no_time_paths; p++)
	{
		TimePath* new_time_path = new TimePath(p);
		add_time_path(new_time_path);
		for (auto entity: entities) 
		{
			TimePath_Entity* new_tp_entity =  new TimePath_Entity(*entity);
			new_time_path->add_time_path_enity(new_tp_entity);
			adjust_count(entity);
			new_tp_entity->set_activation_time();
			new_tp_entity->set_instant();
			new_tp_entity->set_next_active_time();
		}
		entities[0]->count++;
	}
}
void Transaction::adjust_count(Entity* entity)
{
	if(entity->count == -1)
	{
		entity->count++;
		return;
	}
	if(entity->count >= entity->get_instanc_size())
	{
		entity->count = 0;
		increment_next_entity_count(entity);
	}
}
void Transaction::increment_next_entity_count(Entity* _entity)
{
	bool found = false;
	if(_entity == *entities.end())
	{
		/// This is the last entity, so we do not need to do anything.
		//throw runtime_error("could not increment next entity count");
		return;
	}
	try
	{
		for (auto entity : entities)
		{
			if (found)
			{
				entity->count++;
				return;
			}
			if(_entity == entity)
				found = true;
		}
	}
	catch(exception& e)
	{
		cout << "increament_next_entity_count did not find the next entity \n";
		return;
	}
}
void Transaction::calculateTransAgeDelay()
{
	int delay = 0;
	int new_delay = 0;
	for (auto tp : time_paths)
	{
		if(tp->reachable)
		{
			new_delay = tp->get_last_activation_time() + tp->get_last_response_time()	 
			            - tp->get_first_activation_time() ;
			if(new_delay > delay)
				delay = new_delay;
		}
	}
	age_delay = delay;
}

void Transaction::calculateTransReacDelay()
{
	int delay = 0;
	int L2Fdelay = 0;
	int rDelay = 0;
	int reacDelay = 0;
	int pred_ins = 0;

	/// calculate the last to first delay	
	for (auto tp : time_paths)
	{
		if(tp->is_first)
		{
			L2Fdelay = tp->get_last_activation_time() + tp->get_last_response_time()
			           - tp->get_first_activation_time();
			if(L2Fdelay > delay)
				delay = L2Fdelay;
		}
	}
	
	
	/// derive the first pred
	for (auto tp_a : time_paths)
	{
		pred_ins = -1;
		TimePath* pred_tp;
		if(tp_a->is_first)
		{
			for (auto tp_b : time_paths)
			{
				if(tp_b->is_first && tp_b->get_first_instant() <= tp_a->get_first_instant())
				{
					tp_a->is_pred = true;
					if(tp_b->get_first_instant() > pred_ins)
					{
						pred_ins = tp_b->get_first_instant();
						pred_tp = tp_b;
					}
				}
			}
			///[TODO] [Nima] The C code uses tp_b here!!!
			tp_a->pred_tp = pred_tp;
		}
	}
	/// compute the reaction delay
	for (auto tp : time_paths)
	{
		if(tp->is_first)
		{
			///Shouldn't this be based on delay rather than L2Fdelay
			reacDelay = delay + tp->get_first_activation_time() - tp->get_pred_first_activation_time();
			if (reacDelay > rDelay)
			{
				rDelay = reacDelay;
			}
		}
	}
	///Changed this from reacDelay to rDelay
	reaction_delay = rDelay + get_first_task_period();
}

int Transaction::get_first_task_period()
{
	if(!base_transaction.get_entities()[0]->is_task())
		throw runtime_error("first transaction entity is not a task");
	return base_transaction.get_entities()[0]->get_period();	
}
bool Transaction::is_schedulable()
{
	for (auto entity : entities)
	{
		if(!entity->is_schedulable)
			return false;
	}
	return true;
}
