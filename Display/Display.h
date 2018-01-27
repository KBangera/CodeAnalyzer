#pragma once
////////////////////////////////////////////////////////////////////////////
// Display.h - Displays the data based on the analysis 					  //
//																		  //
// Author - Karthik Bangera												  //
////////////////////////////////////////////////////////////////////////////

/*
Module Operations:
==================
The module provides display of data based on the analysis performed as per the analysis

Maintenance History:
====================
ver 1.0 : 5 Mar 2017
- 
*/


#include <iomanip>
#include <unordered_map>
#include "../TypeTable/TypeTable.h"
#include "../DependencyAnalyzer/DepAnal.h"
#include "../NoSqlDb/NoSqlDb.h"

namespace CodeAnalysis
{
	class ProjectDisplay
	{
	public:
		using Key = std::string;
		using MapTypeTable = std::unordered_map<Key, TypeTableDetails>;
		void displayTable(MapTypeTable typeTableMap);
		void displayStrongComponents(std::vector<std::vector<std::string>>& vectorStrongComp);
		ProjectDisplay() {};
		~ProjectDisplay() {};
	};
}


