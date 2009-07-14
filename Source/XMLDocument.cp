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

#include "XMLDocument.h"

#include "XMLNode.h"

namespace xmllib
{

XMLDocument::XMLDocument()
{
	mRoot = new XMLNode(this, NULL, cdstring::null_str);
	mNamespaces.push_back(XMLNamespace(cdstring::null_str));
}


XMLDocument::~XMLDocument()
{
	delete mRoot;
}

// Add the namespace to the documents list, and update the index in
// the namespace object to the index where it is stored. Setting the index
// will avoid the need to use this method again if the namespace object is re-used.
uint32_t XMLDocument::AddNamespace(const XMLNamespace& namespc)
{
	// Check whether already present
	XMLNamespaceList::const_iterator found =  mNamespaces.end();
	bool test_prefix = namespc.Prefix().empty();
	for(XMLNamespaceList::const_iterator iter = mNamespaces.begin(); iter != mNamespaces.end(); iter++)
	{
		// Policy: first try full match with name & prefix
		// then try partial match with name and no prefix
		if (*iter == namespc)
		{
			found = iter;
			break;
		}
		// For partial match just use the first one found
		else if (test_prefix && ((*iter).Name() == namespc.Name()))
		{
			found = iter;
			test_prefix = false;
		}
	}
	
	if (found != mNamespaces.end())
		return found - mNamespaces.begin();

	namespc.SetIndex(mNamespaces.size());
	mNamespaces.push_back(namespc);
	return namespc.Index();
}

const cdstring& XMLDocument::GetNamespace(uint32_t index) const
{
	if (index >= mNamespaces.size())
		index = 0;
	return mNamespaces.at(index).Name();
}

const cdstring& XMLDocument::GetNamespacePrefix(uint32_t index) const
{
	if (index >= mNamespaces.size())
		index = 0;
	return mNamespaces.at(index).Prefix();
}

void XMLDocument::Generate(std::ostream& os, bool indent) const
{
	// Handle namespace:
	//  Make sure each namespace has a unique prefix
	//  Add to root element as xmlns
	cdstrset prefix;
	for(XMLNamespaceList::const_iterator iter = mNamespaces.begin(); iter != mNamespaces.end(); iter++)
	{
		if ((*iter).Name().empty())
			continue;
		if ((*iter).Prefix().empty())
		{
			cdstring name;
			cdstring::size_type pos = (*iter).Name().rfind(':');
			if ((pos != cdstring::npos) && (pos != (*iter).Name().length() - 1))
			{
				const char* p = (*iter).Name().c_str() + (pos + 1);
				char c = ::toupper(*p);
				while(c != 0)
				{
					if ((c >= 'A') && (c <= 'Z'))
					{
						name = c;
						break;
					}
					else
						c = ::toupper(*++p);
				}
				if (name.empty())
					name = 'A';
			}
			else
				name = (char)::toupper((*iter).Name()[(cdstring::size_type)0]);
			while(prefix.count(name) != 0)
				name += (char)::toupper(name[(cdstring::size_type)0]);
			const_cast<XMLNamespace&>(*iter).SetPrefix(name);
		}
		else
			prefix.insert((*iter).Prefix());
		
		mRoot->AddNamespace(*iter);
	}
	
	// Do declaration
	os << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>" << std::endl;
	
	// Do each child of the main root element
	mRoot->Generate(os, 0, indent);
}

}
