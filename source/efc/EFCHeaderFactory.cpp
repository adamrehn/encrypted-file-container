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
#include "EFCHeaderFactory.h"

#include <cstring>
#include "EFCDefaultHeader.h"

EFCHeader* EFCHeaderFactory::createHeader(char version)
{
	switch (version)
	{
		case EFCHeaderVersion::Default:
			return new EFCDefaultHeader();
	}
	
	//Unsupported header version
	return NULL;
}

EFCHeader* EFCHeaderFactory::parseHeader(MeteredIfstream& file)
{
	//Read the magic bytes
	char magicBytes[4];
	file.read(magicBytes, sizeof(magicBytes));
	
	//The header version is the 4th byte
	char headerVersion = magicBytes[3];
	
	//Valid forms for the default header: the default EFC[v=0], or eFCC (for PNG chunk embedding)
	char standardEFC[4]   = { 'E', 'F', 'C', 0x0 };
	char legacyEFCpng[4]  = { 'e', 'F', 'C', 'C' };
	
	//For all other versions, either the standard EFC[v] is supported, or the obfuscated version
	char obfuscatedEFC[3] = { 'E', 'F', 'C' };
	EFCHeader::ObfuscateText(obfuscatedEFC, sizeof(obfuscatedEFC));
	
	//Determine if we are dealing with a valid header
	if
	(
		headerVersion == 0 &&
		(
			memcmp(magicBytes, standardEFC,  sizeof(standardEFC))  == 0 ||
			memcmp(magicBytes, legacyEFCpng, sizeof(legacyEFCpng)) == 0
		)
	)
	{
		//Instantiate the correct version of the header class
		switch (headerVersion)
		{
			case EFCHeaderVersion::Default:
				return new EFCDefaultHeader(file);
		}
	}
	
	//The file does not contain a valid EFC header
	return NULL;
}
