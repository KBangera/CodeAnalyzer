/////////////////////////////////////////////////////////////////////////
// TypeTableAnal.h - Demonstrate the functions to do type table        //
//                   analysis                                          //
// Author: Karthik Bangera                                             //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017   //
/////////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
The module demonstrates the type table analysis.

Build Process:
==============
Required files
- Parser.h
- NoSqlDb.h
- FileSystem.h

Maintenance History:
====================
ver 1.0 : 5 Mar 2017
- demonstrates type table analysis feature.
*/

#include "TypeTableAnal.h"
#include "../Display/Display.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"

using namespace CodeAnalysis;

//fetch the root of the AST tre
ASTNode* TypeTableAnal::getRoot() 
{
	return ASTref_.root();
}

//function to do a dfs walkthrough on the AST tree
void TypeTableAnal::DFS(ASTNode* pNode)
{
	static std::string path = "";
	if (pNode->path_ != path)
	{
		std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
		path = pNode->path_;
	}
	if (doDisplay(pNode))
	{
		std::cout << "\n  " << pNode->name_;
		std::cout << ", " << pNode->type_;
	}
	for (auto pChild : pNode->children_)
		DFS(pChild);
}

//function to create a type table
void TypeTableAnal::createTable(ASTNode* pRoot) {
	for (ASTNode* node : pRoot->children_)
	{
		if (node->children_.size() > 0)
		{
			createTable(node);
		}
		TypeTableDetails details;
		details.type = node->type_;
		details.typeName = node->name_;
		details.path = node->path_;
		details.file = node->package_;
		if (details.type == "using" || details.type == "typedef" || details.type == "enum" || details.type == "struct" || details.type == "class")
		{
			table_.addToTypeTable(details, node->name_);
		}
	}
}

//function that does type analysis on the type table
void TypeTableAnal::doTypeTableAnal()
{
	ASTNode* pRoot = ASTref_.root();
	std::cout << "\nTable type analysis:\n";
	std::cout << "===============================\n";
	std::cout << "\nScan and display AST:\n";
	std::cout << "==================================================\n";
	DFS(pRoot);
	createTable(pRoot);
	std::cout << "\n=======================================\n";
	std::cout << "Output of the Type Analysis:\n";
	std::cout << "=======================================\n";
	ProjectDisplay display;
	display.displayTable(table_.fetchMap());
}

//function that returns the type table
TypeTable TypeTableAnal::fetchTypeTable()
{
	return this->table_;
}

//function that returns the glonal function type table
TypeTable CodeAnalysis::TypeTableAnal::fetchGlobalTable()
{
	return globalFnTable;
}

//function that returns both type table and global function table
std::unordered_map<std::string, TypeTableDetails> CodeAnalysis::TypeTableAnal::fetchBothTables()
{
	std::unordered_map<std::string, TypeTableDetails> gTable = fetchGlobalTable().fetchMap();
	std::unordered_map<std::string, TypeTableDetails> tTable = fetchTypeTable().fetchMap();
	for each (auto table in gTable)
	{
		tTable[table.first] = table.second;
	}
	return tTable;
}

#ifdef TEST_TYPETABLEANAL

int main()
{
	std::cout << "\n\n  Starting type analysis:\n";
	std::cout << "\n  Scanning AST and displaying important things:";
	std::cout << "\n -----------------------------------------------";
	TypeTableAnal ta;
	ASTNode* pRoot = ta.getRoot();
	ta.createTable(pRoot);
	std::cout << "\n\n  Type Analysis:\n\n";
	ta.doTypeTableAnal();
	ta.doGlobalFnAnal();
	return 0;

}
#endif
