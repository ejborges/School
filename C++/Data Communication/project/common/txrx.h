/*
Author:			Emilio Borges
Date:			Oct. 8, 2015
Class:			EECS 3150 Data Communications
Project:		Socket Tx/Rx

File:			txrx (library) .h
*/

#ifndef txrx_h	// start of include guard
#define txrx_h

#include <string>

// make/read frame
unsigned char *makeFrame(unsigned char* buffer, int bufferLength, int bufferPosition, int errorType);
std::string readFrame(unsigned char* frame);

// add/remove parity
unsigned char *addParity(unsigned char* frame);
unsigned char *stripParity(unsigned char* frame);

// frame to/from bit string
std::string frameToBitString(unsigned char* frame, int frameLength);
unsigned char *bitStringToFrame(std::string string, int frameLength);

// frame to/from hamming bit string
std::string frameToHammingBitString(unsigned char* frame, int frameLength);
unsigned char *hammingBitStringToFrame(std::string string, int frameLength, int frameNumber);




// Extra Functions

// convert byte in bit string format to char
unsigned char bitStringByteToChar(std::string string, int endPos);

unsigned char bitStringHammingByteToChar(std::string string, int startPos);

// generate a 12-bit hamming byte from char
std::string generateHammingByteBitString(char character);

#endif	// end of include guard
// No other code should go after this line