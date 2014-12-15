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
#ifndef _APPLICATION_CONFIG
#define _APPLICATION_CONFIG

#include "../compression/CompressionFactory.h"
#include "../encryption/EncryptionFactory.h"
#include <simple-base/base.h>
#include <string>
using std::string;

//Default compression and encryption settings
#define DEFAULT_COMPRESS  CompressionType::Zlib
#define DEFAULT_CIPHER    EncryptionType::AES_256_CFB

//Describe the different methods in which the user can supply a key
namespace KeyMode
{
	static const int RawKey        = 1;  //The user has supplied the key in raw binary form
	static const int KeyFile       = 2;  //The user wishes to use the raw binary contents of a file as the key
	static const int TransformFile = 3;  //The user wishes to use a file to be transformed into the key
	static const int Password      = 4;  //The user has supplied a password to be transformed into the key
}

class ApplicationConfig
{
	public:
		//Parses the application's command line arguments (the mode should be EncryptionMode::Encrypt or EncryptionMode::Decrypt)
		ApplicationConfig(int argc, char* argv[], int mode);
		
		//Control flow settings
		bool      abort;  //If true, the application should abort immediately after parsing is complete
		string    error;  //If we're aborting, then an error message describing the reason might be useful
		
		//Input and output filenames, and whether or not to auto-overwrite the output file if it exists
		string infilePath;
		string outfilePath;
		bool   autoOverwrite;
		
		//Encryption/Decryption key
		string key;
		
		//Override default compression and encryption algorithms
		int cipher;
		int compression;
		
		//Settings specific to decryption
		bool viewOutput;
		bool deleteOutput;
		
		#ifdef _WIN32
		//Windows-only, uses GUI alterts and prompts instead of CLI
		bool GUIMode;
		#endif
	
	private:
		//These strings are used for temporarily storing a password/filename for key transformation, based on the value of keyMode
		int    keyMode;
		string password;
		string transformFile;
		
		//Helper function to parse the application's command line arguments
		void ParseArguments(int argc, char* argv[], int mode);
		
		//Helper function to output the list of supported ciphers
		void ListSupportedCiphers(bool showWhitespace);
};

#endif
