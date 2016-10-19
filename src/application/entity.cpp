#include "entity.hpp"
using namespace std;
Base_Entity::Base_Entity(int _id, int _period, int _priority, set<int> _resources, int _exec, bool _is_task, bool _is_message):
phase(0),
period(_period),
deadline(_period),
codeSize(0),
priority(_priority),
name(""),
type(""),
id(_id),
memCons(0),
preemtable(true),
jitter(0),
resources(_resources),
execution(_exec),
periodic(true),
is_task_type(_is_task),
is_message_type(_is_message){}
Base_Entity::Base_Entity(vector<char*> elements, vector<char*> values, int number):
phase(0),
period(0),
deadline(0),
codeSize(0),
priority(0),
name(""),
type(""),
id(0),
preemtable(true),
periodic(true)
{
	for(unsigned int i=0;i< elements.size();i++)
	{
		try 
		{			
			if(strcmp(elements[i], "phase") == 0)
				phase = atoi(values[i]);
			
			if(strcmp(elements[i], "node") == 0)
				resources.insert(atoi(values[i]));
				
			if(strcmp(elements[i], "jitter") == 0)
				jitter = atoi(values[i]);
					
			if(strcmp(elements[i], "period") == 0)
				period = atoi(values[i]);
				
			if(strcmp(elements[i], "deadline") == 0)
				deadline = atoi(values[i]);
				
			if(strcmp(elements[i], "priority") == 0)
				priority = atoi(values[i]);
			
			if(strcmp(elements[i], "name") == 0)
				name = string(values[i]);
				
			if(strcmp(elements[i], "type") == 0)
			{
				type = string(values[i]);
				name = type + to_string(number);
			}
					
			if(strcmp(elements[i], "id") == 0)
				id = atoi(values[i]);
			
			if(strcmp(elements[i], "memCons") == 0)
				memCons = atoi(values[i]);
				
			if(strcmp(elements[i], "preemtable") == 0)
				preemtable = atoi(values[i]);
			
			if(strcmp(elements[i], "execution") == 0)
				execution = atoi(values[i]);
			
			if(strcmp(elements[i], "activation") == 0)
			{
				if(strcmp(values[i], "periodic") == 0)
					periodic = true;
				else
					periodic = false;
			}
			if(strcmp(elements[i], "link") == 0)
				resources.insert(atoi(values[i]));
				
		}				
		catch(std::exception const & e)
		{
			 cout << "reading entityset xml file error : " << e.what() << endl;
		}
	}
};



Base_Entity::Base_Entity(vector<char*> elements, vector<char*> values, int number, bool _is_task, bool _is_message): 
Base_Entity(elements, values, number)
{
	is_task_type    =_is_task;
	is_message_type = _is_message;
}

Base_Entity::Base_Entity(const Base_Entity& _entity)
{
;
}

std::ostream& operator<< (std::ostream &out, const Base_Entity &entity)
{
	string type = "unknown";
	if(entity.is_task()==true) type = "task";
	if(entity.is_message()==true) type = "message";
	out << " entity:" 		<< entity.name 		<< "[id=" 	<< entity.id
		<< ", type=" 		<< entity.type 
		<< " is a "         << type
		<< ", priority=" 	<< entity.priority;
	for (auto res: entity.resources)
		out << ", res=" 		<< res;
	out	<< "](" 			<< entity.period 		<< ", " 	
	    << entity.deadline << ", "
	    << entity.execution << ")"; 
	return out;
}

double Base_Entity::get_utilization()
{
	return ((double) execution)/period;
}

set<int> Base_Entity::get_resources()
{
	return resources;
}
int Base_Entity::get_period()
{
	return period;
}
int Base_Entity::get_deadline()
{
	return deadline;
}
int Base_Entity::get_id()
{
	return id;
}
int Base_Entity::get_priority()
{
	return priority;
}
int Base_Entity::get_execution()
{
	return execution;
}
int Base_Entity::get_load(int time)
{
	return ((time + jitter)/period)*execution;
}
bool Base_Entity::exist_on_resource(int resource_id)
{
	return resources.find(resource_id) != resources.end();
}

Entity::Entity(Base_Entity& _base_entity):base_entity(_base_entity)
{
	response_time = 0;	///has to be 0 because message adds to it
	count = -1; ///has to be -1 in the begining
	is_schedulable = false;
};
void Entity::add_instace(int new_instance)
{
	instances.push_back(base_entity.get_period() * new_instance);
}
int Entity::get_instance(int indx)
{
	return instances[indx];
}

void Entity::add_to_response_time(int _res_time)
{
	set_response_time(_res_time + get_response_time());
};
int Entity::get_response_time()
{
	if(response_time > 0 )
		return response_time;
	else
		return 0;
}
void Entity::set_response_time(int _response_time)
{
	if(_response_time > 0 )
		response_time = _response_time;
}

std::ostream& operator<< (std::ostream &out, const Entity &entity)
{
	out << entity.base_entity 
		<< "response time: "<< entity.response_time;
	return out;
}
