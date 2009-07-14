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

// Source for XMLObject class

#include "XMLObject.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>

extern unsigned long strindexfind(const char* s, const char** ss, unsigned long default_index = 0);

namespace xmllib
{

const char* cXMLStdAttributeName = "value";
const char* cXMLStdAttributeType = "type";
const char* cXMLTrue = "true";
const char* cXMLFalse = "false";

// Read
void XMLObject::ReadXMLFromParent(const XMLNode* parent)
{
	// Get single child node
	const XMLNode* child = parent->GetChild(mKey);
	if (child != NULL)
		return ReadXML(child);
}

uint32_t XMLObject::ReadValueEnum(const XMLNode* parent, const XMLName& child_name, const char** sarray, uint32_t default_index)
{
	// Get single child node
	const XMLNode* child = parent->GetChild(child_name);
	if (child != NULL)
		return ReadDataEnum(child, sarray, default_index);
	else
		return default_index;
	
}

bool XMLObject::ReadData(const XMLNode* node, cdstring& value)
{
	value = node->Data();
	return true;
}

bool XMLObject::ReadData(const XMLNode* node, uint32_t& value, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		return ReadAttribute(node, cXMLStdAttributeName, value);
	}
	else
	{
		errno = 0;
		uint32_t temp = strtoul(node->Data().c_str(), NULL, 10);
		if (errno == 0)
		{
			value = temp;
			return true;
		}
		else
			return false;
	}
}

bool XMLObject::ReadData(const XMLNode* node, int32_t& value, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		return ReadAttribute(node, cXMLStdAttributeName, value);
	}
	else
	{
		errno = 0;
		int32_t temp = strtol(node->Data().c_str(), NULL, 10);
		if (errno == 0)
		{
			value = temp;
			return true;
		}
		else
			return false;
	}
}

bool XMLObject::ReadData(const XMLNode* node, bool& value, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		return ReadAttribute(node, cXMLStdAttributeName, value);
	}
	else
	{
		if (strcmp(node->Data().c_str(), cXMLTrue) == 0)
		{
			value = true;
			return true;
		}
		else if (strcmp(node->Data().c_str(), cXMLTrue) == 0)
		{
			value = false;
			return true;
		}
		else
			return false;
	}
}

uint32_t XMLObject::ReadDataEnum(const XMLNode* node, const char** sarray, uint32_t default_index, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		return ReadAttributeEnum(node, cXMLStdAttributeType, sarray, default_index);
	}
	else
	{
		cdstring temp = node->Data();
		return ::strindexfind(temp.c_str(), sarray, default_index);
	}
}

bool XMLObject::ReadAttribute(const XMLNode* node, const cdstring& name, cdstring& value)
{
	const XMLAttribute* attr = node->Attribute(name);
	if (attr)
	{
		value = attr->Value();
		return true;
	}
	else
		return false;
}

bool XMLObject::ReadAttribute(const XMLNode* node, const cdstring& name, uint32_t& value)
{
	const XMLAttribute* attr = node->Attribute(name);
	if (attr)
	{
		errno = 0;
		uint32_t temp = strtoul(attr->Value().c_str(), NULL, 10);
		if (errno == 0)
		{
			value = temp;
			return true;
		}
	}
	
	return false;
}

bool XMLObject::ReadAttribute(const XMLNode* node, const cdstring& name, int32_t& value)
{
	const XMLAttribute* attr = node->Attribute(name);
	if (attr)
	{
		errno = 0;
		int32_t temp = strtol(attr->Value().c_str(), NULL, 10);
		if (errno == 0)
		{
			value = temp;
			return true;
		}
	}
	
	return false;
}

bool XMLObject::ReadAttribute(const XMLNode* node, const cdstring& name, bool& value)
{
	const XMLAttribute* attr = node->Attribute(name);
	if (attr)
	{
		if (strcmp(attr->Value().c_str(), cXMLTrue) == 0)
		{
			value = true;
			return true;
		}
		else if (strcmp(attr->Value().c_str(), cXMLTrue) == 0)
		{
			value = false;
			return true;
		}
	}

	return false;
}

uint32_t XMLObject::ReadAttributeEnum(const XMLNode* node, const cdstring& name, const char** sarray, uint32_t default_index)
{
	const XMLAttribute* attr = node->Attribute(name);
	if (attr)
	{
		return ::strindexfind(attr->Value().c_str(), sarray, default_index);
	}
	else
		return default_index;
}

// Write
void XMLObject::WriteXMLToParent(XMLDocument* doc, XMLNode* parent) const
{
	// Create the top-level node
	XMLNode* node = new XMLNode(doc, parent, mKey);
	
	// Now let derived classes add to it
	WriteXML(doc, node);
}

void XMLObject::WriteValueEnum(XMLDocument* doc, XMLNode* parent, const XMLName& child_name, uint32_t index, const char** sarray)
{
	XMLNode* child = new XMLNode(doc, parent, child_name);
	WriteDataEnum(child, index, sarray);
}

void XMLObject::WriteData(XMLNode* node, const cdstring& value)
{
	node->SetData(value);
}

void XMLObject::WriteData(XMLNode* node, uint32_t value, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		WriteAttribute(node, cXMLStdAttributeName, value);
	}
	else
	{
		node->SetData(cdstring(value));
	}
}

void XMLObject::WriteData(XMLNode* node, int32_t value, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		WriteAttribute(node, cXMLStdAttributeName, value);
	}
	else
	{
		node->SetData(cdstring(value));
	}
}

void XMLObject::WriteData(XMLNode* node, bool value, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		WriteAttribute(node, cXMLStdAttributeName, value);
	}
	else
	{
		node->SetData(cdstring(value ? cXMLTrue : cXMLFalse));
	}
}

void XMLObject::WriteDataEnum(XMLNode* node, uint32_t index, const char** sarray, bool use_stdattribute)
{
	if (use_stdattribute)
	{
		WriteAttributeEnum(node, cXMLStdAttributeType, index, sarray);
	}
	else
	{
		node->SetData(cdstring(sarray[index]));
	}
}

void XMLObject::WriteAttribute(XMLNode* node, const cdstring& name, const cdstring& value)
{
	node->AddAttribute(name, value);
}

void XMLObject::WriteAttribute(XMLNode* node, const cdstring& name, uint32_t value)
{
	node->AddAttribute(name, cdstring(value));
}

void XMLObject::WriteAttribute(XMLNode* node, const cdstring& name, int32_t value)
{
	node->AddAttribute(name, cdstring(value));
}

void XMLObject::WriteAttribute(XMLNode* node, const cdstring& name, bool value)
{
	node->AddAttribute(name, cdstring(value ? cXMLTrue : cXMLFalse));
}

void XMLObject::WriteAttributeEnum(XMLNode* node, const cdstring& name, uint32_t data, const char** sarray)
{
	node->AddAttribute(name, cdstring(sarray[data]));
}

}
