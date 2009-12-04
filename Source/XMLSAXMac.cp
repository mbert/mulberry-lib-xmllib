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

// Source for XMLSAXMac class

#include "XMLSAXMac.h"

#include <CFURLAccess.h>
#include <CFXMLNode.h>
#include <strstream>

using namespace xmllib;

XMLSAXMac::XMLSAXMac()
{
}

XMLSAXMac::~XMLSAXMac()
{
}

void XMLSAXMac::ParseData(const char* data)
{
	// Load the XML data using its URL.
	CFDataRef xmlData = ::CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, (const unsigned char*) data, std::strlen(data), kCFAllocatorNull);

	// First, set up the parser callbacks.
	CFXMLParserCallBacks callbacks = {NULL, CreateStructure, AddChild, EndStructure, NULL, HandleError};
	
	// Set up context
	CFXMLParserContext context;
	context.version = 0;
	context.info = this;
	context.retain = NULL;
	context.release = NULL;
	context.copyDescription = NULL;

	// Create the parser with the option to skip whitespace.
	CFXMLParserRef parser = ::CFXMLParserCreate(kCFAllocatorDefault, xmlData, NULL, 
								kCFXMLParserSkipWhitespace, kCFXMLNodeCurrentVersion, &callbacks, &context);

	// Invoke the parser.
	::CFXMLParserParse(parser);
	   
	::CFRelease(parser);
	::CFRelease(xmlData);
}

void XMLSAXMac::ParseFile(const char* file)
{
	// String path
	CFStringRef fpath = ::CFStringCreateWithCString(kCFAllocatorDefault, file, kCFStringEncodingUTF8);

	// Create URL
	CFURLRef sourceURL;
	sourceURL = ::CFURLCreateWithFileSystemPath(kCFAllocatorDefault, fpath, kCFURLPOSIXPathStyle, false);
	
	// Load the XML data using its URL.
	CFDataRef xmlData;
	::CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault, sourceURL, &xmlData, NULL, NULL, NULL);

	// First, set up the parser callbacks.
	CFXMLParserCallBacks callbacks = {NULL, CreateStructure, AddChild, EndStructure, NULL, HandleError};
	
	// Set up context
	CFXMLParserContext context;
	context.version = 0;
	context.info = this;
	context.retain = NULL;
	context.release = NULL;
	context.copyDescription = NULL;

	// Create the parser with the option to skip whitespace.
	CFXMLParserRef parser = ::CFXMLParserCreate(kCFAllocatorDefault, xmlData, NULL, 
								kCFXMLParserSkipWhitespace, kCFXMLNodeCurrentVersion, &callbacks, &context);

	// Invoke the parser.
	::CFXMLParserParse(parser);
	   
	::CFRelease(parser);
	::CFRelease(xmlData);
	::CFRelease(sourceURL);
	::CFRelease(fpath);
}

// Core Foundation callbacks
void* XMLSAXMac::CreateStructure(CFXMLParserRef parser, CFXMLNodeRef node, void* info)
{
	// Get our parser object
	XMLSAXMac* obj = static_cast<XMLSAXMac*>(info);

	// Error check
	if (!obj)
	{
		CFStringRef temp = CFSTR("Could not evaluate XML parser internal data.");
		::CFXMLParserAbort(parser, kCFXMLErrorMalformedStartTag, temp);
		return NULL;
	}

	CFStringRef myDataStr;

	// Use the dataTypeID to determine what to print.
	switch (::CFXMLNodeGetTypeCode(node))
	{
	case kCFXMLNodeTypeDocument:
		obj->StartDocument();
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;

	case kCFXMLNodeTypeElement:
		{
			// Get attributes
			std::string elname;
			XMLAttributeList attrs;
			const CFXMLElementInfo* einfo = static_cast<const CFXMLElementInfo*>(::CFXMLNodeGetInfoPtr(node));
			if (einfo && !einfo->isEmpty)
			{
				for(CFIndex idx = 0; idx < ::CFArrayGetCount(einfo->attributeOrder); idx++)
				{
					std::string name_str;
					std::string value_str;

					CFStringRef name = static_cast<CFStringRef>(::CFArrayGetValueAtIndex(einfo->attributeOrder, idx));
					CFStringRef value = static_cast<CFStringRef>(::CFDictionaryGetValue(einfo->attributes, name));
					
					const char* name_txt = ::CFStringGetCStringPtr(name, kCFStringEncodingUTF8);
					if (name_txt)
						name_str = name_txt;
					else
					{
						char localBuffer[1024]; 
					 	if (::CFStringGetCString(name, localBuffer, 1024, kCFStringEncodingUTF8))
							name_str = localBuffer;
					}
					
					const char* value_txt = ::CFStringGetCStringPtr(name, kCFStringEncodingUTF8);
					if (value_txt)
						value_str = value_txt;
					else
					{
						char localBuffer[1024]; 
					 	if (::CFStringGetCString(value, localBuffer, 1024, kCFStringEncodingUTF8))
							value_str = localBuffer;
					}
					
					if (name_str.length())
						attrs.push_back(new XMLAttribute(name_str, value_str));
				}
			}

			// Get name and store element
			myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFXMLNodeGetString(node));
			const char* node_name = ::CFStringGetCStringPtr(myDataStr, kCFStringEncodingUTF8);
			if (node_name)
				elname = node_name;
			else
			{
				char localBuffer[1024]; 
			 	if (::CFStringGetCString(myDataStr, localBuffer, 1024, kCFStringEncodingUTF8))
					elname = localBuffer;
			}
			obj->StartElement(elname, attrs);
		}
		break;
	case kCFXMLNodeTypeProcessingInstruction:
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;
	case kCFXMLNodeTypeComment:
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;
	case kCFXMLNodeTypeText:
		{
			myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFXMLNodeGetString(node));
			const char* node_name = ::CFStringGetCStringPtr(myDataStr, kCFStringEncodingUTF8);
			if (node_name)
				obj->Characters(node_name);
			else
			{
				char localBuffer[1024]; 
			 	if (::CFStringGetCString(myDataStr, localBuffer, 1024, kCFStringEncodingUTF8))
					obj->Characters(localBuffer);
			}
			::CFRelease(myDataStr);
			myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		}
		break;
	case kCFXMLNodeTypeCDATASection:
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;
	case kCFXMLNodeTypeEntityReference:
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;
	case kCFXMLNodeTypeDocumentType:
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;
	case kCFXMLNodeTypeWhitespace:
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;
	default:
		myDataStr = ::CFStringCreateCopy(kCFAllocatorDefault, CFSTR(""));
		break;
	}

	// Return the data string for use by the addChild and 
	// endStructure callbacks.
	return (void*) myDataStr;
}

void XMLSAXMac::EndStructure(CFXMLParserRef parser, void* xmlType, void* info)
{
	// Get our parser object
	XMLSAXMac* obj = static_cast<XMLSAXMac*>(info);
	const char* node_name = ::CFStringGetCStringPtr(static_cast<CFStringRef>(xmlType), kCFStringEncodingUTF8);

	// Error check
	if (!obj)
	{
		CFStringRef temp = CFSTR("Could not evaluate XML parser internal data.");
		::CFXMLParserAbort(parser, kCFXMLErrorMalformedCloseTag, temp);
		return;
	}

	// Call EndElement
	if (node_name && *node_name)
		obj->EndElement(node_name);
	
	::CFRelease(static_cast<CFStringRef>(xmlType));
}

void XMLSAXMac::AddChild(CFXMLParserRef parser, void* parent, void* child, void* info)
{
	// Do nothing - SAX has no equivalent for this
}

Boolean XMLSAXMac::HandleError(CFXMLParserRef parser, CFXMLParserStatusCode error, void *info)
{
	// Get our parser object
	XMLSAXMac* obj = static_cast<XMLSAXMac*>(info);

	// Error check
	if (!obj)
	{
		CFStringRef temp = CFSTR("Could not evaluate XML parser internal data.");
		::CFXMLParserAbort(parser, kCFXMLErrorMalformedDocument, temp);
		return FALSE;
	}

	char buf[512];
	char* s;

	// Get the error description string from the Parser.
	CFStringRef description = ::CFXMLParserCopyErrorDescription(parser);
	s = (char *) ::CFStringGetCStringPtr(description, kCFStringEncodingUTF8);

	// If the string pointer is unavailable, do some extra work.
	if (!s)
		::CFStringGetCString(description, buf, 512, kCFStringEncodingUTF8);
	::CFRelease(description);

	// Report the exact location of the error.
	std::ostrstream out;
	out << "Parse error (" << error << ") " << s;
	out << " on line " << (int) ::CFXMLParserGetLineNumber(parser) << ", character " << (int) ::CFXMLParserGetLocation(parser);
	out << std::ends;
	obj->FatalError(out.str());
	out.freeze(false);
	
	return FALSE;
}

