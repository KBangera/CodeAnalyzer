////////////////////////////////////////////////////////////////////////////
// StrongComponent.cpp - Tarjan Algorithm to find Strong Components	      //
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

#include "StrongComponents.h"
#include "../Graph/Graph.h"
#include "../DependencyAnalyzer/DepAnal.h"
#include <algorithm>
#include <vector>
#include <sstream>

using namespace CodeAnalysis;

// function to the graph dependency object
void StrongComponents::loadGraphDependencyObj(Scanner::DepAnal& da)
{
	int vertexNumber = 0;
	int edgeRCount = 0;
	int edgeLCount = 0;
	for each (auto depItem in da.fetchDepTable())
	{
		GraphLib::Vertex<std::string, std::string> vtx(depItem.first);
		g.addVertex(vtx);
	}
	for each (auto depEntry in da.fetchDepTable())
	{
		edgeLCount++;
		for each (std::string file in depEntry.second)
		{
			edgeRCount++;
			std::string edgeNumber = std::to_string(edgeLCount) + std::to_string(edgeRCount);
			if (!checkVtxExists(file))
			{
				GraphLib::Vertex<std::string, std::string> vtx(file);
				g.addVertex(vtx);
			}
			g.addEdge(edgeNumber, *fetchVtx(depEntry.first), *fetchVtx(file));
		}
	}
	vertexTotal = g.size();
	analyzeStrongComponents();
}

//Strong components constructor
CodeAnalysis::StrongComponents::StrongComponents(Scanner::DepAnal& da)
{
	loadGraphDependencyObj(da);
	graphOutput();
}

//Analysis of strong components
void CodeAnalysis::StrongComponents::analyzeStrongComponents()
{
	int *disc = new int[vertexTotal];
	int *low = new int[vertexTotal];
	bool *stackMember = new bool[vertexTotal];
	std::stack<int> *st = new std::stack<int>();
	for (int i = 0; i < vertexTotal; i++)
	{
		disc[i] = -1;
		low[i] = -1;
		stackMember[i] = false;
	}
	for (int i = 0; i < vertexTotal; i++)
	{
		if (disc[i] == -1)
			StrongComp(i, disc, low, st, stackMember);
	}
}

//function to fetch strong components
std::vector<std::vector<std::string>>& CodeAnalysis::StrongComponents::fetchStrongComponents()
{
	return SCVector;
}

//fetch the vertex of the graph
vertex* CodeAnalysis::StrongComponents::fetchVtx(std::string file)
{

	for (graph::iterator it = g.begin(); it != g.end(); it++)
	{
		if (it->value() == file)
		{
			return &(*it);
		}
	}
	return &(*g.end());
}

//function to check if the vertex exists
bool CodeAnalysis::StrongComponents::checkVtxExists(std::string file)
{
	for (graph::iterator it = g.begin(); it != g.end(); it++)
	{
		if (it->value() == file)
		{
			return true;
		}
	}
	return false;
}

// A recursive function that finds and prints strongly connected
// components using DFS traversal
// u --> The vertex to be visited next
// disc[] --> Stores discovery times of visited vertices
// low[] -- >> earliest visited vertex (the vertex with minimum
//             discovery time) that can be reached from subtree
//             rooted with current vertex
// *st -- >> To store all the connected ancestors (could be part
//           of SCC)
// stackMember[] --> bit/index array for faster check whether
//                  a node is in stack
void CodeAnalysis::StrongComponents::StrongComp(int u, int disc[], int low[], std::stack<int> *st, bool stackMember[])
{ // A static variable is used for simplicity, we can avoid use of static variable by passing a pointer.
	static int time = 0;// Initialize discovery time and low value
	disc[u] = low[u] = ++time;
	st->push(u);
	stackMember[u] = true;
	GraphLib::Vertex<std::string, std::string> &vert = g[u];
	for (size_t i = 0; i < vert.size(); i++)
	{
		GraphLib::Vertex<std::string, std::string>::Edge e1 = vert[i];
		GraphLib::Vertex<std::string, std::string> &vert2 = g[e1.first];
		for (size_t j = 0; j < g.size(); j++)
		{
			GraphLib::Vertex<std::string, std::string> &v1 = g[j];
			if (v1.value() == vert2.value())
			{ // If v1 is not visited yet, then recur for it
				if (disc[v1.id()] == -1)
				{
					StrongComp((int)v1.id(), disc, low, st, stackMember);
					low[u] = min(low[u], low[v1.id()]);
				}
				else if (stackMember[v1.id()] == true)
					low[u] = min(low[u], disc[v1.id()]);
				break;
			}
		}
	}
	int w = 0;  // head node found, pop the stack and print an SCC To store stack extracted vertices
	std::vector<std::string> scVector;
	if (low[u] == disc[u])
	{
		std::cout << "\n\n";
		while (st->top() != u)
		{
			w = (int)st->top();
			GraphLib::Vertex<std::string, std::string>& vert = g[w];
			scVector.push_back(vert.value());
			stackMember[w] = false;
			st->pop();
		}
		w = (int)st->top();
		GraphLib::Vertex<std::string, std::string> &vert = g[w];
		scVector.push_back(vert.value());
		stackMember[w] = false;
		st->pop();
		SCVector.push_back(scVector);
	}
}

//function to display the graph
void CodeAnalysis::StrongComponents::graphOutput()
{
	std::cout << "==========================================\n";
	std::cout << "Dependency analysis in the form of graph:\n";
	std::cout << "==========================================\n";
	display disp;
	disp.show(g);
}

#ifdef  TEST_STRONGCOMP
int main() {

	StrongComponents sComp;
	sComp.analyzeStrongComponents();
	return 0;


}
#endif // TEST_STRONGCOMP