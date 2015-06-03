/*
Emilio Borges - Mar. 10, 2015
EECS 2510 - Non-Linear Data Structures
Project 1 - Huffman Encoding System
*/

#include "Huffman.h"

using namespace std;

int main()
{
	Huffman huffman;

	// true to run test, false to run Shakespeare
	if (false){

		// Build Huffman tree and generate encoding strings
		huffman.InitializeFromFile("test.txt");

		// encode .txt and save encoded bytes to .enc
		huffman.EncodeFile("test.txt", "test.enc");

		// decode the encoded file and save to the decoded file
		huffman.DecodeFile("test.enc", "test.dec");
	}
	else{

		// Build Huffman tree and generate encoding strings
		huffman.InitializeFromFile("Shakespeare.txt");

		// encode .txt and save encoded bytes to .enc
		huffman.EncodeFile("Shakespeare.txt", "Shakespeare.enc");

		// decode the encoded file and save to the decoded file
		huffman.DecodeFile("Shakespeare.enc", "Shakespeare.dec");
	}
	
	char waitChar;
	cout << "\nProcess complete. Press ENTER to exit.";
	cin.get(waitChar);
	return 0;
}