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

// Header for XMLNamespace class

#ifndef __XMLNamespace__XMLLIB__
#define __XMLNamespace__XMLLIB__

#include "cdstring.h"

namespace xmllib
{

class XMLNamespace;
typedef std::vector<XMLNamespace> XMLNamespaceList;

class XMLNamespace
{
public:
	XMLNamespace(const cdstring& name)
	{
		mName = name;
		mDocIndex = 0xFFFFFFFF;
	}
	XMLNamespace(const cdstring& name, const cdstring& prefix)
	{
		mName = name;
		mPrefix = prefix;
		mDocIndex = 0xFFFFFFFF;
	}
	XMLNamespace(const XMLNamespace& copy)
		{ _copy(copy); }
	~XMLNamespace() {}

	XMLNamespace& operator=(const XMLNamespace& copy)
		{ if (this != &copy) _copy(copy); return *this; }
	
	int operator==(const XMLNamespace& comp) const
		{ return (mName == comp.mName) && (mPrefix == comp.mPrefix); }
	
	// Name
	const cdstring& Name() const
		{ return mName; }
	void SetName(const cdstring& name)
		{ mName = name; }

	// Prefix
	const cdstring& Prefix() const
		{ return mPrefix; }
	void SetPrefix(const cdstring& prefix)
		{ mPrefix = prefix; }

	// Index
	bool HasIndex() const
	{
		return mDocIndex != 0xFFFFFFFF;
	}
	uint32_t Index() const
	{
		return mDocIndex;
	}
	void SetIndex(uint32_t index) const
	{
		mDocIndex = index;
	}

private:
	cdstring				mName;
	cdstring				mPrefix;
	mutable uint32_t		mDocIndex;

	void _copy(const XMLNamespace& copy)
	{
		mName = copy.mName;
		mPrefix = copy.mPrefix;
		mDocIndex = copy.mDocIndex;
	}
};

}
#endif
