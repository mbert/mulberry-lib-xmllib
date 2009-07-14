/*
    Copyright (c) 2007 Cyrus Daboo. All rights reserved.
    
    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
        http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

// Source for XMLGeneratorlibxml2 class

#include "XMLGeneratorlibxml2.h"

#include "XMLNode.h"

using namespace xmllib;

XMLGeneratorlibxml2::XMLGeneratorlibxml2()
{
}

XMLGeneratorlibxml2::~XMLGeneratorlibxml2()
{
}

void XMLGeneratorlibxml2::Generate(const XMLNode* root, std::ostream& out)
{
	// Items to clean-up
	xmlDocPtr doc = NULL;

	try
	{
		// Create new xml document with version 1.0
		doc = ::xmlNewDoc((const xmlChar *) "1.0");

		if (doc)
		{
			// Create a new root node (does recursive create of entire tree)
			MakeNodePtr(doc, NULL, root, true);
			
			// Write to disk
			::xmlSaveFormatFile("out.xml", doc, 1);
		}		
	}
	catch(const std::exception& ex)
	{
	}

	// Clean-up
	if (doc)
		::xmlFreeDoc(doc);
}

xmlNodePtr XMLGeneratorlibxml2::MakeNodePtr(xmlDocPtr doc, xmlNodePtr parent, const XMLNode* node, bool top)
{
	// Create base node
	xmlNodePtr nptr = NULL;

	// If no node then this is the root => just add children at top level
	if (top)
	{
		// Add each child as top-level item
		for(XMLNodeList::const_iterator iter = node->Children().begin(); iter != node->Children().end(); iter++)
			::xmlDocSetRootElement(doc,  MakeNodePtr(doc, NULL, *iter));
	}
	else
	{
		// Create child or top-level document node
		if (parent)
			nptr = ::xmlNewChild(parent, NULL, (const xmlChar *) node->Name().c_str(), ::xmlEncodeEntitiesReentrant(doc, (const xmlChar *) node->Data().c_str()));
		else
			nptr = ::xmlNewDocNode(doc, NULL, (const xmlChar *) node->Name().c_str(), ::xmlEncodeEntitiesReentrant(doc, (const xmlChar *) node->Data().c_str()));
		
		if (nptr)
		{
			// Add all attributes
			for(XMLAttributeList::const_iterator iter = node->Attributes().begin(); iter != node->Attributes().end(); iter++)
				::xmlSetProp(nptr, (const xmlChar *) (*iter)->Name().c_str(), (const xmlChar *) (*iter)->Value().c_str());
			
			// Now add each child
			for(XMLNodeList::const_iterator iter = node->Children().begin(); iter != node->Children().end(); iter++)
				MakeNodePtr(doc, nptr, *iter);
		}
	}

	// Return result
	return nptr;
}
