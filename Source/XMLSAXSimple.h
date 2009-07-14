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

// Header for XMLSAXSimple class

#ifndef __XMLSAXSIMPLE__XMLLIB__
#define __XMLSAXSIMPLE__XMLLIB__

#include "XMLParserSAX.h"

#include "CStreamBuffer.h"

#include <sstream>

namespace xmllib
{

class XMLSAXSimple : public XMLParserSAX
{
public:
	XMLSAXSimple();
	virtual ~XMLSAXSimple();

	virtual void ParseData(const char* data);
	virtual void ParseFile(const char* filename);
	virtual void ParseStream(std::istream& is);

protected:
	CStreamBuffer	mBuffer;

private:
	enum EXMLTag
	{
		TAG_NONE = 0,
		TAG_CDATA,
		TAG_DOCTYPE,
		TAG_DECLARATION,
		TAG_COMMENT,
		TAG_PROCESSING,
		TAG_ELEMENT_END,
		TAG_ELEMENT
	};

	std::ostringstream	mChars;

	// Actually parsing
	void ParseIt();
	
	bool ParseDoctype();
	bool ParseDeclaration();
	bool ParseComment();
	bool ParseProcessing();

	bool ParseElement();
	bool ParseElementEnd();
	bool ParseCharacters();
	bool ParseCDATA();

	bool ParseName(cdstring& name);
	bool ParseAttributeValue(cdstring& value);

	void XMLDecode(cdstring& value);

	EXMLTag GetCurrentTag();
	
	void SkipWS();
};

}
#endif
