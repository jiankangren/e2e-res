
#include "schedulability.hpp"

using namespace Gecode;
using namespace Int;
using namespace std;


Schedulability::Schedulability(Space& home, 
                  ViewArray<IntView> _theta_array,
                  ViewArray<IntView> _pi_array,
                  ViewArray<IntView> _res_times_array,
                  vector<Base_Transaction*> _base_transactions)
  : Propagator(home), theta_array(_theta_array), pi_array(_pi_array), res_times_array(_res_times_array), base_transactions(_base_transactions)
{
  theta_array.subscribe(home, *this, Int::PC_INT_BND);
  pi_array.subscribe(home, *this, Int::PC_INT_BND);
  home.notice(*this, AP_DISPOSE);   
}

size_t Schedulability::dispose(Space& home){
  home.ignore(*this, AP_DISPOSE);
  (void) Propagator::dispose(home);  
  return sizeof(*this);
}
 

Propagator* Schedulability::copy(Space& home, bool share){
	return new (home) Schedulability(home, share, *this);
}


PropCost Schedulability::cost(const Space& home, const ModEventDelta& med) const{
  return PropCost::linear(PropCost::HI,theta_array.size());
}

Schedulability::Schedulability(Space& home, bool share, Schedulability& p)
  : Propagator(home, share, p),
    theta_array(p.theta_array),
    pi_array(p.pi_array),
    res_times_array(p.res_times_array),
    base_transactions(p.base_transactions)
    {
  theta_array.update(home, share, p.theta_array);
  pi_array.update(home, share, p.pi_array);
  res_times_array.update(home, share, p.res_times_array);
}


ExecStatus Schedulability::propagate(Space& home, const ModEventDelta&){
    /**
     * First we calculate the response time assuming the minimum bandwidth
     * i.e., min budget and max period
     * We can use the result of the analysis only if the systems is schedulable
     * otherwise the response times might be zero
     */ 
    vector<Reservation*> reservations;
    for(int i = 0; i<theta_array.size();i++)
    {  
		int id = i;
		int resource_id = i;
		int theta = theta_array[i].min();
		int pi = pi_array[i].max();
		int priority = 1;
		reservations.push_back(new Reservation(id, theta, pi, priority, resource_id));
	}
	Application* application = new Application(base_transactions, reservations);
	bool schedulable = application->schedulability();
	if(schedulable)
	{
		for (unsigned int i=0;i<base_transactions.size();i++)
		{
			if(application->get_response_time(*base_transactions[i]) < res_times_array[i].max())
			{
				cout << "contracting \n";
				cout << "Wort-case res times \n" << *application << res_times_array << endl		 
				     << theta_array << endl
					 << pi_array
					 << endl << endl;
			}
			GECODE_ME_CHECK(res_times_array[i].lq(home,application->get_response_time(*base_transactions[i])));
		}
    }
	
	/**
	 * Now we calculate response time assuming the highest bandwidths.
	 */ 
	delete application;	
    reservations.clear();
    
    for(int i = 0; i<theta_array.size();i++)
    {  
		int id = i;
		int resource_id = i;
		int pi = pi_array[i].min();
		int theta = min(pi, theta_array[i].max());
		int priority = 1;
		reservations.push_back(new Reservation(id, theta, pi, priority, resource_id));
	}
	application = new Application(base_transactions, reservations);
	schedulable = application->schedulability();

	if(schedulable)
	{
		for (unsigned int i=0;i<base_transactions.size();i++)
		{
			if(application->get_response_time(*base_transactions[i]) < res_times_array[i].max())
			{	
				cout << "Best-case res times \n" << *application << res_times_array << endl
					 << theta_array << endl
					 << pi_array
					 << endl << endl;
			}
			GECODE_ME_CHECK(res_times_array[i].gq(home,application->get_response_time(*base_transactions[i])));
			if(application->get_response_time(*base_transactions[i]) > base_transactions[i]->get_deadline())
			{
				cout << "ES_FAILED!!!\n";
				delete application;	
				return ES_FAILED;
			}
		}
	}
	else
	{
		cout << "ES_FAILED!!!\n";
		delete application;	
		return ES_FAILED;
	}
	 
	delete application;	
	
	if(theta_array.assigned() && pi_array.assigned())
    {
		cout << "All assigned!!!\n"
		     << res_times_array  << endl
			 << theta_array << endl
			 << pi_array
			 << endl << endl;;
		return home.ES_SUBSUMED(*this);
	}
    else
    {
		return ES_FIX;
	}
    
  return ES_FIX;
}

void Schedulability( Space& home, 
					 const IntVarArgs& _theta_args,
					 const IntVarArgs& _pi_args,
					 const IntVarArgs& _res_tims_args,
					 vector<Base_Transaction*> _base_transactions)
{
  if (home.failed()) 
    return;
  ViewArray<Int::IntView> tmp_theta(home, _theta_args);
  ViewArray<Int::IntView> tmp_pi(home, _pi_args);  
  ViewArray<Int::IntView> tmp_res_times(home, _res_tims_args);  
  if (_theta_args.size() != _pi_args.size()) {
    throw Gecode::Int::ArgumentSizeMismatch("Schedulability constraint, theta_args & pi_args");
  }
  if (Schedulability::post(home, tmp_theta, tmp_pi, tmp_res_times, _base_transactions) != ES_OK) {
    home.fail();
  }
}
