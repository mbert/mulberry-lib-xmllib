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

// Header for XMLSAXlibxml2 class

#ifndef __XMLSAXLIBXML2__XMLLIB__
#define __XMLSAXLIBXML2__XMLLIB__

#include "XMLParserSAX.h"

#include <libxml/parser.h>

namespace xmllib
{

class XMLSAXlibxml2 : public XMLParserSAX
{
public:
	XMLSAXlibxml2();
	virtual ~XMLSAXlibxml2();

	virtual void ParseData(const char* data);
	virtual void ParseFile(const char* filename);

protected:
	virtual void HandleException(const std::exception& ex);

private:
	xmlParserCtxtPtr 	mParseContext;
	xmlSAXHandler		mSAXHandler;

	// Callbacks
	static void _StartDocument(void* parser);
	static void _EndDocument(void* parser);
	static void _StartElement(void* parser, const xmlChar* name, const xmlChar** p);
	static void _EndElement(void* parser, const xmlChar* name);
	static void _Characters(void* parser, const xmlChar* ch, int len);
	static void _Comment(void* parser, const xmlChar* value);
	static void _Warning(void* parser, const char* fmt, ...);
	static void _Error(void* parser, const char* fmt, ...);
	static void _FatalError(void* parser, const char* fmt, ...);

};

}
#endif
