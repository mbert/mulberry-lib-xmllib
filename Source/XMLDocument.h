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

#ifndef XMLDocument_H
#define XMLDocument_H

#include <stdint.h>
#include <ostream>
#include <vector>


#include "cdstring.h"

#include "XMLNamespace.h"

namespace xmllib {

class XMLNode;

class XMLDocument
{
public:
	XMLDocument();
	virtual ~XMLDocument();

	XMLNode* GetRoot()
	{
		return mRoot;
	}

	uint32_t			AddNamespace(const XMLNamespace& namespc);
	const cdstring&		GetNamespace(uint32_t index) const;
	const cdstring&		GetNamespacePrefix(uint32_t index) const;
	
	void	Generate(std::ostream& os, bool indent = true) const;

protected:
	XMLNode*			mRoot;				// Root element of document
	XMLNamespaceList	mNamespaces;		// List of all namespaces used in the document
};

}	// namespace xmllib

#endif	// XMLDocument_H
