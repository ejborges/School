/*
Author:			Emilio Borges
Date:			Oct. 8, 2015
Class:			EECS 3150 Data Communications
Project:		Socket Tx/Rx

VS Project:		transmitter
File:			main (client) .cpp
*/

#include "txrx.h"
#include "transmitter.h"

#include <iostream>

// defines constants some includes need
#define _WIN32_WINNT 0x0501

//Socket includes
#include <sys/types.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

using namespace std;

bool socketsEnabled = true;

char const arg_help[] = "help";
char const arg_c[] = "c";
char const arg_crc[] = "crc";
char const arg_h[] = "h";
char const arg_hamming[] = "hamming";
char const arg_n[] = "n";
char const arg_none[] = "none";
char errorType;
int numberOfErrors;

void myExit(){
	cout << "\n\n</transmitter>\n";
	cout << "\nProgram has finished\nPress ENTER to exit\7";
	getchar();
	exit(0);
}

//Arguments needed: transmitter.exe <host> <port> <file> <type> <errors> OR help
int main(int argc, char *argv[]){

	cout << "\n<transmitter>\n\n";

	if (argc == 2 && strcmp(argv[1], arg_help) == 0) {
		cout << "\tArguments:"
			<< "\n\t\t<host>\tis the host name/number to connect to"
			<< "\n\t\t\tit can be localhost or an IP address\n"
			<< "\n\t\t<port>\tis the port number to connect to"
			<< "\n\t\t\tit should be the same port used with receiver.exe\n"
			<< "\n\t\t<file>\tis the file to transmit"
			<< "\n\t\t\tthis argument has to include the file extention\n"
			<< "\n\t\t<type>\tis the type of error correction you want to use"
			<< "\n\t\t\toptions: crc, hamming, none\n"
			<< "\n\t\t<errors>\tis the number of bit errors to introduce to each frame"
			<< "\n\t\t\tthis can be any positive integer value";
		myExit();
	}

	if (argc != 6){
		cout << "\tNot the correct arguments. Need <host> <port> <file> <type> <errors> OR help";
		myExit();
	}

	// which type of error correction to use, type will be included in the frame header
	if (strcmp(argv[4], arg_c) == 0 || strcmp(argv[4], arg_crc) == 0) errorType = 'c';
	else if (strcmp(argv[4], arg_h) == 0 || strcmp(argv[4], arg_hamming) == 0) errorType = 'h';
	else if (strcmp(argv[4], arg_n) == 0 || strcmp(argv[4], arg_none) == 0) errorType = 'n';
	else {
		cout << "\tUnknown <type> \"" << argv[3] << "\"";
		myExit();
	}

	// how many errors to cause
	numberOfErrors = atoi(argv[5]);

	int sockfd; // socket file descriptor

	if (socketsEnabled) {

		// initialize winsock stuff
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		int port = atoi(argv[2]);		// argv[] returns strings, atoi() takes string returns int
	  //int sockfd; moved out of if statement to please overlord compiler
		struct sockaddr_in serv_addr;	// socket address for the internet, describes what socket is used for
		struct hostent *server;			// hostent contains info needed to connect to host defined in argv[1]

		sockfd = socket(AF_INET, SOCK_STREAM, 0);		// returns socket file descriptor (int)

		if (sockfd < 0) {
			cout << "\tERROR opening socket";
			myExit();
		}

		server = gethostbyname(argv[1]);

		if (server == NULL) {
			fprintf(stderr, "\tERROR, no such host");
			exit(0);
		}

		memset((char*)&serv_addr, 0, sizeof(serv_addr));	//memset() sets everything in sockaddr_in to zero so it doesn't have garbage in it

		serv_addr.sin_family = AF_INET;

		serv_addr.sin_port = htons(port);	// htons = host to network; makes sure all things on network are in the same order (network byte order)

		memmove((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length); //copies IP info to this program to know what to connect to

		if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
			printf("%d", WSAGetLastError());
			cout << "\n\tERROR connecting :'(";
			myExit();
		}
	}

	transmitter tx;

	cout << "\tLoading: " << argv[3] << endl;
	tx.readFile(argv[3]);

	if (tx.fileLength){

		// print buffer content:
		//cout.write(tx.fileBuffer, tx.fileLength);

		unsigned char* frame;	// [SYN|SYN|length|errorType|64-byte data][CRC|CRC]
		unsigned int bufferPosition = 0;
		string bitString;
		string errorTypeString;
		int frameLength;
		int frameNumber = 0;

		cout << "\tSending...\n";

		do{
			// create frame and update bufferPosition
			frame = makeFrame(tx.fileBuffer, tx.fileLength, bufferPosition, errorType);	// frame = [SYN|SYN|length|errorType|64-byte data][CRC|CRC]

			bufferPosition += frame[2];

			if(errorType == 'n' || errorType == 'h') frameLength = frame[2] + 4;	// frame = [SYN|SYN|length|errorType|64-byte data]
			else if (errorType == 'c') frameLength = frame[2] + 6;					// frame = [SYN|SYN|length|errorType|64-byte data|CRC|CRC]

			// add odd parity bit
			frame = addParity(frame);

			// create bit string from frame
			// each bit in each char in the frame will be turned into either a '0' or '1' char
			if(errorType == 'n') bitString = frameToBitString(frame, frameLength);
			else if (errorType == 'h') bitString = frameToHammingBitString(frame, frameLength);
			else if (errorType == 'c') bitString = frameToCrcBitString(frame, frameLength);

			// cause numberOfErrors to random bits in the message/data portion of the bitString 
			for (int i = 0; i < numberOfErrors; i++){
				if (errorType == 'n' || errorType == 'c') {
					int random = (rand() % ((frameLength - 4) * 8)) + 32;
					cout << "\t\tIntroduced error in frame " << frameNumber << " byte " << random / 8 << " bit " << random % 8 << endl;
					if (bitString.at(random) == '1') bitString.at(random) = '0';
					else							 bitString.at(random) = '1';
				}
				else if (errorType == 'h') {
					int random = (rand() % ((frameLength - 4) * 12)) + 32;
					cout << "\t\tIntroduced error in frame " << frameNumber << " byte " << random / 12 << " bit " << random % 12 << "\trandom = " << random << endl;
					if (bitString.at(random) == '1') bitString.at(random) = '0';
					else							 bitString.at(random) = '1';
				}
			}
			
			
			

			// Testing receiver procedure
			/*
			delete[] frame;			
			if (bitString.at(37) == '1') bitString.at(37) = '0';
			else						 bitString.at(37) = '1';
			if (bitString.at(47) == '1') bitString.at(47) = '0';
			else						 bitString.at(47) = '1';
			if (errorType == 'n') frame = bitStringToFrame(bitString, frameLength);
			else if (errorType == 'h') frame = hammingBitStringToFrame(bitString, frameLength, frameNumber);
			else if (errorType == 'c') frame = crcBitStringToFrame(bitString, frameLength, frameNumber);
			frame = stripParity(frame);
			cout << readFrame(frame, frame[2] + 4);
			*/

			delete[] frame;

			// send frame
			if (socketsEnabled) send(sockfd, bitString.c_str(), bitString.length(), 0);

			frameNumber++;

		} while (bufferPosition < tx.fileLength);

		cout << "\tSent";

		// deconstructor is called when main() exits, deconstructor takes care of deleting buffer
		//delete[] tx.fileBuffer;
	}
	else cout << "\tThere was a problem loading " << argv[3] << " :'(";
	
	if (socketsEnabled) closesocket(sockfd);

	myExit();
}