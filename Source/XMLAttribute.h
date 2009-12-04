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

// Header for XMLAttribute class

#ifndef __XMLATTRIBUTE__XMLLIB__
#define __XMLATTRIBUTE__XMLLIB__

#include <list>
#include <map>

#include "cdstring.h"

namespace xmllib
{

class XMLAttribute;
typedef std::map<cdstring, XMLAttribute*> XMLAttributeMap;
typedef std::list<XMLAttribute*> XMLAttributeList;

class XMLAttribute
{
public:
	XMLAttribute(const cdstring& name, const cdstring& value = cdstring::null_str)
		{ mName = name; mValue = value; }
	explicit XMLAttribute(const XMLAttribute& copy)
		{ _copy(copy); }

	XMLAttribute& operator=(const XMLAttribute& copy)
		{ if (this != &copy) _copy(copy); return *this; }
	
	const cdstring& Name() const
		{ return mName; }
	void SetName(const cdstring& name)
		{ mName = name; }

	const cdstring& Value() const
		{ return mValue; }
	void SetValue(const cdstring& value)
		{ mValue = value; }

private:
	cdstring	mName;
	cdstring	mValue;
	
	void _copy(const XMLAttribute& copy)
		{ mName = copy.mName; mValue = copy.mValue; }
};

inline void XMLAttributeList_DeleteItems(XMLAttributeList& list)
{
	for(XMLAttributeList::iterator iter = list.begin(); iter != list.end(); iter++)
		delete *iter;
}

}
#endif
