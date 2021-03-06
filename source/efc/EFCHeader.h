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
#ifndef _EFC_HEADER
#define _EFC_HEADER

#include <stdint.h>
#include <string>
using std::string;

#include "../utility/MeteredFilestream.h"
#include "../compression/CompressionFactory.h"
#include "../encryption/EncryptionFactory.h"

class EFCHeader
{
	public:
		
		//(De)obfuscates a string
		static string ObfuscateText(string s);
		
		//(De)obfuscates a series of bytes
		static void ObfuscateText(char* s, size_t n);
		
		//Serialises the header to an output file
		virtual void WriteHeader(MeteredOfstream& outputFile) = 0;
		
		//Standard Header fields
		int32_t compression; //The compression type used, i.e: CompressionType::[...]
		int32_t cipher;      //The encryption type used,  i.e: EncryptionType::[...]
		string  filename;    //The filename of the original file
		int32_t payloadSize; //The length (in bytes) of the payload, including IV
	
	private:
		//Helper function to facilitate the (de)obfuscation of individual bytes
		static unsigned char ObfuscateByte(unsigned char theByte);
};

#endif
