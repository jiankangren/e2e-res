#include "transaction.hpp"

using namespace std;

Transaction::Transaction(){};
Transaction::~Transaction()
{
	for (auto entity : entities)
		delete entity;
	
	for (auto path : time_paths)
		delete path;
};
Transaction::Transaction(vector<char*> elements, vector<char*> values):
id(0)
{
	for(unsigned int i=0;i< elements.size();i++)
	{
		try 
		{			
			if(strcmp(elements[i], "task") == 0)
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
void Transaction::Build_transaction(vector<RunTime_Entity*> all_entities)
{
	if(entity_ids.size() == 0)
	{
		cout << "The transaction is empty \n";
		return;
	}
	for(unsigned j=0;j<entity_ids.size();j++)
	{
		bool entity_j_found = false;
		for(unsigned i=0;i<entity_ids.size();i++)
		{
			if(all_entities[i]->get_id() == entity_ids[j])
			{
				entities.push_back(all_entities[i]);
				entity_j_found = true;
				break;
			}
		}
		if(!entity_j_found)
		{
			cout << "error: entity " << entity_ids[j] << " is not defined\n";
			return;
		}
	}
}
double Transaction::get_utilization(int n)
{
	double utilization = 0.0;
	for(unsigned int i=0;i<entities.size();i++)
	{
		if(entities[i]->get_node() == n)
		{
			utilization += entities[i]->get_utilization();
		}
	}
	
	return utilization;
}
std::ostream& operator<< (std::ostream &out, const Transaction &trans)
{
	out << " Transaction: [" 		<< trans.id 		<< "]: ";
	for(unsigned int i=0;i<trans.entity_ids.size();i++)
	{
		out << trans.entity_ids[i];
		if(i+1 < trans.entity_ids.size())
			out << " -> ";
	}	 
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
}

void Transaction::findAllTimedPaths()
{
	int no_time_paths = 1;
	for (auto entity: entities) 
	{
		no_time_paths *= entity->get_instanc_size();	
	}
	/// we loop for the total number of timed paths
	for (int p = 0; p < no_time_paths; p++)
	{
		TimePath* new_time_path = new TimePath();
		add_time_path(new_time_path);
		for (auto entity: entities) 
		{
			TimePathEntity* new_tp_entity =  new TimePathEntity(*entity);
			new_time_path->add_time_path_enity(new_tp_entity);
			adjust_count(entity);
			new_tp_entity->set_instant(entity->count + 1);
			new_tp_entity->set_next_active_time();			
		}
		entities[0]->count++;
	}
}
void Transaction::adjust_count(RunTime_Entity* entity)
{
	if(entity->count == -1)
	{
		entity->count++;
		return;
	}
	if(entity->count >= entity->get_instanc_size())
	{
		entity->count = 0;
		increament_next_entity_count(entity);
	}
}
void Transaction::increament_next_entity_count(RunTime_Entity* _entity)
{
	bool found = false;
	if(_entity == *entities.end())
	{
		/// This is the last entity, so we do not need to do anything.
		return;
	}
	try
	{
		for (auto entity : entities)
		{
			if (found)
				entity->count++;
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
			new_delay = tp->get_first_activation_time() - tp->get_last_activation_time();
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
			L2Fdelay = tp->get_last_activation_time() - tp->get_first_activation_time();
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
			///The C code uses tp_b here!!!
			tp_a->pred_tp = pred_tp;
		}
	}
	/// compute the reaction delay
	for (auto tp : time_paths)
	{
		if(tp->is_first)
		{
			///Shouldn't this be based on delay rather than L2Fdelay
			reacDelay = L2Fdelay + tp->get_first_activation_time() - tp->get_pred_first_activation_time();
			if (reacDelay > rDelay)
			{
				rDelay = reacDelay;
			}
		}
	}
	reaction_delay = reacDelay;
}
