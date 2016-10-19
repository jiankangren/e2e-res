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
 * This class encapsulates both tasks and messages
 */ 
#pragma once
#include <math.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/math/common_factor.hpp>

#include "entity.hpp"

using namespace std;
class Base_Transaction
{
private:
	int id;										/*! The id of transaction.*/
	vector<Base_Entity*>	entities;           		/*!< A set of pointers pointing to the tasks and messages.*/
	vector<int> 	entity_ids;					/*!< The sequence of tasks and messages based on their ids.*/
	int				deadline;					/*!< The end-to-end deadline of transaction.*/
	int				age_delay_deadline;			/*!< The age_delay_deadline deadline of transaction.*/
	int				reaction_delay_deadline;	/*!< The reaction_delay_deadline deadline of transaction.*/	
public:
	/**
	 * constructs a periodic task using vectors of elements and values
	 * to be used for reading tasks from xml files
	 */ 
	Base_Transaction(vector<char*> elements, vector<char*> values);
	Base_Transaction(int _id, vector<int> _entity_ids, vector<Base_Entity*> _entities, int _deadline, int _age_deadline, int _reaction_deadline);
	int get_id(){return id;};
	vector<int> get_entity_ids(){return entity_ids;};
	vector<Base_Entity*> get_entities(){return entities;};
	int get_deadline(){return deadline;};
	int get_age_delay_deadline(){return age_delay_deadline;};
	int get_reaction_delay_deadline(){return reaction_delay_deadline;};
	void Build_transaction(vector<Base_Entity*> all_entities);
};
class Transaction{
public:
	Base_Transaction& base_transaction;
	vector<Entity*>	entities;           		/*!< A set of pointers pointing to the tasks and messages.*/
	int				response_time;				/*!< The response time of transaction.*/
	int				age_delay;					/*!< The age delay of transaction.*/
	int				reaction_delay;				/*!< The reaction delay of transaction.*/
	vector<TimePath*> time_paths;				/*!> Vector of all time paths.*/
	Transaction(Base_Transaction& _base_transaction);
	~Transaction();
	/**
	 * This function finds builds the task vector, i.e. tasks based on the task_ids vector
	 */ 
	/**
	 * Returns the utilization of elements on node n.
	 */  
	double get_utilization(int n);
	
	/**
	 * calculates the response time for the transaction and updates the member
	 */ 
	void calculateTransResponseTime();
	 
	void add_time_path(TimePath* _time_path);
	/**
	 * finds all time paths of the transaction
	 */ 
	void findAllTimedPaths();
	/**
	 * Sets the count for the timepath enitity
	 */ 
	void adjust_count(Entity* entity);
	void increment_next_entity_count(Entity* _entity);
	void calculateTransAgeDelay();
	void calculateTransReacDelay();
	int get_response_time();
	int get_first_task_period();
	bool is_schedulable();
	friend std::ostream& operator<< (std::ostream &out, const Transaction &trans);
	
};
