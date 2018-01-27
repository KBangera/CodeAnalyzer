#pragma once
////////////////////////////////////////////////////////////////////////////
// StrongComponent.h - Tarjan Algorithm to find Strong Components	      //
//																		  //
// Author - Karthik Bangera												  //
////////////////////////////////////////////////////////////////////////////

/*
Module Operations:
==================
This module provides the declarations for strong components analysis.

Build Process:
==============
Required files
- Graph.h, DepAnal.h

Maintenance History:
====================
ver 1.0 : 12 Mar 2017
- added strong components analysis feature.
*/

#include "../Graph/Graph.h"
#include "../DependencyAnalyzer/DepAnal.h"
#include <algorithm>
#include <vector>
#include <sstream>
#define min(a,b)            (((a) < (b)) ? (a) : (b))
namespace CodeAnalysis
{
	//Graph properties
	typedef GraphLib::Vertex<std::string, std::string> vertex;
	typedef GraphLib::Graph<std::string, std::string> graph;
	typedef GraphLib::DisplayGraph<std::string, std::string> display;

	class StrongComponents
	{
	public:
		StrongComponents(Scanner::DepAnal& da);
		void analyzeStrongComponents();
		std::vector<std::vector<std::string>>& fetchStrongComponents();
		vertex* fetchVtx(std::string file);

		StrongComponents() {};
		~StrongComponents() {};
	private:
		graph g;
		
		std::vector<std::vector<std::string>> SCVector;
		size_t vertexTotal;
		void loadGraphDependencyObj(Scanner::DepAnal& da);
		void graphOutput();
		bool checkVtxExists(std::string file);
		void StrongComp(int u, int disc[], int low[], std::stack<int> *st, bool stackMember[]);
	};

}