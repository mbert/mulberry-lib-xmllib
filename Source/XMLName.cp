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

// Source for XMLName class

#include "XMLName.h"

#include "XMLNode.h"

#include <cstdlib>

using namespace xmllib;

XMLName::XMLName(const XMLNode& node)
{
	mOwnsData = true;
	mName = ::strdup(node.Name().c_str());
	mNamespace = ::strdup(node.Namespace().c_str());
}

int XMLName::operator==(const XMLName& comp) const
{
	if ((mName == NULL) ^ (comp.mName == NULL))
		return 0;
	else if ((mName != NULL) && (strcmp(mName, comp.mName) != 0))
		return 0;
	
	if ((mNamespace == NULL) ^ (comp.mNamespace == NULL))
		return 0;
	else if ((mNamespace != NULL) && (strcmp(mNamespace, comp.mNamespace) != 0))
		return 0;
	
	return 1;
}

// Full name - cached when needed
const cdstring& XMLName::FullName() const
{
	if (mFullName.empty() && (mName != NULL))
	{
		if (mNamespace != NULL)
		{
			mFullName = mNamespace;
		}
		mFullName += mName;
	}
	
	return mFullName;
}
