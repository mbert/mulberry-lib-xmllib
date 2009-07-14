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

// Source for XMLNode class

#include "XMLNode.h"

#include "XMLDocument.h"
#include "XMLName.h"
#include "XMLNamespace.h"

#include <cstdlib>
#include <ostream>

namespace xmllib
{

XMLNode::XMLNode(XMLDocument* doc, XMLNode* parent, const XMLName& name)
{
	if (name.Namespace() != NULL)
	{
		XMLNamespace temp(name.Namespace());
		_init(doc, parent, name.Name(), &temp);
	}
	else
		_init(doc, parent, name.Name());
}

XMLNode::~XMLNode()
{
	// Clean out list items
	CleanAttributes();
	CleanChildren();
}

void XMLNode::_init(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace* namespc)
{
	mDocument = doc;
	mParent = parent;
	mName = name;
	if (namespc != NULL)
	{
		mNamespaceIndex = namespc->HasIndex() ? namespc->Index() : doc->AddNamespace(*namespc);
		mNamespaceDefault = (mNamespaceIndex == 0);
	}
	else
	{
		mNamespaceIndex = 0;
		mNamespaceDefault = true;
	}
	
	// Add to parent
	if (mParent != NULL)
		mParent->AddChild(this);
}

void XMLNode::_copy(const XMLNode& copy)
{
	mDocument = copy.mDocument;

	mName = copy.mName;
	mData = copy.mData;
	mChildren = copy.mChildren;
	
	// Must reset parent of copied children to this one
	for(XMLNodeList::iterator iter = mChildren.begin(); iter != mChildren.end(); iter++)
	{
		(*iter)->mParent = this;
	}
	
	CleanAttributes();
	SetAttributes(copy.mAttributeList);
	
	mNamespaceIndex = copy.mNamespaceIndex;
	mNamespaceDefault = copy.mNamespaceDefault;
}

void XMLNode::SetName(const cdstring& name, const XMLNamespace& namespc)
{
	mName = name;
	mNamespaceIndex = namespc.HasIndex() ? namespc.Index() : mDocument->AddNamespace(namespc);
	mNamespaceDefault = mNamespaceIndex == 0;
}

void XMLNode::SetName(const XMLName& name)
{
	mName = name.Name();
	if (name.Namespace() != NULL)
	{
		XMLNamespace temp(name.Namespace());
		mNamespaceIndex = mDocument->AddNamespace(temp);
		mNamespaceDefault = (mNamespaceIndex == 0);
	}
	else
	{
		mNamespaceIndex = 0;
		mNamespaceDefault = true;
	}
}

bool XMLNode::CompareFullName(const XMLName& xmlname) const
{
	return (mName == xmlname.Name()) &&
			(mDocument->GetNamespace(mNamespaceIndex) == xmlname.Namespace());
}

// Create attribute defining the namespace
void XMLNode::AddNamespace(const XMLNamespace& namespc)
{
	cdstring xmlns = "xmlns:";
	xmlns += namespc.Prefix();
	AddAttribute(xmlns, namespc.Name());
}

const cdstring& XMLNode::Namespace() const
{
	return mDocument->GetNamespace(mNamespaceIndex);
}

bool XMLNode::DataValue(cdstring& value) const
{
	value = mData;
	return true;
}

bool XMLNode::DataValue(uint32_t& value) const
{
	if (mData.empty())
		return false;
	else
	{
		value = strtoul(mData.c_str(), NULL, 10);
		return true;
	}
}

bool XMLNode::DataValue(int32_t& value) const
{
	if (mData.empty())
		return false;
	else
	{
		value = strtol(mData.c_str(), NULL, 10);
		return true;
	}
}

const char* cXMLValueTrue = "true";
const char* cXMLValueFalse = "false";

bool XMLNode::DataValue(bool& value) const
{
	if (mData.empty())
		return false;
	else
	{
		value = (mData == cXMLValueTrue);
		return true;
	}
}

void XMLNode::SetData(uint32_t data)
{
	mData = data;
}

void XMLNode::SetData(int32_t data)
{
	mData = data;
}

void XMLNode::SetData(bool data)
{
	mData = data ? cXMLValueTrue : cXMLValueFalse;
}

void XMLNode::CleanAttributes()
{
	// Delete each attribute in the list
	for(XMLAttributeList::iterator iter = mAttributeList.begin(); iter != mAttributeList.end(); iter++)
		delete *iter;
	mAttributeList.clear();
	mAttributeMap.clear();
}

void XMLNode::CleanChildren()
{
	// Delete each attribute in the list
	for(XMLNodeList::iterator iter = mChildren.begin(); iter != mChildren.end(); iter++)
		delete *iter;
	mChildren.clear();
}

void XMLNode::SetAttributes(const XMLAttributeList& attributes)
{
	// Clean out old set
	CleanAttributes();
	
	// Add each new one to list and map
	for(XMLAttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); iter++)
	{
		XMLAttribute* attr = new XMLAttribute(**iter);
		if (attr)
		{
			mAttributeList.push_back(attr);
			mAttributeMap[attr->Name()] = attr;
		}
	}
}

bool XMLNode::HasAttribute(const cdstring& name) const
{
	// Check map for item
	XMLAttributeMap::const_iterator found = mAttributeMap.find(name);
	return (found != mAttributeMap.end());
}

XMLAttribute* XMLNode::Attribute(const cdstring& name)
{
	// Find it
	XMLAttributeMap::const_iterator found = mAttributeMap.find(name);
	if (found != mAttributeMap.end())
		return found->second;
	else
		return NULL;
}

bool XMLNode::AttributeValue(const cdstring& name, cdstring& value) const
{
	const XMLAttribute* found = Attribute(name);
	if (found)
	{
		value = found->Value();
		return true;
	}
	else
		return false;	
}

bool XMLNode::AttributeValue(const cdstring& name, uint32_t& value) const
{
	const XMLAttribute* found = Attribute(name);
	if (found)
	{
		value = strtoul(found->Value().c_str(), NULL, 10);
		return true;
	}
	else
		return false;	
}

bool XMLNode::AttributeValue(const cdstring& name, int32_t& value) const
{
	const XMLAttribute* found = Attribute(name);
	if (found)
	{
		value = strtol(found->Value().c_str(), NULL, 10);
		return true;
	}
	else
		return false;	
}

bool XMLNode::AttributeValue(const cdstring& name, uint32_t& index, const char** array) const
{
	// Get the string value
	cdstring txt;
	AttributeValue(name, txt);

	// Loop over all items in NULL terminated array looking for a match
	index = 0;
	const char* p = array[index];
	while(p != NULL)
	{
		if (txt == p)
			return true;
		
		p = array[++index];
	}
	
	index = 0;
	return false;
}

bool XMLNode::AttributeValue(const cdstring& name, bool& value) const
{
	const XMLAttribute* found = Attribute(name);
	if (found)
	{
		value = found->Value() == cXMLValueTrue;
		return true;
	}
	else
		return false;	
}

void XMLNode::AddAttribute(const cdstring& name, const cdstring& value)
{
	// Does it already exist
	if (mAttributeMap.find(name) != mAttributeMap.end())
		return;
	
	// Create the new attribute
	XMLAttribute* attr = new XMLAttribute(name, value);
	if (attr)
	{
		mAttributeList.push_back(attr);
		mAttributeMap[attr->Name()] = attr;
	}
}

void XMLNode::AddAttribute(const cdstring& name, uint32_t value)
{
	// Convert to string and add
	AddAttribute(name, cdstring(value));
}

void XMLNode::AddAttribute(const cdstring& name, uint32_t index, const char** array)
{
	// We will assume that the index has been checked against the size of the array already
	
	AddAttribute(name, array[index]);
}

void XMLNode::AddAttribute(const cdstring& name, bool value)
{
	// Convert to string and add
	AddAttribute(name, value ? cXMLValueTrue : cXMLValueFalse);
}

void XMLNode::AddAttribute(XMLAttribute* attr)
{
	if (attr == NULL)
		return;

	// Delete existing one
	if (HasAttribute(attr->Name()))
		RemoveAttribute(attr->Name());
	
	// Add to list and map
	mAttributeList.push_back(attr);
	mAttributeMap[attr->Name()] = attr;
}

void XMLNode::RemoveAttribute(const cdstring& name)
{
	// Must exist
	XMLAttributeMap::iterator found = mAttributeMap.find(name);
	if (found != mAttributeMap.end())
	{
		// Get the attribute
		XMLAttribute* attr = found->second;

		// Remove from list and map
		mAttributeList.remove(attr);
		mAttributeMap.erase(found);
		
		// Delete it
		delete attr;
	}
}

void XMLNode::SetChildren(const XMLNodeList& children)
{
	// Clean out old set
	CleanChildren();
	
	// Add each new one to list and map
	for(XMLNodeList::const_iterator iter = children.begin(); iter != children.end(); iter++)
	{
		XMLNode* child = new XMLNode(**iter, this);
		if (child)
			mChildren.push_back(child);
	}
}

void XMLNode::AddChild(XMLNode* child)
{
	// Just add to list if it exists
	if (child)
		mChildren.push_back(child);
}

const XMLNode* XMLNode::GetChild(const cdstring& name) const
{
	// Find the first one with the required name
	for(XMLNodeList::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++)
	{
		if ((*iter)->GetFullName() == name)
			return *iter;
	}
	
	return NULL;
}

const XMLNode* XMLNode::GetChild(const XMLName& name) const
{
	// Find the first one with the required name
	for(XMLNodeList::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++)
	{
		if ((*iter)->CompareFullName(name))
			return *iter;
	}
	
	return NULL;
}

const XMLNodeMap* XMLNode::ChildMap() const
{
	XMLNodeMap* result = new XMLNodeMap;

	// Now add children
	for(XMLNodeList::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++)
	{
		result->insert(XMLNodeMap::value_type((*iter)->Name(), *iter));
	}
	
	return result;
}

void XMLNode::DetermineNamespace()
{
	// Look for xmlns attributes
	
	// Look for the default one first
	bool had_default = false;
	if (mAttributeMap.count("xmlns") != 0)
	{
		cdstring default_ns = (*mAttributeMap.find("xmlns")).second->Value();
		mNamespaceIndex = mDocument->AddNamespace(XMLNamespace(default_ns));
		mNamespaceDefault = true;
		had_default = true;
	}
	
	// Now look for prefixes
	for(XMLAttributeList::const_iterator iter = mAttributeList.begin(); iter != mAttributeList.end(); iter++)
	{
		if ((*iter)->Name().compare(0, 6, "xmlns:") == 0)
		{
			// Get the prefix and namespace name (attribute value)
			cdstring prefix((*iter)->Name(), 6, cdstring::npos);
			cdstring ns = (*iter)->Value();
			
			// Add namespace to document
			uint32_t ns_index = mDocument->AddNamespace(XMLNamespace(ns, prefix));
			
			// Add to map
			mNamespaceLookup.insert(XMLNamespaceLookup::value_type(prefix, ns_index));
		}
	}
	
	// Now determine this elements namespace
	cdstring::size_type cpos = mName.find(':');
	if (cpos != cdstring::npos)
	{
		// Get the prefix
		cdstring prefix(mName, 0, cpos);
		
		// Lookup the index
		mNamespaceIndex = GetNamespaceIndexFromPrefix(prefix);
		mNamespaceDefault = false;
		
		// Reset the name to exclude the prefix
		mName.erase(0, cpos + 1);
	}
	else if (!had_default)
	{
		// Find the parent that has a default namepsace definition and use that
		XMLNode* parent = mParent;
		while (parent != NULL)
		{
			if (parent->mNamespaceDefault)
			{
				mNamespaceIndex = parent->mNamespaceIndex;
				break;
			}
			else
				parent = parent->mParent;
		}
	}
}

uint32_t XMLNode::GetNamespaceIndexFromPrefix(const cdstring& prefix) const
{
	// See if its in this node's prefix map
	XMLNamespaceLookup::const_iterator found = mNamespaceLookup.find(prefix);
	if (found != mNamespaceLookup.end())
	{
		return (*found).second;
	}
	
	// Otherwise ask parent node
	return mParent != NULL ? mParent->GetNamespaceIndexFromPrefix(prefix) : 0;
}

cdstring XMLNode::GetFullName() const
{
	cdstring result = mDocument->GetNamespace(mNamespaceIndex);
	result += Name();
	
	return result;
}

cdstring XMLNode::GetPrefixName() const
{
	cdstring result = mDocument->GetNamespacePrefix(mNamespaceIndex);
	if (!result.empty())
		result += ":";
	result += Name();
	
	return result;
}

void XMLNode::Generate(std::ostream& os, uint32_t level, bool indent)
{
	// Initially we will not do xmlns shortcuts
	
	// Indent
	if (indent)
	{
		for(uint32_t ctr = 0; ctr < level; ctr++)
			os << "\t";
	}

	// Do name with prefix namespace
	os << "<" << GetPrefixName();
	
	// Do each attribute
	for(XMLAttributeList::const_iterator iter = mAttributeList.begin(); iter != mAttributeList.end(); iter++)
	{
		os << " " << (*iter)->Name() << "=\"" << (*iter)->Value() << "\"";
	}
	
	// See if we have an empty tag and close it
	if (mData.empty() && mChildren.empty())
	{
		os << "/>" << std::endl;
		return;
	}
	else
		os << ">";
	
	// Do children
	if (mChildren.size())
	{
		os << std::endl;
		GenerateChildren(os, level + 1, indent);
	}

	// Now do data
	if (!mData.empty())
		os << mData;
	
	// Indent
	if (indent && (mChildren.size() != 0))
	{
		for(uint32_t ctr = 0; ctr < level; ctr++)
			os << "\t";
	}

	// End tag
	os << "</" << GetPrefixName() << ">" << std::endl;
}

void XMLNode::GenerateChildren(std::ostream& os, uint32_t level, bool indent)
{
	// Now do children
	for(XMLNodeList::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++)
	{
		(*iter)->Generate(os, level, indent);
	}
}

// Useful for debugging - this does not do namespace shortcuts
void XMLNode::DebugPrint(std::ostream& os, uint32_t level) const
{
	// Always start new line for new node
	os << std::endl;

	// Indent
	for(uint32_t ctr = 0; ctr < level; ctr++)
		os << "\t";

	os << "<" << GetPrefixName();
	for(XMLAttributeList::const_iterator iter = mAttributeList.begin(); iter != mAttributeList.end(); iter++)
	{
		os << " " << (*iter)->Name() << "=\"" << (*iter)->Value() << "\"";
	}
	
	// See if we have an empty tag and close it
	if (mData.empty() && mChildren.empty())
	{
		os << "/>" << std::endl;
		return;
	}
	else
		os << ">";
	
	// Now do children
	for(XMLNodeList::const_iterator iter = mChildren.begin(); iter != mChildren.end(); iter++)
	{
		(*iter)->DebugPrint(os, level + 1);
	}
	
	// Now do data
	if (!mData.empty())
		os << mData;
	
	// Indent
	if (mChildren.size() != 0)
	{
		for(uint32_t ctr = 0; ctr < level; ctr++)
			os << "\t";
	}

	// End tag
	os << "</" << GetPrefixName() << ">" << std::endl;
}

}
