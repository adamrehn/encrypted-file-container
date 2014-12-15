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
#include "AESDecrypter.h"

void AESDecrypter::InitialiseKeyAndIV()
{
	//Read the IV from the input file
	byte iv[AES::BLOCKSIZE];
	inputFile->read((char*)iv, sizeof(iv));
	
	//Initialise the decryption engine using the key and IV
	d.SetKeyWithIV((byte*)key->data(), SHA256::DIGESTSIZE, iv);
	
	//Read the checksum from the file and decrypt it
	char* encryptedCheksum = new char[checksum->length()];
	char* decryptedCheksum = new char[checksum->length()];
	inputFile->read((char*)encryptedCheksum, checksum->length());
	d.ProcessData((byte*)decryptedCheksum, (const byte*)encryptedCheksum, checksum->length());
	checksum->assign(decryptedCheksum, checksum->length());
	delete[] encryptedCheksum;
	delete[] decryptedCheksum;
}

void AESDecrypter::PreCompressionStep(char* inputData, size_t length)
{
	//Decrypt the data prior to decompression
	
	//Copy the current block
	char* currentBlock = new char[length];
	memcpy(currentBlock, inputData, length);
	
	//Perform the decryption
	d.ProcessData((byte*)inputData, (const byte*)currentBlock, length);
	
	//Free the buffer
	delete[] currentBlock;
}

void AESDecrypter::PostCompressionStep(string& outputData)
{
	//Write the block
	outputFile->write(outputData.data(), outputData.length());
}
