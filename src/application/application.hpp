/**
 * This class encapsulates applications
 */ 

#include "transaction.hpp"
#include "resource.hpp"
#include <stdexcept>

extern "C" {
  // Get declaration for f(int i, char c, float x)
  /**
   * (1) gcc -Wall -c *.c
   * (2) ar -cvq libsched.a analysis.o
   */ 

  int perform_analysis();
}
using namespace std;

class Application{
public:
	Application(vector<Transaction*> _transactions);
	/**
	 * Returns the minimum deadline among all transactions.
	 */ 
	int get_min_trans_deadline();
	/**
	 * returns the utilization of elements on node n
	 */
	double get_utilization(int n); 
	/**
	 * performs schedulability based on the input budget and input period
	 */
	bool schedulability(int budget, int period);

private:
	vector<Transaction*> transactions;	/*!< The set of transactions that compose the application.*/
	vector<Resource*> resources;	/*!< The set of transactions that compose the application.*/
	
	/**
	 * @param [in] task: task under analysis.
	 * @param trans: transaction that task belongs to.
	 * 
	 * Returns the response time of the task
	 */
	int calculateTaskResponseTime(Task &task, int time); 	
	/**
	 * @param [in] task: task under analysis.
	 * @param trans: transaction that task belongs to.
	 * 
	 * Returns the request bound function for input task at a given time
	 */
	int rbf(Task &task, int trans);   
	/**
	 * @param task that we want its resource
	 * @return the reference to the respurce that task uses
	 */ 
	Resource& get_resource(Task& task);
	friend std::ostream& operator<< (std::ostream &out, const Application &trans);
	
};
