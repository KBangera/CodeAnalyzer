#pragma once
/////////////////////////////////////////////////////////////////////
//  TypeTable.h - This file acts as a for a type table			   //
//  Author:        Karthik Bangera								   //
/////////////////////////////////////////////////////////////////////

/*
Package Operations:
==================
This module provides the type table to store the analysis data from the AST tree.

Maintenance History:
====================
ver 1.0 : 5 Mar 2017
- Type table package created.
*/

#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

namespace CodeAnalysis
{
	class TypeTableDetails {
			
		using FileName = std::string;
		using TypeName = std::string;
		using Path = std::string;
		public:
		std::string type;
		FileName file;
		TypeName typeName;
		Path path;		
	};

	class TypeTable {
	
	public:
		using Key = std::string;
		using MapTypeTable = std::unordered_map<Key, TypeTableDetails>;
		void addToTypeTable(TypeTableDetails value, Key key);
		MapTypeTable& fetchMap();

	private:
		MapTypeTable typeTableMap;
	};
}