/**
 * Copyright (c) 2016    Nima Khalilzad   <nkhal@kth.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include <math.h>
#include <vector>

#include <gecode/int.hh>
#include <gecode/set.hh>
#include <gecode/gist.hh>
#include <gecode/minimodel.hh>


#include "../settings/settings.hpp"
#include "schedulability.hpp"


using namespace Gecode;

/**
 * Gecode space containing the scheduling model based on the paper.
 */
class CP_model : public Space 
{
  
private:
	vector<Base_Transaction*> base_transactions;/**< A vector of pointers to the base transactions. */  
	Settings*	 			settings;	/**< Pointer to the setting object. */


	IntVarArray 			budget;		/**< the budget of resources. */
	IntVarArray 			period;		/**< the period of resources. */
	IntVarArray 			res_times;		/**< the response time of all transactions. */
	IntVarArray 			age_delays;		/**< the age delays of all transactions. */
	IntVarArray 			reac_delays;		/**< the reaction delays of all transactions. */
	IntVarArray 			utilization;	/**< the utilization of resources in percentage. */		
	IntVar					total_utilization;/**< the sum of application utilizations. */
	Application* 			application;	
	int						no_resources;	
  
public:

	CP_model(vector<Base_Transaction*> _base_transactions, Settings* _settings);

	CP_model(bool share, CP_model& s);
	~CP_model();
	virtual Space* copy(bool share);
	
	/**
	* Prints the variables
	*/ 
	void print(std::ostream& out) const;
	/**
	* Prints variables for csv output
	*/ 
	void printCSV(std::ostream& out) const;
	/**
	* Prints mappings and modes for csv output
	*/ 
	virtual void constrain(const Space& _b)
	{
		const CP_model& b = static_cast<const CP_model&>(_b);
		const int ratio = 100;//b.base_transactions[0]->get_deadline();
		
		int cost = (b.total_utilization.max()*ratio) + b.res_times[0].max();
		IntArgs c(2); IntVarArgs x(2);
		c[0]=ratio;
		c[1]=1; 
		x[0]=total_utilization;
		x[1]=res_times[0];
		
		//(b.period).slice(0,1, no_cpu);
		IntVarArgs old_period(*this, application->get_max_resource_id()+1, 0, Int::Limits::max);
		old_period = b.period;
		int no_cpu =  application->get_max_cpu_resource_id()+1;
		
		switch(settings->getOptCriterion())
		{
			case(Settings::RES_TIME):
				rel(*this, sum(res_times) < sum(b.res_times));
				break;
			case(Settings::AGE_DELAY):
				rel(*this, sum(age_delays) < sum(b.age_delays));
				break;
			case(Settings::REACTION_DELAY):
				rel(*this, sum(reac_delays) < sum(b.reac_delays));
				break;		
			case(Settings::UTILIZATION):
				rel(*this, total_utilization < b.total_utilization);
				break;	
			case(Settings::OVERHEAD):
				rel(*this, sum(period.slice(0,1, no_cpu)) > sum(old_period.slice(0,1, no_cpu)));
				break;
			case(Settings::COST):	
			cout << "*************cost=" << cost << endl;			
				//rel(*this, total_utilization*base_transactions[0]->get_deadline() + res_times[0] < cost);
				linear(*this, c , x , IRT_LE, cost);
				break;			
			
			default:
				cout << "unknown optimization criterion !!!\n";
				throw CException("");
				break;
		}   
	}
  
};
