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

// Header for XMLGeneratorlibxml2 class

#ifndef __XMLGENERATORLIBXML2__XMLLIB__
#define __XMLGENERATORLIBXML2__XMLLIB__

#include "XMLGenerator.h"

#include <libxml/tree.h>

namespace xmllib
{

class XMLGeneratorlibxml2 : public XMLGenerator
{
public:
	XMLGeneratorlibxml2();
	virtual ~XMLGeneratorlibxml2();

	virtual void Generate(const XMLNode* root, std::ostream& out);

private:
	xmlNodePtr MakeNodePtr(xmlDocPtr doc, xmlNodePtr parent, const XMLNode* node, bool top = false);
};

}
#endif
