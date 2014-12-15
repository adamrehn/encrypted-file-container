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
#include "EFCDefaultHeader.h"

//Needed for endianness detection
#include <simple-base/base.h>

EFCDefaultHeader::EFCDefaultHeader()
{
	this->compression = CompressionType::None;
	this->cipher      = EncryptionType::None;
	this->filename    = "";
	this->payloadSize = 0;
}

EFCDefaultHeader::EFCDefaultHeader(MeteredIfstream& inputFile)
{
	//Set the default values
	EFCDefaultHeader();
	
	//Read the "filesize" field (will include header length, which we need to remove)
	inputFile.ReadLittleEndian((char*)&this->payloadSize, sizeof(this->payloadSize));
	
	//Read the file extension (we need to replace the input file's extension with it to get the filename)
	string extension = "";
	inputFile.getline(extension, '\0');
	this->payloadSize -= extension.length() + 1;
	this->filename = replace_extension(basename(inputFile.GetFileName()), this->ObfuscateText(extension));
	
	//Read the "cipher" field, and transform it into a valid EncryptionType member
	string cipherName = "";
	inputFile.getline(cipherName, '\0');
	this->payloadSize -= cipherName.length() + 1;
	this->cipher = (this->ObfuscateText(cipherName) == "AES_256_CFB") ? EncryptionType::AES_256_CFB : EncryptionType::None;
	
	//Read whether or not compression is being used
	bool useCompression = false;
	inputFile.read((char*)&(useCompression), sizeof(useCompression));
	this->payloadSize -= sizeof(useCompression);
	this->compression = (useCompression) ? CompressionType::Zlib : CompressionType::None;
}

void EFCDefaultHeader::WriteHeader(MeteredOfstream& outputFile)
{
	//Write the magic bytes
	char magicBytes[4] = { 'E', 'F', 'C', 0x0 };
	outputFile.write(magicBytes, sizeof(magicBytes));
	
	//Retrieve the file extension from the filename, and obfuscate it
	string extension = get_extension(this->filename);
	extension = this->ObfuscateText(extension);
	
	//Generate the cipher name from the EncryptionType member, and obfuscate it
	string cipherName = (this->cipher == EncryptionType::AES_256_CFB) ? "AES_256_CFB" : "";
	cipherName = this->ObfuscateText(cipherName);
	
	//Set the compression flag based on the CompressionType member
	bool compressionUsed = (this->compression == CompressionType::Zlib);
	
	//Add the header lengths to the playload size
	this->payloadSize += extension.length() + 1;
	this->payloadSize += cipherName.length() + 1;
	this->payloadSize += sizeof(compressionUsed);
	
	//Write the "filesize" field
	outputFile.WriteLittleEndian((char*)&this->payloadSize, sizeof(this->payloadSize));
	
	//Write the extension
	outputFile.write(extension.c_str(), extension.length() + 1);
	
	//Write the cipher name
	outputFile.write(cipherName.c_str(), cipherName.length() + 1);
	
	//Write the compression flag
	outputFile.write((char*)&compressionUsed, sizeof(compressionUsed));
}
