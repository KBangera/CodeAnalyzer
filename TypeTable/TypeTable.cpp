////////////////////////////////////////////////////////////////////////////
// TypeTableAnal.cpp - Demonstrate type table addition 					  //
//																		  //
// Author -   Karthik Bangera											  //
////////////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
The module demonstrates how items are added to the type table and a function to return it.


Maintenance History:
====================
ver 1.0 : 5 Mar 2017
- added code to add items in the type table.
- added a function to return the type table.
*/


#include "TypeTable.h"
using namespace CodeAnalysis;

//function for adding type table entries
void TypeTable::addToTypeTable(TypeTableDetails typeDetails, Key typeName)
{
	if (typeTableMap[typeName].type != "")
	{
		if (typeTableMap[typeName].path != typeDetails.path)
		return;
	}
	typeTableMap[typeName] = typeDetails;
}

//function to fetch the type tabla map
CodeAnalysis::TypeTable::MapTypeTable & CodeAnalysis::TypeTable::fetchMap()
{
	return typeTableMap;
}
#if TEST_TYPETABLE
int main()
{
	std::cout << "Type Table Test Stub: \n";
	CodeAnalysis::TypeTableDetails details;
	CodeAnalysis::TypeTable table;
	table.GetTypeMap();
	return 0;
}
#endif // TEST_TYPETABLE
