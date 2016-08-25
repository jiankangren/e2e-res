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

#include "task.hpp"

using namespace std;

class Transaction{
public:
	int id;										/*! The id of transaction.*/
	vector<Task*> 	tasks;						/*!< A set of pointers pointing to the tasks and messages.*/
	vector<int> 	task_ids;					/*!< The sequence of tasks and messages based on their ids.*/
	int				deadline;					/*!< The end-to-end deadline of transaction.*/
	int				age_delay_deadline;			/*!< The end-to-end deadline of transaction.*/
	int				reaction_delay_deadline;	/*!< The end-to-end deadline of transaction.*/
	Transaction();
	
	/**
	 * constructs a periodic task using vectors of elements and values
	 * to be used for reading tasks from xml files
	 */ 
	Transaction(vector<char*> elements, vector<char*> values);
	/**
	 * This function finds builds the task vector, i.e. tasks based on the task_ids vector
	 */ 
	/**
	 * Returns the utilization of elements on node n.
	 */  
	double get_utilization(int n);
	void Build_transaction(vector<Task*> all_tasks);
	
	friend std::ostream& operator<< (std::ostream &out, const Transaction &trans);
	
};
