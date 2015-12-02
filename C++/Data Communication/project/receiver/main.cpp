/*
Author:			Emilio Borges
Date:			Oct. 8, 2015
Class:			EECS 3150 Data Communications
Project:		Socket Tx/Rx

VS Project:		receiver
File:			main (server) .cpp
*/

#include "txrx.h"

#include <iostream>
#include <fstream>
#include <string>


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

char const arg_help[] = "help";
char const arg_c[] = "c";
char const arg_crc[] = "crc";
char const arg_h[] = "h";
char const arg_hamming[] = "hamming";
char const arg_n[] = "n";
char const arg_none[] = "none";
char const arg_d[] = "d";
char const arg_hdb3[] = "hdb3";
char errorType;

void myExit(){
	cout << "\n\n</receiver>\n";
	cout << "\nProgram has finished\nPress ENTER to exit\7";
	getchar();
	exit(0);
}

//Arguments needed: receiver.exe <port> <file> <type> OR help
int main(int argc, char *argv[]){

	cout << "\n<receiver>\n\n";

	if (argc == 2 && strcmp(argv[1], arg_help) == 0) {
		cout << "\tArguments:\n\t\t<port>\tis the port number to look for connections"
			<< "\n\t\t\tit should be the same port used with transmitter.exe\n"
			<< "\n\t\t<file>\tis the file to save the received message to"
			<< "\n\t\t\tthis argument has to include the file extention\n"
			<< "\n\t\t<type>\tis the type of error correction you want to use"
			<< "\n\t\t\toptions: crc, hamming, none, hdb3";
		myExit();
	}

	if (argc != 4){
		cout << "\tNot the correct arguments. Need <port> <file> <type> OR help";
		myExit();
	}

	// which type of error correction to use, type will be included in the frame header
	if (strcmp(argv[3], arg_c) == 0 || strcmp(argv[3], arg_crc) == 0) errorType = 'c';
	else if (strcmp(argv[3], arg_h) == 0 || strcmp(argv[3], arg_hamming) == 0) errorType = 'h';
	else if (strcmp(argv[3], arg_n) == 0 || strcmp(argv[3], arg_none) == 0) errorType = 'n';
	else if (strcmp(argv[3], arg_d) == 0 || strcmp(argv[3], arg_hdb3) == 0) errorType = 'd';
	else {
		cout << "\tUnknown <type> \"" << argv[3] << "\"";
		myExit();
	}

	// initialize winsock stuff
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	// set up listener socket
	cout << "\tSetting up listener\n";

	int listenerfd, sockfd, port, clientLength;
	struct sockaddr_in serv_addr, cli_addr; // server address, client address

	listenerfd = socket(AF_INET, SOCK_STREAM, 0);

	memset((char*)&serv_addr, 0, sizeof(serv_addr)); // clear socket address

	port = atoi(argv[1]); // get port number

	serv_addr.sin_family = AF_INET;		// sets up connection type we are listening for

	serv_addr.sin_addr.s_addr = INADDR_ANY;		// listen for any IP

	serv_addr.sin_port = htons(port);	// set port number, convert to network byte order (htons)

	// binds listener socket to listen on umm the information provided in serv_addr
	if (bind(listenerfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
	{
		cout << "\tERROR on binding";
		myExit();
	}

	listen(listenerfd, 5); // socket to listen on, max connections

	clientLength = sizeof(cli_addr);

	// connects to socket
	cout << "\tListening...";
	sockfd = accept(listenerfd, (struct sockaddr*) &cli_addr, &clientLength);
	cout << "\n\tConnected to port " << argv[1] << endl;

	// open outputFile and create a new file named FileName
	ofstream outputFile;
	outputFile.open(argv[2], ios::binary);

	// if outputFile was created successfully
	if (!outputFile.fail()){

		cout << "\tReceiving data...\n";
		char inputStringBuffer[68 * 8];
		char inputStringBufferHamming[(4 * 8) + (64 * 12)];
		char inputStringBufferCrc[70 * 8];
		size_t received,		// number of bytes received
			   totalReceived;	// total number of bytes we have gotten from recv during loop 
		fd_set readfds;
		string inputString;
		bool streamClosed = false;
		int bytesReceived = 0;
		int frameNumber = 0;
		int bufferSize;

		do{
			received = 0;
			totalReceived = 0;

			// receive frame bit string, received may not receive the whole frame at once
			do{
				FD_ZERO(&readfds);
				FD_SET(sockfd, &readfds);
				select(sockfd, &readfds, NULL, NULL, NULL);

				if (errorType == 'n' || errorType == 'd') {
					bufferSize = sizeof(inputStringBuffer);

					// bytes		what	address where to put bytes received, number of bytes remaining to read, flag stuff
					// received		socket
					received = recv(sockfd, inputStringBuffer + totalReceived, sizeof(inputStringBuffer) - totalReceived, 0);
					totalReceived += received;
				}
				else if (errorType == 'h') {
					bufferSize = sizeof(inputStringBufferHamming);

					// bytes		what	address where to put bytes received, number of bytes remaining to read, flag stuff
					// received		socket
					received = recv(sockfd, inputStringBufferHamming + totalReceived, sizeof(inputStringBufferHamming) - totalReceived, 0);
					totalReceived += received;
				}
				else if (errorType == 'c') {
					bufferSize = sizeof(inputStringBufferCrc);

					// bytes		what	address where to put bytes received, number of bytes remaining to read, flag stuff
					// received		socket
					received = recv(sockfd, inputStringBufferCrc + totalReceived, sizeof(inputStringBufferCrc) - totalReceived, 0);
					totalReceived += received;
				}


			} while (totalReceived != bufferSize && received != 0);

			if (received == 0) streamClosed = true;
			
			if (totalReceived){

				if (errorType == 'n' || errorType == 'd') {
					inputString.assign(inputStringBuffer, totalReceived);
				}
				else if (errorType == 'h') {
					inputString.assign(inputStringBufferHamming, totalReceived);
				}
				else if (errorType == 'c') {
					inputString.assign(inputStringBufferCrc, totalReceived);
				}
				

				// bit string to frame
				unsigned char* frame;	// [SYN|SYN|length|64-byte data]
				
				if (errorType == 'n') frame = bitStringToFrame(inputString, totalReceived / 8);
				else if (errorType == 'h') frame = hammingBitStringToFrame(inputString, (4 + ((totalReceived - 32) / 12)), frameNumber);
				else if (errorType == 'c') frame = crcBitStringToFrame(inputString, totalReceived / 8, frameNumber);
				else if (errorType == 'd') frame = hdb3BitStringToFrame(inputString, totalReceived / 8);
				
				
				
				// remove parity bit
				frame = stripParity(frame);
				
				bytesReceived += frame[2];

				// read frame
				outputFile << readFrame(frame, frame[2] + 4);

				delete[] frame;
			}

			frameNumber++;

		} while (!streamClosed);
  
		outputFile.close();
		cout << "\tReceived " << bytesReceived << " bytes\n";
		cout << "\tData saved to " << argv[2] << "\n\tDone";
	}
	else cout << "\tFailed to create/open " << argv[2] << " :'(";
	
	myExit();
}