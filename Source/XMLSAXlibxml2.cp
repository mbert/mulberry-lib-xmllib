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

// Source for XMLSAXlibxml2 class

#include "XMLSAXlibxml2.h"

#include <libxml/parserInternals.h>

#include <cstdarg>
#include <strstream>

using namespace xmllib;

XMLSAXlibxml2::XMLSAXlibxml2()
{
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

XMLSAXlibxml2::~XMLSAXlibxml2()
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

void XMLSAXlibxml2::HandleException(const std::exception& ex)
{
	XMLParserSAX::HandleException(ex);
	::xmlStopParser(mParseContext);
}

#pragma mark ____________________________callbacks

void XMLSAXlibxml2::_StartDocument(void* parser)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Do callback method
	try
	{
		xmlparser->StartDocument();
	}
	catch(const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLSAXlibxml2::_EndDocument(void* parser)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Don't bother if on error state
	if (xmlparser->mError)
		return;

	// Do callback method
	try
	{
		xmlparser->EndDocument();
	}
	catch (const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLSAXlibxml2::_StartElement(void* parser, const xmlChar* name, const xmlChar** p)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Get attributes
	XMLAttributeList attrs;
	if (p)
	{
		for(const xmlChar** item = p; item && *item; item++)
		{
			std::string name = (const char *) *item++;
			std::string value = (const char *) *item;

			attrs.push_back(new XMLAttribute(name, value));
		}
	}

	// Do callback method
	try
	{
		xmlparser->StartElement(std::string((const char *) name), attrs);
	}
	catch (const std::exception& e)
	{
		xmlparser->HandleException(e);
	}

	// Clean-up
	XMLAttributeList_DeleteItems(attrs);
};

void XMLSAXlibxml2::_EndElement(void* parser, const xmlChar* name) 
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Do callback method
	try
	{
		xmlparser->EndElement(std::string((const char *) name));
	}
	catch(const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLSAXlibxml2::_Characters(void* parser, const xmlChar* ch, int len)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Do callback method
	try
	{
		xmlparser->Characters(std::string((const char *) ch, len));
	}
	catch(const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLSAXlibxml2::_Comment(void* parser, const xmlChar* value)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Do callback method
	try
	{
		xmlparser->Comment(std::string((const char *) value));
	}
	catch(const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLSAXlibxml2::_Warning(void* parser, const char* fmt, ...)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Get arguments
	std::va_list arg;
	char buff[1024];
	va_start(arg, fmt);
	std::vsprintf(buff, fmt, arg);
	va_end(arg);

	// Do callback method
	try
	{
		xmlparser->Warning(std::string(buff));
	}
	catch (const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLSAXlibxml2::_Error(void* parser, const char* fmt, ...)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Don't bother if on error state
	if (xmlparser->mError)
		return;

	// Get arguments
	std::va_list arg;
	char buff[1024];

	va_start(arg, fmt);
	std::vsprintf(buff, fmt, arg);
	va_end(arg);

	// Do callback method
	try
	{
		xmlparser->Error(std::string(buff));
	}
	catch (const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};

void XMLSAXlibxml2::_FatalError(void* parser, const char *fmt, ...)
{
	// Get parser object
	XMLSAXlibxml2* xmlparser = static_cast<XMLSAXlibxml2*>(parser);

	// Don't bother if on error state
	if (xmlparser->mError)
		return;

	// Get arguments
	std::va_list arg;
	char buff[1024];

	va_start(arg, fmt);
	std::vsprintf(buff, fmt, arg);
	va_end(arg);

	// Do callback method
	try
	{
		xmlparser->FatalError(std::string(buff));
	}
	catch (const std::exception& e)
	{
		xmlparser->HandleException(e);
	}
};