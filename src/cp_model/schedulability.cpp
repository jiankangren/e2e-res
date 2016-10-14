
#include "schedulability.hpp"

using namespace Gecode;
using namespace Int;
using namespace std;


Schedulability::Schedulability(Space& home, 
                  ViewArray<IntView> _theta_array,
                  ViewArray<IntView> _pi_array,
                  vector<Base_Transaction*> _base_transactions)
  : Propagator(home), theta_array(_theta_array), pi_array(_pi_array), base_transactions(_base_transactions)
{
  theta_array.subscribe(home, *this, Int::PC_INT_VAL);
  pi_array.subscribe(home, *this, Int::PC_INT_VAL);
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
    base_transactions(p.base_transactions)
    {
  theta_array.update(home, share, p.theta_array);
  pi_array.update(home, share, p.pi_array);
}


ExecStatus Schedulability::propagate(Space& home, const ModEventDelta&){
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
  if(application->schedulability())
    {
		if(theta_array.assigned() && pi_array.assigned())
      {
		return home.ES_SUBSUMED(*this);
	}
      else
      {
		return ES_FIX;
	   }
    }
  else
    {
		cout << theta_array << endl << pi_array << endl;
      return ES_FAILED;
    }
  return ES_FIX;
}

void Schedulability( Space& home, 
					 const IntVarArgs& _theta_args,
					 const IntVarArgs& _pi_args,
					 vector<Base_Transaction*> _base_transactions)
{
  if (home.failed()) 
    return;
  ViewArray<Int::IntView> tmp_theta(home, _theta_args);
  ViewArray<Int::IntView> tmp_pi(home, _pi_args);  
  if (_theta_args.size() != _pi_args.size()) {
    throw Gecode::Int::ArgumentSizeMismatch("Schedulability constraint, theta_args & pi_args");
  }
  if (Schedulability::post(home, tmp_theta, tmp_pi, _base_transactions) != ES_OK) {
    home.fail();
  }
}
