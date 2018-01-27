#pragma once
/////////////////////////////////////////////////////////////////////
// Persist.h - store and retrieve NoSqlDb contents                 //
// ver 2.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines a single class, Persist, that provides methods
* save and restore to save db contents in an XML string, and restore
* or create a new db from the XML.
*
* The Persist class also provides methods saveToFile and restoreFromFile
* that use save and restore.
*
* Required Files:
* ---------------
*   NoSqlDb.h, NoSqlDb.cpp,
*   CppProperties.h, CppProperties.cpp,
*   XmlUtilities.h, XmlUtilities.cpp,
*   Display.h, Display.cpp
*
* Maintenance History:
*---------------------
* ver 2.0 : 19 Feb 2017
* - Removed wrapper classes that were introduced in version 1.0.
*   Those seemed like a good idea, but turned out to add to the
*   complexity without providing a lot of generality.
* - The wrapper functionality is now being provided with an
*   XmlConvert class, from XmlUtilities, that provides generic
*   conversions to and from XML, and allows specializations to
*   support needs of specific Data classes.
* - See XmlUtilities::XmlConvert<T> for details.
* ver 1.0 : 06 Feb 2017
* - first release
*/

#include "../NoSqlDb/NoSqlDb.h"
#include "../CppProperties/CppProperties.h"
#include "../XmlUtilitites/XmlUtilities.h"
#include <fstream>
#include <vector>
#include <string>

namespace NoSQLDB
{
	/////////////////////////////////////////////////////////
	// Record class is not being used, but is here as a
	// reminder for future refactoring.
	//
	template<typename Data>
	class Record
	{
	public:
		Property<Key> key;
		Property<Element<Data>> element;
	};
	/////////////////////////////////////////////////////////
	// Persist class holds a reference to db and uses
	// that to save and restore the db data.
	//
	// I plan to change the reference to a std::shared_ptr
	// so the db can be changed.  Now we have to make a new
	// instance of Persist to save a second db.
	//
	template<typename Data>
	class Persist
	{
	public:
		using Xml = std::string;
		using SPtr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;
		using SPtrs = std::vector<SPtr>;
		//void saveSCCtoFile(std::vector<std::vector<std::string>>& SCCVector, std::string SCCxml);
		void saveStrongComp(std::string xmlStrongComp, std::vector<std::vector<std::string>>& vectorStrongComp);
		Persist(NoSqlDb<Data>& db) : db_(db) {}
		Xml save();
		void restore(const Xml& xml);
		bool saveToFile(const std::string& fileSpec);
		bool restoreFromFile(const std::string& fileSpec);
		std::string compress(const Xml& xml);
	private:
		NoSqlDb<Data>& db_;
		std::string getTextBodyFromElement(SPtr sPtr);
		std::string getTextBodyFromElement(SPtrs sPtrs);
	};

	//----< saves db contents to file, using save >--------------------

	template<typename Data>
	bool Persist<Data>::saveToFile(const std::string& fileSpec)
	{
		std::ofstream out(fileSpec);
		if (!out.good())
			return false;
		Persist<Data> persist(db_);
		Xml xml = persist.save();
		if (out << xml)
		{
			std::cout << "\n=================\n";
			std::cout << "Requirement 7:\n";
			std::cout << "=================\n";
			std::cout << "\nXML location :" << fileSpec << "\n";
		}
		else
		{
			out.clear();
			out = std::ofstream("DependencyXml.xml");
			out << xml;
			std::cout << "\nXML location : DependencyXml.xml\n";
		}
		out.close();
		return true;
	}
	//----< compresses xml string >------------------------------------

	template<typename Data>
	std::string Persist<Data>::compress(const Xml& xml)
	{
		std::string compressed;
		for (char ch : xml)
		{
			if (ch == '\n' || ch == '\t' || ch == ' ')
				continue;
			compressed += ch;
		}
		return compressed;
	}
	//----< restores db contents from file, using restore >------------

	template<typename Data>
	bool Persist<Data>::restoreFromFile(const std::string& fileSpec)
	{
		//std::string newFS = "C:\\Users\\James\\Desktop\\InstrSolProject1S2017\\Project1.xml";
		std::ifstream in(fileSpec);
		if (!in.good())
		{
			std::cout << "\n  failed to open file";
			return false;
		}
		Xml xml;

		// build XML string from file

		while (in.good())
		{
			char ch = in.get();

			/////////////////////////////////////////////////////
			// This code removes whitespace from XML contents
			// but is unnecessart and adds a small CPU load.
			// I left it here because I intent to use it in
			// another package, for display.
			/////////////////////////////////////////////////////
			// if (ch == '\n' || ch == '\t' || ch == ' ')
			//   continue;
			/////////////////////////////////////////////////////

			if (!in.good())
				break;
			xml += ch;
		}
		in.close();

		restore(xml);
		return true;
	}
	//----< save db contents to XML string >---------------------------

	template<typename Data>
	typename Persist<Data>::Xml Persist<Data>::save()
	{
		using SPtr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;

		SPtr pDbRoot = XmlProcessing::makeTaggedElement("noSqlDb");
		XmlProcessing::XmlDocument doc;
		doc.docElement()->addChild(pDbRoot);

		// This works too.
		// XmlProcessing::XmlDocument doc(XmlProcessing::makeDocElement(pDbRoot));

		Keys keys = db_.keys();
		for (Key k : keys)
		{
			SPtr pRecord = XmlProcessing::makeTaggedElement("record");
			SPtr pKey = XmlProcessing::makeTaggedElement("key");
			SPtr pText = XmlProcessing::makeTextElement(Utilities::Convert<Key>::toString(k));
			pKey->addChild(pText);
			pRecord->addChild(pKey);

			Element<Data> elem = db_.value(k);
			SPtr pName = XmlProcessing::makeTaggedElement("name", static_cast<std::string>(elem.name));
			pRecord->addChild(pName);

			SPtr pCata = XmlProcessing::makeTaggedElement("category", static_cast<std::string>(elem.category));
			pRecord->addChild(pCata);

			DateTime dt = elem.dateTime;
			SPtr pDateTime = XmlProcessing::makeTaggedElement("dateTime", dt.time());
			pRecord->addChild(pDateTime);

			SPtr pChildren = XmlProcessing::makeTaggedElement("children");
			Keys childKeys = elem.children;
			for (Key child : childKeys)
			{
				SPtr pChild = XmlProcessing::makeTaggedElement("childKey", static_cast<std::string>(child));
				pChildren->addChild(pChild);
			}
			pRecord->addChild(pChildren);

			Data data = elem.data;
			SPtr pData = XmlProcessing::XmlConvert<Data>::toXmlElement(data, "data");
			pRecord->addChild(pData);

			pDbRoot->addChild(pRecord);
		}
		std::cout << "\n====================================" << "\n" << "Dependency Analysis in XML:" << "\n" << "====================================" << "\n" << doc.toString() << "\n\n";
		return doc.toString();
	}

	//----< save strong components to XML string >---------------------------

	template<typename Data>
	void Persist<Data>::saveStrongComp(std::string xmlStrongComp, std::vector<std::vector<std::string>>& vectorStrongComp)
	{
		int sCompId = 0;
		using SPtr = std::shared_ptr<XmlProcessing::AbstractXmlElement>;
		SPtr pDbRoot = XmlProcessing::makeTaggedElement("StrongComponents");
		XmlProcessing::XmlDocument doc;
		doc.docElement()->addChild(pDbRoot);
		for (auto comp : vectorStrongComp)
		{
			SPtr pRecord = XmlProcessing::makeTaggedElement("record");
			SPtr pKey = XmlProcessing::makeTaggedElement("key");
			SPtr pText = XmlProcessing::makeTextElement(Utilities::Convert<Key>::toString(std::to_string(++sCompId)));
			pKey->addChild(pText);
			pRecord->addChild(pKey);

			SPtr pChildren = XmlProcessing::makeTaggedElement("children");
			for (std::string file : comp)
			{
				SPtr pChild = XmlProcessing::makeTaggedElement("childKey", static_cast<std::string>(file));
				pChildren->addChild(pChild);
			}
			pRecord->addChild(pChildren);
			pDbRoot->addChild(pRecord);
		}
		std::ofstream out(xmlStrongComp);
		Xml xml = doc.toString();
		std::cout << "\n====================\n";
		std::cout << "\nStrong Component XML: \n";
		std::cout << "====================\n";
		std::cout<< xml << "\n";

		if (out << xml)
		{
			std::cout << "\n=================\n";
			std::cout << "Requirement 7:\n";
			std::cout << "=================\n";
			std::cout << "\nXML location :" << xmlStrongComp << "\n";
		}
		else
		{
			out.clear();
			out = std::ofstream("StrongComponents.xml");
			out << xml;
			std::cout << "\nXML is saved at : StrongComponents.xml\n";
		}
		out.close();
	}


	//----< retrive db contents from XML string >----------------------
	/*
	* Private helper function - provides safe extraction of element text
	* from single element pointer.
	*/
	template<typename Data>
	std::string Persist<Data>::getTextBodyFromElement(SPtr sPtr)
	{
		if (sPtr->children().size() == 0)
			return "";
		return trim(sPtr->children()[0]->value());
	}
	/*
	* Private helper function - provides safe extraction of element text
	* from the first element pointer in a vector of element pointers.
	*/
	template<typename Data>
	std::string Persist<Data>::getTextBodyFromElement(SPtrs sPtrs)
	{
		if (sPtrs.size() == 0)
			return "";
		SPtr sPtr = sPtrs[0];
		if (sPtr->children().size() == 0)
			return "";
		return trim(sPtr->children()[0]->value());
	}
	/*
	* This is where the restoration takes place.
	*/
	template<typename Data>
	void Persist<Data>::restore(const Xml& xml)
	{
		using SPtr = std::shared_ptr<AbstractXmlElement>;
		using SPtrs = std::vector<SPtr>;

		XmlDocument doc(xml, XmlDocument::str);
		SPtrs pRecords = doc.descendents("record").select();
		for (SPtr pRecord : pRecords)
		{
			XmlDocument doc;
			doc.docElement()->addChild(pRecord);

			Key key = getTextBodyFromElement(doc.element("key").select());

			Name name = getTextBodyFromElement(doc.element("name").select());

			Category category = getTextBodyFromElement(doc.element("category").select());

			Children children;
			SPtrs pChildren = doc.descendents("childKey").select();
			for (SPtr pChild : pChildren)
			{
				children.push_back(getTextBodyFromElement(pChild));
			}

			SPtr pData = doc.element("data").select()[0];

			Element<Data> elem(name, category);
			elem.children = children;
			elem.data = XmlConvert<Data>::fromXmlElement(pData);

			bool saved = db_.saveRecord(key, elem);  // save new record
			if (!saved)
				db_.saveValue(key, elem);  // overwrite old record element
		}
	}
}
