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

// Source for XMLDOMlibxml2 class

#include "XMLDOMlibxml2.h"

#include <libxml/parserInternals.h>

#include <cstdarg>
#include <strstream>

using namespace xmllib;

XMLDOMlibxml2::XMLDOMlibxml2()
{
	mRoot = NULL;
	mParseContext = NULL;

	xmlSAXHandler handlers =
	{
		NULL,						// internalSubset
		NULL,						// isStandalone
		NULL,						// hasInternalSubset
		NULL,						// hasExternalSubset
		NULL,						// resolveEntity
		NULL,						// getEntity
		NULL,						// entityDecl
		NULL,						// notationDecl
		NULL,						// attributeDecl
		NULL,						// elementDecl
		NULL,						// unparsedEntityDecl
		NULL,						// setDocumentLocator
		_StartDocument,				// startDocument
		_EndDocument,				// endDocument
		_StartElement,				// startElement
		_EndElement,				// endElement
		NULL,						// reference
		_Characters,				// characters
		NULL,						// ignorableWhitespace
		NULL,						// processingInstruction
		_Comment,					// comment
		_Warning,					// warning
		_Error,						// error
		_FatalError,				// fatalError
		NULL,						// getParameterEntity
		NULL,						// cdataBlock
		NULL						// externalSubset
	};
	mSAXHandler = handlers;
	
	mError = false;
}

XMLDOMlibxml2::~XMLDOMlibxml2()
{
	if(mParseContext)
		::xmlFreeParserCtxt(mParseContext);
}

void XMLSAXlibxml2::ParseData(const char* data)
{
	mParseContext = ::xmlCreateFileParserCtxt("test.xml");

	mParseContext->sax = &mSAXHandler;
	mParseContext->userData = this;

	::xmlKeepBlanksDefault(0);

	::xmlParseDocument(mParseContext);

	mParseContext->sax = NULL;
	::xmlFreeParserCtxt(mParseContext);
	mParseContext = NULL;
}

void XMLSAXlibxml2::ParseFile(const char* file)
{
	mParseContext = ::xmlCreateFileParserCtxt(file);

	mParseContext->sax = &mSAXHandler;
	mParseContext->userData = this;

	::xmlKeepBlanksDefault(0);

	::xmlParseDocument(mParseContext);

	mParseContext->sax = NULL;
	::xmlFreeParserCtxt(mParseContext);
	mParseContext = NULL;
}

void XMLDOMlibxml2::HandleException(const std::exception& ex)
{
	mError = true;
	::xmlStopParser(mParseContext);
}

#pragma mark ____________________________callbacks

void XMLDOMlibxml2::_StartDocument(void* parser)
{
	// Get parser object
	XMLDOMlibxml2* xmlparser = static_cast<XMLDOMlibxml2*>(parser);

	// Add a default (empty) root item
	xmlparser->mRoot = new XMLNode(std::string());
	xmlparser->mNodeList.push_back(xmlparser->mRoot);

};

void XMLDOMlibxml2::_EndDocument(void* parser)
{
	// Nothing to do
};

void XMLDOMlibxml2::_StartElement(void* parser, const xmlChar* name, const xmlChar** p)
{
	// Get parser object
	XMLDOMlibxml2* xmlparser = static_cast<XMLDOMlibxml2*>(parser);

	// Don't bother if on error state
	if (xmlparser->mError)
		return;

	try
	{
		// Create a new node
		XMLNode* node = new XMLNode(std::string((const char *) name));
		
		// Get attributes
		if (node && p)
		{
			for(const xmlChar** item = p; item && *item; item++)
			{
				std::string name = (const char *) *item++;
				std::string value = (const char *) *item;

				node->AddAttribute(name, value);
			}
		}
		
		// Make it a child of current stack element
		if (xmlparser->mNodeList.size() && (xmlparser->mNodeList.back() != NULL))
			xmlparser->mNodeList.back()->AddChild(node);

		// Push onto stack
		xmlparser->mNodeList.push_back(node);
	}
	catch (const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLDOMlibxml2::_EndElement(void* parser, const xmlChar* name) 
{
	// Get parser object
	XMLDOMlibxml2* xmlparser = static_cast<XMLDOMlibxml2*>(parser);

	// Don't bother if on error state
	if (xmlparser->mError)
		return;

	try
	{
		// Pop current item off the stack
		xmlparser->mNodeList.pop_back();
	}
	catch(const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLDOMlibxml2::_Characters(void* parser, const xmlChar* ch, int len)
{
	// Get parser object
	XMLDOMlibxml2* xmlparser = static_cast<XMLDOMlibxml2*>(parser);

	// Don't bother if on error state
	if (xmlparser->mError)
		return;

	try
	{
		// Add data to current stack element
		if (xmlparser->mNodeList.size() && (xmlparser->mNodeList.back() != NULL))
			xmlparser->mNodeList.back()->SetData(std::string((const char *) ch, len));
	}
	catch(const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLDOMlibxml2::_Comment(void* parser, const xmlChar* value)
{
	// Nothing to do
};

void XMLDOMlibxml2::_Warning(void* parser, const char* fmt, ...)
{
	// Nothing to do
};

void XMLDOMlibxml2::_Error(void* parser, const char* fmt, ...)
{
	// Get parser object
	XMLDOMlibxml2* xmlparser = static_cast<XMLDOMlibxml2*>(parser);

	// Set error state
	xmlparser->mError = true;
};

void XMLDOMlibxml2::_FatalError(void* parser, const char *fmt, ...)
{
	// Get parser object
	XMLDOMlibxml2* xmlparser = static_cast<XMLDOMlibxml2*>(parser);

	// Set error state
	xmlparser->mError = true;
};