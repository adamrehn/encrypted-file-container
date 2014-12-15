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
#ifndef _AES_ENCRYPTION
#define _AES_ENCRYPTION

#include "EncryptionStrategy.h"

#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <cryptopp/ccm.h>
#include <cryptopp/sha.h>

using CryptoPP::AutoSeededRandomPool;
using CryptoPP::AES;
using CryptoPP::CFB_FIPS_Mode;
using CryptoPP::SHA256;

#define AES256_KEYSIZE SHA256::DIGESTSIZE

class AESEncryption : public EncryptionStrategy
{
	public:
		void TransformFile(CompressionStrategy* compressionTransform, MeteredIfstream& inputFile, MeteredOfstream& outputFile, string& key, string& checksum);
		
		string GenerateKeyFromPassword(string password);
		string GenerateKeyFromFile(string filename);
	
	protected:
		virtual void InitialiseKeyAndIV() = 0;
		virtual void PreCompressionStep(char* inputData, size_t length) = 0;
		virtual void PostCompressionStep(string& outputData) = 0;
		
		MeteredIfstream* inputFile;
		MeteredOfstream* outputFile;
		string*       key;
		string*       checksum;
};

#endif
