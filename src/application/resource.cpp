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

int Resource::sbf_proc(int time)
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
int Resource::sbf_net(int time, int idle)
{
	int sbf = 0;
    if (time >= pi - theta - idle)
	{
		int xx = 2 * (pi - theta - idle);
		int yyDiv = (time - (pi - theta - idle)) / pi;
		int yy = (int)floor(yyDiv);
		int maxCalc = time - xx - (yy * pi);
		if (maxCalc < 0)
			maxCalc = 0;
		sbf = maxCalc + (yy * (pi - idle));
	}
	return sbf;
}
int Resource::get_id()
{
	return id;
}
