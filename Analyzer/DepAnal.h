/*#pragma once


#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>
#include "../Parser/Parser.h"
#include "../Analyzer/Executive.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../TypeTableAnal/TypeTableAnal.h"
#include "../Graph/Graph.h"

#pragma warning (disable : 4101)  // disable warning re unused variable x, below

namespace Scanner
{
  class TypeAnal
  {
  public:
	  using Pattern = std::string;
	  using File = std::string;
	  using Files = std::vector<File>;

	  using FileMap = std::unordered_map<Pattern, Files>;
	  using DependMap = std::unordered_map<std::string, std::set<std::string>>;
	  
	  using Path = std::string;
	  using Patterns = std::vector<Pattern>;	 
	  using Ext = std::string;
	  using Options = std::vector<char>;
	 
	  TypeAnal::TypeAnal() : typeTable_(CodeAnalysis::Repository::getInstance()->table()),
		  depGraph_(CodeAnalysis::Repository::getInstance()->depGraph())
	  {
		  pToker = new Toker;
		  pToker->returnComments(false);
		  pSemi = new SemiExp(pToker);
		  pParser = new CodeAnalysis::Parser(pSemi);		  
	  }

	  TypeAnal::~TypeAnal()
	  {
		  delete pParser;
		  delete pSemi;
		  delete pToker;
		  if (pIn != nullptr)
			  pIn->close();
		  delete pIn;
	  }
	 
	  void fetchSources();
	  bool processCommandLine(int argc, char* argv[]);
	  void processSources();
	  bool Append(const std::string& name);
	  void addDependencyGraph(const std::string &fileName1_, const std::string &fileName2_);
	  void addVertices();
	  void displayGraph(std::string dependencyFileName);

	  GraphLib::Graph<std::string, std::string>& getDependGraph() 
	  { 
		  return depGraph_; 
	  }
	  void saveStrongCompToDb(std::list<std::vector<std::string>> list, std::string strongComponent);

  private:
	  Scanner::Toker* pToker;
	  Scanner::SemiExp* pSemi;
	  CodeAnalysis::Parser* pParser;
	  CodeAnalysis::TypeTable &typeTable_;

	  Path path_;
	  Patterns patterns_;
	  Options options_;
	  FileMap fileMap_;
	  std::ifstream* pIn;  
	  DependMap dependMap_;

	  CodeAnalysis::Graph<std::string, std::string> &depGraph_;
	  CodeAnalysis::Display<std::string, std::string> depDisplay_;
	  void saveDepAnalNoSql(CodeAnalysis::Graph<std::string, std::string>& graph, std::string dependencyFileName);
  };
}*/