#include "input_reader.hpp"

using namespace std;
InputReader::InputReader(std::string _inputsPath):
inputsPath(_inputsPath)
{
};
int InputReader::count_element_children( xmlNodePtr node )
{
  int n = 0;
  if ( !node || !node->children ) return n;
  xmlNodePtr child = node->children;
  for( xmlNodePtr curr = child ; curr ; curr = curr->next )
    if ( curr->type == XML_ELEMENT_NODE )
      n++;
  return n;
}

	void InputReader::walk_tree( xmlNodePtr node )
	{
		vector<char*> elements;
		vector<char*> values;
		if (!node) 
			return;
		for( xmlNodePtr curr = node ; curr ; curr = curr->next )
		{
			if ( curr->type == XML_ELEMENT_NODE )
			{
				char* currElement = (char *) curr->name;
				int nchild = count_element_children(curr);
				if ( curr->children && nchild == 0 )
				{
					char* currValue = (char *) curr->children->content ;
					elements.push_back(currElement);
					values.push_back(currValue);
				}				
			}
			if ( curr->children )
				walk_tree( curr->children );
		}
		switch(xmlReadingMode)
		{
			case(TASK):
				parsTaskXMLNode(elements, values, node);
				break;
			case(TRANSACTION):
				parsTransXMLNode(elements, values, node);
				break;
			default:
				break;
		}
		
	}
	
	void InputReader::parsTaskXMLNode(vector<char*> elements, vector<char*> values, xmlNodePtr node)
	{
		const xmlChar* taskNodeName = (xmlChar*) "periodicTask";
		/**
		 * if the node is a pr task, then create one and push it in the tasks vactor
		 */ 
		if ( node->parent && xmlStrEqual(node->parent->name, taskNodeName) == 1)
		{
			int taskNumber = atoi(getValueOfElement(elements, values, (char *)"number"));
			for(int i=0;i<taskNumber;i++)
				tasks.push_back(new Task(elements, values, i));
			return;
		}
	}
	
	void InputReader::parsTransXMLNode(vector<char*> elements, vector<char*> values, xmlNodePtr node)
	{
		const xmlChar* transNodeName = (xmlChar*) "transaction";
		/**
		 * if the node is a transaction, then create one and push it in the transaction vactor
		 */ 
		if ( node->parent && xmlStrEqual(node->parent->name, transNodeName) == 1)
		{
			transactions.push_back(new Transaction(elements, values));
			return;
		}
	}
	
	char* InputReader::getValueOfElement(xmlNodePtr node, char* inElement)
	{
		if (!node) 
			return '\0';
		/// go to the begining of the node
		while( node->prev )
			node = node->prev;
		for( xmlNodePtr curr = node ; curr ; curr = curr->next )
		{
			if ( curr->type == XML_ELEMENT_NODE )
			{
				char* currElement = (char *) curr->name;
				if ( curr->children && strcmp(currElement, inElement) == 0)
				{
					return (char *) curr->children->content;
				}				
			}
		}
		return '\0';
	}
	
	
	char* InputReader::getValueOfElement(vector<char*> elements, vector<char*> values, char* inElement)
	{
		for(unsigned int i=0; i<elements.size();i++)
		{
			if(strcmp(elements[i], inElement) == 0)
			{
				return values[i];
			}
		}
		
		return '\0';
	}
	
	vector<Task*> InputReader::ReadTaskset(const string fileName)
	{
		
		string filePath = inputsPath;
		filePath += fileName;
		xmlReadingMode = TASK;
		readXML(filePath);
		
		return tasks;
	}
	
	vector<Transaction*> InputReader::ReadTransactions(const string fileName)
	{
		
		string filePath = inputsPath;
		filePath += fileName;
		xmlReadingMode = TRANSACTION;
		readXML(filePath);
		
		return transactions;
	}
	
	void InputReader::readXML(string filePath)
	{
		
		char myArray[filePath.size()+1];///Converting to char array
		strcpy(myArray, filePath.c_str());
		
		xmlDocPtr doc;
		doc = xmlParseFile(myArray);
		if (doc == NULL) 
		{
			fprintf(stderr, "Failed to parse document: ");
			cout << filePath << endl;
			return;
		}
		xmlNodePtr root_node = xmlDocGetRootElement(doc);

		walk_tree(root_node);
		
		xmlFreeDoc(doc);
		
		return;
	}
	std::ostream& operator<< (std::ostream &out, const InputReader &inputReader)
	{
		out << "inputsPath:" << inputReader.inputsPath 		<< endl;
	 
		return out;
	}

vector<vector<string>> InputReader::ReadCSV(string filePath)
{
	char myArray[filePath.size()+1];///Converting to char array
	strcpy(myArray, filePath.c_str());
	
	ifstream file(myArray);
	string line;
	const char sep = ',';
	vector<vector<string>> table;
	while(getline(file,line))
	{
		vector<string> row;

		stringstream lineStream(line);
		string cell;

		while(getline(lineStream, cell, sep))
		{
			row.push_back(cell);		  
		}
		
		table.push_back(row);
	}
	cout << "read "	<<	filePath	<<	endl;
	return table;
}
