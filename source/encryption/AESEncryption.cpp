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
#include "AESEncryption.h"

void AESEncryption::TransformFile(CompressionStrategy* compressionTransform, MeteredIfstream& inputFile, MeteredOfstream& outputFile, string& key, string& checksum)
{
	//Bind our references to the arguments to save passing them around all the time
	this->inputFile  = &inputFile;
	this->outputFile = &outputFile;
	this->key        = &key;
	this->checksum   = &checksum;
	
	//Initialise the cipher with the key and IV
	InitialiseKeyAndIV();
	
	//Create a buffer to hold the data
	size_t bufSize = 512*1024;
	char* buffer = new char[bufSize];
	
	//Loop through the data
	size_t bytesRead = 0;
	while ((bytesRead = inputFile.read(buffer, bufSize)))
	{
		//Create a buffer to hold the current block of data (only the bytes read)
		char* inputData = new char[bytesRead];
		
		//Copy the current block into the buffer
		memcpy(inputData, buffer, bytesRead);
		
		//Perform the pre-(de)compression step
		PreCompressionStep(inputData, bytesRead);
		
		//Perform the (de)compression
		string transformedData = compressionTransform->TransformInput(inputData, bytesRead, !inputFile.BytesRemaining());
		
		//Perform the post-(de)compression step
		PostCompressionStep(transformedData);
		
		//Free the buffer
		delete[] inputData;
	}
	
	//Free the buffer
	delete[] buffer;
}

string AESEncryption::GenerateKeyFromPassword(string password)
{
	//Create a buffer to hold the generated key
	byte* theKey = new byte[SHA256::DIGESTSIZE];
	
	//Instantiate a SHA-256 digest calculator
	SHA256 digest;
	
	//Calculate the SHA-256 digest of the password
	digest.CalculateDigest(theKey, (const byte*)password.c_str(), password.length());
	
	//Copy the key into a string and free the buffer
	string key;
	key.assign((char*)theKey, SHA256::DIGESTSIZE);
	delete[] theKey;
	
	//Return the string containing the key
	return key;
}

string AESEncryption::GenerateKeyFromFile(string filename)
{
	//Create a buffer to hold the generated key
	byte* theKey = new byte[SHA256::DIGESTSIZE];
	
	//Instantiate a SHA-256 digest calculator
	SHA256 digest;
	
	//Attempt to open the specified file
	ifstream infile(filename.c_str(), ios::binary);
	if (infile.is_open())
	{
		//Read the file one block at a time
		char buffer[1024*1024];
		size_t bytesRead = 0;
		while ( (bytesRead = infile.read(buffer, sizeof(buffer)).gcount()) != 0 )
		{
			//Add the data to the hash
			digest.Update((byte*)buffer, bytesRead);
		}
		
		//Finalise the digest
		digest.Final(theKey);
	}
	else {
		throw string("Could not open keyfile to be hashed");
	}
	
	//Copy the key into a string and free the buffer
	string key;
	key.assign((char*)theKey, SHA256::DIGESTSIZE);
	delete[] theKey;
	
	//Return the string containing the key
	return key;
}
