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
	clog << "EFC Decryption Utility" << endl << "Copyright (c) 2011, Adam Rehn" << endl << endl;
	
	//Keep track of whether or not we encounter any errors so we can generate the right exit code
	bool errorOcurred = false;
	
	//Check if any arguments were supplied
	if (argc > 1)
	{
		//Parse the command line arguments
		ApplicationConfig config(argc, argv, EncryptionMode::Decrypt);
		
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
			//Read the file's header
			EFCHeader* header = EFCHeaderFactory::parseHeader(infile);
			if (header != NULL)
			{
				//Create the compression instance
				CompressionStrategy* compression = CompressionFactory::CreateCompression(header->compression, CompressionMode::Decompress);
				if (compression != NULL)
				{
					//Create the encryption instance
					EncryptionStrategy* encryption = EncryptionFactory::CreateEncryption(header->cipher, EncryptionMode::Decrypt);
					if (encryption != NULL)
					{
						//Attempt to open the output file
						MeteredOfstream outfile(config.outfilePath);
						if (outfile.is_open())
						{
							//Restrict input to the specified payload length
							infile.SetReadLimit(header->payloadSize);
							
							//Create a blank checksum (will be filled by the decryption algorithm)
							string checksum = ChecksumUtility::GenerateBlankChecksum();
							
							//Decrypt the file
							encryption->TransformFile(compression, infile, outfile, config.key, checksum);
							
							//Close the output file
							outfile.close();
							
							//Determine the chcksum of the written output file
							string outputChecksum = ChecksumUtility::GenerateFileChecksum(config.outfilePath);
							
							//Output the checksums
							clog << "Original Checksum:  " << hex(checksum.data(), checksum.length()) << endl;
							clog << "Decrypted Checksum: " << hex(outputChecksum.data(), outputChecksum.length()) << endl;
							
							//Determine if the two checksums match
							if (memcmp(checksum.data(), outputChecksum.data(), checksum.length()) == 0)
							{
								clog << "The checksums match!" << endl;
							}
							else
							{
								#ifdef _WIN32
								if (config.GUIMode == true)
								{
									MessageBox(NULL, "The checksums do not match!", "Error Decrypting", MB_ICONERROR);
								}
								#endif
								clog << "The checksums do not match!" << endl;
								errorOcurred = true;
							}
							
							//Check if we are opening the output file for viewing with the default application
							if (config.viewOutput == true)
							{
								//View the output file
								string command = "\"" + config.outfilePath + "\"";
								
								#ifdef _WIN32
									//Under Windows, we can open the file with the default application easily
									system(command.c_str());
								#else
									//If we are under Linux, both Gnome and KDE have opening utilities, but they are non-blocking
									if (config.deleteOutput == false)
									{
										system(string("gnome-open " + command).c_str());
										system(string("kde-open "   + command).c_str());
									}
									else
									{
										//If we are going to delete the file afterwards, our only choice is to output it raw
										clog << endl << endl << "Decrypted Output:" << endl << endl;
										system(string("cat " + command).c_str());
									}
								#endif
								
								//Check if we are deleting the output file after viewing
								if (config.deleteOutput == true)
								{
									//Delete the output file
									remove(config.outfilePath.c_str());
								}
							}
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
		clog << "No arguments supplied." << endl << "Use \"efcdecode --help\" for usage syntax." << endl;
	}
	
	//All done!
	return (errorOcurred == true);
}
