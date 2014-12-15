/*
//  Encrypted File Container
//  Copyright (c) 2011, Adam Rehn
//  
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
*/
#include "MeteredIfstream.h"

#include <simple-base/base.h>

MeteredIfstream::MeteredIfstream(string file)
{
	stream.open(file.c_str(), ios::binary);
	filename = file;
	savedPos = 0;
	ResetReadCount();
}

//Accessors
string MeteredIfstream::GetFileName()
{
	return filename;
}

size_t MeteredIfstream::ReadCount()
{
	return readCount;
}

//Mutators
void MeteredIfstream::ResetReadCount()
{
	readCount = 0;
	readLimit = 0;
	readLimitReached = false;
}

void MeteredIfstream::SavePos()
{
	savedPos = tellg();
}

void MeteredIfstream::RestorePos()
{
	seekg(savedPos);
}

void MeteredIfstream::SetReadLimit(size_t limit)
{
	ResetReadCount();
	readLimit = limit;
}

size_t MeteredIfstream::read(char* s, size_t n)
{
	//Determine how many bytes can be read, based on the read limit
	if (readLimit != 0)
	{
		//If we have already reached the limit, no bytes can be read
		if (readLimitReached == true) {
			n = 0;
		}
		else
		{
			//Below the limit, check if this is the final read
			size_t desiredEnd = readCount + n;
			int    diff       = desiredEnd - readLimit;
			if (diff >= 0)
			{
				//Allow the remaining bytes to be read, but no more
				n -= diff;
				readLimitReached = true;
			}
		}
	}
	
	//Perform the read
	if (n > 0)
	{
		stream.read(s, n);
		readCount += stream.gcount();
		return stream.gcount();
	}
	
	return 0;
}

//Helper function for the endian-specific functions
size_t MeteredIfstream::ReadAsTarget(char* s, size_t n, int targetEndianness)
{
	//Perform the read
	size_t bytesRead = this->read(s, n);
	
	//If the system endianness does not match the target endianness, we need to flip the order of the bytes
	if (endianness() != targetEndianness) {
		flipBytes(s, n);
	}
	
	//Return the number of bytes read
	return bytesRead;
}

//Reads a number of bytes, and treats them as being little endian (flips them on big endian systems)
size_t MeteredIfstream::ReadLittleEndian(char* s, size_t n)
{
	return this->ReadAsTarget(s, n, LITTLE_ENDIAN);
}

//Reads a number of bytes, and treats them as being big endian (flips them on little endian systems)
size_t MeteredIfstream::ReadBigEndian(char* s, size_t n)
{
	return this->ReadAsTarget(s, n, BIG_ENDIAN);
}

//Functions directly delegated to the fstream object
bool MeteredIfstream::is_open()
{
	return stream.is_open();
}

void MeteredIfstream::close()
{
	stream.close();
}

void MeteredIfstream::seekg(streamoff pos)
{
	stream.clear();
	stream.seekg(pos, ios::beg);
}

streampos MeteredIfstream::tellg()
{
	return stream.tellg();
}

size_t MeteredIfstream::gcount()
{
	return stream.gcount();
}

bool MeteredIfstream::BytesRemaining()
{
	if (readLimitReached == true) {
		return false;
	}
	
	return !stream.eof();
}

void MeteredIfstream::getline(string& s, char delim)
{
	std::getline(stream, s, delim);
}

