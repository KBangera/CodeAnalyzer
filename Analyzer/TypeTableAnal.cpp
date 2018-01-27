////////////////////////////////////////////////////////////////////////////
// TypeTableAnal.cpp - Demonstrate type table analysis					  //
//																		  //
// Author -   Karthik Bangera											  //
////////////////////////////////////////////////////////////////////////////

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

ASTNode* TypeTableAnal::getRoot() {
	return ASTref_.root();
}

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

void TypeTableAnal::createTable(ASTNode* pRoot) {
	for (ASTNode* node : pRoot->children_)
	{
		if (node->children_.size() > 0)
		{
			createTable(node);
		}
		TypeTableDetails details;
		details.type = node->type_;
		details.path = node->path_;
		details.file = node->package_;
		if (details.type == "using" || details.type == "typedef" || details.type == "enum" || details.type == "struct" || details.type == "class")
		{
			table_.addToTypeTable(node->name_, details);
		}
		if (node->parentType_ != "class" && details.type == "function")
		{
			table_.addToTypeTable(node->name_, details);
		}
	}
}

void TypeTableAnal::doTypeTableAnal()
{
	ASTNode* pRoot = ASTref_.root();
	std::cout << "\nTable type analysis:\n";
	std::cout << "===============================\n";
	std::cout << "\nScan and display AST:\n";
	std::cout << "==================================================\n";
	DFS(pRoot);
	createTable(pRoot);
	std::cout << "\nOutput of the Type Analysis:\n";
	std::cout << "==================================================\n";
	ProjectDisplay display;
	display.displayTable(table_.fetchMap());
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
	ta.doTypeAnal();
}
#endif
