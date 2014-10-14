#pragma once

#ifndef DICTIONARYLOADER_DEFINED
#define DICTIONARYLOADER_DEFINED

#include <iostream>
#include <unordered_map>

class DictionaryLoader
{
public:
	// Struct to hold an entry created from each line in a dictionary file
	struct entry {
		std::string word;
		unsigned char hash[20];
		char hex_str[41];
	};

	void dictionaryLookup(bool, std::unordered_map<std::string, entry*>&, std::string&);
};


#endif