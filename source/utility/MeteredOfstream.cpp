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
#include "MeteredOfstream.h"

#include <simple-base/base.h>

MeteredOfstream::MeteredOfstream(string file)
{
	stream.open(file.c_str(), ios::binary);
	filename = file;
	savedPos = 0;
	ResetWriteCount();
}

//Accessors
string MeteredOfstream::GetFileName()
{
	return filename;
}

size_t MeteredOfstream::WriteCount()
{
	return writeCount;
}

//Mutators
void MeteredOfstream::ResetWriteCount()
{
	writeCount = 0;
}

void MeteredOfstream::SavePos()
{
	savedPos = tellp();
}

void MeteredOfstream::RestorePos()
{
	seekp(savedPos);
}

void MeteredOfstream::write(const char* s, size_t n)
{
	stream.write(s, n);
	writeCount += n;
}

//Helper function for the endian-specific functions
void MeteredOfstream::WriteForTarget(char* s, size_t n, int targetEndianness)
{
	//If the system endianness does not match the target endianness, we need to flip the order of the bytes
	if (endianness() != targetEndianness)
	{
		//Create a copy of the original bytes
		char* flippedBytes = new char[n];
		memcpy(flippedBytes, s, n);
		
		//Flip the copy and write that array instead of the original
		flipBytes(flippedBytes, n);
		this->write(flippedBytes, n);
		
		//Free the copy array
		delete[] flippedBytes;
	}
	else
	{
		//Perform the write as usual
		this->write(s, n);
	}
}

//Writes a number of bytes, and treats the output as being little endian (flips the bytes on big endian systems)
void MeteredOfstream::WriteLittleEndian(char* s, size_t n)
{
	this->WriteForTarget(s, n, LITTLE_ENDIAN);
}

//Writes a number of bytes, and treats the output as being big endian (flips the bytes on little endian systems)
void MeteredOfstream::WriteBigEndian(char* s, size_t n)
{
	this->WriteForTarget(s, n, BIG_ENDIAN);
}

//Functions directly delegated to the fstream object
bool MeteredOfstream::is_open()
{
	return stream.is_open();
}

void MeteredOfstream::close()
{
	stream.close();
}

void MeteredOfstream::seekp(streamoff pos)
{
	stream.seekp(pos, ios::beg);
	stream.clear();
}

streampos MeteredOfstream::tellp()
{
	return stream.tellp();
}
