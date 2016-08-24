#pragma once
#include <math.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <cassert>
#include <sstream>

#include <libxml/xmlreader.h>
#include "../application/transaction.hpp"


#include <libxml/parser.h>

#include <sdf3/base/xml/xml.h>
#include <sdf3/base/string/cstring.h>
#include <sdf3/base/exception/exception.h>

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


namespace fs = boost::filesystem;

using namespace std;

class InputReader
{
public:
	InputReader(std::string _inputsPath);;
	/**
	 * reads a file named taskset.xml from the input directory and
	 * returns a TaskSet object
	 */ 
	vector<Task*> ReadTaskset(const string fileName);
	/**
	 * reads a file named transactions.xml from the input directory and
	 * returns a vector of Transactions
	 */ 
	vector<Transaction*> ReadTransactions(const string fileName);
	/**
	 * reads csv files and 
	 * returns a two dimentional vector of string
	 */ 
	vector<vector<string>> ReadCSV(string file);
	
		
	friend std::ostream& operator<< (std::ostream &out, const InputReader &inputReader);
	
private:
	enum 						ReadingModes{TASK, TRANSACTION, PLATFORM, WCET, CONSTRAINTS};
	ReadingModes				xmlReadingMode;
	string 						inputsPath;
	vector<Task*> 				tasks;
	vector<Transaction*> 		transactions;
	
	void 	walk_tree( xmlNodePtr node );
	int 	count_element_children( xmlNodePtr node );
	void 	parsTaskXMLNode(vector<char*> elements, vector<char*> values, xmlNodePtr node);
	void 	parsTransXMLNode(vector<char*> elements, vector<char*> values, xmlNodePtr node);
	void 	readXML(string filePath);
	char* 	getValueOfElement(vector<char*> elements, vector<char*> values, char* inElement);
	char* 	getValueOfElement(xmlNodePtr node, char* inElement);
	
};


