// password.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dictionaryDecrypter.h"
#include "dictionaryLoader.h"

#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>

// Prototypes
void hashWordMenu();
void dictionaryLookupMenu();
void decryptDictionaryMenu();

// Globals
std::unordered_map<std::string, DictionaryLoader::entry*> entryMap;
std::map<int, DictionaryDecrypter::SolvedEntry*> solvedPasswords;
std::string dictionaryFilename;

/* Main menu that gives four options for the user. 
*/
void mainMenu(){
	bool validEntry = false;
	int input;

	// Loop until user input is valid
	while (!validEntry){ 
		std::cout << "Select an option:" << std::endl;
		std::cout << "1. Basic Hashing" << std::endl;
		std::cout << "2. Load Dictionary" << std::endl;
		std::cout << "3. Decrypt" << std::endl;
		std::cout << "4. Exit" << std::endl;

		std::string stringInput;
		std::getline(std::cin, stringInput);

		std::stringstream(stringInput) >> input;

		if (input == 1 || input == 2 || input == 3 || input == 4){
			validEntry = true;
		}
		else {
			std::cout << "Not a valid menu choice." << std::endl << std::endl;
		}
	}

	switch (input){
	case (1) :
		hashWordMenu();
		mainMenu();
		break;
	case (2) :
		dictionaryLookupMenu();
		break;
	case (3) :
		decryptDictionaryMenu();
		break;
	case (4) :
		std::cout << "Exiting program now." << std::endl;
		return;
	default:
		break;
	}
}

/* Menu for user to input words. Hashed hexadecimal equivalent of word
is printed. 
*/
void hashWordMenu(){
	std::string stringInput;

	std::cout << "Enter password to hash: ";
	std::getline(std::cin, stringInput);

	// Containers
	unsigned char hash[20];
	char hex_str[41];

	DictionaryDecrypter dd;	// Hash function is within DictionaryDecrypter class

	dd.hashWord(stringInput, hash, hex_str);

	// Prints out hashed hexadecimal
	std::cout << "\nHashed ";				
	int i = 0;
	while (hex_str[i] != '\0'){
		std::cout << hex_str[i];
		i++;
	}
	std::cout << std::endl << std::endl;
}

/* Menu for user to load a dictionary file into the global entryMap
*/

void dictionaryLookupMenu(){
	bool validEntry = false;
	int input;

	// Loop until user input is valid
	while (!validEntry){
		std::cout << "1. Use default dictionary file d8.txt" << std::endl;
		std::cout << "2. Use custom dictionary file" << std::endl;
		std::cout << "3. Return to main menu" << std::endl;

		std::string stringInput;
		std::getline(std::cin, stringInput);

		std::stringstream(stringInput) >> input;

		if (input == 1 || input == 2 || input == 3){
			validEntry = true;
		}
		else {
			std::cout << "Not a valid menu choice." << std::endl << std::endl;
		}
	}

	DictionaryLoader dictionaryLoad; // Lookup function in this class

	switch (input){
	case (1) :
		dictionaryLoad.dictionaryLookup(true, entryMap, dictionaryFilename);	// True - Using default d8.txt file
		mainMenu();																// entryMap passed by reference
		break;
	case (2) :
		dictionaryLoad.dictionaryLookup(false, entryMap, dictionaryFilename);	// False - Using custom filename, method asks for user input
		mainMenu();																// entryMap passed by reference
		break;
	case (3) :
		mainMenu();
		break;
	default:
		break;
	}
}

/* Menu for user to decrypt a password file.
*/

void decryptDictionaryMenu(){
	if (entryMap.size() == 0) { // If no dictionary has been loaded
		std::cout << "Dictionary file must be loaded first\n\n";
		mainMenu();
	}
	else {
		bool validEntry = false;
		int input;

		// Loop until user input is valid
		while (!validEntry){
			std::cout << "1. Decrypt default password file pass.txt" << std::endl;
			std::cout << "2. Decrypt custom dictionary file" << std::endl;
			std::cout << "3. Return to main menu" << std::endl;

			std::string stringInput;
			std::getline(std::cin, stringInput);

			std::stringstream(stringInput) >> input;

			if (input == 1 || input == 2 || input == 3){
				validEntry = true;
			}
			else {
				std::cout << "Not a valid menu choice." << std::endl << std::endl;
			}
		}

		DictionaryDecrypter dictionaryDecrypt; // Decrypt method is in this class

		switch (input){
		case (1) :
			dictionaryDecrypt.decryptDictionary(true, entryMap, solvedPasswords);	// True for default pass.txt file
			mainMenu();																// entryMap and solvedPasswords passed by reference
			break;
		case (2) :
			dictionaryDecrypt.decryptDictionary(false, entryMap, solvedPasswords);	// False for custom file. User input asked for in method
			mainMenu();																// entryMap and solvedPasswords passed by reference
			break;
		case (3) :
			mainMenu();
			break;
		default:
			break;
		}
	}
}

/* Main function
*/
int _tmain(int argc, _TCHAR* argv[])
{
	mainMenu();

	return 0;
}

