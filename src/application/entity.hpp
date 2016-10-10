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
/**
 * This class encapsulates runtime entities
 */ 

#include <math.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/math/common_factor.hpp>
#include <exception>
using namespace std;

class RunTime_Entity{
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
	int node;		 	/*!< Task node. We assume that the entities are already mapped on processors.*/
	int execution; 		/*!< Task execution time on its node.*/
	
	int response_time;	/*!< The response time of entities, this has to be set by the analysis.*/

	bool periodic;	/*!< True if the entity is released periodically.*/
	vector<int> instances; /*!< activation instance of the entity. */
	
public:
	RunTime_Entity(int _phase, int _period, int _deadline, int _mem, int _cS, int _priority, string _name, string _type, int _id);

	RunTime_Entity(int _period, int _mem, int _cS, string _name, string _type, int _id);
	
	/**
	 * constructs a periodic entity using vectors of elements and values
	 * to be used for reading entities from xml files
	 */ 
	RunTime_Entity(vector<char*> elements, vector<char*> values, int number);

	/**
	 * returns the entity utilization
	 */
	 double get_utilization(); 
	 /**
	  * returns the entity load at a given time point
	  */
	int count; /*! stores the entity instance that is relevant for the analysis.*/	  
	int get_load(int time); 
	int get_node();
	int get_id();
	int get_priority();
	int get_execution();
	int get_period();
	int get_response_time();
	void set_response_time(int _response_time);
	virtual bool is_task()const{return false;};
	virtual bool is_message()const{return false;};
	bool is_periodic(){return periodic;};
	void add_instace(int new_instance);
	int get_instance(int indx);
	int get_instanc_size(){return instances.size();};
	friend std::ostream& operator<< (std::ostream &out, const RunTime_Entity &entity);
	
};

class Task : public RunTime_Entity
{
public:
    Task(vector<char*> elements, vector<char*> values, int number): RunTime_Entity(elements, values, number)
    {}
    bool is_task()const{return true;}
};

class Message : public RunTime_Entity
{
public:
    Message(vector<char*> elements, vector<char*> values, int number): RunTime_Entity(elements, values, number)
    {}
    bool is_message()const{return true;};
    void add_to_response_time(int _res_time)
    {
		set_response_time(_res_time + get_response_time());
	};
    vector<int> links;	/*!< A set of links that the message crosses.*/
};

class TimePathEntity : public RunTime_Entity
{
private:
	int instant;
	int activation_time;
	int next_activ_time;
	
public:	
	TimePathEntity(RunTime_Entity& entity) : RunTime_Entity(entity)
	{
		instant = 0;
		activation_time = 0;
		next_activ_time = 0;
	}
	void set_instant(int _instant){instant = _instant;};
	int get_instant(){return instant;};
	int get_activation_time(){return activation_time;};
	int get_next_activ_time(){return next_activ_time;};
	
	void set_next_active_time()
	{
		if (count + 1 < get_instanc_size())
		{
			next_activ_time = get_instance(count + 1);
		}
		else
		{
			next_activ_time = get_instance(count) + get_period();
		}
	};
};

class TimePath
{
public:
	bool reachable = false;
	bool is_first = false;
	bool is_pred = false;
	TimePath* pred_tp = nullptr;
	void add_time_path_enity(TimePathEntity* tp_entity){tp_entities.push_back(tp_entity);};
	vector<TimePathEntity*> tp_entities;	
	int get_activation_next_entity(TimePathEntity* _tp_entity)
	{
		bool found = false;
		for (auto tp_entity : tp_entities)
		{
			if(found)
				return tp_entity->get_activation_time();
			if(_tp_entity == tp_entity)
				found = true;			
		}
		return -1;
	}
	int get_priority_next_entity(TimePathEntity* _tp_entity)
	{
		bool found = false;
		for (auto tp_entity : tp_entities)
		{
			if(found)
				return tp_entity->get_priority();
			if(_tp_entity == tp_entity)
				found = true;			
		}
		return INT_MAX;
	}
	int get_first_instant()
	{
		return (*tp_entities.begin())->get_instant();
	}
	int get_last_instant()
	{
		return (*tp_entities.end())->get_instant();
	}
	int get_first_activation_time()
	{
		return (*tp_entities.begin())->get_next_activ_time();
	}
	int get_last_activation_time()
	{
		return (*tp_entities.end())->get_next_activ_time();
	}
	int get_pred_first_activation_time()
	{
		if(pred_tp == nullptr)
			return 0;
		return pred_tp->get_first_activation_time();	
	}
};
