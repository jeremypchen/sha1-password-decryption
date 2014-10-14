// DictionaryLoader.cpp : Menu Option 2, allows user to load in database 
// of dictionary words to hash and match against potential passwords

#include "dictionaryLoader.h"
#include "dictionaryDecrypter.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <Windows.h>

/* Function goes line-by-line through a dictionary file and hashes each word, storing the hashed hexadecimal
value into an entryMap which is passed in by reference from password.cpp.
*/

void DictionaryLoader::dictionaryLookup(bool defaultFilename, std::unordered_map<std::string, entry*> &entryMap,
	std::string &storedFilename){

	std::string filename;
	entryMap.rehash(100000);						// Rehash to optimize unordered_map performance

	if (!defaultFilename){
		std::cout << "Enter custom filename: ";
		std::getline(std::cin, filename);
	}
	else {
		filename = "d8.txt";
	}

	if (filename == storedFilename){
		std::cout << "Dictionary file has already been stored\n\n";
		return;
	}
	else {
		storedFilename = filename;
		std::ifstream file(filename);

		// Used to calculate performance time of hashing and storing the words
		LARGE_INTEGER freq, before, after; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&before);

		DictionaryDecrypter dd;						// HashWord function is in this class

		if (file.is_open()){
			std::string password;
			while (std::getline(file, password)){
				entry * newEntry = new entry;
				newEntry->word = password;			// Store the plain-text word
				// Determine the hashed and hexadecimal values, store in newEntry
				dd.hashWord(password, newEntry->hash, newEntry->hex_str);
				// Make a pair of hexadecimal value and pointer to entry so it can be inserted in entryMap
				std::pair<std::string, entry*> newPair(newEntry->hex_str, newEntry);
				entryMap.insert(newPair);
			}

			QueryPerformanceCounter(&after);
			float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;

			std::cout << "\nDictionary loaded. \nTime elapsed: " << fElapsed << " seconds \n\n";
		}
		else {
			std::cout << "File could not be opened." << std::endl << std::endl;
		}
	}
}

