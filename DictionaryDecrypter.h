#pragma once

#ifndef DICTIONARYDECRYPTER_DEFINED
#define DICTIONARYDECRYPTER_DEFINED

#include <iostream>
#include <map>
#include <unordered_map>
#include "DictionaryLoader.h"

class DictionaryDecrypter
{	
public:
	// Struct to hold the decrypted password information
	struct SolvedEntry {
		int passwordEntryNumber;
		std::string hex_str;
		std::string plainTextSolution;
	};

	void hashWord(std::string, unsigned char*, char*);
	void decryptDictionary(bool, std::unordered_map<std::string, DictionaryLoader::entry*>&,
		std::map<int, SolvedEntry*>&);
	void bruteForceSolve(SolvedEntry*, std::map<int, SolvedEntry*>&);
	char convertDigitToCharacter(int);
};


#endif