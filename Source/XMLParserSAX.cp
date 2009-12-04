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

// Source for XMLParserSAX class

#include "XMLParserSAX.h"

#include "XMLDocument.h"

using namespace xmllib;

XMLParserSAX::XMLParserSAX()
{
	mDocument = NULL;
	mError = false;
}

XMLParserSAX::~XMLParserSAX()
{
	delete mDocument;
}

void XMLParserSAX::HandleException(const std::exception& ex)
{
	mError = true;
}

void XMLParserSAX::StartDocument()
{
	// Create the document with its root element
	mDocument = new XMLDocument;
}

void XMLParserSAX::EndDocument()
{
	// Nothing to do
}

void XMLParserSAX::StartElement(const cdstring& name, const XMLAttributeList& attributes)
{
	// Don't bother if on error state
	if (mError)
		return;
	
	// We always need a document
	if (mDocument == NULL)
		StartDocument();

	try
	{
		// See if this is the first one
		XMLNode* node;
		if (mNodeList.size() == 0)
		{
			// Get the root node and change its name to the real root
			node = mDocument->GetRoot();
			node->SetName(name);
		}
		else
			// Create a new node
			node = new XMLNode(mDocument, mNodeList.back(), name);
		node->SetAttributes(attributes);
		node->DetermineNamespace();
		
		// Push onto stack
		mNodeList.push_back(node);
	}
	catch (const std::exception& e)
	{
		HandleException(e);
	}
}

void XMLParserSAX::EndElement(const cdstring& name)
{
	// Don't bother if on error state
	if (mError)
		return;

	try
	{
		// Pop current item off the stack
		mNodeList.pop_back();
	}
	catch(const std::exception& e)
	{
		HandleException(e);
	}
}

void XMLParserSAX::Characters(const cdstring& data)
{
	// Don't bother if on error state
	if (mError)
		return;

	try
	{
		// Add data to current stack element
		if (mNodeList.size() && (mNodeList.back() != NULL))
			mNodeList.back()->AppendData(data);
	}
	catch(const std::exception& e)
	{
		HandleException(e);
	}
}

void XMLParserSAX::Comment(const cdstring& text)
{
	// Nothing to do
}

void XMLParserSAX::Warning(const cdstring& text)
{
	// Nothing to do
}

void XMLParserSAX::Error(const cdstring& text)
{
	// Set error state
	mError = true;
}

void XMLParserSAX::FatalError(const cdstring& text)
{
	// Set error state
	mError = true;
}
