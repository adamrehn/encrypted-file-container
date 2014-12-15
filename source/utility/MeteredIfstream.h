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
#ifndef _METERED_IFSTREAM
#define _METERED_IFSTREAM

#include <fstream>
#include <string>
using std::ifstream;
using std::string;
using std::ios;
using std::streampos;
using std::streamoff;

class MeteredIfstream
{
	public:
		MeteredIfstream(string file);
		
		//Accessors
		string GetFileName();
		size_t ReadCount();
		bool   BytesRemaining();
		
		//Mutators
		void ResetReadCount();
		void SavePos();
		void RestorePos();
		
		//Resets the read count and only allows n number of bytes to be read hereafter (until we call ResetReadCount() again)
		void SetReadLimit(size_t limit);
		
		//Increments the counter and returns the number of bytes read
		size_t read(char* s, size_t n);
		
		//Reads a number of bytes, and treats them as being little endian (flips them on big endian systems)
		size_t ReadLittleEndian(char* s, size_t n);
		
		//Reads a number of bytes, and treats them as being big endian (flips them on little endian systems)
		size_t ReadBigEndian(char* s, size_t n);
		
		//Functions directly delegated to the ifstream object
		bool is_open();
		void close();
		void seekg(streamoff pos);
		streampos tellg();
		size_t gcount();
		//bool eof();
		void getline(string& s, char delim);
		
	private:
		ifstream stream;
		string filename;
		
		size_t readCount;
		
		streampos savedPos;
		
		size_t readLimit;
		bool   readLimitReached;
		
		//Helper function for the endian-specific functions
		size_t ReadAsTarget(char* s, size_t n, int targetEndianness);
};

#endif
