#include "resource.hpp"

using namespace std;

Resource::Resource(int _id, int _theta, int _pi, int _priority):
id(_id),
theta(_theta),
pi(_pi),
priority(_priority){};

std::ostream& operator<< (std::ostream &out, const Resource &resource)
{
	out << " budget= " 	<< resource.theta
		<< ", period=" 		<< resource.pi 
		<< ", priority=" 	<< resource.priority;
 
	return out;
}

int Resource::sbf(int time)
{
	int supply = 0;
	int delta = 2 * (pi - theta);
	int divK = (time - delta) / pi;
	int sK = (int)floor(divK);
	if (time < delta)
	{
		supply = 0;
	}
	else if ((time >= delta + sK * pi) || (time < delta + sK * pi + theta))
	{
		supply = time - (delta + sK * (pi - theta));
	}
	else if ((time >= delta + sK * pi + theta) || (time < delta + (sK + 1) * pi))
	{
		supply = (sK + 1) * theta;
	}
	return supply;
}
int Resource::get_id()
{
	return id;
}
