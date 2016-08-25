/**
 * This is the main file for the e2e-res tool
*/

#include "settings/input_reader.hpp"
#include "execution/execution.cpp"



using namespace Gecode;
using namespace Int;
using namespace std;

int main(int argc, char ** argv)
 {
	const string messageStart		= "==========";
	const string tasksetFile	 	= "taskset.xml";
	const string transactionFile	= "transactions.xml";
	int exit_status = 0;

	cout <<	endl	<<	messageStart + " e2e-res started " + messageStart << endl;
	
	cout << messageStart + "Reading settings ..." << endl;
	Settings* settings =  new Settings("./src/settings/settings.cfg");
	settings->ParseCommandLine(argc, argv); /** in case we want to overwrite on some options from .cfg file. */
	cout << *settings;
	
	cout << messageStart + "Creating InputReader object ..." << endl;
	InputReader* iReader = new InputReader(settings->getInputsPath());
	cout << *iReader;
	
	cout << messageStart + "Reading tasks ..." << endl;
	vector<Task*> tasks = iReader->ReadTaskset(tasksetFile);
	cout <<	"found "	<<	tasks.size() << " tasks: \n";
	/**
	 * Assuming that the "n" nodes are numbered from 0 to "n-1",
	 * we derive the number of nodes from the task nodes.
	 */ 
	int no_nodes = 0;
	for(unsigned int i=0;i<tasks.size();i++)
	{
		cout << *tasks[i]	<< endl;
		if(tasks[i]->node > no_nodes)
			no_nodes = tasks[i]->node;
	}
	
	cout << "The system has " << no_nodes << " nodes\n";
	
	cout << messageStart + "Reading transactions ..." << endl;
	vector<Transaction*> transactions = iReader->ReadTransactions(transactionFile);
	cout <<	"found "	<<	transactions.size() << " transactions: \n";
	for(unsigned int i=0;i<transactions.size();i++)
	{
		cout << *transactions[i]	<< endl;
		transactions[i]->Build_transaction(tasks);
	}	
	
	cout << messageStart + "Creating an application object ... " << endl;
	Application * application = new Application(transactions);
	cout << *application;
	
	cout << messageStart + "Creating a constraint model object ... " << endl;
	CP_model* model = new CP_model(application, settings, no_nodes);	
	
	cout << messageStart + "Creating an execution object ... " << endl;
	Execution<CP_model> execObj(model, settings);
	
	cout << messageStart + "Running the model object ... " << endl;
	execObj.Execute();
	
	cout << messageStart + " e2e-res ended " + messageStart << endl	<< endl;
	return exit_status;	  
}
 
