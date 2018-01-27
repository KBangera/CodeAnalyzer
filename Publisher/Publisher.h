#pragma once
/////////////////////////////////////////////////////////////////////////
// Publisher.h - Demonstrate how to start developing dependency		   //
//               analyzer                                              //
// Author: Karthik Bangera                                             //
/////////////////////////////////////////////////////////////////////////
/*
Package Operations:
==================
The module demonstrates the dependency analysis.

Build Process:
==============
Required files
- DepAnal.h
- Display.h
- FileMgr.h
- ConfigureParser.h
- Parser.h
- Persist.h
- StrongComponents.h
- TypeTableAnal.h

Maintenance History:
====================
ver 1.0 : 1 April 2017
- demonstrates
*/
#include <string>
#include <vector>
#include <unordered_map>
#include <iosfwd>
#include <sstream>
#include "../DependencyAnalyzer/DepAnal.h"
#include "../Display/Display.h"
#include "../FileMgr/FileMgr.h"
#include "../Parser/ConfigureParser.h"
#include "../Parser/Parser.h"
#include "../Persist/Persist.h"
#include "../StrongComponents/StrongComponents.h"
#include "../TypeTableAnal/TypeTableAnal.h"


namespace CodeAnalysis
{

	class Publisher
	{
	private:

		std::unordered_map<Scanner::FileName, std::vector<Scanner::FileName>> dependencyTable;
		std::string folder;
		std::string fetchCssLink();
		std::string fetchDepDiv(std::string file);
		std::string fetchHeadTag();
		std::string fetchJavascriptLink();
		bool ifExists(std::string l, char ch);

	public:
		void createHTML(std::string htmlFile);
		void depTable(std::unordered_map<Scanner::FileName, std::vector<Scanner::FileName>> &dTable);
		std::string htmlPrologue(std::string file);
		void generateHomePage();
		std::string htmlConvert(std::string file, std::string format);
		void replaceHTMLCharacters(std::string htmlFile);
		void saveJavascriptCss();
		void setRepository(std::string html);
		Publisher() {
			std::cout << "\n=================\n";
			std::cout << "Requirement 9:\n";
			std::cout << "=================\n";
			std::cout << "\n All important files of the project are placed in the CodeToBeTested folder(files subfolder)\n";
			std::cout << "\n=================\n";
			std::cout << "Requirement 3:\n";
			std::cout << "=================\n";
			std::cout << "\nWeb Pages created from files in the CodeToBeTested folder using Publisher package and placed in Repository folder\n";
			std::cout << "\n=================\n";
			std::cout << "Requirement 4:\n";
			std::cout << "=================\n";
			std::cout << "\nCreated files have '+' and '-' buttons to expand and collapse\n";
		}
		~Publisher() {

		}
	};

}

