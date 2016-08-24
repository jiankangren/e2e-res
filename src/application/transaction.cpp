#include "transaction.hpp"

using namespace std;

Transaction::Transaction(){};

Transaction::Transaction(vector<char*> elements, vector<char*> values):
id(0)
{
	for(unsigned int i=0;i< elements.size();i++)
	{
		try 
		{			
			if(strcmp(elements[i], "task") == 0)
				task_ids.push_back(atoi(values[i]));	
			if(strcmp(elements[i], "id") == 0)
				id = atoi(values[i]);				
		}				
		catch(std::exception const & e)
		{
			 cout << "reading transactions xml file error : " << e.what() << endl;
		}
	}
};
void Transaction::Build_transaction(vector<Task*> all_tasks)
{
	if(task_ids.size() == 0)
	{
		cout << "The transaction is empty \n";
		return;
	}
	for(unsigned j=0;j<task_ids.size();j++)
	{
		bool task_j_found = false;
		for(unsigned i=0;i<all_tasks.size();i++)
		{
			if(all_tasks[i]->id == task_ids[j])
			{
				tasks.push_back(all_tasks[i]);
				task_j_found = true;
				break;
			}
		}
		if(!task_j_found)
		{
			cout << "error: task " << task_ids[j] << " is not defined\n";
			return;
		}
	}
}
std::ostream& operator<< (std::ostream &out, const Transaction &trans)
{
	out << " Transaction: [" 		<< trans.id 		<< "]: ";
	for(unsigned int i=0;i<trans.task_ids.size();i++)
	{
		out << trans.task_ids[i];
		if(i+1 < trans.task_ids.size())
			out << " -> ";
	}	 
	return out;
}
