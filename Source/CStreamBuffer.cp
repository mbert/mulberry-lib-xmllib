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

#include "CStreamBuffer.h"

#include <cstring>

const int cBufferSize = 8192;

CStreamBuffer::CStreamBuffer()
{
	mStream = NULL;
	mData = NULL;
	bbegin = bnext = beof = bend = NULL;
	bfail = false;
	bcount = 0;
	mStream = NULL;
}

CStreamBuffer::~CStreamBuffer()
{
	// Only if no data
	if (mData == NULL)
	{
		delete[] bbegin;
		bbegin = bnext = bend = NULL;
	}
}

const char* CStreamBuffer::operator++()	// ++p
{
	get();
	return bnext;
}

const char* CStreamBuffer::operator++(int)	// p++
{
	// Make sure we have at least two bytes before we bump to the next one
	// as we need to refer back to the previous one and don't want the buffer adjusted
	NeedData(2);

	get();
	return bnext - 1;
}

CStreamBuffer& CStreamBuffer::operator+=(uint32_t bump)
{
	// Fill the buffer with more
	NeedData(bump);
	
	// Verify we have enough
	if (bnext + bump > beof)
	{
		bfail = true;
		bcount = beof - bnext;
		bnext = beof;
	}
	else
	{
		bnext += bump;
		bcount += bump;
	}
	return *this;
}

void CStreamBuffer::SetStream(std::istream& is)
{
	// Create internal buffer
	beof = bnext = bbegin = new char[cBufferSize];
	bend = bbegin + cBufferSize;

	// Assign stream and read in first block
	mStream = &is;
	FillFromStream();
}

void CStreamBuffer::SetData(const char* data)
{
	// Assign data
	mData = data;
	
	// Set internal buffer
	bnext = bbegin = mData;
	beof = bend = bbegin + ::strlen(data);
}

char CStreamBuffer::get()
{
	// Load more into buffer
	if (bnext == beof)
	{
		ReadMore();
	}
	
	// If no more then we are done
	if (bnext == beof)
	{
		bfail = true;
		return 0;
	}

	bcount++;
	return *bnext++;
}

void CStreamBuffer::FillFromStream()
{
	// Not if using fixed buffer
	if (mData != NULL)
		return;

	// Read as much fromt he stream as possible
	
	// Stream must be working
	if (mStream->fail())
	{
		bfail = true;
		return;
	}
	
	// Determine how much can be read in
	uint32_t remaining_space = bend - beof;
	
	// Read in up to that much
	mStream->read(const_cast<char*>(beof), remaining_space);
	
	// Adjust for the amount actually read in
	beof += mStream->gcount();
}

void CStreamBuffer::NeedData(uint32_t amount)
{
	// Ensure that remaining data in buffer is at least amount bytes long
	if (Remaining() < amount)
	{
		ReadMore();
	}
}

void CStreamBuffer::ReadMore()
{
	// Not if using fixed buffer
	if (mData != NULL)
		return;

	uint32_t bytes_used = bnext - bbegin;
	uint32_t bytes_to_copy = beof - bnext;
	if (bytes_to_copy != 0)
	{
		// Shift down the remaining bytes so we can append some more
		::memmove(const_cast<char*>(bbegin), bnext, bytes_to_copy);
	}

	// Adjust buffer pointers
	bnext = bbegin;
	beof -= bytes_used;

	// Fill remaining
	FillFromStream();
}
