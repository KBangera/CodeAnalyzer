#pragma once
/////////////////////////////////////////////////////////////////////////
// DepAnal.h - Demonstrate how to start developing dependency analyzer //
//                                                                     //
// Author: Karthik Bangera                                             //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017   //
/////////////////////////////////////////////////////////////////////////
/*
Package Operations:
==================
The module demonstrates the dependency analysis.

Build Process:
==============
Required files
- Parser.h
- Graph.h
- NoSqlDb.h
- itokcollection.h
- ScopeStack.h 
- Tokenizer.h
- SemiExp.h
- TypeTableAnal.h
- TypeTable.h

Maintenance History:
====================
ver 1.0 : 10 Mar 2017
- demonstrates dependency analysis feature.
*/
#include <unordered_map>
#include <set>
#include <iostream>
#include <string>
#include <vector>

#include "../Parser/Parser.h"
#include "../Analyzer/Executive.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../TypeTableAnal/TypeTableAnal.h"
#include "../TypeTable/TypeTable.h"
#include "../Graph/Graph.h"
#include "../NoSqlDb/NoSqlDb.h"

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

namespace Scanner
{
	using FileName = std::string;
	class DepAnal
	{

	public:
		DepAnal()
		{
			toker = nullptr;
		}
		~DepAnal() 
		{

		}

		std::unordered_map<FileName, std::vector<FileName>>& fetchDepTable();
		void doDepAnal(std::string file);
		void setGlobalObj(CodeAnalysis::TypeTable gTable);
		void setTokerObj(Scanner::Toker tok);
		void addDep(std::string rFile, std::string lFile);
		bool isGlobalType(std::string);
		std::string fetchFile(std::string tok);
		void insertInDb();
		void saveToDBInXml(std::string fileName);
		void setMapObject(std::unordered_map<std::string, CodeAnalysis::TypeTableDetails> tTable);
		void depDFS(std::string r, std::string l);
		std::vector<std::string> st;
		bool ifExists(std::string line, std::string text);
		void dependencyAnalysis(std::vector<std::string> allFiles, std::string file);
		
	private:
		NoSQLDB::NoSqlDb<std::string> db;
		Scanner::Toker* toker;
		CodeAnalysis::TypeTable globalTable;
		std::unordered_map<std::string, CodeAnalysis::TypeTableDetails> typeTable;
		std::unordered_map<FileName, std::vector<FileName>> depTable;

	};
}