/*
Author:			Emilio Borges
Date:			Oct. 8, 2015
Class:			EECS 3150 Data Communications
Project:		Socket Tx/Rx

File:			transmitter.cpp
*/

#include "transmitter.h"

using namespace std;

//constructor
transmitter::transmitter(){

}

//deconstructor
transmitter::~transmitter(){
	delete[] fileBuffer;
}

// Reads the file byte-by-byte and puts each byte into a 
// buffer (char array) that is publicly accessible
// User MUST delete[] fileBuffer; when done with the buffer!
void transmitter::readFile(string FileName){

	// Open FileName
	ifstream inputFile;
	inputFile.open(FileName.c_str(), ios::binary);

	// If FileName opened successfully
	if (!inputFile.fail()){

		// get length of file:
		inputFile.seekg(0, inputFile.end);
		fileLength = inputFile.tellg();
		inputFile.seekg(0, inputFile.beg);

		// allocate memory:
		fileBuffer = new unsigned char[fileLength];

		// read data as a block:
		inputFile.read((char*)fileBuffer, fileLength);

		inputFile.close();
	}
}