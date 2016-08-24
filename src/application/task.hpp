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
public:
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
	vector<int> links;	/*!< A set of links that the message crosses.*/

	Task(int _phase, int _period, int _deadline, int _mem, int _cS, int _priority, string _name, string _type, int _id);

	Task(int _period, int _mem, int _cS, string _name, string _type, int _id);
	
	/**
	 * constructs a periodic task using vectors of elements and values
	 * to be used for reading tasks from xml files
	 */ 
	Task(vector<char*> elements, vector<char*> values, int number);

	friend std::ostream& operator<< (std::ostream &out, const Task &task);
	
};
