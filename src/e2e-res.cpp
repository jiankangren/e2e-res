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
 * This is the main file for the e2e-res tool
*/

#include "test/test.cpp"



using namespace Gecode;
using namespace Int;
using namespace std;

int main(int argc, char ** argv)
 {
	const string messageStart		= "==========";
	const string entitysetFile	 	= "entityset.xml";
	const string transactionFile	= "transactions.xml";
	int exit_status = 0;

	cout <<	endl	<<	messageStart + " e2e-res started " + messageStart << endl;
	
	cout << messageStart + "Reading settings ..." << endl;
	Settings* settings =  new Settings("./src/settings/settings.cfg");
	
	if(settings->IsTest())
	{
		Test* test = new Test(settings);
		test->run_tests();
		delete test;
		return 0;
	}
	
	settings->ParseCommandLine(argc, argv); /** in case we want to overwrite on some options from .cfg file. */
	cout << *settings;
	

	cout << messageStart + "Creating InputReader object ..." << endl;
	InputReader* iReader = new InputReader(settings->getInputsPath());
	cout << *iReader;
	
	cout << messageStart + "Reading entities ..." << endl;
	vector<Base_Entity*> base_entities = iReader->ReadEntityset(entitysetFile);
	cout <<	"found "	<<	base_entities.size() << " entities: \n";
	
	for(auto basic_ent: base_entities)
	    cout << *basic_ent << endl;
	
	cout << messageStart + "Reading transactions ..." << endl;
	vector<Base_Transaction*> base_transactions = iReader->ReadTransactions(transactionFile);
	cout <<	"found "	<<	base_transactions.size() << " transactions: \n";
	for(unsigned int i=0;i<base_transactions.size();i++)
	{
		base_transactions[i]->Build_transaction(base_entities);
	}	
	
	try
	{
		cout << messageStart + "Creating a constraint model object ... " << endl;
		CP_model* model = new CP_model(base_transactions, settings);	
		
		cout << messageStart + "Creating an execution object ... " << endl;
		Execution<CP_model> execObj(model, settings);
		
		cout << messageStart + "Running the model object ... " << endl;
		execObj.Execute();
		
		cout << messageStart + " e2e-res ended " + messageStart << endl	<< endl;
	}
	catch(std::exception const & e)
	{
		cout << e.what() << endl;
	}
	return exit_status;	  
}
 
