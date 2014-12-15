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
#include "ChecksumUtility.h"

#include <stdint.h>

string ChecksumUtility::GenerateFileChecksum(string filename)
{
	MeteredIfstream file(filename.c_str());
	if (file.is_open())
	{
		string checksum = GenerateFileChecksum(file);
		file.close();
		return checksum;
	}
	else {
		throw string("Couldn't open input file to calculate checksum!");
	}
}

string ChecksumUtility::GenerateFileChecksum(MeteredIfstream& file)
{
	//Record the original position of the get pointer
	off_t oldPos = file.tellg();
	
	//Create a buffer to hold the checksum
	char* theChecksum = new char[ChecksumUtility::ChecksumSize];
	
	//Check that the file opened properly
	if (file.is_open())
	{
		//Create a SHA-1 instance to calculate the checksum
		SHA1 chcksum;
		
		//Create a buffer to hold the data
		size_t bufSize = 512*1024;
		char* buffer = new char[bufSize];
		
		//Read the data
		size_t bytesRead = 0;
		while ((bytesRead = file.read(buffer, bufSize)))
		{
			//Add the contents of the buffer to the checksum
			chcksum.Input(buffer, bytesRead);
		}
		
		//Free the buffer
		delete[] buffer;
		
		//Calculate the checksum
		uint32_t checksum_bytes[5];
		if (!chcksum.Result(checksum_bytes)) {
			throw "Couldn't compute checksum!";
		}
		
		//SHA-1 is big-endian, so for little-endian systems, flip the endianness
		if (endianness() == LITTLE_ENDIAN)
		{
			for (int i = 0; i < 5; ++i) {
				checksum_bytes[i] = flipEndianness(checksum_bytes[i]);
			}
		}
		
		//Copy the binary checksum to our buffer
		memcpy(theChecksum, (char*)checksum_bytes, sizeof(checksum_bytes));
	}
	else {
		throw "File stream not open!";
	}
	
	//Seek the file back to the original position
	file.seekg(oldPos);
	
	//Create a string to hold the checksum and free the buffer
	string checksum;
	checksum.assign(theChecksum, ChecksumUtility::ChecksumSize);
	delete[] theChecksum;
	
	//Return the generated checksum
	return checksum;
}

string ChecksumUtility::GenerateBlankChecksum()
{
	//Create a string of the right length, filled with null bytes
	return string(ChecksumUtility::ChecksumSize, 0);
}
