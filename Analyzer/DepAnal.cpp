// DepAnal

#include "DepAnal.h"
#include "../FileSystem/FileSystem.h"
#include "../Parser/Parser.h"
#include <vector>
#include <sstream>
#include "../NoSqlDb/NoSqlDb.h"
#include "../Serialize/Convert.h"

using namespace CodeAnalysis;

void Scanner::TypeAnal::saveDepAnalNoSql(Graph<std::string, std::string>& graph, std::string dependencyFileName) {
	Graph<std::string, std::string>::iterator iter = graph.begin();
	std::cout << "\n\n\nCreating NoSql Database to store dependency analysis" <<
		" with int DB to store vertex id's into the Data field and key for vertex(node) values.\n\n";
	//NoSqlDb<int> depAnalDB(dependencyFileName);
	NoSqlDb<int> depAnalDB;
	//depAnalDB.turnOffAutoSave();
	while (iter != graph.end())
	{
		Element<int> element;
		Vertex<std::string, std::string> v = *iter;
		element.category = "DepAnal";
		element.data = (int)v.id();
		element.name = (v.value()).c_str();
		std::stringstream stream;
		stream << "Node for file " << (v.value()).c_str() << " and Data stores Vertex ID";
		element.description = stream.str();
		time_t time = std::time(0);
		element.timeDate = Convert<time_t>::toString(time);
		std::vector<std::string> childKeys;
		for (size_t i = 0; i<v.size(); ++i)
		{
			Vertex<std::string, std::string>::Edge edge = v[i];
			childKeys.push_back(graph[edge.first].value().c_str());
		}
		element.children = childKeys;
		//depAnalDB.saveDBElement(element.name, element);
		depAnalDB.save(element.name, element);
		++iter;
	}
	depAnalDB.saveDB(false);
}



void Scanner::TypeAnal::saveStrongCompToDb(std::list<std::vector<std::string>> scList, std::string strongComponentFileName) {
	std::cout << "\n\n\nCreating NoSql Database to store Stong Components" <<
		" with int DB to store vertex id's into the Data field and key for vertex(node) values.\n\n";
	NoSqlDb<int> stronCompDB(strongComponentFileName);
	stronCompDB.turnOffAutoSave();
	int counter = 0;
	for (std::vector<std::string> scVector : scList) {
		Element<int> element;
		counter++;
		element.category = "StrongComp";
		element.data = counter;
		std::stringstream stream;
		stream << "StrongComp:" << counter;
		element.name = stream.str();
		stream << " , data is just an unique identifier and children shows strongly connected components";
		element.textDescription = stream.str();
		time_t time = std::time(0);
		element.timeDate = Convert<time_t>::toString(time);
		std::vector<std::string> childKeys;
		for (std::string vertex : scVector) {
			childKeys.push_back(vertex);
		}
		element.children = childKeys;
		stronCompDB.saveDBElement(element.name, element);
	}
	stronCompDB.saveDB(false);
}


void DepAnal::displayGraphWithDependencies(std::string dependencyFileName)
{
	std::cout << "\n\n\t\t\t\tDisplaying the dependency Ananlysis\n";
	std::cout << "    =========================================================================================";
	depDisplay_.show(depGraph_);
	saveDepAnalNoSql(depGraph_, dependencyFileName);
}

void DepAnal::addVerticesToGraph()
{
	static int i = 0;
	for (auto item : fileMap_)
	{
		for (auto file : item.second)
		{
			Vertex<std::string, std::string> vert(file.substr(file.find_last_of("\\") + 1), i);
			if (depGraph_.addVertex(vert) == true)
			{
				i++;
			}
		}
	}
}

void DepAnal::addDependencyToGraph(const std::string &fileName1_, const std::string &fileName2_)
{
	static int i = 0;
	int index1 = -1;
	int index2 = -1;
	for (size_t j = 0; j < depGraph_.size(); j++)
	{
		Vertex<std::string, std::string>& v1 = depGraph_[j];
		if (v1.value() == fileName1_)
		{
			index1 = (int)j;
		}
		if (v1.value() == fileName2_)
		{
			index2 = (int)j;
		}
	}

	std::stringstream stream;
	stream.str("");
	stream << "edge" << i;
	if ((index1 != -1) && (index2 != -1))
	{
		if (depGraph_.addEdge(stream.str(), depGraph_[index1], depGraph_[index2]) == true)
		{
			i++;
		}
	}
}


/*
* Arguments are:
* - path: possibly relative path to folder containing all analyzed code,
*   e.g., may be anywhere in the directory tree rooted at that path
* - patterns: one or more file patterns of the form *.h, *.cpp, and *.cs
* - options: /m (show metrics), /s (show file sizes), and /a (show AST)
*/


bool DepAnal::Attach(const std::string& name)
{
	if (pToker == 0)
		return false;
	if (pIn != nullptr)
	{
		pIn->close();
		delete pIn;
	}
	pIn = new std::ifstream(name);
	if (!pIn->good())
		return false;

	// check for Byte Order Mark (BOM)
	char a, b, c;
	a = pIn->get();
	b = pIn->get();
	c = pIn->get();
	if (a != (char)0xEF || b != (char)0xBB || c != (char)0xBF)
		pIn->seekg(0);

	if (!pIn->good())
	{
		// empty file, but openable, so return true, avoids error message
		return true;
	}
	return pToker->attach(pIn);
}

bool DepAnal::ProcessCommandLine(int argc, char* argv[])
{
	if (argc < 2)
	{
		return false;
	}
	try {
		path_ = FileSystem::Path::getFullFileSpec(argv[1]);
		if (!FileSystem::Directory::exists(path_))
		{
			std::cout << "\n\n  path \"" << path_ << "\" does not exist\n\n";
			return false;
		}
		for (int i = 2; i < argc; ++i)
		{
			if (argv[i][0] == '/')
				options_.push_back(argv[i][1]);
			else
				patterns_.push_back(argv[i]);
		}
		if (patterns_.size() == 0)
		{
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  command line argument parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}

void DepAnal::getSourceFiles()
{
	AnalFileMgr fm(path_, fileMap_);
	for (auto patt : patterns_)
		fm.addPattern(patt);
	fm.search();
}

void DepAnal::processSourceCode()
{
	addVerticesToGraph();
	for (auto item : fileMap_)
	{
		for (auto file : item.second)
		{
			Attach(file);
			std::string fileName_;
			if (file.find_last_of("\\") < file.length())
			{
				fileName_ = file.substr(file.find_last_of("\\") + 1);
			}
			while (pParser->next())
			{
				std::vector<std::string>::iterator it = pSemi->begin();
				for (it; it != pSemi->end(); it++)
				{
					/* check if the token is present in the TypeTable */
					if (typeTable_.getTypeTableMap().find(*it) != typeTable_.getTypeTableMap().end())
					{
						//change here for full path
						if (fileName_ != typeTable_.getTypeTableMap().find(*it)->second.fileName)
						{
							if (dependMap_[fileName_].insert(typeTable_.getTypeTableMap().find(*it)->second.fileName).second
								!= false)
							{
								//change here for full path
								std::string fileName2_ = typeTable_.getTypeTableMap().find(*it)->second.fileName;
								//	std::cout << "File: " << fileName_ << "  depends on: " << typeTable_.getTypeTable().find(*it)->second << "\n";
								addDependencyToGraph(fileName_, fileName2_);
							}
						}
					}
				}
			}
		}
	}
}


#ifdef TEST_DEPANAL

int main()
{

}
#endif
