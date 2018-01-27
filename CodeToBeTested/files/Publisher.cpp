/////////////////////////////////////////////////////////////////////////
// Publisher.cpp - Publishes the input files with dependencies         //
// Author: Karthik Bangera                                             //
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017   //
/////////////////////////////////////////////////////////////////////////
#include "Publisher.h"

//function to fetch the css files
std::string CodeAnalysis::Publisher::fetchCssLink()
{
	std::stringstream ssObj;
	std::string cssPath = FileSystem::Path::getFullFileSpec("Css");
	std::vector<std::string> cssFiles = FileSystem::Directory::getFiles(cssPath, "*.css");
	ssObj.clear();
	for each (std::string cssFile in cssFiles)
	{
		std::string cssLink = "\n<link rel = \"stylesheet\" href =\"" + std::string("Css") + "\\" + cssFile + "\">" + "</link>";
		ssObj << cssLink;
	}
	return ssObj.str();
}

//function to fetch the dependency file analysis list for the particular selected file in their repective html list
std::string CodeAnalysis::Publisher::fetchDepDiv(std::string file)
{
	std::stringstream ssObj;
	ssObj.clear();
	std::string fileName = FileSystem::Path::getName(file);
	fileName = fileName.substr(0, fileName.find_last_of('.'));	
	ssObj << "\n<div class=\"col-sm-12 \">\n";
	ssObj << "<h4>" << "<b>" << "<u>" << "<i>";
	ssObj << "Dependencies of the selected file:";
	ssObj << "</u>" << "</b>" << "</h4>";
	ssObj << "<ol class=\"nav\">";
	std::vector<std::string> depList = dependencyTable[fileName];
	for each (std::string file in depList)
	{
		ssObj << "<ul align=\"left\" style = \"list-style-type:square\" color=\"darkslategrey\">";
		ssObj << "<li align=\"left\">" << "<a  color=\"darkslategrey\" align=\"center\" href=\"" + file + ".html" + "\">" << file;
		ssObj << "</a>";
		ssObj << "</li>";
		ssObj << "</ul>";
	}
	ssObj << "\n</i>\n";
	ssObj << "\n</ol>\n";
	ssObj << "\n</div>\n";
	return ssObj.str();
}

//function to fetch the head tag with the links to the js and css scripts
std::string CodeAnalysis::Publisher::fetchHeadTag()
{
	std::stringstream ssObj;
	ssObj.clear();
	ssObj << "\n<head>\n";
	ssObj << "<meta name=\"viewport\" content=\"width = device-width, initial-scale = 1\">\n";
	ssObj << fetchJavascriptLink();
	ssObj << fetchCssLink();
	ssObj << "\n</head>\n";
	return ssObj.str();
}

//function to fetch the javascript files
std::string CodeAnalysis::Publisher::fetchJavascriptLink()
{
	std::stringstream ssObj;
	std::string javaScriptPath = FileSystem::Path::getFullFileSpec("Javascripts");
	std::vector<std::string> javaScriptFiles = FileSystem::Directory::getFiles(javaScriptPath, "*.js");
	ssObj.clear();
	for each (std::string javaScriptFile in javaScriptFiles)
	{
		std::string javaScriptLink = "<script src = \"" + std::string("Javascripts") + "\\" + javaScriptFile + "\">" + "</script>";
		ssObj << javaScriptLink;
	}
	return ssObj.str();
}

//function to check if the character exists in the line and return true or false accordingly
bool CodeAnalysis::Publisher::ifExists(std::string l, char ch)
{
	if (l.npos== l.find(ch))
	{
		return false;
	}
	else
	{
		return true;
	}

}

//generate the html file with a button to the home page, a dependency list and the code to be displayed with the expand/collapse feature
void CodeAnalysis::Publisher::createHTML(std::string htmlFile)
{
	std::stringstream ssObj;
	std::string l;
	char ch;
	int block = 1;
	int bcountClosed = 0;
	ssObj.clear();
	ssObj << "\n<!DOCTYPE html>\n";
	ssObj << "<html>\n";
	std::cout << "\n=================\n";
	std::cout << "Requirement 6:\n";
	std::cout << "=================\n";
	std::cout << "\nEmbedding javascripts and styling in head for file\n";
	ssObj << fetchHeadTag();
	ssObj << "\n<body>\n";
	ssObj << "\n<div class=\"blog-header\">\n";
	ssObj << "<a class=\" btn-primary btn-sm active\" href=\"HomePage.html\">";
	ssObj << "Back To HomePage";
	ssObj << "\n</a>\n";
	ssObj << "<div style=\"font-family:calibri\">\n";
	ssObj << fetchDepDiv(htmlFile);
	ssObj << "</div>\n";
	ssObj << "\n<div class=\"col-sm-8 \">\n";
	std::string header = FileSystem::Path::getName(htmlFile, true);
	ssObj << "\n<h3 align=\"center\" style=\"font-family:calibri\" color=\"darkslategrey\">" << "<b><u><i>" << header.substr(0, header.find_last_of('.')) << "</i></u></b></h3>\n";
	ssObj << "\n</div>\n";
	ssObj << "\n</div>\n";
	ssObj << "\n<div  class=\"container col-sm-8 blog-main\">\n";
	std::ifstream isObj(htmlFile);
	while (std::getline(isObj, l))
	{
		ssObj << "\n";
		if (ifExists(l, '{'))
		{
			ssObj << "<a type=\"button\"   class=\"\"   data-toggle=\"collapse\" data-target=\"." + std::to_string(block) + "\"  onclick=\"btnUpdate(this," + std::to_string(block) + ")\">";
			ssObj << "+";
			ssObj << "</a>";
		}
		bool openLine = false;
		int bcounter = 0;
		std::stringstream ss;
		ss << l;

		while (ss.get(ch))
		{
			if (ch == '{')
			{
				if (!openLine)
				{
					bcounter++;
					openLine = true;
					ssObj << "<span  class=\"collapse in " + std::to_string(block) + "\">";
					ssObj << "{...}";
					ssObj << "</span>";
					ssObj << "<span  class=\"collapse " + std::to_string(block++) + "\">";
					ssObj << ch;
				}
				else
				{
					bcounter++;
					ssObj << ch;
				}
			}
			else if (ch == '}')
			{
				bcounter--;
				ssObj << ch;
			}
			else
			{
				ssObj << ch;
			}
		}
		if (ifExists(l, '}'))
		{
			if (bcounter <= 0)
			{
				ssObj << "</span>";
				openLine = false;
			}
		}
	}
	ssObj << "\n</div>\n" << "\n</body>\n" << "\n</html>\n";
	isObj.close();
	std::ofstream osObj(htmlFile);
	osObj << ssObj.str();
}

//function to set the dependency table
void CodeAnalysis::Publisher::depTable(std::unordered_map<Scanner::FileName, std::vector<Scanner::FileName >> &dTable)
{
	this->dependencyTable = dTable;
}

//function builds the home page which displays the list of analysed files and redirects to the partcular selection on click.
void CodeAnalysis::Publisher::generateHomePage()
{
	std::stringstream ssObj;
	ssObj.clear();
	std::vector<std::string> htmlFiles = FileSystem::Directory::getFiles(folder, "*.html");//location of the html files retrieved
	ssObj << "\n<!DOCTYPE html>\n";//html page built for the homepage
	ssObj << "<html>";
	std::cout << "\n=================\n";
	std::cout << "Requirement 6:\n";
	std::cout << "=================\n";
	std::cout << "\nEmbedding javascripts and styling in head for file\n";
	ssObj << fetchHeadTag();
	ssObj << "<body>";
	ssObj << "\n<div class=\"blog-header\" >\n";
	ssObj << "<h3 align=\"left\" style=\"font-family:calibri\" color=\"darkslategrey\">" << "<b><u><i>" << "Dependency-Based Code Publisher" << "</i></u></b></h3>\n";
	ssObj << "<h3 align=\"left\" style=\"font-family:calibri\" color=\"darkslategrey\">" << "<b><u><i>" << "List of files analysed:" << "</u></b></h3>";
	ssObj << "</div>\n";
	ssObj << "\n<div align=\"left\">\n"<< "<ol align=\"left\" class=\"nav\">\n";
	std::vector<std::string> fileList = FileSystem::Directory::getFiles(folder, "*.html");//List of analysed file's html pages retrieved
	for each (std::string htmlFile in htmlFiles)
	{
		if (htmlFile != "HomePage.html")
		{
			ssObj << "<ul align=\"left\" style = \"list-style-type:square\" color=\"darkslategrey\">";
			ssObj << "<li align=\"left\">" << "<a  color=\"darkslategrey\" align=\"center\" href=\"" + htmlFile + "\">" << htmlFile;
			ssObj << "</a>"<< "</li>"<< "</ul>";
		}
	}
	ssObj << "</i>"<< "\n</ol>\n"<< "\n</div>\n"<< "\n</body>\n";
	std::ofstream osObj(folder + "\\" + "HomePage.html");
	osObj << ssObj.str();
	osObj.close();
	std::string homePage = folder + "\\" + "HomePage.html";
	std::wstring homeWindow = std::wstring(homePage.begin(), homePage.end());
	//opens the homepage
	LPCWSTR sw = homeWindow.c_str();
	ShellExecute(0, 0, sw, 0, 0, SW_SHOW);
}

//function to convert the .h and .cpp files into the html format and save it into the destination path
std::string CodeAnalysis::Publisher::htmlConvert(std::string format, std::string file)
{
	std::string destination= FileSystem::Path::getFullFileSpec(folder);
		FileSystem::FileInfo fileDetails(file);
		std::string sourceFileName = fileDetails.name();
		if (!FileSystem::Directory::exists(destination))
		{
			FileSystem::Directory::create(destination);
		}
		destination = destination + "\\" + sourceFileName + "." + format;
		
		if (!FileSystem::File::copy(file, destination))
		{
			std::cout << "\nFile cannot be created:" + destination + "\n";
		}
	return destination;
}

//function to replace the characters mentioned in the switch cases with the html readable data
void CodeAnalysis::Publisher::replaceHTMLCharacters(std::string htmlFile)
{
	std::ifstream is(htmlFile);
	char ch;
	std::stringstream ssObj;
	std::string  fileName = htmlFile + "temp.html";
	ssObj <<"<pre>\n";
	while (is.get(ch)) 
	{
		switch (ch)
		{
		case '–':
			ssObj << "&ndash";
			break;
		case '>':
			ssObj << "&gt";
			break;
		case '<':
			ssObj << "&lt";
			break;
		case'—':
			ssObj << "&mdash";
			break;
		default:
			ssObj << ch;
			break;
		}
	}
	ssObj << "\n</pre>\n";
	is.close();
	std::ofstream os(htmlFile);
	os << ssObj.str();
	os.close();
	ssObj.clear();
}

//function to save the css and javascript files in the repository
void CodeAnalysis::Publisher::saveJavascriptCss()
{
	std::string directory = FileSystem::Directory::getCurrentDirectory();
	std::string cssSourcePath = directory + "\\Css";
	std::string javaScriptSourcePath = directory + "\\Javascripts";
	std::string cssDestinationPath = folder + "\\Css";
	std::string javaScriptDestinationPath = folder + "\\Javascripts";
	if (!FileSystem::Directory::exists(cssDestinationPath))
	{
		FileSystem::Directory::create(cssDestinationPath);
	}
	if (!FileSystem::Directory::exists(javaScriptDestinationPath))
	{
		FileSystem::Directory::create(javaScriptDestinationPath);
	}
	for each (std::string file in FileSystem::Directory::getFiles(cssSourcePath, "*.css"))
	{
		std::string source = cssSourcePath + "\\" + file;
		std::string destination = cssDestinationPath + "\\" + file;
		FileSystem::File::copy(source, destination);
	}
	for each (std::string file in FileSystem::Directory::getFiles(javaScriptSourcePath, "*.js"))
	{
		std::string source = javaScriptSourcePath + "\\" + file;
		std::string destination = javaScriptDestinationPath + "\\" + file;
		FileSystem::File::copy(source, destination);
	}
}

//set the folder variable as the repository which contains all the html files as well as javascript and css 
void CodeAnalysis::Publisher::setRepository(std::string html)
{
	this->folder = html;
}



