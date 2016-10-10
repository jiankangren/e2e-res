#include "entity.hpp"

using namespace std;

RunTime_Entity::RunTime_Entity(int _phase, int _period, int _deadline, int _mem, int _cS, int _priority, string _name, string _type, int _id):
phase(_phase),
period(_period),
deadline(_deadline),
codeSize(_cS),
priority(_priority),
name(_name),
type(_type),
id(_id),
memCons(_mem),    
preemtable(true),
response_time(-1),
count(-1){};

RunTime_Entity::RunTime_Entity(int _period, int _mem, int _cS, string _name, string _type, int _id):
phase(0),
period(_period),
deadline(_period),
codeSize(_cS),
priority(0),
name(_name),
type(_type),
id(_id),
memCons(_mem),
preemtable(true),
response_time(-1),
count(-1){};

RunTime_Entity::RunTime_Entity(vector<char*> elements, vector<char*> values, int number):
phase(0),
period(0),
deadline(0),
codeSize(0),
priority(0),
name(""),
type(""),
id(0),
preemtable(true),
response_time(-1),
periodic(true),
count(-1)
{
	for(unsigned int i=0;i< elements.size();i++)
	{
		try 
		{			
			if(strcmp(elements[i], "phase") == 0)
				phase = atoi(values[i]);
			
			if(strcmp(elements[i], "node") == 0)
				node = atoi(values[i]);
				
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
				
		}				
		catch(std::exception const & e)
		{
			 cout << "reading entityset xml file error : " << e.what() << endl;
		}
	}
};

std::ostream& operator<< (std::ostream &out, const RunTime_Entity &entity)
{
	string type = "unknown";
	if(entity.is_task()==true) type = "task";
	if(entity.is_message()==true) type = "message";
	out << " entity:" 		<< entity.name 		<< "[id=" 	<< entity.id
		<< ", type=" 		<< entity.type 
		<< " is a "         << type
		<< ", priority=" 	<< entity.priority
		<< ", node=" 		<< entity.node
		<< "](" 			<< entity.period 		<< ", " 	<< entity.deadline << ")";
 
	return out;
}

double RunTime_Entity::get_utilization()
{
	return ((double) execution)/period;
}

int RunTime_Entity::get_node()
{
	return node;
}
int RunTime_Entity::get_period()
{
	return period;
}
int RunTime_Entity::get_id()
{
	return id;
}
int RunTime_Entity::get_priority()
{
	return priority;
}
int RunTime_Entity::get_execution()
{
	return execution;
}
int RunTime_Entity::get_load(int time)
{
	return ((time + jitter)/period)*execution;
}
void RunTime_Entity::set_response_time(int _response_time)
{
	if(_response_time > 0 )
		response_time = _response_time;
}
int RunTime_Entity::get_response_time()
{
	if(response_time > 0 )
		return response_time;
	else
		return -1;
}
void RunTime_Entity::add_instace(int new_instance)
{
	instances.push_back(period * new_instance);
}
int RunTime_Entity::get_instance(int indx)
{
	return instances[indx];
}

