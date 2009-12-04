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

// Header for XMLName class

#ifndef __XMLName__XMLLIB__
#define __XMLName__XMLLIB__

#include "cdstring.h"

#include <vector>

namespace xmllib
{

class XMLName;
typedef std::vector<XMLName> XMLNameList;
class XMLNode;

class XMLName
{
public:
	XMLName(const char* name = NULL, const char* namespc = NULL, bool take_ownership = false)
	{
		mOwnsData = take_ownership;
		mName = name;
		mNamespace = namespc;
	}
	explicit XMLName(const XMLNode& node);
	XMLName(const XMLName& copy)
		{ _init(); _copy(copy); }
	~XMLName()
	{
		_tidy();
	}

	XMLName& operator=(const XMLName& copy)
		{ if (this != &copy) _copy(copy); return *this; }
	
	int operator==(const XMLName& comp) const;
	
	// Data
	void SetItems(const char* name = NULL, const char* namespc = NULL, bool take_ownership = false)
	{
		_tidy();
		mOwnsData = take_ownership;
		mName = name;
		mNamespace = namespc;
	}

	// Name
	const char* Name() const
		{ return mName; }

	// Namespace
	const char* Namespace() const
		{ return mNamespace; }

	// Full name
	const cdstring& FullName() const;

private:
	bool				mOwnsData;
	const char*			mName;
	const char*			mNamespace;
	mutable cdstring	mFullName;

	void _init()
	{
		mOwnsData = false;
		mName = NULL;
		mNamespace = NULL;
	}

	void _copy(const XMLName& copy)
	{
		_tidy();
		mOwnsData = copy.mOwnsData;
		mName = mOwnsData ? ::strdup(copy.mName) : copy.mName;
		mNamespace = mOwnsData ? ::strdup(copy.mNamespace) : copy.mNamespace;
	}
	
	void _tidy()
	{
		if (mOwnsData)
		{
			delete mName;
			delete mNamespace;
		}
		mOwnsData = false;
		mName = NULL;
		mNamespace = NULL;
	}
};

}
#endif
