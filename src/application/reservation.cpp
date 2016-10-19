#include "reservation.hpp"

using namespace std;
Reservation::Reservation(){};
Reservation::Reservation(int _id, int _theta, int _pi, int _priority, int _resource_id):
id(_id),
theta(_theta),
pi(_pi),
priority(_priority),
resource_id(_resource_id){};

std::ostream& operator<< (std::ostream &out, const Reservation &reservation)
{
	out << " budget= " 	<< reservation.theta
		<< ", period=" 		<< reservation.pi 
		<< ", priority=" 	<< reservation.priority
		<< ", resource=" 	<< reservation.resource_id;
 
	return out;
}

int Reservation::sbf_proc(int time)
{
	int supply = 0;
	int delta = 2 * (pi - theta);
	int divK = (time - delta) / pi;
	int sK = (int)floor(divK);
	if (time < delta)
	{
		supply = 0;
	}
	else if ((time >= delta + sK * pi) && (time < delta + sK * pi + theta))
	{
		supply = time - (delta + sK * (pi - theta));
	}
	else if ((time >= delta + sK * pi + theta) && (time < delta + (sK + 1) * pi))
	{
		supply = (sK + 1) * theta;
	}
	return supply;
}
int Reservation::sbf_net(int time, int idle)
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
		sbf = maxCalc + (yy * (theta - idle));
	}
	return sbf;
}
int Reservation::get_resource_id()
{
	return resource_id;
}
