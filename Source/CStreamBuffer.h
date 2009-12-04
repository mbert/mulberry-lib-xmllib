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

#ifndef CStreamBuffer_H
#define CStreamBuffer_H

#include <stdint.h>
#include <istream>

class CStreamBuffer
{
public:
	CStreamBuffer();
	virtual ~CStreamBuffer();

	void SetStream(std::istream& is);
	void SetData(const char* data);

	char operator*()
	{
		return *bnext;
	}

	const char* operator++();	// ++p
	const char* operator++(int);	// p++

	CStreamBuffer& operator+=(uint32_t bump);

	bool HasData() const
	{
		return bnext != bbegin;
	}
	void NeedData(uint32_t amount);

	const char* next()
	{
		return bnext;
	}
	uint32_t count() const
	{
		return bcount;
	}
	uint32_t Remaining()
	{
		return beof - bnext;
	}

	bool fail() const
	{
		return bfail;
	}

private:
	std::istream*		mStream;
	const char*		mData;
	const char*	 	bbegin;
	const char* 	bnext;
	const char* 	beof;
	const char* 	bend;
	bool			bfail;
	uint32_t		bcount;

	char get();

	void ReadMore();
	void FillFromStream();
};

#endif	// CStreamBuffer_H
