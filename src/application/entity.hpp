/**
 * Copyright (c) 2016    Nima Khalilzad   <nkhal@kth.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <math.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/math/common_factor.hpp>
#include <exception>
#include <stdexcept>
#include <set>

using namespace std;
/**
 * This class encapsulates the base information of runtime entities
 */ 
class Base_Entity{
private:
	int phase;
	int period;
	int deadline;
	int codeSize;
	int priority;
	string name; 		/*!< Name of the entity. */
	string type; 		/*!< Type of the entity. */
	int id; 	 		/*!< Id of the entity. */
	int memCons;
	bool preemtable;
	int jitter;		 	/*!< Task jitter. */
	int execution; 		/*!< Task execution time on its node.*/
	bool periodic;	/*!< True if the entity is released periodically.*/
	bool is_task_type;/*! is true when the entity is a task. */
	bool is_message_type;/*! is true when the entity is a message. */
	set<int> resources;	/*!< A set of resources that is accessed by the entity
								A task can only have one resource, while a message can have multiple resources (links).*/    								
public:    
	/**
	 * constructs a periodic entity using vectors of elements and values
	 * to be used for reading entities from xml files
	 */ 
	Base_Entity(vector<char*> elements, vector<char*> values, int number);
	Base_Entity(vector<char*> elements, vector<char*> values, int number, bool _is_task, bool _is_message);
	Base_Entity(int _id, int _period, int _priority, set<int> _resources, int _exec, bool _is_task, bool _is_message);
	virtual ~Base_Entity(){};
	/**
	 * returns the entity utilization
	 */
	 /**
	  * Copy constructor.
	  * It resets all variables that are affected by the schedulability analysis.
	  */ 
	 Base_Entity(const Base_Entity& _entity);
	 double get_utilization(); 
	 /**
	  * returns the entity load at a given time point
	  */
	int get_load(int time); 
	set<int> get_resources();
	bool exist_on_resource(int resource_id); /*! returns true if entity exists on resource with id equal to resource_id.*/
	int get_id();
	int get_priority();
	int get_deadline();
	int get_execution();
	int get_period();
	bool is_task()const{return is_task_type;};
	bool is_message()const{return is_message_type;};
	bool is_periodic(){return periodic;};
	friend std::ostream& operator<< (std::ostream &out, const Base_Entity &entity);
	
};

class Entity
{
private:	
	int response_time;	/*!< The response time of entities, this has to be set by the analysis.*/
	vector<int> instances; /*!< activation instance of the entity. */
	
public:
    bool is_schedulable;
	Base_Entity& base_entity;/*!< Reference to the base entity.*/
	int count; /*! stores the entity instance that is relevant for the analysis.*/	  
	Entity(Base_Entity& _base_entity);
	void add_instace(int new_instance);
	void set_response_time(int _response_time);
	int get_instance(int indx);
	int get_response_time();
	int get_instanc_size(){return instances.size();};
	void add_to_response_time(int _res_time);	
	friend std::ostream& operator<< (std::ostream &out, const Entity &entity);	
};
class Task : public Base_Entity
{
public:
    Task(vector<char*> elements, vector<char*> values, int number): Base_Entity(elements, values, number, true, false){}
    Task(int _id, int _period, int _priority, set<int> _resources, int _exec):Base_Entity(_id, _period, _priority, _resources, _exec, true, false)
    {
		if(_resources.size() != 1)
			cout << " A task can only use one resource, while task[" << _id
			     << "] uses " << _resources.size() << " resources\n"; 
	}
    int get_resource_id(){return *get_resources().begin();};
    virtual ~Task(){};
};

class Message : public Base_Entity
{
public:
    Message(vector<char*> elements, vector<char*> values, int number): Base_Entity(elements, values, number, false, true){}
    Message(int _id, int _period, int _priority, set<int> _resources, int _exec):Base_Entity(_id, _period, _priority, _resources, _exec, false, true){}
	virtual ~Message(){};
};

class TimePath_Entity 
{
private:
	int instant;
	int activation_time;
	int next_activ_time;
	
public:	
	Entity& entity;
	TimePath_Entity(Entity& _entity) :
	instant(0),
	activation_time(0),
	next_activ_time(0),
	entity(_entity){}
	void set_instant(){instant = entity.count + 1;};
	int get_instant(){return instant;};
	int get_activation_time(){return activation_time;};
	int get_next_activ_time(){return next_activ_time;};

	void set_next_active_time()
	{
		if (entity.count + 1 < entity.get_instanc_size())
		{
			next_activ_time = entity.get_instance(entity.count + 1);
		}
		else
		{
			next_activ_time = entity.get_instance(entity.count) + entity.base_entity.get_period();
		}
	};
	void set_activation_time()
	{
		if(entity.count > entity.get_instanc_size() || entity.count < 0)
		{
			cout << "count=" << entity.count << " is out of range\n";
			throw runtime_error("count is out of range");;
		}
		activation_time = entity.get_instance(entity.count);
	};
};

class TimePath
{
public:
	bool reachable = false;
	bool is_first = false;
	bool is_pred = false;
	int  id = -1;
	TimePath* pred_tp = nullptr;
	TimePath(int _id){id = _id;};	
	void add_time_path_enity(TimePath_Entity* tp_entity){tp_entities.push_back(tp_entity);};
	vector<TimePath_Entity*> tp_entities;	
	int get_activation_next_entity(TimePath_Entity* _tp_entity)
	{
		bool found = false;
		for (auto tp_entity : tp_entities)
		{
			if(found)
			{
				return tp_entity->get_activation_time();
			}
			if(_tp_entity == tp_entity)
				found = true;			
		}
		throw runtime_error("could not find the next entity for entity=");
		return -1;
	}
	int get_priority_next_entity(TimePath_Entity* _tp_entity)
	{
		bool found = false;
		for (auto tp_entity : tp_entities)
		{
			if(found)
				return tp_entity->entity.base_entity.get_priority();
			if(_tp_entity == tp_entity)
				found = true;			
		}
		return INT_MAX;
	}
	int get_first_instant()
	{
		try
		{
		    return (*tp_entities.begin())->get_instant();
		}
		catch(exception& e)
		{
			cout << "get_first_instant failed \n";
		}    
	}
	int get_last_instant()
	{
		try
		{
			return (*tp_entities.back()).get_instant();
		}
		catch(exception& e)
		{
			cout << "get_last_instant failed \n";
		}
	}
	int get_first_activation_time()
	{
		try
		{
			return (*tp_entities.begin())->get_next_activ_time();
		}
		catch(exception& e)
		{
			cout << "get_first_activation_time failed \n";
		}
	}
	int get_last_activation_time()
	{
		try
		{
			return (*tp_entities.back()).get_next_activ_time();
		}
		catch(exception& e)
		{
			cout << "get_last_activation_time failed \n";
		}
	}
	int get_last_response_time()
	{
		try
		{
			return (*tp_entities.back()).entity.get_response_time();
		}
		catch(exception& e)
		{
			cout << "get_last_activation_time failed \n";
		}
	}
	int get_pred_first_activation_time()
	{
		if(pred_tp == nullptr)
			return 0;
		return pred_tp->get_first_activation_time();	
	}
};
