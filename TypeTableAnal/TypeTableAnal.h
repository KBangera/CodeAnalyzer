#pragma once
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

#include "../Parser/ActionsAndRules.h"
#include "../TypeTable/TypeTable.h"
#include <iostream>
#include <functional>

#pragma warning (disable : 4101) 

namespace CodeAnalysis
{
	class TypeTableAnal
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;
		TypeTableAnal();
		void doGlobalFnAnal();
		ASTNode* getRoot();
		void DFS(ASTNode* pNode);
		void createTable(ASTNode* pRoot);
		void doTypeTableAnal();
		TypeTable fetchTypeTable();
		TypeTable fetchGlobalTable();
		std::unordered_map<std::string, TypeTableDetails> fetchBothTables();

	private:
		void extractGlobalFn(ASTNode * globalNode);
		TypeTable table;
		TypeTable globalFnTable;
		AbstrSynTree& ASTref_;
		TypeTable& table_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
	};

	inline TypeTableAnal::TypeTableAnal() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack()),
		toker_(*(Repository::getInstance()->Toker())),
		table_(Repository::getInstance()->table())
	{
		std::function<void()> test = [] { int x; };  
	}                                             

	inline bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {"function", "lambda", "class", "struct", "enum", "alias", "typedef"};
		for (std::string type : toDisplay)
		{
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}

	//inline funcion for analysis of global functions
	inline void TypeTableAnal::doGlobalFnAnal()
	{
		extractGlobalFn(Repository::getInstance()->getGlobalScope());
	}

	//inline function to find the global functions
	inline void TypeTableAnal::extractGlobalFn(ASTNode * gNode)
	{	
		std::cout << "\nGlobal functions are going to be used for dependency analysis\n";
		for (auto node : gNode->children_) {
			if (node->type_ == "function") {
				std::cout << "Global function found : " << node->name_ << "\n";
				TypeTableDetails typeDetails;
				typeDetails.type = node->type_;
				typeDetails.path = node->path_;
				typeDetails.typeName = node->name_;
				typeDetails.file = node->package_;
				if (typeDetails.type != "show" &&typeDetails.type != "operator=" && typeDetails.type != "main" && typeDetails.type != "void" && typeDetails.type != "test" && typeDetails.type != "operator>>" && typeDetails.type != "operator<<")
				{
					globalFnTable.addToTypeTable(typeDetails, typeDetails.type);
				}
			}
		}
	}

}