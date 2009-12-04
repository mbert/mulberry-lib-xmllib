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

// Header for XMLObject class

#ifndef __XMLOBJECT__XMLLIB__
#define __XMLOBJECT__XMLLIB__

#include <stdint.h>


#include "cdstring.h"

#include "XMLName.h"
#include "XMLNode.h"

namespace xmllib
{

class XMLDocument;

class XMLObject
{
public:
	XMLObject(const XMLName& key)
	{
		mKey = key;
	}
	XMLObject(const XMLObject& copy)
	{
		mKey = copy.mKey;
	}
	virtual ~XMLObject() {}

	XMLObject& operator=(const XMLObject& copy)		// assign to self
		{ if (this != &copy) mKey = copy.mKey; return *this; }

	const XMLName& GetKey() const
	{
		return mKey;
	}

	// Public read/write
			void ReadXMLFromParent(const XMLNode* parent);
	virtual void ReadXML(const XMLNode* node) = 0;
			void WriteXMLToParent(XMLDocument* doc, XMLNode* parent) const;
	virtual void WriteXML(XMLDocument* doc, XMLNode* node) const = 0;

	template <typename T> static bool ReadValue(const XMLNode* parent, const XMLName& child_name, T& value);
						  static uint32_t ReadValueEnum(const XMLNode* parent, const XMLName& child_name, const char** sarray, uint32_t default_index);
	template <typename T> static void WriteValue(XMLDocument* doc, XMLNode* parent, const XMLName& child_name, const T& value);
						  static void WriteValueEnum(XMLDocument* doc, XMLNode* parent, const XMLName& child_name, uint32_t index, const char** sarray);

	// Read
	static bool ReadData(const XMLNode* node, cdstring& value);
	static bool ReadData(const XMLNode* node, uint32_t& value, bool use_stdattribute = true);
	static bool ReadData(const XMLNode* node, int32_t& value, bool use_stdattribute = true);
	static bool ReadData(const XMLNode* node, bool& value, bool use_stdattribute = true);
	static uint32_t ReadDataEnum(const XMLNode* node, const char** sarray, uint32_t default_index, bool use_stdattribute = true);

	static bool ReadAttribute(const XMLNode* node, const cdstring& name, cdstring& value);
	static bool ReadAttribute(const XMLNode* node, const cdstring& name, uint32_t& value);
	static bool ReadAttribute(const XMLNode* node, const cdstring& name, int32_t& value);
	static bool ReadAttribute(const XMLNode* node, const cdstring& name, bool& value);
	static uint32_t ReadAttributeEnum(const XMLNode* node, const cdstring& name, const char** sarray, uint32_t default_index);
	
	// Write
	static void WriteData(XMLNode* node, const cdstring& value);
	static void WriteData(XMLNode* node, uint32_t value, bool use_stdattribute = true);
	static void WriteData(XMLNode* node, int32_t value, bool use_stdattribute = true);
	static void WriteData(XMLNode* node, bool value, bool use_stdattribute = true);
	static void WriteDataEnum(XMLNode* node, uint32_t index, const char** sarray, bool use_stdattribute = true);

	static void WriteAttribute(XMLNode* node, const cdstring& name, const cdstring& value);
	static void WriteAttribute(XMLNode* node, const cdstring& name, uint32_t value);
	static void WriteAttribute(XMLNode* node, const cdstring& name, int32_t value);
	static void WriteAttribute(XMLNode* node, const cdstring& name, bool value);
	static void WriteAttributeEnum(XMLNode* node, const cdstring& name, uint32_t index, const char** sarray);

protected:
	XMLName mKey;
};

template <typename T> bool XMLObject::ReadValue(const XMLNode* parent, const XMLName& child_name, T& value)
{
	// Get single child node
	const XMLNode* child = parent->GetChild(child_name);
	if (child != NULL)
		return ReadData(child, value);
	else
		return false;
}

template <typename T> void XMLObject::WriteValue(XMLDocument* doc, XMLNode* parent, const XMLName& child_name, const T& value)
{
	XMLNode* child = new XMLNode(doc, parent, child_name);
	WriteData(child, value);
}

}
#endif
