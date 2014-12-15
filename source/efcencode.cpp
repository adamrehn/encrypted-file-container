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
#include "utility/ApplicationConfig.h"
#include "efc/EFCHeaderFactory.h"

using namespace std;

int main (int argc, char* argv[])
{
	//Output the program's header and copyright information
	clog << "EFC Encryption Utility" << endl << "Copyright (c) 2011, Adam Rehn" << endl << endl;
	
	//Keep track of whether or not we encounter any errors so we can generate the right exit code
	bool errorOcurred = false;
	
	//Check if any arguments were supplied
	if (argc > 1)
	{
		//Parse the command line arguments
		ApplicationConfig config(argc, argv, EncryptionMode::Encrypt);
		
		//Output any errors generated during argument parsing
		clog << config.error;
		
		//Abort immediately if requested
		if (config.abort == true) {
			exit(1);
		}
		
		//Attempt to open the input file
		MeteredIfstream infile(config.infilePath);
		if (infile.is_open())
		{
			//Create a new EFC header
			EFCHeader* header = EFCHeaderFactory::createHeader(EFCHeaderVersion::Default);
			if (header != NULL)
			{
				//Set the header field values
				header->filename    = config.infilePath;
				header->compression = config.compression;
				header->cipher      = config.cipher;
				
				//Create the compression instance
				CompressionStrategy* compression = CompressionFactory::CreateCompression(header->compression, CompressionMode::Compress);
				if (compression != NULL)
				{
					//Create the encryption instance
					EncryptionStrategy* encryption = EncryptionFactory::CreateEncryption(header->cipher, EncryptionMode::Encrypt);
					if (encryption != NULL)
					{
						//Attempt to open the output file
						MeteredOfstream outfile(config.outfilePath);
						if (outfile.is_open())
						{
							//Generate the checksum of the input file
							string checksum = ChecksumUtility::GenerateFileChecksum(infile);
							clog << "Input file checksum: " << hex(checksum.data(), checksum.length()) << endl;
							
							//Write the incomplete header as a placeholder
							header->WriteHeader(outfile);
							
							//Reset the output file's write count
							outfile.ResetWriteCount();
							
							//Encrypt the file
							encryption->TransformFile(compression, infile, outfile, config.key, checksum);
							
							//Fill in the payload size in the header
							header->payloadSize = outfile.WriteCount();
							
							//Seek back to the beginning and write the completed header
							outfile.seekp(0);
							header->WriteHeader(outfile);
							
							clog << "Done!" << endl;
							
							//Close the output file
							outfile.close();
						}
						else {
							clog << "Error: could not open output file!" << endl;
							errorOcurred = true;
						}
						
						//Free the encryption instance
						delete encryption;
					}
					else {
						clog << "Error: unsupported encryption algorithm (" << header->cipher << ")!" << endl;
						errorOcurred = true;
					}
					
					//Free the compression instance
					delete compression;
				}
				else {
					clog << "Error: unsupported compression mode (" << header->compression << ")!" << endl;
					errorOcurred = true;
				}
			}
			else {
				clog << "Error: invalid EFC header!" << endl;
				errorOcurred = true;
			}
			
			//Close the input file
			infile.close();
		}
		else {
			clog << "Error: could not open input file (" << config.infilePath << ")!" << endl;
			errorOcurred = true;
		}
	}
	else
	{
		//No arguments were supplied
		clog << "No arguments supplied." << endl << "Use \"efcencode --help\" for usage syntax." << endl;
	}
	
	//All done!
	return (errorOcurred == true);
}
