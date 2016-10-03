/**
 * This class encapsulates both tasks and messages
 */ 

#include <math.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/math/common_factor.hpp>

using namespace std;

class Task{
private:
	int phase;
	int period;
	int deadline;
	int codeSize;
	int priority;
	string name; 		/*!< Name of the task. */
	string type; 		/*!< Type of the task. */
	int id; 	 		/*!< Id of the task. */
	int memCons;
	bool preemtable;
	int jitter;		 	/*!< Task jitter. */
	int node;		 	/*!< Task node. We assume that the tasks are already mapped on processors.*/
	int execution; 		/*!< Task execution time on its node.*/
	vector<int> links;	/*!< A set of links that the message crosses.*/
	
	int response_time;	/*!< The response time of tasks, this has to be set by the analysis.*/

public:
	Task(int _phase, int _period, int _deadline, int _mem, int _cS, int _priority, string _name, string _type, int _id);

	Task(int _period, int _mem, int _cS, string _name, string _type, int _id);
	
	/**
	 * constructs a periodic task using vectors of elements and values
	 * to be used for reading tasks from xml files
	 */ 
	Task(vector<char*> elements, vector<char*> values, int number);

	/**
	 * returns the task utilization
	 */
	 double get_utilization(); 
	 /**
	  * returns the task load at a given time point
	  */
	int get_load(int time); 
	int get_node();
	int get_id();
	int get_priority();
	int get_execution();
	int get_period();
	void set_response_time(int _response_time);
	friend std::ostream& operator<< (std::ostream &out, const Task &task);
	
};
