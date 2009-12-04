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

// Header for XMLSAXMac class

#ifndef __XMLSAXMAC__XMLLIB__
#define __XMLSAXMAC__XMLLIB__

#include "XMLParserSAX.h"

#include <CFXMLParser.h>

namespace xmllib
{

class XMLSAXMac : public XMLParserSAX
{
public:
	XMLSAXMac();
	virtual ~XMLSAXMac();

	virtual void ParseData(const char* data);
	virtual void ParseFile(const char* filename);

private:
	// Core Foundation callbacks
	static void* CreateStructure(CFXMLParserRef parser, CFXMLNodeRef node, void* info);
	static void EndStructure(CFXMLParserRef parser, void* xmlType, void* info);
	static void AddChild(CFXMLParserRef parser, void* parent, void* child, void* info);
	static Boolean HandleError(CFXMLParserRef parser, CFXMLParserStatusCode error, void *info);
};

}
#endif
