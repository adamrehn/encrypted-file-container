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
#include "ApplicationConfig.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "../encryption/EncryptionFactory.h"
#include "../efc/EFCHeaderFactory.h"
#include <iostream>
using std::clog;
using std::endl;

//Default constructor
ApplicationConfig::ApplicationConfig(int argc, char* argv[], int mode)
{
	abort = false;
	error = "";
	
	infilePath    = "";
	outfilePath   = "";
	autoOverwrite = false;
	
	keyMode = 0;  //Sentinel value, does not match a valid KeyMode member
	
	cipher      = DEFAULT_CIPHER;
	compression = DEFAULT_COMPRESS;
	
	viewOutput   = false;
	deleteOutput = false;
	
	#ifdef _WIN32
	this->GUIMode = false;
	#endif
	
	//Parse the command line arguments
	ParseArguments(argc, argv, mode);
}

//Helper function to output the list of supported ciphers
void ApplicationConfig::ListSupportedCiphers(bool showWhitespace)
{
	//List the supported ciphers
	int whitespace = 18 - 1; //Take into account the space before the cipher name
	for (int i = EncryptionFactory::IteratorBegin(); i < EncryptionFactory::IteratorEnd(); ++i)
	{
		//Retrieve the cipher name and decription
		string cipherName        = EncryptionFactory::CipherMapping(i);
		string cipherDescription = EncryptionFactory::TypeDescription(i);
		
		//Pad everything out so that it lines up nicely
		int paddingNeeded = whitespace - cipherName.length();
		if (paddingNeeded > 0 && showWhitespace == true)
		{
			for (int p = 0; p < paddingNeeded; ++p) {
				cipherName += " ";
			}
		}
		else
		{
			//When there is no gap between the name and description, insert a colon
			cipherName += ":";
		}
		
		//Output the line
		clog << ((showWhitespace) ? " " : "") << cipherName << cipherDescription << ((i == EncryptionFactory::IteratorBegin()) ? " (Default)" : "") << endl;
	}
}

//Parses the application's command line arguments
void ApplicationConfig::ParseArguments(int argc, char* argv[], int mode)
{
	//Loop through the arguments (ignoring 0, which is the program itself)
	for (int argNum = 1; argNum < argc; argNum++)
	{
		//Grab the current argument
		string currArg = argv[argNum];
		string nextArg = "";
		
		//Grab the next argument, if there is one
		if (argNum+1 < argc) {
			nextArg = argv[argNum+1];
		}
		
		//The main logic loop. For two-part args, the counter will increment extra to skip appropriately
		if (currArg == "-i")
		{
			//The next argument is the input file
			this->infilePath = nextArg;
			
			//Skip ahead, as we have consumed the next argument
			argNum++;
		}
		else if (currArg == "-o")
		{
			//The next argument is the output file
			this->outfilePath = nextArg;
			
			//Skip ahead, as we have consumed the next argument
			argNum++;
		}
		else if (currArg == "--y" || currArg == "-y" || currArg == "--overwrite")
		{
			//Automatically overwrite the output file if it already exists
			this->autoOverwrite = true;
		}
		else if (currArg == "-cipher")
		{
			//The next argument is the cipher to be used
			int suppliedCipher = EncryptionFactory::CipherMapping(nextArg);
			
			//Check if the specified cipher is valid
			if (suppliedCipher != EncryptionType::None) {
				this->cipher = suppliedCipher;
			}
			else
			{
				//Invalid cipher specified
				this->error += "Invalid cipher \"" + nextArg + "\"\n";
			}
			
			//Skip ahead, as we have consumed the next argument
			argNum++;
		}
		else if (currArg == "-pass" || currArg == "-password")
		{
			//The next argument is the password
			this->keyMode = KeyMode::Password;
			password = nextArg;
			
			//Skip ahead, as we have consumed the next argument
			argNum++;
			
			//Check if we are actually reading the password from stdin (newline delimited)
			if (nextArg == "-")
			{
				//Read the password from stdin, hiding the characters if using a console window
				password = get_cli_password_hidden("Password: ");
				
				//Account for windows line endings (\r\n)
				password = strip_chars("\r", password);
			}
		}
		else if (currArg == "-keyfile")
		{
			//The next argument is a keyfile
			this->keyMode  = KeyMode::KeyFile;
			
			//Skip ahead, as we have consumed the next argument
			argNum++;
			
			//Read the contents of the keyfile
			this->key = file_get_contents(nextArg);
			
			//Check that the keyfile read properly
			if (this->key.length() == 0) {
				this->error += "Invalid keyfile supplied!\n";
			}
		}
		else if (currArg == "-hkeyfile")
		{
			//The next argument is a file to be transformed into the key
			this->keyMode  = KeyMode::TransformFile;
			transformFile   = nextArg;
			
			//Skip ahead, as we have consumed the next argument
			argNum++;
		}
		else if (currArg == "--view")
		{
			//Open the output file for viewing after decryption
			this->viewOutput = true;
		}
		else if (currArg == "--delete")
		{
			//Delete the output file after viewing
			this->deleteOutput = true;
		}
		
		#ifdef _WIN32
		else if (currArg == "--gui")
		{
			//We are being utilised by a GUI Application
			this->GUIMode = true;
		}
		#endif
		
		else if (currArg == "--help")
		{
			//Display the usage syntax
			clog << "Usage Syntax:" << endl << ((mode == EncryptionMode::Encrypt) ? "efcencode" : "efcdecode") << " -i INFILE [options, must include key in some form] [-o OUTFILE]" << endl << endl
			     << "Output Options:" << endl
			     << " -o OUTFILE       Set output filename" << endl
			     << " -y, --overwrite  Don't prompt for file overwrite" << endl << endl
			     << "Key Options:" << endl
			     << " -pass PASS       Hash the supplied password and use the hash as the key," << endl
			     << "                  use \"-\" for interactive keyboard input" << endl
			     << " -keyfile  FILE   Read the key as raw data from FILE" << endl
			     << " -hkeyfile FILE   Hash the contents of FILE and use that as the key" << endl << endl;
			
			//Output the decryption-specific options
			if (mode == EncryptionMode::Decrypt)
			{
				clog << "Post-decryption Options:" << endl
				     << " --view           Open the output file for viewing with the default program" << endl
				     << " --delete         Delete the output file after viewing" << endl << endl;
			}
			
			//Output the encryption options
			clog << "Encryption Options:" << endl
			     << " -cipher CIPHER   Use the specified cipher for encryption/decryption" << endl
				 << "                  See below for supported values." << endl << endl
			     << "Supported Ciphers:" << endl;
			
			//List the supported ciphers
			ListSupportedCiphers(true);
			
			//Instruct the application to abort immediately, halting the parsing process
			this->abort = true;
			this->error = "";
			return;
		}
		else if (currArg == "--list-ciphers")
		{
			//List the supported ciphers
			ListSupportedCiphers(false);
			
			//Instruct the application to abort immediately, halting the parsing process
			this->abort = true;
			this->error = "";
			return;
		}
		else
		{
			//Unrecognised option
			this->error += "Unrecognised option \"" + currArg + "\"\n";
		}
	}
	
	//Input file is a required argument
	if (this->infilePath.length() == 0) {
		this->error += "No input file specified.\n";
	}
	
	//If there were no errors, we can perform the advanced steps
	if (this->error.length() == 0)
	{
		//If we are decrypting, we need to read the filename and cipher information from the input EFC file's header
		if (mode == EncryptionMode::Decrypt)
		{
			//Attempt to open the input file
			MeteredIfstream infile(this->infilePath);
			if (infile.is_open())
			{
				//Attemp to read the file's header
				EFCHeader* header = EFCHeaderFactory::parseHeader(infile);
				if (header != NULL)
				{
					//Read the correct encryption algorithm to use
					this->cipher = header->cipher;
					
					//If the current output filename is a directory with a trailing slash, truncate it (including it may cause is_dir() to return false)
					if (ends_with("/", this->outfilePath) || ends_with("\\", this->outfilePath)) {
						this->outfilePath = this->outfilePath.substr(0, this->outfilePath.length() - 1);
					}
					
					//Read the output filename
					if (this->outfilePath != "" && (is_dir(this->outfilePath) || (this->outfilePath.length() == 2 && this->outfilePath[1] == ':')))
					{
						//The current output filename is a directory, so make sure it includes a trailing slash
						if (!ends_with("/", this->outfilePath) && !ends_with("\\", this->outfilePath)) {
							this->outfilePath += "/";
						}
						
						//Append to the directory name
						this->outfilePath += basename(header->filename);
					}
					else if (this->outfilePath == "")
					{
						//Read the output filename as-is
						this->outfilePath = dirname(this->infilePath) + basename(header->filename);
					}
				}
				
				//Close the input file
				infile.close();
			}
		}
		else if (mode == EncryptionMode::Encrypt)
		{
			//Unless already set, the output filename is the .efc version of the input filename
			if (this->outfilePath == "") {
				this->outfilePath = replace_extension(this->infilePath, "efc");
			}
		}
		
		//If we are transforming a password or file to generate the key, we will need to instantiate the appropriate encryption context
		if (this->keyMode == KeyMode::Password || KeyMode::TransformFile)
		{
			//Instantiate the encryption context for the correct cipher
			EncryptionStrategy* encryption = EncryptionFactory::CreateEncryption(this->cipher, mode);
			
			//If the cipher is invalid, throw an exception
			if (encryption == NULL) {
				throw "Invalid cipher!";
			}
			
			//Perform the relevant transformation
			if (this->keyMode == KeyMode::Password) {
				this->key = encryption->GenerateKeyFromPassword(password);
			}
			else if (this->keyMode == KeyMode::TransformFile) {
				this->key = encryption->GenerateKeyFromFile(transformFile);
			}
			
			//Free the encryption instance
			delete encryption;
		}
		
		//If the specified output file already exists, prompt (unless auto overwrite is enabled)
		if (this->outfilePath != "" && this->autoOverwrite == false && file_exists(this->outfilePath))
		{
			//Keep track of whether or not the users confirms the overwrite
			bool overwrite = false;
			string prompt = "The file \"" + this->outfilePath + "\" already exists.";
			
			#ifdef _WIN32
			//If we are in GUI mode, we use a graphical prompt
			if (this->GUIMode == true)
			{
				prompt += "\n\nAre you sure you want to overwrite it?";
				if (MessageBox(NULL, prompt.c_str(), "Overwrite?", MB_YESNO | MB_ICONQUESTION) == IDYES) {
					overwrite = true;
				}
			}
			else
			#endif
			{
				//CLI prompt
				clog << prompt << " Overwrite? (y/[n]): ";
				char response = 'n';
				std::cin >> response;
				if (response == 'Y' || response == 'y') {
					overwrite = true;
				}
			}
			
			//If we are not overwriting the file, abort the application
			if (overwrite == false) {
				this->abort = true;
			}
		}
	}
	
	//Check that we really did end up with a key
	if (this->key.length() == 0) {
		this->error += "No key specified.\n";
	}
	
	//If any errors were generated, the application should abort
	if (this->error.length() > 0) {
		this->abort = true;
	}
	
	#ifdef _WIN32
	//If we are in GUI mode, we output errors in a graphical dialog
	if (this->GUIMode == true && this->error.length() > 0)
	{
		MessageBox(NULL, this->error.c_str(), "Error(s) Ocurred", MB_ICONERROR);
		this->error = "";
	}
	#endif
}
