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
#ifndef _ENCRYPTION_FACTORY
#define _ENCRYPTION_FACTORY

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "EncryptionStrategy.h"

//Neater usage syntax in C++ than an enum
namespace EncryptionMode
{
	static const bool Encrypt = true;
	static const bool Decrypt = false;
}

namespace EncryptionType
{
	static const int None        = 0;  //Sentinel value, since encryption is mandatory
	static const int AES_256_CFB = 1;
}

class EncryptionFactory
{
	public:
		static EncryptionStrategy* CreateEncryption(int algorithm, bool mode);
		
		//Gives a verbose description of a given cipher
		static string              TypeDescription(int algorithm);
		
		//Used to map between the literal values and the string representations of algorithms
		static string              CipherMapping(int algorithm);
		static int                 CipherMapping(string algorithm);
		
		//Used for iterating through the algorithms
		static int                 IteratorBegin();
		static int                 IteratorEnd();
};

#endif
