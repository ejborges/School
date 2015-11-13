/*
Author:			Emilio Borges
Date:			Oct. 8, 2015
Class:			EECS 3150 Data Communications
Project:		Socket Tx/Rx

File:			txrx (library) .cpp
*/

#include <string>
#include <iostream>

unsigned char *makeFrame(unsigned char* buffer, int bufferLength, int bufferPosition, int errorType){

	int bytes;

	if ((bufferLength - bufferPosition) > 64) bytes = 64 + 4;
	else bytes = bufferLength - bufferPosition + 4;

	unsigned char* frame = new unsigned char[bytes];	// [SYN|SYN|length|errorType|64-byte data]

	// initialize each char to null
	for (int i = 0; i < bytes; i++) frame[i] = 0;

	// set SYN characters
	frame[0] = frame[1] = 22;

	// set data length
	frame[2] = bytes - 4;

	// set error type
	frame[3] = errorType;

	// write data to frame
	for (int i = 4; i < bytes; i++, bufferPosition++) frame[i] = buffer[bufferPosition];
	return frame;
}

// extract the data from the frame
std::string readFrame(unsigned char* frame){

	std::string frameString = "";
	int length;
	switch (frame[3]) {
		case 'n': length = 68; break;	//error type = none
		case 'h': length = 68; break;	//error type = hamming
		case 'c': length = 70; break;	//error type = crc
	}
	for (int i = 4; i < length; i++) frameString += frame[i];
	return frameString;
}

// adds a odd parity bit to the most significat bit of every byte in the frame
// odd parity == there must be an odd # of 1s in the byte
unsigned char *addParity(unsigned char* frame){

	int bytes = frame[2] + 4;

	// loop through each byte in the frame
	for (int byte = 0; byte < bytes; byte++){

		int count = 0;

		// count the number of ones in the character
		for (unsigned char i = 0; i < 8; i++)
			if (frame[byte] & (1 << i))	count++;

		// if there were an even number of ones, set parity bit to 1 to get odd parity
		if (!(count % 2)) frame[byte] = frame[byte] | 128;
	}
			
	return frame;
}

// remove parity bit
unsigned char *stripParity(unsigned char* frame){

	frame[2] &= ~(1 << 7);

	int bytes = frame[2] + 4;

	for (int byte = 0; byte < bytes; byte++)
		frame[byte] &= ~(1 << 7);
	return frame;
}

// create bit string from frame
// each bit in each char in the frame will be turned into either a '0' or '1' char
std::string frameToBitString(unsigned char* frame, int frameLength){

	std::string string = "";

	for (int byte = 0; byte < frameLength; byte++)
		for (int bit = 0; bit < 8; bit++){
			if (frame[byte] & (1 << bit)) string += "1";
			else						  string += "0";
		}

	return string;
}

// creates a frame from a string of 1s and 0s 
// "01101000..." to [SYN|SYN|length|64-byte data]
// byte bits are ordered from least significant bit to most sign
unsigned char *bitStringToFrame(std::string string, int frameLength){

	unsigned char* frame = new unsigned char[frameLength];	// [SYN|SYN|length|errorType|64-byte data]

	for (int byte = 0; byte < frameLength; byte++){

		// clear the byte 
		frame[byte] = 0;

		// fill the byte
		for (int bit = 0; bit < 8; bit++)
			if (string.at(byte * 8 + (7 - bit)) == '1') frame[byte] |= (1 << (7 - bit));
	}

	return frame;
}

// create bit string from frame
// each bit in each char in the frame will be turned into either a '0' or '1' char
// while processing each byte in the message portion of the frame, run the byte through 
//   the hamming code algorythm. The result will be a 12 bit byte with bits 0, 1, 3, and 7 
//   as the parity check bits.
std::string frameToHammingBitString(unsigned char* frame, int frameLength) {
	
	// frame = [SYN|SYN|length|errorType|64-byte data]

	std::string string = "";

	// frame header to bit string
	for (int byte = 0; byte < 4; byte++)
		for (int bit = 0; bit < 8; bit++) {
			if (frame[byte] & (1 << bit)) string += "1";
			else						  string += "0";
		}

	// frame message to hamming code to bit string
	for (int byte = 4; byte < frameLength; byte++) {
		string += getHammingByte(frame[byte]);
	}
		
	return string;
	
}

// creates a frame from a string of 1s and 0s 
// "01101000..." to [SYN|SYN|length|errorType|64-byte data]   // 12-bit hamming bytes
// byte bits are ordered from least significant bit to most sign
// hamming byte = p1 p2 b0 p4 b1 b2 b3 p8 b4 b5 b6 b7
//
// if any error in message, print out:
//		- frame number, byte in frame, bit in byte
// and attempt to fix error
unsigned char *hammingBitStringToFrame(std::string string, int frameLength, int frameNumber) {
	
	unsigned char* frame = new unsigned char[frameLength];	// [SYN|SYN|length|errorType|64-byte data]

	// add the 4 header bytes to frame	// [SYN|SYN|length|errorType|
	for (int byte = 0; byte < 4; byte++) {

		// clear the byte 
		frame[byte] = 0;

		// fill the byte
		for (int bit = 0; bit < 8; bit++)
			if (string.at(byte * 8 + (7 - bit)) == '1') frame[byte] |= (1 << (7 - bit));
	}

	// add the rest of the bytes to frame
	// detect any 1-bit errors in each byte, report bit error location, fix single bit error
	for (int hammingByte = 4; hammingByte < frameLength; hammingByte++) {

		char dataBits = 0;
		std::string tempBitString = "";

		// clear the byte
		frame[hammingByte] = 0;





		// hamming = p1 p2 b0 p4 b1 b2 b3 p8 b4 b5 b6 b7

		// grab data bits from hamming byte
		/*for (int hammingCharBit = 2, int j = 0; hammingCharBit < 12; hammingCharBit++) {
			if (hammingCharBit == 3 || hammingCharBit == 7) continue;
			if (string.at(32 + ((hammingByte - 4) * 12) + hammingCharBit) == '1') dataBits |= (1 << j);
			j++;
		}

		// run dataBits through Hamming parity check to find any errors

		// hamming = p1 p2 b0 p4 b1 b2 b3 p8 b4 b5 b6 b7

		int p1, p2, p4, p8;
		p1 = p2 = p4 = p8 = 0;

		// count 1's for p1
		for (int i = 0; i < 8; i++) {
			if (i == 2 || i == 5 || i == 7) continue;
			if (character & (1 << i)) p1++;
		}

		// count 1's for p2
		for (int i = 0; i < 8; i++) {
			if (i == 1 || i == 4 || i == 7) continue;
			if (character & (1 << i)) p2++;
		}

		// count 1's for p4
		for (int i = 1; i < 8; i++) {
			if (i == 4 || i == 5 || i == 6) continue;
			if (character & (1 << i)) p4++;
		}

		// count 1's for p8
		for (int i = 4; i < 8; i++) {
			if (character & (1 << i)) p8++;
		}

		// odd parity, so if even # of 1's, p1 = 1 to keep odd parity

		// write p1
		if (p1 % 2) string += "0";
		else		string += "1";

		// write p2
		if (p2 % 2) string += "0";
		else		string += "1";*/




		// fill the byte
		//for (int bit = 0; bit < 8; bit++)
		//	if (string.at(hammingByte * 8 + (7 - bit)) == '1') frame[hammingByte] |= (1 << (7 - bit));
	}

	return frame;

}











// convert byte in bit string format to char
unsigned char bitStringByteToChar(std::string string, int end) {

	if (end < 7) return '!';

	unsigned char character = 0;

	// fill the byte
	for (int bit = 0; bit < 8; bit++)
		if (string.at(end - bit) == '1') character |= (1 << (7 - bit));

	//strip parity
	character &= 127;

	return character;
}



// generate a 12-bit hamming byte from char
std::string getHammingByte(char character) {
	std::string string = "";

	// hamming = p1 p2 b0 p4 b1 b2 b3 p8 b4 b5 b6 b7

	int p1, p2, p4, p8;
	p1 = p2 = p4 = p8 = 0;

	// count 1's for p1
	for (int i = 0; i < 8; i++) {
		if (i == 2 || i == 5 || i == 7) continue;
		if (character & (1 << i)) p1++;
	}

	// count 1's for p2
	for (int i = 0; i < 8; i++) {
		if (i == 1 || i == 4 || i == 7) continue;
		if (character & (1 << i)) p2++;
	}

	// count 1's for p4
	for (int i = 1; i < 8; i++) {
		if (i == 4 || i == 5 || i == 6) continue;
		if (character & (1 << i)) p4++;
	}

	// count 1's for p8
	for (int i = 4; i < 8; i++) {
		if (character & (1 << i)) p8++;
	}

	// odd parity, so if even # of 1's, p1 = 1 to keep odd parity

	// write p1
	if (p1 % 2) string += "0";
	else		string += "1";

	// write p2
	if (p2 % 2) string += "0";
	else		string += "1";

	// write bit 0
	if (character & (1 << 0)) string += "1";
	else						string += "0";

	// write p4
	if (p4 % 2) string += "0";
	else		string += "1";

	// write bit 1
	if (character & (1 << 1)) string += "1";
	else						string += "0";

	// write bit 2
	if (character & (1 << 2)) string += "1";
	else						string += "0";

	// write bit 3
	if (character & (1 << 3)) string += "1";
	else						string += "0";

	// write p8
	if (p8 % 2) string += "0";
	else		string += "1";

	// write bit 4
	if (character & (1 << 4)) string += "1";
	else						string += "0";

	// write bit 5
	if (character & (1 << 5)) string += "1";
	else						string += "0";

	// write bit 6
	if (character & (1 << 6)) string += "1";
	else						string += "0";

	// write bit 7
	if (character & (1 << 7)) string += "1";
	else						string += "0";

	return string;
}















/*// hamming = p1 p2 b0 p4 b1 b2 b3 p8 b4 b5 b6 b7

	int p1, p2, p4, p8;
	p1 = p2 = p4 = p8 = 0;

	// count 1's for p1
	for (int i = 0; i < 8; i++) {
		if (i == 2 || i == 5 || i == 7) continue;
		if (frame[byte] & (1 << i)) p1++;
	}

	// count 1's for p2
	for (int i = 0; i < 8; i++) {
		if (i == 1 || i == 4 || i == 7) continue;
		if (frame[byte] & (1 << i)) p2++;
	}

	// count 1's for p4
	for (int i = 1; i < 8; i++) {
		if (i == 4 || i == 5 || i == 6) continue;
		if (frame[byte] & (1 << i)) p4++;
	}

	// count 1's for p8
	for (int i = 4; i < 8; i++) {
		if (frame[byte] & (1 << i)) p8++;
	}

	// odd parity, so if even # of 1's, p1 = 1 to keep odd parity

	// write p1
	if (p1 % 2) string += "0";
	else		string += "1";

	// write p2
	if (p2 % 2) string += "0";
	else		string += "1";

	// write bit 0
	if (frame[byte] & (1 << 0)) string += "1";
	else						string += "0";

	// write p4
	if (p4 % 2) string += "0";
	else		string += "1";

	// write bit 1
	if (frame[byte] & (1 << 1)) string += "1";
	else						string += "0";

	// write bit 2
	if (frame[byte] & (1 << 2)) string += "1";
	else						string += "0";

	// write bit 3
	if (frame[byte] & (1 << 3)) string += "1";
	else						string += "0";

	// write p8
	if (p8 % 2) string += "0";
	else		string += "1";

	// write bit 4
	if (frame[byte] & (1 << 4)) string += "1";
	else						string += "0";

	// write bit 5
	if (frame[byte] & (1 << 5)) string += "1";
	else						string += "0";

	// write bit 6
	if (frame[byte] & (1 << 6)) string += "1";
	else						string += "0";

	// write bit 7
	if (frame[byte] & (1 << 7)) string += "1";
	else						string += "0";*/