/**
 * This class encapsulates periodic resources
 */ 

#include <math.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <boost/math/common_factor.hpp>

using namespace std;

class Resource{
private:
	int id;			/*!< Resource id. */
	int theta;		/*!< Resource budget. */
	int pi;			/*!< Resource period. */
	int priority;	/*!< Resource priority. */
	
	friend std::ostream& operator<< (std::ostream &out, const Resource &resource);
public:
	Resource(int _id, int _theta, int _pi, int _priority);
	/**
	 * returns the supply bound function at a given time given that
	 * the resource belongs to a processor
	 */
	 int sbf_proc(int time); 
	 /**
	 * returns the supply bound function at a given time given that
	 * the resource belongs to a network switch
	 */
	 int sbf_net(int time, int idle); 
	 int get_id();
	
};
