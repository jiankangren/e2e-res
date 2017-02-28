
#include "schedulability.hpp"

using namespace Gecode;
using namespace Int;
using namespace std;


Schedulability::Schedulability(Space& home, 
				  ViewArray<IntView> _utilization_array,
                  ViewArray<IntView> _theta_array,
                  ViewArray<IntView> _pi_array,
                  ViewArray<IntView> _res_times_array,
                  ViewArray<IntView> _age_delay_array,
                  ViewArray<IntView> _reac_delay_array,
                  vector<Base_Transaction*> _base_transactions)
  : Propagator(home), utilization_array(_utilization_array), theta_array(_theta_array), pi_array(_pi_array), 
    res_times_array(_res_times_array),
    age_delay_array(_age_delay_array),
    reac_delay_array(_reac_delay_array),
    base_transactions(_base_transactions)
{
  utilization_array.subscribe(home, *this, Int::PC_INT_BND);
  utilization_array.subscribe(home, *this, Int::ME_INT_VAL);
  theta_array.subscribe(home, *this, Int::PC_INT_BND);
  pi_array.subscribe(home, *this, Int::PC_INT_BND);
  home.notice(*this, AP_DISPOSE);   
}

size_t Schedulability::dispose(Space& home){
  utilization_array.cancel(home, *this, Int::PC_INT_BND);
  utilization_array.cancel(home, *this, Int::ME_INT_VAL);
  theta_array.cancel(home, *this, Int::PC_INT_BND);
  pi_array.cancel(home, *this, Int::PC_INT_BND);
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
    utilization_array(p.utilization_array),
    theta_array(p.theta_array),
    pi_array(p.pi_array),
    res_times_array(p.res_times_array),
    age_delay_array(p.age_delay_array),
    reac_delay_array(p.reac_delay_array),
    base_transactions(p.base_transactions)
    {
	utilization_array.update(home, share, p.utilization_array);
  theta_array.update(home, share, p.theta_array);
  pi_array.update(home, share, p.pi_array);
  res_times_array.update(home, share, p.res_times_array);
  age_delay_array.update(home, share, p.age_delay_array);
  reac_delay_array.update(home, share, p.reac_delay_array);
}


ExecStatus Schedulability::propagate(Space& home, const ModEventDelta&){
    Application* application;
	vector<Reservation*> reservations;
    bool schedulable;	
    /**
     * First we calculate the response time assuming the minimum bandwidth
     * i.e., min budget and max period
     * We can use the result of the analysis only if the systems is schedulable
     * otherwise the response times might be zero
     */ 
    
    ///# This case is usefull when optimization for the cost (?)
    /*
    for(int i = 0; i<theta_array.size();i++)
    {  
		int id = i;
		int resource_id = i;
		int theta = theta_array[i].min();
		int pi = pi_array[i].max();
		int priority = 1;
		reservations.push_back(new Reservation(id, theta, pi, priority, resource_id));
	}
	
    application = new Application(base_transactions, reservations);
	schedulable = application->schedulability();
    cout << "worst case\n";
    print(application);
	
	if(schedulable)
	{
		for (unsigned int i=0;i<base_transactions.size();i++)
		{
			GECODE_ME_CHECK(res_times_array[i].lq(home,application->get_response_time(*base_transactions[i])));
			GECODE_ME_CHECK(age_delay_array[i].lq(home,application->get_age_delay(*base_transactions[i])));
			GECODE_ME_CHECK(reac_delay_array[i].lq(home,application->get_reaction_delay(*base_transactions[i])));
			
			if(application->get_response_time(*base_transactions[i]) > base_transactions[i]->get_deadline()
			  || application->get_age_delay(*base_transactions[i]) > base_transactions[i]->get_age_delay_deadline()
			  || application->get_reaction_delay(*base_transactions[i]) > base_transactions[i]->get_reaction_delay_deadline())
				cout << "ERROR!!!\n\n";
		}
    }
	 
	delete application;	
    reservations.clear();
    */ 
    /**
	 * Now we calculate response time assuming the highest bandwidths.
	 */
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
	//cout << "===== Testing with the best case\n";
	schedulable = application->schedulability();
    //cout << "best case-----------------------------------------------------\n";
    //print(application);
    if(schedulable)
	{
		for (unsigned int i=0;i<base_transactions.size();i++)
		{
			/*if(application->get_response_time(*base_transactions[i]) < res_times_array[i].min())
			{	
				cout << "*** ERROR \n Best-case res times \n" << *application 
					<< res_times_array << endl
					 << age_delay_array  << endl
					 << reac_delay_array  << endl
					 << theta_array << endl
					 << pi_array
					 << endl << endl;
			}*/
			GECODE_ME_CHECK(res_times_array[i].gq(home,application->get_response_time(*base_transactions[i])));
			GECODE_ME_CHECK(age_delay_array[i].gq(home,application->get_age_delay(*base_transactions[i])));
			GECODE_ME_CHECK(reac_delay_array[i].gq(home,application->get_reaction_delay(*base_transactions[i])));			
		}
	}
	else
	{
		//cout << "Not schedulable with the best-case reservations ES_FAILED!!!\n" ;
         /* 
		     << *application << endl
		     << utilization_array << endl;*/
		delete application;	
		for (auto res : reservations)
			delete res;
		return ES_FAILED;
	}
	 
	
	
	if(theta_array.assigned() && pi_array.assigned())
    {
		/*cout << "All assigned!!!\n"
		     << *application
		     << res_times_array  << endl
		     << age_delay_array  << endl
		     << reac_delay_array  << endl
			 << theta_array << endl
			 << pi_array
			 << endl << endl;*/
        for (unsigned int i=0;i<base_transactions.size();i++)
		{
            GECODE_ME_CHECK(res_times_array[i].eq(home,application->get_response_time(*base_transactions[i])));
            GECODE_ME_CHECK(age_delay_array[i].eq(home,application->get_age_delay(*base_transactions[i])));
            GECODE_ME_CHECK(reac_delay_array[i].eq(home,application->get_reaction_delay(*base_transactions[i])));	     
        }
	    delete application;	
	    for (auto res : reservations)
			delete res;
		return home.ES_SUBSUMED(*this);
	}
    else
    {
		for (auto res : reservations)
			delete res;
		delete application;	
        return ES_FIX;        
	}
}

void Schedulability( Space& home, 
					 const IntVarArgs& _utilization_args,
					 const IntVarArgs& _theta_args,
					 const IntVarArgs& _pi_args,
					 const IntVarArgs& _res_tims_args,
					 const IntVarArgs& _age_delay_args,
					 const IntVarArgs& _reac_delay_args,
					 vector<Base_Transaction*> _base_transactions)
{
  if (home.failed()) 
    return;
  ViewArray<Int::IntView> tmp_util(home, _utilization_args);  
  ViewArray<Int::IntView> tmp_theta(home, _theta_args);
  ViewArray<Int::IntView> tmp_pi(home, _pi_args);  
  ViewArray<Int::IntView> tmp_res_times(home, _res_tims_args);  
  ViewArray<Int::IntView> tmp_age_delay(home, _age_delay_args);  
  ViewArray<Int::IntView> tmp_reac_delay(home, _reac_delay_args);  
  if (_theta_args.size() != _pi_args.size()) {
    throw Gecode::Int::ArgumentSizeMismatch("Schedulability constraint, theta_args & pi_args");
  }
  if (Schedulability::post(home, tmp_util, tmp_theta, tmp_pi, tmp_res_times, tmp_age_delay, tmp_reac_delay, _base_transactions) != ES_OK) {
    home.fail();
  }
}
void Schedulability::print(Application* application)
{
    cout     
             //<< *application << endl
		     << utilization_array << endl
             << res_times_array << endl
             << age_delay_array << endl
             << reac_delay_array << endl;
   for (unsigned int i=0;i<base_transactions.size();i++)  
        cout << application->get_response_time(*base_transactions[i])  << " ";   
   cout << endl;     
   for (unsigned int i=0;i<base_transactions.size();i++)  
        cout << application->get_age_delay(*base_transactions[i])  << " ";   
   cout << endl;     
   for (unsigned int i=0;i<base_transactions.size();i++)  
        cout << application->get_reaction_delay(*base_transactions[i])  << " ";   
   cout << endl;     
}
