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
#include "EncryptionFactory.h"

#include "AESEncrypter.h"
#include "AESDecrypter.h"

EncryptionStrategy* EncryptionFactory::CreateEncryption(int algorithm, bool mode)
{
	switch (algorithm)
	{
		//AES with a 256-bit key in CFB Mode
		case EncryptionType::AES_256_CFB:
			if (mode == EncryptionMode::Encrypt)
			{
				return new AESEncrypter();
			}
			else
			{
				return new AESDecrypter();
			}
		
		//Unrecognised encryption algorithm
		default:
			return NULL;
	}
}

string EncryptionFactory::TypeDescription(int algorithm)
{
	switch (algorithm)
	{
		//AES with a 256-bit key in CFB Mode
		case EncryptionType::AES_256_CFB:
			return "AES 256-bit CFB Mode";
		
		//Unrecognised encryption algorithm
		default:
			return "[Unrecognised Algorithm]";
	}
}

string EncryptionFactory::CipherMapping(int algorithm)
{
	switch (algorithm)
	{
		//AES with a 256-bit key in CFB Mode
		case EncryptionType::AES_256_CFB:
			return "aes256cfb";
		
		//Unrecognised encryption algorithm
		default:
			return "";
	}
}

int EncryptionFactory::CipherMapping(string algorithm)
{
	if (algorithm == "aes256cfb")
	{
		return EncryptionType::AES_256_CFB;
	}
	else
	{
		return EncryptionType::None;
	}
}

int EncryptionFactory::IteratorBegin()
{
	return EncryptionType::AES_256_CFB;
}

int EncryptionFactory::IteratorEnd()
{
	return EncryptionType::AES_256_CFB + 1;
}
