/////////////////////////////////////////////////////////////////////////
// DepAnal.cpp - Demonstrate how to start developing dependency		   //
//               analyzer                                              //
// Author: Karthik Bangera                                             //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017   //
/////////////////////////////////////////////////////////////////////////
/*
Package Operations:
==================
The module demonstrates the dependency analysis.

Build Process:
==============
Required files
- Parser.h
- Graph.h
- NoSqlDb.h
- itokcollection.h
- ScopeStack.h
- Tokenizer.h
- SemiExp.h
- TypeTableAnal.h
- TypeTable.h

Maintenance History:
====================
ver 1.0 : 10 Mar 2017
- demonstrates dependency analysis feature.
*/
#include "DepAnal.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../Persist/Persist.h"
#include "../TypeTable/TypeTable.h"
#include "../Display/Display.h"


using namespace Scanner;

//function to fetch the dependency table
std::unordered_map<FileName, std::vector<FileName>> & DepAnal::fetchDepTable()
{
	return(depTable);
}

//function to do the dependency analysis
void DepAnal::doDepAnal(std::string file)
{
	std::string fileName= file;
	std::ifstream in(fileName);
	if (!in.good())
	{
		std::cout << "\nCannot open" << fileName << "\n\n";
		return;
	}
	{
		std::cout << "\nAnalysing dependency for :" << file.substr(file.find_last_of("\\") + 1);
		Toker toker;
		toker.returnComments(false);
		toker.attach(&in);
		do
		{
			std::string tok = toker.getTok();
			if (tok != "\n" && tok != "#" && tok != "include" && tok != "<" && tok != "=" && tok != "==" && tok != ">" && tok != "string" && tok != "cout")
			{
				if (isGlobalType(tok))
				{
					std::string ntok = toker.getTok();
					if (ntok != ".")
					{
						if (ntok == "::")
						{
							std::string lFile = fetchFile(tok);
							std::string rFile = file.substr(file.find_last_of("\\") + 1);
							std::string lFile1 = lFile.substr(0, 2);
							std::string rFile1 = rFile.substr(0, 2);

							if (lFile1 == rFile1)
							{
								this->addDep(file.substr(file.find_last_of("\\") + 1), fetchFile(tok));
							}
						}
						this->addDep(fetchFile(tok), file.substr(file.find_last_of("\\") + 1));
					}
				}
			}
		} while (in.good());
	}
}

//function to set the global function table object
void DepAnal::setGlobalObj(CodeAnalysis::TypeTable gTable)
{
	this->globalTable = gTable;
}

//function to set the type table map object
void DepAnal::setMapObject(std::unordered_map<std::string, CodeAnalysis::TypeTableDetails> tTable)
{
	this->typeTable = tTable;
}

//function to set the tokenizer object
void DepAnal::setTokerObj(Scanner::Toker tok)
{
	this->toker = &tok;
}

//function to add the dependency 
void DepAnal::addDep(std::string rFile, std::string lFile)
{
	if (lFile == rFile)
		return;
	for each (auto fileRequired in depTable[lFile])
	{
		if (fileRequired == rFile)
		{
			return;
		}
	}
	depTable[lFile].push_back(rFile);
}

//function to check if the function is a global function
bool DepAnal::isGlobalType(std::string tok)
{
	if (typeTable.find(tok) != typeTable.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

//function to fetch the required file
std::string DepAnal::fetchFile(std::string tok)
{
	return typeTable.find(tok)->second.file;
}

//function to load into the db
void DepAnal::insertInDb()
{
	for each (auto entry in depTable)
	{
		DateTime date;
		NoSQLDB::Element<std::string> element;
		element.name = entry.first;
		element.category = "Depedency Analysis";
		element.data = entry.first;
		element.dateTime = date.now();
		NoSQLDB::Children children;
		for each (auto childEntry in entry.second)
		{
			children.push_back(childEntry);
		}
		element.children = children;
		db.saveRecord(entry.first, element);
	}
}

//function to save the dependency analysis in xml
void DepAnal::saveToDBInXml(std::string fileName)
{
	NoSQLDB::Persist<std::string> persist(db);
	persist.saveToFile(fileName);
}

//function to perform DFS on the dependency table
void DepAnal::depDFS(std::string r, std::string l)
{
	auto it = st.begin();
	for (it = st.begin(); it != st.end(); it++)
	{
		if (l == *it)
		{
			break;
		}
	}
	if (it != st.end())
	{
		return;
	}
	st.push_back(l);
	for each (auto entry in depTable)
	{
		if (entry.first == l)
		{
			std::cout << entry.first << "\n";
			if (entry.first == r)
			{
				std::cout << "\nfound" << r << "\n";
				return;
			}
			for each (auto file in entry.second)
			{
				depDFS(r, file);
			}
		}
	}

}

//function to check if the text exists in the line
bool DepAnal::ifExists(std::string line, std::string text)
{
	if (line.find(text) == line.npos)
	{
		return false;
	}
	else
	{
		return true;
	}
}

// function to correct the existing dependency for all the files
void DepAnal::dependencyAnalysis(std::vector<std::string> files, std::string file)
{
	std::string line;
	std::ifstream isObj(file);
	while (std::getline(isObj, line))
	{
		if (ifExists(line, "#include"))
		{
			for each (std::string fName in files)
			{
				FileSystem::FileInfo fileInfo(file);
				std::string lFileName = fileInfo.name();
				FileSystem::FileInfo fileDetails(fName);
				std::string rFileName = fileDetails.name();
				if (ifExists(line, rFileName))
				{
					addDep(rFileName, lFileName);
				}
			}
		}
	}
}

#if TEST_DEPANALYSIS
int main()
{
	std::cout << "Dependency Analysis test stub\n";
	DepAnal da;
	
}
#endif // TEST_DEPANALYSIS

