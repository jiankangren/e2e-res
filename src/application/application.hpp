/**
 * This class encapsulates applications
 */ 

#include "transaction.hpp"
#include <stdexcept>
using namespace std;

class Application{
public:
	vector<Transaction*> transactions;	/*!< The set of transactions that compose the application.*/
	Application(vector<Transaction*> _transactions);
	/**
	 * Returns the minimum deadline among all transactions.
	 */ 
	int get_min_trans_deadline();
	/**
	 * returns the utilization of elements on node n
	 */
	 double get_utilization(int n); 
	friend std::ostream& operator<< (std::ostream &out, const Application &trans);
	
};
