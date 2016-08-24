/**
 * This is the main file for the e2e-res tool
*/

#include "settings/settings.hpp"
#include "settings/input_reader.hpp"
#include <gecode/int.hh>
#include <gecode/set.hh>
#include <gecode/gist.hh>
#include <gecode/minimodel.hh>




using namespace Gecode;
using namespace Int;
using namespace std;

int main(int argc, char ** argv)
 {
	const string messageStart	= "==========";
	const string tasksetFile 	= "taskset.xml";
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
	for(unsigned int i=0;i<tasks.size();i++)
		cout << *tasks[i]	<< endl;
	
	cout << messageStart + " e2e-res ended " + messageStart << endl	<< endl;
	return exit_status;	  
}
 
