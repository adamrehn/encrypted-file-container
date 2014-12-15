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
#include "ZlibCompression.h"

string ZlibCompression::TransformInput(char* input, size_t length, bool isFinalInput)
{
	//Accept the input
	strm.next_in  = (Bytef*)input;
	strm.avail_in = length;
	
	//Use no flush mode, until we reach the last of the input
	int flush = (isFinalInput) ? Z_FINISH : Z_NO_FLUSH;
	
	//Create a string to hold all of the produced output
	string output;
	
	//Create a buffer to hold each round of output
	char* outputBuffer = new char[length];
	
	int count = 0;
	do
	{
		//Point the stream to the output buffer
		strm.avail_out = length;
		strm.next_out  = (Bytef*)outputBuffer;
		
		//Perform the transformation
		PerformTransform(strm, flush);
		
		//If any bytes were produced, add them to the output string
		count = length - strm.avail_out;
		if (count) {
			output.append(outputBuffer, count);
		}
		
	} while (strm.avail_out == 0);
	
	//Free memory and return the output
	delete[] outputBuffer;
	return output;
}
