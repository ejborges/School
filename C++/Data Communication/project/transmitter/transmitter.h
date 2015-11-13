/*
Author:			Emilio Borges
Date:			Oct. 8, 2015
Class:			EECS 3150 Data Communications
Project:		Socket Tx/Rx

File:			transmitter.h
*/

#ifndef transmitter_h	// start of include guard
#define transmitter_h

#include <fstream>
#include <string>

class transmitter {
	public:
		
		//constructor
		transmitter();

		//deconstructor
		~transmitter();

		//public functions
		void readFile(std::string FileName);

		//public variables
		unsigned char *fileBuffer;
		unsigned int fileLength = 0;

	private:

};

#endif	// end of include guard
// No other code should go after this line