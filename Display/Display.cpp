#pragma once
////////////////////////////////////////////////////////////////////////////
// Display.cpp - Displays the data based on the analysis 			      //
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
-Added the code to display the type table
*/

#include <iostream>
#include "Display.h"


using namespace CodeAnalysis;

//function to display the type table
void ProjectDisplay::displayTable(MapTypeTable typeTable) {
	//std::cout << "\n=================\n";
	//std::cout << "Requirement 4:\n";
	//std::cout << "=================\n";
	std::cout << "\n=================\n";
	std::cout << "Type Table:\n";
	std::cout << "=================\n";
	std::cout << std::setw(25) << "Type";
	std::cout << std::setw(25) << "Type Name";
	std::cout << std::setw(25) << "File Name                \n";
	for (auto row : typeTable) 
	{
		std::cout << std::setw(25) << row.first;
		std::cout << std::setw(25) << row.second.type;
		std::cout << std::setw(25) << row.second.file << "\n";
	}
}

//function to display the strong components
void ProjectDisplay::displayStrongComponents(std::vector<std::vector<std::string>>& vectorStrongComp)
{
	std::cout << "\n======================\n";
	std::cout << "Strong Components:\n";
	std::cout << "======================\n";
	int SCSCounter = 0;
	for each (auto var in vectorStrongComp)
	{
		std::cout << "\nStrong Component :" << ++SCSCounter;
		for each (std::string file in var)
			std::cout << "\n" << file;
	}
}


#if TEST_DISPLAY
int main()
{

}
#endif