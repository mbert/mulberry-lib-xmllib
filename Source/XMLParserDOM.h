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

// Header for XMLParserDOM class

#ifndef __XMLPARSERDOM__XMLLIB__
#define __XMLPARSERDOM__XMLLIB__

#include "XMLParser.h"

namespace xmllib
{

class XMLNode;

class XMLParserDOM : public XMLParser
{
public:
	XMLParserDOM() {}
	virtual ~XMLParserDOM() {}

	virtual XMLNode* Root() = 0;

};

}
#endif
