// DictionaryDecrypter.cpp : Menu Option 3, allows user to select a password file and decrypt all of the
// hexadecimal values. Also contains the hashWord function.

#include "DictionaryDecrypter.h"
#include "DictionaryLoader.h"
#include "sha1.h"

#include <iostream>
#include <sstream>
#include <list>
#include <fstream>
#include <Windows.h>
#include <unordered_map>
#include <map>
#include <ppl.h>

// Prototypes
void hashWord(std::string, unsigned char*, char*);
void bruteForceSolve(bool);
char convertDigitToCharacter(int x);

/* Function used in DictionaryLoader class and bruteForceSolve function below. Uses the sha1 class's methods
to first hash a word, then convert to a hexadecimal string. Values stored in hash[] and hex_str[]
*/
void DictionaryDecrypter::hashWord(std::string stringInput, unsigned char hash[], char hex_str[]){
	sha1::calc(stringInput.c_str(), stringInput.length(), hash);

	sha1::toHexString(hash, hex_str);
}

/* Function loads a password file and gets each hexadecimal string line-by-line and attempts to match
the each one with the hex strings in entryMap, which must be preloaded from Menu Option 2 (DictionaryLoader).
For those that cannot be found in entryMap, the passwords are cracked with the bruteForceSolve method.
*/
void DictionaryDecrypter::decryptDictionary(bool defaultFilename,
	std::unordered_map<std::string, DictionaryLoader::entry*> &entryMap,
	std::map<int, DictionaryDecrypter::SolvedEntry*> &solvedPasswords){

	std::string filename;
	std::list<SolvedEntry*> unsolvedPasswords;

	int entryNumber = 0;

	if (!defaultFilename){
		std::cout << "Enter custom filename: ";
		std::getline(std::cin, filename);
	}
	else {
		filename = "pass.txt";
	}

	std::ifstream file(filename);
	std::ofstream outfile("pass_solved.txt");

	if (file.is_open()){
		std::cout << "Decrypting " + filename + "...\n";
		std::string line;
		while (std::getline(file, line)){
			SolvedEntry * newEntry = new SolvedEntry;
			newEntry->passwordEntryNumber = entryNumber++;	// Increment entryNumber for each entry and store it so the passwords are
			// outputted correctly after all are decrypted
			newEntry->hex_str = line;						// Store the hexadecimal string

			if (entryMap.find(line) != entryMap.end()){		// If an entry in the entryMap matches the hex string...
				newEntry->plainTextSolution = entryMap[line]->word; // Store the plainText solution from the entryMap
				std::pair<int, SolvedEntry*> newPair(newEntry->passwordEntryNumber, newEntry); // Make a pair of the entryNumber and pointer to the entry
				solvedPasswords.insert(newPair);
			}
			else {
				unsolvedPasswords.push_back(newEntry);	// If a hex doesn't get a match in the entryMap, store in
			}											// list of unsolved passwords to later brute force solve
		}

		// Measures performance time of brute force solve
		LARGE_INTEGER freq, before, after; QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&before);

		// Uses Concurrency to brute force solve multiple passwords
		// Lambda that passes in solvedPasswords by reference and each of the unsolved passwords into bruteForceSolve
		Concurrency::parallel_for_each(unsolvedPasswords.begin(), unsolvedPasswords.end(), [this, &solvedPasswords](SolvedEntry* x) {
			DictionaryDecrypter::bruteForceSolve(x, solvedPasswords);
		});

		QueryPerformanceCounter(&after);
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;

		std::cout << "\nTime elapsed: " << fElapsed << " seconds" << std::endl;

		std::map<int, SolvedEntry*>::iterator iter;
		std::string strToReturn;

		// Once all passwords are decrypted, print them out into the outfile "pass_solved.txt"
		for (iter = solvedPasswords.begin(); iter != solvedPasswords.end(); ++iter) {
			strToReturn.append(iter->second->hex_str);					// First item is the hexadecimal string
			strToReturn.append(", ");
			strToReturn.append(iter->second->plainTextSolution + "\n"); // Second item is the actual password
		}

		outfile << strToReturn;

		std::cout << "Output file pass_solved.txt created. \n\n";
		outfile.close();

		// Avoid memory leak and delete all SolvedEntrys
		for (SolvedEntry * x : unsolvedPasswords){
			delete x;
		}
	}
	else {
		std::cout << "File could not be opened." << std::endl << std::endl;
	}
}

/* Function that brute force solves any unsolved passwords by trying every possible permutation
for four-letter passwords. Passes in a pointer to an unsolvedPassword entry, and the solvedPasswords
map by reference.
*/
void DictionaryDecrypter::bruteForceSolve(SolvedEntry* unsolvedPassword,
	std::map<int, DictionaryDecrypter::SolvedEntry*> &solvedPasswords){

	// Initialization
	int digits[4] = { 0, 0, 0, 0 };
	char potentialPassword[5] = { NULL };
	unsigned char hash[20];
	char hex_str[41];

	/* Loop through all unsolved passwords and try each permutation four digits long until
	all passwords are solved. For loop of int i iterates 1679616 times because that is 36^4,
	the number of permutations of four characters.
	Digits: [0][1][2][3]
	Potential Password: [0][1][2][3]
	*/
	for (int i = 0; i < 1679616; i++){
		if (digits[0] == 36){	// Carryover to tens digit
			digits[0] = 0;
			digits[1]++;
		}
		if (digits[1] == 36){	// Carryover to hundreds digit
			digits[1] = 0;
			digits[2]++;
		}
		if (digits[2] == 36){	// Carryover to thousands digit
			digits[2] = 0;
			digits[3]++;
		}

		// Always convert the ones digit to a character
		potentialPassword[0] = convertDigitToCharacter(digits[0]);

		// Only convert the tens digit to a character once a carryover to the tens digit has occurred
		if (i >= 36){
			if (i == 36) digits[1] = 0; // Must initialize to 0, otherwise the carryover will increment
			// and just start at 1
			potentialPassword[1] = convertDigitToCharacter(digits[1]);
		}

		// Only convert the hundreds digit to a character once a carryover to the hundreds digit has occurred
		if (i >= 1296){
			if (i == 1296) digits[2] = 0;	// Must initialize to 0, otherwise the carryover will increment
			// and just start at 1
			potentialPassword[2] = convertDigitToCharacter(digits[2]);
		}

		// Only convert the thousands digit to a character once a carryover to the thousands digit has occurred
		if (i >= 46656){
			if (i == 46656) digits[3] = 0;	// Must initialize to 0, otherwise the carryover will increment
			// and just start at 1
			potentialPassword[3] = convertDigitToCharacter(digits[3]);
		}

		digits[0]++; // Increment the ones digit here rather than earlier so [0000] is checked as a possible solution 

		hashWord(potentialPassword, hash, hex_str);		// Convert the permutation into hex

		if (unsolvedPassword->hex_str == hex_str){		// Check if the hex matches the unsolved password hex
			// If it does, store everything!
			unsolvedPassword->plainTextSolution = potentialPassword;
			std::pair<int, SolvedEntry*> newPair(unsolvedPassword->passwordEntryNumber, unsolvedPassword);
			solvedPasswords.insert(newPair);
			return; // Break out of this function because the password has been decrypted, don't need to try other permutations
		}
	}

	// Password was not able to be decrypted
	unsolvedPassword->plainTextSolution = "????";
	std::pair<int, SolvedEntry*> newPair(unsolvedPassword->passwordEntryNumber, unsolvedPassword);
	solvedPasswords.insert(newPair);
}

char DictionaryDecrypter::convertDigitToCharacter(int x){
	char c = ' ';
	// ASCII logic, converts integer x to appropriate ASCII value
	if (x <= 25){
		c = x + 97;			// Character is a letter
	}
	else {
		c = x + 22;			// Character is a number 0-9
	}
	return c;
}
