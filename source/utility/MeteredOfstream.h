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
#ifndef _METERED_OFSTREAM
#define _METERED_OFSTREAM

#include <fstream>
#include <string>
using std::ofstream;
using std::string;
using std::ios;
using std::streampos;
using std::streamoff;

class MeteredOfstream
{
	public:
		MeteredOfstream(string file);
		
		//Accessors
		string GetFileName();
		size_t WriteCount();
		
		//Mutators
		void ResetWriteCount();
		void SavePos();
		void RestorePos();
		
		//Writes to the file and increments the counter
		void write(const char* s, size_t n);
		
		//Writes a number of bytes, and treats the output as being little endian (flips the bytes on big endian systems)
		void WriteLittleEndian(char* s, size_t n);
		
		//Writes a number of bytes, and treats the output as being big endian (flips the bytes on little endian systems)
		void WriteBigEndian(char* s, size_t n);
		
		//Functions directly delegated to the fstream object
		bool is_open();
		void close();
		void seekp(streamoff pos);
		streampos tellp();
		
	private:
		ofstream stream;
		string filename;
		
		size_t writeCount;
		
		streampos savedPos;
		
		//Helper function for the endian-specific functions
		void WriteForTarget(char* s, size_t n, int targetEndianness);
};

#endif
