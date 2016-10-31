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
 * This class is a test class that test the other classes using a given inut and a known output.
 */ 
#include "../settings/input_reader.hpp"
#include "../execution/execution.cpp"

using namespace std;

class Test
{
private:	
	vector<Base_Entity*> entities;
	vector<Base_Transaction*> transactions;
	vector<Reservation*> reservations;
	Application* app;
	Settings* settings;
	const string messageStart		= "==========";
public:
    Test(Settings* _settings):
    settings(_settings)
    {
		init();
	}
    Test()
    {
		init();
	}
    void init()
    {
		int id = 0;
		int period = 40000;
		int execution = 100;
		int priority = 1;
		set<int> node{0};
		Task * task1 = new Task(id, period, priority, node, execution);
		
		id = 1;
		period = 40000;
		execution = 200;
		priority = 1;
		node.erase(0);
		node.insert(1);
		Task * task2 = new Task(id, period, priority, node, execution);
		
		id = 2;
		period = 40000;
		int transmission = 40;
		priority = 1;
		set<int> links{2,3};
		Message * msg1 = new Message(id, period, priority, links, transmission);
		
		entities.push_back(task1);
		entities.push_back(task2);
		entities.push_back(msg1);
		
		for (auto entity: entities)
			cout << * entity << endl;
		
		/**
		 * transaction = task1 -> msg1 -> task2
		 */ 
		vector<int> trans_ids{0,2,1};	
		transactions.push_back(new Base_Transaction(0, trans_ids, entities, 60000, 60000*2, 60000*3));	
		
		cout << "creating reservations\n";
		/// Server on node 0
		id = 0;
		int resource_id = 0;
		int theta = 18000;
		int pi = 20000;
		priority = 1;
		reservations.push_back(new Reservation(id, theta, pi, priority, resource_id));
		/// Server on node 1
		id = 1;
		resource_id = 1;
		theta = 18000;
		pi = 20000;
		priority = 1;
		reservations.push_back(new Reservation(id, theta, pi, priority, resource_id));
		/// Server on link 0 which is node 2	
		id = 2;
		resource_id = 2;
		theta = 18000;
		pi = 20000;
		priority = 1;
		reservations.push_back(new Reservation(id, theta, pi, priority, resource_id));
		/// Server on link 1 which is node 3	
		id = 2;
		resource_id = 3;
		theta = 18000;
		pi = 20000;
		priority = 1;
		reservations.push_back(new Reservation(id, theta, pi, priority, resource_id));
		
		app = new Application(transactions, reservations);
		cout << *app << endl;
			
	};
	~Test()
	{
		/**
		 * Application destructor deltes all transactions and reservations
		 * and transaction destructor deltes all entities.
		 */		
		delete app;
	}
	void run_tests()
	{
		cout << "started tests\n";
		try
		{
			bool schedulability = app->schedulability();
			if(!schedulability)
			    cout << "The system is NOT schedulabe \n";
		}
		catch(std::exception const & e)
		{
			cout << "Test class could not run the schedulability test:\n"
			     << e.what() << endl;
		}
		
		
		bool failed = false;
		/*
		 * trans 0 task 0 res. time=4100
			trans 0 task 1 res. time=4200
			trans 0 msg 0 res. time=8080 ret=40000
			--trans 0 res. time=16380
			calculating delays
			==trans 0 resTime 16380 age_delay=84200 reac_delay=84200
		*/
		vector<int> expected_res_times{4100, 4200, 8080};
		for (auto entity :  entities)
		{
			if(app->get_response_time(*entity) != expected_res_times[entity->get_id()])
			{
				cout << "response time of the following entity is calculated incorrectly!!! expected value: "
				     << expected_res_times[entity->get_id()]
				     << " calculated value: " << app->get_response_time(*entity) << endl;
				cout << *entity << endl;
				failed = true;
			}
		}
		vector<int> expected_trans_res_times{16380};
		for (auto trans :  transactions)
		{		
			if(expected_trans_res_times[0] != app->get_response_time(*trans))
			{
				cout << "response time of the transaction is calculated incorrectly!!! expected value: "
				     << expected_trans_res_times[0]
				     << " calculated value: " << app->get_response_time(*trans) << endl;
				failed = true;
			}
			if(app->get_age_delay(*trans) != 84200)
			{
				cout << "age delay of the transaction is calculated incorrectly!!! expected value: 84200"
				     << " calculated value: " << app->get_age_delay(*trans) << endl;
				failed = true;
			}
			if(app->get_reaction_delay(*trans)  != 124200)
			{
				cout << "reaction delay of the transaction is calculated incorrectly!!! expected value: 84200"
				     << " calculated value: " << app->get_reaction_delay(*trans) << endl;
				failed = true;
			}
			
		}
		if(failed)
		{
			cout << "at least one test has failed!!!\n";
			return;
		}
		else
			cout << "all tests are finished successfully\n";
			
		try
		{
			cout << messageStart + "Creating a constraint model object ... " << endl;
			CP_model* model = new CP_model(transactions, settings);	
			
			cout << messageStart + "Creating an execution object ... " << endl;
			Execution<CP_model> execObj(model, settings);
			
			cout << messageStart + "Running the model object ... " << endl;
			execObj.Execute();
			
		}
		catch(std::exception const & e)
		{
			cout << "Test failed to create CP model:\n"
			     << e.what() << endl;
		}	
			
	};
};
