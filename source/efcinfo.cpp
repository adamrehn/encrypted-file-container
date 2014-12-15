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
#include <iostream>
#include <fstream>
#include <simple-base/base.h>
#include "compression/CompressionFactory.h"
#include "encryption/EncryptionFactory.h"
#include "utility/ChecksumUtility.h"
#include "utility/MeteredFilestream.h"
#include "efc/EFCHeaderFactory.h"

using namespace std;

int main (int argc, char* argv[])
{
	//EFC files have an arbitrary name field which, whilst adding indirection, could pose security problems in certain situations,
	//so it is important that applications are able to determine the output filename prior to decrypting an EFC file, to validate it.
	//We use the special flag --only-filename to suppress all other output and print only the output filename.
	bool onlyOutputFilename = (argc > 2 && string(argv[2]) == "--only-filename");
	
	if (!onlyOutputFilename)
	{
		//Output the program's header and copyright information
		cout << "EFC Header Probe Utility" << endl << "Copyright (c) 2011, Adam Rehn" << endl << endl;
	}
	
	//Keep track of whether or not we encounter any errors so we can generate the right exit code
	bool errorOcurred = false;
	
	if (argc > 1)
	{
		//Attempt to open the input file
		MeteredIfstream infile(argv[1]);
		if (infile.is_open())
		{
			//Read the file's header
			EFCHeader* header = EFCHeaderFactory::parseHeader(infile);
			if (header != NULL)
			{
				if (!onlyOutputFilename)
				{
					cout << "Valid EFC File Detected, details as follows..." << endl;
					cout << "Filename:     " << header->filename << endl;
					cout << "Compression:  " << CompressionFactory::TypeDescription(header->compression) << endl;
					cout << "Encryption:   " << EncryptionFactory::TypeDescription(header->cipher) << endl;
					cout << "Payload size: " << header->payloadSize << " bytes" << endl << endl;
					cout << "Use --only-filename to print only the filename field's value." << endl;
				}
				else
				{
					cout << header->filename << endl;
				}
			}
			else {
				cout << "Error: invalid EFC header!" << endl;
				errorOcurred = true;
			}
			
			//Close the input file
			infile.close();
		}
		else {
			cout << "Error: could not open input file (" << argv[1] << ")!" << endl;
			errorOcurred = true;
		}
	}
	else
	{
		//No arguments were supplied
		cout << "Usage Syntax:" << endl << "efcinfo INFILE" << endl;
	}
	
	//All done!
	return (errorOcurred == true);
}
