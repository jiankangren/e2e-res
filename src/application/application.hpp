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
/**
 * This class encapsulates applications
 */ 

#include "transaction.hpp"
#include "reservation.hpp"
#include <stdexcept>

using namespace std;

class Application{
public:
	Application(vector<Base_Transaction*> _transactions);
	Application(vector<Base_Transaction*> _transactions, vector<Reservation*> _reservations);
	~Application();
	void init(vector<Base_Transaction*> _transactions);
	set<int> resources;
	/**
	 * Returns the minimum deadline among all transactions.
	 */ 
	int get_min_trans_deadline();
	/**
	 * returns the utilization of elements on node n
	 */
	double get_utilization(int n); 
	/**
	 * performs schedulability 
	 */
	bool schedulability();
	
	int get_max_resource_id();
	
	int get_no_trans();
	
	void set_reservations(vector<Reservation*> _reservations);
	
	int get_response_time(Base_Entity& entity);
	int get_response_time(Base_Transaction& _base_transaction);
	int get_age_delay(Base_Transaction& _base_transaction);
	int get_reaction_delay(Base_Transaction& _base_transaction);
	int get_shortest_element_period();
private:
	vector<Transaction*> transactions;	/*!< The set of transactions that compose the application.*/
	vector<Reservation*> reservations;	/*!< The set of resource reservations that belong to this application.*/
	
	/**
	 * @param [in] task: task under analysis.
	 * @param time: time point
	 * 
	 * Calculates the response time of the input task and sets it.
	 */
	void calculateTaskResponseTime(Entity &entity); 	
	/**
	 * @param [in] message: message under analysis.
	 * 
	 * Calculates the response time of the input message and sets it.
	 */
	void calculateMessageResponseTime(Entity& entity); 
	/**
	 * @param [in] task: task under analysis.
	 * @param trans: transaction that task belongs to.
	 * 
	 * Returns the request bound function for input task at a given time
	 */
	void calculateResponseTime(Entity* entity);
	int rbf_task(Task &task, int trans);  
	/**
	 * @param [in] message: the message under analysis.
	 * @param link: link under analysis.
	 * @param time: time point which we want rbf
	 * 
	 * Returns the request bound function for input message at a given time
	 */
	int rbf_message(Message &message, int time, int link);
	/**
	 * @param resource_id is the id of resource that reservation belongs to
	 * @return the reference to the respurce that task uses
	 */ 
	Reservation& get_resource(int resource_id);
	/**
	 * returns the LCM of the period all tasks and messages in the application.
	 */ 
	int get_app_LCM();
	int LCM(int x, int y);
	/**
	 * Derives the activation instances of entities in the application
	 */
	void deriveActivationInstants(); 
	/**
	 * Finds all time paths for all transactions in the application
	 */
	void findAllTimedPaths();  
	void findReachableTimePaths();
	void findFirstTimePaths();
	Transaction& find_transaction(Base_Transaction& _base_transaction);
	friend std::ostream& operator<< (std::ostream &out, const Application &trans);
	
};
