#include <iostream>
#include <fstream>

constexpr auto CommandLineInstructions = "> bin28xp [input file]";

void chkWriteByte(std::ofstream& file, uint8_t byte, uint16_t& chk)
{
	file.write((char*)&byte, 1);
	chk += byte;
}

void chkWriteWord(std::ofstream & file, uint16_t word, uint16_t& chk)
{
	file.write((char*)&word, 2);
	chk += static_cast<uint8_t>((word << 8) >> 8);
	chk += static_cast<uint8_t>(word >> 8);
}

void chkWrite(std::ofstream& file, const void* buf, int len, uint16_t& chk)
{
	file.write((const char*)buf, len);
	for (int i = 0; i < len; i++)
		chk += static_cast<const uint8_t*>(buf)[i];
}

std::string genName(std::string fileName)
{
	std::string name = "";
	name.resize(8, 0);

	int charactersAdded = 0;
	for (int i = 0; i < static_cast<int32_t>(fileName.length()) && charactersAdded < 8; i++)
	{
		char character = fileName[i];
		if (character >= '0' && character <= '9' && charactersAdded > 0)
			name[charactersAdded] = character;
		else if (character >= 'A' && character <= 'Z')
			name[charactersAdded] = character;
		else if (character >= 'a' && character <= 'z')
			name[charactersAdded] = character - 32;
		else
			continue;

		charactersAdded++;
	}

	if (name[0] == 0)
	{
		name[0] = 'N';
		name[1] = 'U';
		name[2] = 'L';
		name[3] = 'L';
	}

	return name;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "File name missing:\n" << CommandLineInstructions << "\n";
		return 1;
	}

	/* Open files */
	std::string inputName = argv[argc - 1];
	std::string rawName = inputName;
	if (rawName.find('.'))
		rawName = rawName.substr(0, rawName.find('.'));
	std::string outputName = rawName + ".8xp";

	std::ifstream inFile(inputName, std::ios::binary);

	inFile.seekg(0, std::ios_base::end);
	uint32_t inputSize = static_cast<uint32_t>(inFile.tellg());
	inFile.seekg(0, std::ios_base::beg);

	if (!inFile.good())
	{
		std::cout << "Failed to load input file\n";
		inFile.close();
		return 1;
	}

	std::ofstream outFile(outputName, std::ios::binary);

	if (!outFile.good())
	{
		std::cout << "Failed to load output file\n";
		inFile.close();
		outFile.close();
		return 1;
	}

	// Check for 0xbb6d
	uint16_t token = 0;
	inFile.read((char*)&token, 2);
	inFile.seekg(0, std::ios_base::beg);

	bool tokenExists = false;
	if (token == 0x6DBB)
		tokenExists = true;

	/* File Header */
	outFile.write("**TI83F*\x1a\x0a\x00", 11);

	// Comment
	std::string comment = "Generated by bin28xp";
	comment.resize(42, 0);
	outFile.write(comment.c_str(), 42);

	// Data section length
	uint16_t dataLength = static_cast<uint16_t>(inputSize + 19 + (tokenExists == false ? 2 : 0));
	outFile.write((char*)&dataLength, 2);

	/* Data Section */
	uint16_t chk = 0; // Check sum : Lowest 16 bit sum of all data

	// Type marker
	chkWriteByte(outFile, 0x0D, chk);
	chkWriteByte(outFile, 0x00, chk); // Won't effect checksum

	// Actual data length + check sum length
	dataLength = static_cast<uint16_t>(inputSize + 2 + (tokenExists == false ? 2 : 0));
	chkWriteWord(outFile, dataLength, chk);

	// Type id
	chkWriteByte(outFile, 0x06, chk);

	// Name
	std::string progName = genName(rawName);
	chkWrite(outFile, progName.c_str(), 8, chk);
	chkWriteWord(outFile, 0x0000, chk); // Open source code I found added these bytes so I am doing it too. Update: Doesn't seem to work without these bytes
	

	// Data length twice
	chkWriteWord(outFile, dataLength, chk);
	chkWriteWord(outFile, dataLength - 2, chk);

	// Write data
	chkWriteByte(outFile, 0xBB, chk);
	chkWriteByte(outFile, 0x6D, chk);

	uint8_t* dataBuf = static_cast<uint8_t*>(malloc(inputSize));
	inFile.read((char*)dataBuf, inputSize);
	if (tokenExists)
		chkWrite(outFile, dataBuf + 2, inputSize - 2, chk);
	else
		chkWrite(outFile, dataBuf, inputSize, chk);

	// Checksum
	outFile.write((char*)&chk, 2);

	/* Cleanup */
	free(dataBuf);
	inFile.close();
	outFile.close();

	std::cout << "Finished converting '" << inputName << "' to '" << outputName << "'\n";

	return 0;
}