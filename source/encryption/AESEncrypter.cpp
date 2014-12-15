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
#include "AESEncrypter.h"

#include <iostream>
using namespace std;

void AESEncrypter::InitialiseKeyAndIV()
{
	//Instantiate a pseudo-random number generator and generate an IV
	AutoSeededRandomPool prng;
	byte iv[AES::BLOCKSIZE];
    prng.GenerateBlock(iv, sizeof(iv));
	
	//Write the IV to the output file
	outputFile->write((char*)iv, sizeof(iv));
	
	//Initialise the encryption engine using the key and IV
	e.SetKeyWithIV((byte*)key->data(), SHA256::DIGESTSIZE, iv);
	
	//Encrypt the checksum and write it to the file
	char* chksm = new char[checksum->length()];
	e.ProcessData((byte*)chksm, (const byte*)checksum->data(), checksum->length());
	outputFile->write((char*)chksm, checksum->length());
	delete[] chksm;
}

void AESEncrypter::PreCompressionStep(char* inputData, size_t length)
{
	//Do nothing here, we encrypt the compressed data to increase performance
}

void AESEncrypter::PostCompressionStep(string& outputData)
{
	//Fill a buffer with the current block of data
	char* currentBlock = new char[outputData.length()];
	memcpy(currentBlock, outputData.data(), outputData.length());
	
	//Encrypt the block
	e.ProcessData((byte*)currentBlock, (const byte*)outputData.data(), outputData.length());
	
	//Write the block
	outputFile->write(currentBlock, outputData.length());
	
	//Free the buffer
	delete[] currentBlock;
}
