#pragma once
#include <math.h>
#include <vector>

#include <gecode/int.hh>
#include <gecode/set.hh>
#include <gecode/gist.hh>
#include <gecode/minimodel.hh>

#include "../application/transaction.hpp"
#include "../settings/settings.hpp"

using namespace Gecode;

/**
 * Gecode space containing the scheduling model based on the paper.
 */
class CP_model : public Space 
{
  
private:
	vector<Transaction*> 	transactions;/**< Vector of pointer to the transactions. */  
	Settings*	 			settings;	/**< Pointer to the setting object. */


	IntVarArray 			budget;		/**< the budget of resources. */
	IntVarArray 			period;		/**< the period of resources. */
	

  
public:

	CP_model(vector<Transaction*> 	_transactions, Settings* _settings, int _nodes);

	CP_model(bool share, CP_model& s);

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

		switch(settings->getOptCriterion())
		{
			case(Settings::POWER):
				//rel(*this, sys_power < b.sys_power);
				break;
			default:
				cout << "unknown optimization criterion !!!\n";
				throw CException("");
				break;
		}   
	}
  
};
