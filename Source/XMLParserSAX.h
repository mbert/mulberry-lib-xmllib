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

// Header for XMLParserSAX class

#ifndef __XMLPARSERSAX__XMLLIB__
#define __XMLPARSERSAX__XMLLIB__

#include "XMLParser.h"
#include "XMLAttribute.h"
#include "XMLNode.h"

namespace xmllib
{

class XMLDocument;

class XMLParserSAX : public XMLParser
{
public:
	XMLParserSAX();
	virtual ~XMLParserSAX();

	virtual XMLDocument* Document()
	{
		return mDocument;
	}

	// Hand over control of document to caller
	virtual XMLDocument* ReleaseDocument()
	{
		XMLDocument* temp = mDocument;
		mDocument = NULL;
		return temp;
	}

protected:
	XMLDocument*	mDocument;
	XMLNodeList		mNodeList;
	bool			mError;

	virtual void StartDocument();
	virtual void EndDocument();
	virtual void StartElement(const cdstring& name, const XMLAttributeList& attributes);
	virtual void EndElement(const cdstring& name);
	virtual void Characters(const cdstring& data);
	virtual void Comment(const cdstring& text);
	virtual void Warning(const cdstring& text);
	virtual void Error(const cdstring& text);
	virtual void FatalError(const cdstring& text);

	virtual void HandleException(const std::exception& ex);

private:
	// SAX callbacks - these will actually appear in the OS-specific derived class
};

}
#endif
