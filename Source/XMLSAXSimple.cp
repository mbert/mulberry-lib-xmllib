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

// Source for XMLSAXSimple class

#include "XMLSAXSimple.h"

#include <cstdlib>
#include <fstream>
#include <strstream>

using namespace xmllib;

XMLSAXSimple::XMLSAXSimple()
{
}

XMLSAXSimple::~XMLSAXSimple()
{
}

void XMLSAXSimple::ParseData(const char* data)
{
	mBuffer.SetData(data);
	ParseIt();
}

void XMLSAXSimple::ParseFile(const char* file)
{
	std::ifstream fin(file);
	if (fin.fail())
		return;

	mBuffer.SetStream(fin);
	ParseIt();
}

void XMLSAXSimple::ParseStream(std::istream& is)
{
	if (is.fail())
		return;

	mBuffer.SetStream(is);
	ParseIt();
}

void XMLSAXSimple::ParseIt()
{
	// Always skip whitespace before the first real data
	SkipWS();

	while(!mBuffer.fail())
	{
		EXMLTag tag = GetCurrentTag();
		
		// If error then end document
		if (mBuffer.fail())
		{
			// End document if it was started
			if (mDocument != NULL)
				EndDocument();
			return;
		}

		switch(tag)
		{
		case TAG_NONE:
			// Have character data - parse as much as possible
			ParseCharacters();
			break;
		
		case TAG_CDATA:
			// Have character data - parse into character buffer
			ParseCDATA();
			break;
		
		case TAG_DOCTYPE:
			if (!ParseDoctype())
			{
				FatalError("Could not parse <!DOCTYPE ... >");
				return;
			}
			break;
		
		case TAG_DECLARATION:
			if (!ParseDeclaration())
			{
				FatalError("Could not parse <?xml ... ?>");
				return;
			}
			
			// If we have a declaration we are at the start of the document - but check we
			// only do this once
			if (mDocument != NULL)
			{
				FatalError("Multiple declarations");
				return;
			}
			
			// Now do start callback
			StartDocument();
			break;

		case TAG_COMMENT:
			if (!ParseComment())
			{
				FatalError("Could not parse comment");
				return;
			}
			break;

		case TAG_PROCESSING:
			if (!ParseProcessing())
			{
				FatalError("Could not parse processing");
				return;
			}
			break;

		case TAG_ELEMENT_END:
			if (!ParseElementEnd())
			{
				FatalError("Could not parse element");
				return;
			}
			break;

		case TAG_ELEMENT:
			if (!ParseElement())
			{
				FatalError("Could not parse element");
				return;
			}
			break;
		}
	}
}

XMLSAXSimple::EXMLTag XMLSAXSimple::GetCurrentTag()
{
	EXMLTag tag = TAG_NONE;

	// Need up to 9 characters worth in the buffer
	mBuffer.NeedData(9);

	if (::strncmp(mBuffer.next(), "<![CDATA[", 9) == 0)
	{
		tag = TAG_CDATA;
		mBuffer += 9;
	}
	else if (::strncmp(mBuffer.next(), "<!DOCTYPE", 9) == 0)
	{
		tag = TAG_DOCTYPE;
		mBuffer += 9;
	}
	else if (::strncmp(mBuffer.next(), "<?xml", 5) == 0)
	{
		tag = TAG_DECLARATION;
		mBuffer += 5;
	}
	else if (::strncmp(mBuffer.next(), "<!--", 4) == 0)
	{
		tag = TAG_COMMENT;
		mBuffer += 4;
	}
	else if (::strncmp(mBuffer.next(), "<?", 2) == 0)
	{
		tag = TAG_PROCESSING;
		mBuffer += 2;
	}
	else if (::strncmp(mBuffer.next(), "</", 2) == 0)
	{
		tag = TAG_ELEMENT_END;
		mBuffer += 2;
	}
	else if (*mBuffer == '<')
	{
		tag = TAG_ELEMENT;
		mBuffer += 1;
	}
	return tag;
}

void XMLSAXSimple::SkipWS()
{
	// Skip over each whitespace character
	while(!mBuffer.fail() && isspace(*mBuffer))
		mBuffer++;
}

bool XMLSAXSimple::ParseDoctype()
{
	// Look for ending '>'
	while(!mBuffer.fail() && (*mBuffer != '>'))
		mBuffer++;
	
	// Punt over '>'
	if (mBuffer.fail())
		return false;
	mBuffer++;

	return true;
}

bool XMLSAXSimple::ParseDeclaration()
{
	// Get first '?'
	while(!mBuffer.fail() && (*mBuffer != '?'))
		mBuffer++;
	
	// Punt over '?'
	if (mBuffer.fail())
		return false;
	mBuffer++;

	// Punt over '>'
	if (mBuffer.fail())
		return false;
	mBuffer++;

	return true;
}

bool XMLSAXSimple::ParseComment()
{
	while(!mBuffer.fail())
	{
		// Get first '-'
		while(!mBuffer.fail() && (*mBuffer != '-'))
			mBuffer++;
		
		// Punt over '-'
		if (mBuffer.fail())
			return false;
		mBuffer++;

		// Punt over '-'
		if (mBuffer.fail())
			return false;

		// Look for second '-'
		if (*mBuffer == '-')
		{
			// Punt over '-'
			mBuffer++;

			// Punt over '-'
			if (mBuffer.fail())
				return false;

			// Look for second '>'
			if (*mBuffer == '>')
				return true;
		}
	}
	
	return false;
}

bool XMLSAXSimple::ParseProcessing()
{
	// Get first '?'
	while(!mBuffer.fail() && (*mBuffer != '?'))
		mBuffer++;
	
	// Punt over '?'
	if (mBuffer.fail())
		return false;
	mBuffer++;

	// Punt over '>'
	return !mBuffer.fail() && (*mBuffer++ == '>');
}

char cValidElementName[256] = 
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0x00 - 0x0F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0x10 - 0x1F 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 	// 0x20 - 0x2F 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0x30 - 0x3F 
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 	// 0x40 - 0x4F 
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,  	// 0x50 - 0x5F
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  	// 0x60 - 0x6F
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  	// 0x70 - 0x7F
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0x80 - 0x8F 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0x90 - 0x9F 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0xA0 - 0xAF 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0xB0 - 0xBF 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0xC0 - 0xCF 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0xD0 - 0xDF 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0xE0 - 0xEF 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 	// 0xF0 - 0xFF 
};

const size_t cBufSize = 256;

bool XMLSAXSimple::ParseName(cdstring& name)
{
	// Get element name
	cdstring buffer;
	buffer.reserve(cBufSize);
	char* p = buffer.c_str_mod();
	unsigned long ctr = 0;
	while(!mBuffer.fail() && (cValidElementName[(unsigned char)*mBuffer] == 0x01))
	{
		*p++ = *mBuffer++;
		if (++ctr == cBufSize)
		{
			name.append(buffer, 0, ctr);
			p = buffer.c_str_mod();
			ctr = 0;
		}
	}

	// Write remainder from buffer
	name.append(buffer, 0, ctr);
	
	return !mBuffer.fail();
}

bool XMLSAXSimple::ParseAttributeValue(cdstring& value)
{
	// Character to match at the end
	char match = *mBuffer++;

	// Get attribute text
	cdstring buffer;
	buffer.reserve(cBufSize);
	char* p = buffer.c_str_mod();
	unsigned long ctr = 0;
	while(!mBuffer.fail() && (*mBuffer != match))
	{
		*p++ = *mBuffer++;
		if (++ctr == cBufSize)
		{
			value.append(buffer, 0, ctr);
			p = buffer.c_str_mod();
			ctr = 0;
		}
	}
	
	// Write remainder from buffer
	value.append(buffer, 0, ctr);
	
	// Punt over matching end character
	if (!mBuffer.fail())
		mBuffer++;
	
	// Must decode common entities
	XMLDecode(value);

	return !mBuffer.fail();
}

bool XMLSAXSimple::ParseElement()
{
	// Get element name
	cdstring name;
	if (!ParseName(name))
	{
		FatalError("Could not parse element name");
		return false;
	}

	// Skip ws
	SkipWS();
	
	// Look for attribute or end of tag
	XMLAttributeList attribs;
	while(!mBuffer.fail() && (*mBuffer != '/') && (*mBuffer != '>'))
	{
		// Get attribute name
		cdstring aname;
		if (!ParseName(aname))
		{
			FatalError("Could not parse attribute name");
			return false;
		}
		
		// Must have '='
		if ((*mBuffer++ != '=') || mBuffer.fail())
		{
			FatalError("Could not parse attribute value");
			return false;
		}

		// Get attribute value
		cdstring avalue;
		if (!ParseAttributeValue(avalue))
		{
			FatalError("Could not parse attribute name");
			return false;
		}

		// Now add attribute to list
		attribs.push_back(new XMLAttribute(aname, avalue));

		// Skip ws
		SkipWS();
	}
	
	// See what is next
	if (*mBuffer == '/')
	{
		// Punt over '/'
		if (!mBuffer.fail())
			mBuffer++;

		if (!mBuffer.fail() && *mBuffer++ == '>')
		{
			StartElement(name, attribs);
			EndElement(name);
			return true;
		}
		else
		{
			FatalError("Illegal character in element");
			return false;
		}
	}
	else if (*mBuffer == '>')
	{
		// Punt over '>'
		if (!mBuffer.fail())
			mBuffer++;

		// We have an element
		StartElement(name, attribs);
		return true;
	}
	
	// Must have an error if we get here
	FatalError("Could not parse element");
	return false;
}

bool XMLSAXSimple::ParseElementEnd()
{	
	// Get element name
	cdstring name;
	if (!ParseName(name))
	{
		FatalError("Could not parse element name");
		return false;
	}

	// Skip ws
	SkipWS();
	
	// Check for and punt '>'
	if(mBuffer.fail() && (*mBuffer != '>'))
	{
		FatalError("Could not parse element end");
		return false;
	}
	mBuffer++;

	// We have an element end
	EndElement(name);
	return true;
}

bool XMLSAXSimple::ParseCharacters()
{
	// Read legal characters
	std::ostrstream data;
	bool only_whitespace = true;
	while(!mBuffer.fail() && (*mBuffer != '<'))
	{
		// Look for entity
		if (*mBuffer == '&')
		{
			only_whitespace = false;
			mBuffer++;
			cdstring amp;
			while(!mBuffer.fail() && (*mBuffer != ';'))
			{
				amp += *mBuffer++;
			}
			if (!mBuffer.fail())
				mBuffer++;
			if (amp[(cdstring::size_type)0] == '#')
			{
				unsigned long decoded = 0;
				if (amp[(cdstring::size_type)1] == 'x')
				{
					decoded = std::strtoul(&amp[(cdstring::size_type)2], NULL, 16);
				}
				else
				{
					decoded = std::strtoul(&amp[(cdstring::size_type)1], NULL, 10);
				}
				
				// Must be less than or equal to 0xFF for utf8
				if (decoded < 0x100)
					data.put((char)decoded);
			}
			else if (amp.compare("amp", true) == 0)
				data.put('&');
			else if (amp.compare("lt", true) == 0)
				data.put('<');
			else if (amp.compare("gt", true) == 0)
				data.put('>');
			else if (amp.compare("apos", true) == 0)
				data.put('\'');
			else if (amp.compare("quote", true) == 0)
				data.put('"');
		}
		else
		{
			char c = *mBuffer++;
			
			// Do whitespace test only if still required
			if (only_whitespace)
			{
				switch(c)
				{
				case '\t':
				case '\r':
				case '\n':
				case ' ':
					break;
				default:
					only_whitespace = false;
					break;
				}
			}
			
			// Write data
			data.put(c);
		}
	}
	
	// Grab the data
	data << std::ends;
	cdstring temp;
	temp.steal(data.str());

	// Now do callback if data contains more than just whitespace
	if (!only_whitespace)
		Characters(temp);

	return !mBuffer.fail();
}

bool XMLSAXSimple::ParseCDATA()
{
	// Read legal characters
	std::ostrstream data;
	while(!mBuffer.fail())
	{
		// Look for entity
		if (*mBuffer == ']')
		{
			// Look for ']]>' termination
			mBuffer.NeedData(3);
			if ((mBuffer.next()[1] == ']') && (mBuffer.next()[2] == '>'))
			{
				mBuffer += 3;
				break;				
			}
		}

		data.put(*mBuffer++);
	}
	
	// Grab the data
	data << std::ends;
	cdstring temp;
	temp.steal(data.str());

	// Now do callback
	Characters(temp);
	
	// Punt oive
	return !mBuffer.fail();
}

void XMLSAXSimple::XMLDecode(cdstring& value)
{
	// Look for any entities
	if (value.find('&') == std::string::npos)
		return;
	
	std::ostrstream data;
	std::string::size_type start_pos = 0;
	std::string::size_type end_pos = value.find('&', start_pos);
	while(end_pos != std::string::npos)
	{
		// Copy up to the current position
		data.write(value.c_str() + start_pos, end_pos - start_pos);
		
		// Now do entity lookup
		
		// Adjust pointers and do another search
		start_pos = end_pos;
		end_pos = value.find('&', start_pos);
	}
	
	// Write out the remainder
	if (start_pos < value.length())
		data.write(value.c_str() + start_pos, value.length() - start_pos);
	
	// Grab the data
	data << std::ends;
	value.steal(data.str());
}
