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

// Header for XMLNode class

#ifndef __XMLNODE__XMLLIB__
#define __XMLNODE__XMLLIB__

#include "XMLAttribute.h"
#include "XMLNamespace.h"

#include <stdint.h>
#include <map>


#include "cdstring.h"

namespace xmllib
{

class XMLNode;
typedef std::list<XMLNode*> XMLNodeList;
typedef std::map<cdstring, XMLNode*> XMLNodeMap;

class XMLDocument;
class XMLNamespace;
class XMLName;

class XMLNode
{
public:
	XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name)
	{
		_init(doc, parent, name);
	}
	XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace& namespc)
	{
		_init(doc, parent, name, &namespc);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const XMLName& name);
	XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const cdstring& data)
	{
		_init(doc, parent, name);
		SetData(data);
	}
	XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace& namespc, const cdstring& data)
	{
		_init(doc, parent, name, &namespc);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const char* data)
	{
		_init(doc, parent, name);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace& namespc, const char* data)
	{
		_init(doc, parent, name, &namespc);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, uint32_t data)
	{
		_init(doc, parent, name);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace& namespc, uint32_t data)
	{
		_init(doc, parent, name, &namespc);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, int32_t data)
	{
		_init(doc, parent, name);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace& namespc, int32_t data)
	{
		_init(doc, parent, name, &namespc);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, bool data)
	{
		_init(doc, parent, name);
		SetData(data);
	}
	explicit XMLNode(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace& namespc, bool data)
	{
		_init(doc, parent, name, &namespc);
		SetData(data);
	}
	explicit XMLNode(const XMLNode& copy)
		{ _copy(copy); }
	explicit XMLNode(const XMLNode& copy, XMLNode* parent)
		{ mParent = parent; _copy(copy); }
	~XMLNode();

	XMLNode& operator=(const XMLNode& copy)
		{ if (this != &copy) _copy(copy); return *this; }
	
	// Name
	const cdstring& Name() const
		{ return mName; }
	void SetName(const cdstring& name)
		{ mName = name; }
	void SetName(const cdstring& name, const XMLNamespace& namespc);
	void SetName(const XMLName& name);

	bool CompareFullName(const XMLName& xmlname) const;

	// Namespace
	void AddNamespace(const XMLNamespace& namespc);
	const cdstring& Namespace() const;

	// Data content
	const cdstring& Data() const
		{ return mData; }
	bool DataValue(cdstring& value) const;
	bool DataValue(uint32_t& value) const;
	bool DataValue(int32_t& value) const;
	bool DataValue(bool& value) const;
	void SetData(const cdstring& data)
		{ mData = data; }
	void SetData(const char* data)
		{ mData = data; }
	void SetData(uint32_t data);
	void SetData(int32_t data);
	void SetData(bool data);
	void AppendData(const cdstring& data)
		{ mData += data; }

	// Attributes
	const XMLAttributeList& Attributes() const
		{ return mAttributeList; }
	void SetAttributes(const XMLAttributeList& attributes);

	bool HasAttribute(const cdstring& name) const;
	XMLAttribute* Attribute(const cdstring& name);
	const XMLAttribute* Attribute(const cdstring& name) const
		{ return const_cast<XMLNode*>(this)->Attribute(name); }

	bool AttributeValue(const cdstring& name, cdstring& value) const;
	bool AttributeValue(const cdstring& name, uint32_t& value) const;
	bool AttributeValue(const cdstring& name, int32_t& value) const;
	bool AttributeValue(const cdstring& name, uint32_t& index, const char** array) const;
	bool AttributeValue(const cdstring& name, bool& value) const;

	void AddAttribute(const cdstring& name, const cdstring& value);
	void AddAttribute(const cdstring& name, const char* value)
	{
		// Convert to string and add
		AddAttribute(name, cdstring(value));
	}
	void AddAttribute(const cdstring& name, uint32_t value);
	void AddAttribute(const cdstring& name, int32_t value);
	void AddAttribute(const cdstring& name, uint32_t index, const char** array);
	void AddAttribute(const cdstring& name, bool value);
	void AddAttribute(XMLAttribute* value);

	void RemoveAttribute(const cdstring& name);

	// Child nodes
	const XMLNodeList& Children() const
		{ return mChildren; }
	void SetChildren(const XMLNodeList& children);
	void AddChild(XMLNode* child);
	const XMLNode* GetChild(const cdstring& name) const;
	const XMLNode* GetChild(const XMLName& name) const;
	const XMLNodeMap* ChildMap() const;

	// Namespace handling
	void DetermineNamespace();
	uint32_t GetNamespaceIndexFromPrefix(const cdstring& prefix) const;
	cdstring GetFullName() const;
	cdstring GetPrefixName() const;

	// Generating XML
	void Generate(std::ostream& os, uint32_t level = 0, bool indent = true);
	void GenerateChildren(std::ostream& os, uint32_t level = 0, bool indent = true);
	
	// Useful for debugging
	void DebugPrint(std::ostream& os, uint32_t level = 0) const;

private:
	typedef std::map<cdstring, uint32_t>	XMLNamespaceLookup;

	XMLDocument*		mDocument;
	XMLNode*			mParent;

	cdstring			mName;
	cdstring			mData;
	
	XMLAttributeList	mAttributeList;
	XMLAttributeMap		mAttributeMap;
	
	XMLNodeList			mChildren;

	uint32_t			mNamespaceIndex;
	bool				mNamespaceDefault;
	XMLNamespaceLookup	mNamespaceLookup;

	void _init(XMLDocument* doc, XMLNode* parent, const cdstring& name, const XMLNamespace* namespc = NULL);
	void _copy(const XMLNode& copy);
	
	void CleanAttributes();
	void CleanChildren();
};

}
#endif
