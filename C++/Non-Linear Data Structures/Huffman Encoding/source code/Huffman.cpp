/*
Emilio Borges - Mar. 10, 2015
EECS 2510 - Non-Linear Data Structures
Project 1 - Huffman Encoding System (HES)
*/


#include "Huffman.h"

using namespace std;

// Huffman Constructor
Huffman::Huffman(){

	// Make sure each array slot in frequencyTable points to a unique charNode
	// and all the charNodes have the appropriate char assigned to them.
	for (int i = 0; i < 256; i++) {
		frequencyTable[i] = new node;
		frequencyTable[i]->character = (char)i;
	}
}


// Initialize
//
// 1. Build the Huffman tree by:
//		1.1 Creating and populating a character frequency table based on how many times each of the 256
//			ASCII appeared in the file.
//		1.2 Sorting the character frequency table by character count in decending order.
//		1.3 Creating a parent node for the two lowest count nodes in the frequency table, replacing those 
//			two nodes with the parent and NULL, sorting the table, and repeating this step until we are
//			only left with the 0 index position in the frequency table array not being NULL. The 0 index
//			position will then hold the Huffman tree's root node.
// 2. Generate encoding strings by recursively traversing the tree and assigning 0 when taking a left path
//	  and a 1 when taking a right path in the tree.
//
// Prints:
//		filename used to build the tree and encoding strings
//		number of bytes read
//		elapsed time this process took in seconds with milisecond precision
//
void Huffman::InitializeFromFile(string FileName){

	// Save start time
	clock_t START_TIME = clock();

	// Statistics variable
	unsigned long input_bytes = 0;


	// Open FileName
	ifstream inputFile;
	inputFile.open(FileName.c_str(), ios::binary);

	// If FileName opened successfully
	if (!inputFile.fail()){

	// 1. Build the Huffman tree

	// 1.1 Populate Frequency Table
	
		// Read one byte, or char, at a time until all bytes have been read
		char in_char;
		inputFile.get(in_char);		// The first byte is read outside the loop to prevent a duplicate character at the
										// end due to the end-of-file (eof) flag triggering when trying to read a byte after last byte.
		while (!inputFile.eof()){
			frequencyTable[((int)in_char) & 0xff]->count++;	// While traversing through each char in the file, increment it's count in the frequency table.
																// (int)character may return a negative value since chars are signed types and using an unsigned char
																// wont work with ifstream functions, so we use (((int)character) & 0xff) to ensure we get a positive number.
			input_bytes++;
			inputFile.get(in_char);
		}

		// close FileName
		inputFile.close();

	// 1.2 Sort the Frequency Table by count value

		// Sorting will be used a lot while building the tree in step 1.3, so it's helpful to have the sorting code as a function.
		sortFrequencyTable();

		// This line is for testing and debugging purposes. Prints the frequency table array.
		//cout << "\nIndex\tChar\tASCII\tcount\n";
		//for (int i = 0; i < 256; i++) cout << i << "\t" << frequencyTable[i]->character << "\t" << (((int)frequencyTable[i]->character) & 0xff) << "\t" << frequencyTable[i]->count << endl;

	// 1.3 Build the Huffman Tree

		// Starting at the very end of the frequency table (starting with the nodes with the lowest count)
		// and working our way towards the array's 0 index position. 
		for (int length = 255; length > 0; length--)
		{
			// Create a new node that will be a parent to the two lowest count nodes in the frequency table.
			// The parent's count value will be the sum of the count values from it's two children.
			node *temp = new node;
			temp->count = frequencyTable[length]->count + frequencyTable[length - 1]->count;
			temp->left_ptr = frequencyTable[length - 1];
			temp->right_ptr = frequencyTable[length];
			
			// Set the parent's height to the correct value. This is useful if the sorting algorithm is set to also sort by height.
			if (frequencyTable[length]->height > frequencyTable[length - 1]->height) temp->height = ++frequencyTable[length]->height;
			else temp->height = ++frequencyTable[length - 1]->height;
			
			// The parent node now points to the two lowest count nodes in frequency table, so we can replace those 
			// two spots in the frequency table array with the parent node and NULL.
			// The parent node must be placed in the spot closest to the 0 index position in the frequency table array.
			frequencyTable[length - 1] = temp;
			frequencyTable[length] = NULL;

			// Sort the table so we can pick the two new lowest count nodes when we loop back up.
			sortFrequencyTable();
		}

		// At this point, frequencyTable[0] holds the root pointer for the Huffman tree.
		// Easier to write tree instead of frequencyTable[0] every time.
		tree = frequencyTable[0];

		// This line is for testing and debugging purposes. Prints out the tree's root count and the tree's height.
		// tree->count should output the same as bytes_scanned
		//cout << "\ntree root count: " << tree->count << "\ntree height: " << tree->height << endl;


	// 2. Generate encoding strings

		// Start at tree (the root node), with an initial blank encoding string.
		// This code is recursive, so it had to be made into a function.
		generateEncodingStrings(tree, "");

		// This line is for testing and debugging purposes. Prints out the generated encoding strings.
		//cout << "\nIndex\tChar\tEncoding String\n";
		//for (int i = 0; i < 123; i++) cout << i << "\t" << (char)i << "\t" << encodingStrings[i] << endl;

	// DONE WITH INITIALIZATION

		// Calculate elapsed time in miliseconds.
		unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

		// Set the output float numbers to have 3 decimal place precision.
		// Used to display elapsed time correctly.
		// If elapsed time = 1.8 seconds, this will make it output 1.800 seconds.
		cout.setf(ios::fixed, ios::floatfield);
		cout.precision(3);

		// Print out statistics
		cout << "\nInitialization\n"
			<< "\tinput file:\t" << FileName << endl
			<< "\tinput bytes:\t" << input_bytes << endl
			<< "\telapsed time:\t" << ((float)ELAPSED_TIME) / 1000 << " seconds\n"
			<< endl;

		init = true;	// true if InitializeFromFile() has been successfully executed
	}

	// else, there was a problem opening FileName
	else {
		char waitChar;
		cout << "!Error openning " << FileName << "\n\nPress ENTER to exit.";
		cin.get(waitChar);
		exit(0);
	}
}



// Encode
//
//    Creates an encoded and compressed file, with an extension of .enc, by reading the input file byte-by-byte
//	  and using the generated encodingStrings for each of the 256 ASCII characters to generate new byte-sized bit-strings
//    (strings of length 8 full of a combination of 1's and 0's). These byte-sized bit-strings are then written to the 
//	  .enc file as an encoded and compressed version of the original .txt file. 
//
//    This encoding method generates and saves an encoded byte at a time instead of generating a huge string of encoded
//    bytes, which might overwhelm the computer if the file we're encoding is ridiculously huge. That will never happen
//    for our purposes, but you never know... Plus, it's good programming practice to keep those things in mind too.
//
// Prints:
//		input filename used
//		output filename written to
//		input byte count
//		output byte count
//		compression percentage (1 - (out_bytes / in_bytes)) * 100
//		elapsed time this process took in seconds with milisecond precision
//
void Huffman::EncodeFile(string InFile, string OutFile){

	// if initialization has run
	if (init){

		// Save start time
		clock_t START_TIME = clock();

		// Statistics variables
		unsigned long input_bytes = 0;
		unsigned long output_bytes = 0;


		// Open InFile
		ifstream inputFile;
		inputFile.open(InFile.c_str(), ios::binary);

		// If InFile opened successfully
		if (!inputFile.fail()){

			// Open OutFile
			ofstream outputFile;
			outputFile.open(OutFile.c_str(), ios::binary);

			// If OutFile opened successfully
			if (!outputFile.fail()){

				
				int out_bit_count = 0;			// out_bit_count keeps track of the number of "bits" that have been placed in the "byte" being created.
												// When out_bit_count hits 8, the "byte" is fully loaded and written to the .enc file.

				int enc_str_bit_count = 0;		// enc_str_bit_count keeps track of the number of "bits" used, thus far, in the current encoding string of the current in_char
												// that is being used to build the temp_str, which is converted into the out_char when out_bit = 8.

				int temp = 0;					// temp keeps track of the number of "bits" added to temp_str during each iteration inside the for-loop.

				char in_char;					// in_char holds the current char read in from the .txt file.

				string temp_str = "";			// temp_str holds the byte-string (a string with max length of 8 made up of 0's and 1's chars) currently being built.


				inputFile.get(in_char);			// The first byte is read outside the loop to prevent a duplicate character at the
												// end due to the end-of-file (eof) flag triggering when trying to read a byte after last byte.

				// Read one byte/char at a time from InFile until all bytes have been read.
				while (!inputFile.eof()){

					// if done with the current encoding string, get the next input byte/char
					if (enc_str_bit_count == 0) input_bytes++;

					// while temp_str is still under 8 chars in length AND the last char of the encoding string has not been passed.
					for (temp = 0; (temp + out_bit_count) < 8 && enc_str_bit_count < encodingStrings[(int)in_char].length(); temp++, enc_str_bit_count++) 
						temp_str += encodingStrings[(int)in_char].at(enc_str_bit_count); // append a bit from the encoding string to temp_str

					out_bit_count += temp; // update out_bit_count to see if the 8-bit length has been achieved in temp_str

					// if all the "bits" in the encoding string have been used, signal to move on to the next char 
					// from InFile, which will give us a new encoding string
					if (enc_str_bit_count == encodingStrings[(int)in_char].length()) enc_str_bit_count = 0;

					// if temp_str has its 8 "bits", convert that bit-string 
					// into a byte value of 0 - 255 and write that byte value to the OutFile.
					// Clear temp_str so it's ready for the new bit-string it'll hold and 
					// clear out_bit_count.
					if (out_bit_count == 8){
						outputFile.put(byte_string_to_decimal(temp_str));
						temp_str = "";
						out_bit_count = 0;
						output_bytes++;
					}					

					// if done with the current encoding string, get the next input byte/char
					if(enc_str_bit_count == 0) inputFile.get(in_char);
				}

				// If padding bits need to be added to the last byte
				if (out_bit_count > 0){
					temp_str += generatePaddingBitString(8 - out_bit_count, tree, "");
					outputFile.put(byte_string_to_decimal(temp_str));
					temp_str = "";
					out_bit_count = 0;
					output_bytes++;
				}
				

				// close OutFile
				outputFile.close();

	// DONE WITH ENCODING

				// Calculate elapsed time in miliseconds
				unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

				// Print out statistics
				cout << "Encoding\n"
					<< "\tinput file:\t" << InFile << endl
					<< "\toutput file:\t" << OutFile << endl
					<< "\tinput bytes:\t" << input_bytes << endl
					<< "\toutput bytes:\t" << output_bytes << endl
					<< "\tcompression:\t" << (1 - (((float)output_bytes) / ((float)input_bytes))) * 100 << "%\n"
					<< "\telapsed time:\t" << ((float)ELAPSED_TIME) / 1000 << " seconds\n"
					<< endl;

				enc = true;		// true if EncodeFile() has been successfully executed
			}

			// else, there was a problem opening OutFile
			else {
				char waitChar;
				cout << "!Error openning " << OutFile << "\n\nPress ENTER to exit.";
				cin.get(waitChar);
				exit(0);
			}

			// close InFile
			inputFile.close();
		}

		// else, there was a problem opening InFile
		else {
			char waitChar;
			cout << "!Error openning " << InFile << "\n\nPress ENTER to exit.";
			cin.get(waitChar);
			exit(0);
		}
	}

	// else, initialization did not run
	else{
		char waitChar;
		cout << "\nCannot encode " << InFile << " since initialization did not run.\n\nPress ENTER to exit.";
		cin.get(waitChar);
		exit(0);
	}
}



// Decode
//
//    Creates a decoded file, with an extension of .dec, by reading the encoded input file byte-by-byte
//	  and using the generated byte-strings (strings with a length of 8 made up of 0's and 1's chars
//    generated from the encoded bytes read in from the encoded file) to traverse the Huffman tree by 
//    taking the left child of the current node if the current "bit", from the generatedy byte-string,
//    being examined equals a '0' or taking the right child if the "bit" equals a '1'. If a leaf node 
//    hasn't been reached by the time we finish going through all the "bits" in the current byte-string,
//    then stay at the current node, read in the next byte (is possible), generate the byte-string of 1's 
//    and 0s from that encoded byte, and continue traversing the tree from where we left off with the new
//    byte-string. When a leaf node is finally reached, print that node's character to the decoded file,
//    go back up to the root node, and continue traversing the tree using the next "bit".
//
// Prints:
//		input filename used
//		output filename written to
//		input byte count
//		output byte count
//		elapsed time this process took in seconds with milisecond precision
//
void Huffman::DecodeFile(string InFile, string OutFile){

	// if initialization and encoding have both been executed successfully
	if (init){

		// Save start time
		clock_t START_TIME = clock();

		// Statistics variables
		unsigned long input_bytes = 0;
		unsigned long output_bytes = 0;


		// Open InFile
		ifstream inputFile;
		inputFile.open(InFile.c_str(), ios::binary);

		// If InFile opened successfully
		if (!inputFile.fail()){

			// Open OutFile
			ofstream outputFile;
			outputFile.open(OutFile.c_str(), ios::binary);

			// If OutFile opened successfully
			if (!outputFile.fail()){

				char in_char;					// in_char holds the current char read in from the .enc file.

				string enc_str = "";			// enc_str holds the encoded byte-string (a string with a length of 8 made up of 0's and 1's chars) currently being decoded.

				int enc_str_index = 0;			// enc_str_index keeps track of the "bit" location currently being processed in the byte-string generated from the encoded file.

				node *current_node = tree;		// Holds the address to the node we're currently at in the Huffman tree while traversing it.

				inputFile.get(in_char);			// The first byte is read outside the loop to prevent a duplicate character at the
												// end due to the end-of-file (eof) flag triggering when trying to read a byte after last byte.
				
				// Read one byte/char at a time from InFile until all bytes have been read.
				while (!inputFile.eof()){

					// read in a new byte from the encoded file, so increment this count
					input_bytes++;

					// Generate the byte-string from the encoded byte.
					// (int)char				returns a value from -128 to 127
					// ((int)in_char) & 0xff	returns a value from 0 to 255
					// Need a value range from 0 to 255 to properlly generate 
					// the byte-string.
					enc_str = decimal_to_byte_string(((int)in_char) & 0xff);

					// Traverse the tree by taking the left child pointer if the "bit" at enc_str_index in enc_str is equal to '0'
					// or take the right child pointer if the "bit" equals '1' and increment enc_str_index to read the next "bit". 
					// If we ran out of "bits" in the current enc_str, then break this loop and get the next enc_str.
					// Once a leaf node is reached, output that node's character to the decoded file, return to the root node and 
					// continue traversing the tree.
					// enc_str_index < 8   The 8 is for 8 bits in a byte.
					for (enc_str_index = 0; enc_str_index < 8;){

						// if we're at an internal node or root node
						if (current_node->left_ptr != NULL && current_node->right_ptr != NULL){

							// go to the left child node if "bit" is a 0, else go to the right child node since "bit" is a 1.
							if (enc_str.at(enc_str_index) == '0') current_node = current_node->left_ptr;
							else current_node = current_node->right_ptr;

							// Increment is down here because if we skip this code in this if-statement to execute
							// the code in the else statement (outputFile.put(); output_bytes++; ... ) and increment after that,
							// then we will end up skipping over one "bit" in the byte-string, which will ruin our decoding.
							enc_str_index++;
						}

						// else, we have reached a leaf node in the tre.
						// Output that node's character to the decoded file,
						// return to the root node, and increment output_bytes.
						else if (current_node->character != NULL){
							outputFile.put(current_node->character);
							output_bytes++;
							current_node = tree;
						}
					}

					// finished processing the current byte, so get the next byte
					inputFile.get(in_char);
				}

				// if we reached the 8th "bit" of the last byte/char in the encoded file, which also happens to leave us at a leaf node, 
				// but, because of how the for() loop is structured, we broke out of the loop since enc_str_index was equal to 8 and we 
				// didn't get to print the last character because we broke out of the while() loop, it will be printed now.
				if (current_node->left_ptr == NULL && current_node->right_ptr == NULL && current_node->character != NULL){
					outputFile.put(((int)current_node->character) & 0xff);
					output_bytes++;
					current_node = tree;
				}

	// DONE WITH DECODING

				// close OutFile
				outputFile.close();

				// Calculate elapsed time in miliseconds
				unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

				// Print out statistics
				cout << "Decoding\n"
					<< "\tinput file:\t" << InFile << endl
					<< "\toutput file:\t" << OutFile << endl
					<< "\tinput bytes:\t" << input_bytes << endl
					<< "\toutput bytes:\t" << output_bytes << endl
					<< "\telapsed time:\t" << ((float)ELAPSED_TIME) / 1000 << " seconds\n"
					<< endl;

				dec = true;		// true if DecodeFile() has been successfully executed
			}

			// else, there was a problem opening OutFile
			else {
				char waitChar;
				cout << "!Error openning " << OutFile << "\n\nPress ENTER to exit.";
				cin.get(waitChar);
				exit(0);
			}

			// close InFile
			inputFile.close();
		}

		// else, there was a problem opening InFile
		else {
			char waitChar;
			cout << "!Error openning " << InFile << "\n\nPress ENTER to exit.";
			cin.get(waitChar);
			exit(0);
		}
	}

	// else, either initialization or encoding or both did not run
	else{
		char waitChar;
		cout << "\nCannot decode " << InFile << " since initialization did not run.\n\nPress ENTER to exit.";
		cin.get(waitChar);
		exit(0);
	}
}



// --- Helper Functions --- //

// Takes the frequency table and sorts it based on the node's count and height in 
// decending order (node with largest count value will be in array position 0).
// For multiple nodes with the same count, the node with the largest height value
// will be closer to the array's 0 index position. 
// Very similar to insertion sort algorithm.
void Huffman::sortFrequencyTable(){

	for (int i = 1; i < 256 && frequencyTable[i] != NULL; i++){	// i is the position value in the frequency table array of where we're sorting from.
																// Everything behind i is already sorted.

		// After a few tests, I found out that also taking height into consideration when sorting doesn't improve or worsen the encoding compression at all 
		// and the time it takes to initialize and encode doesn't change much either. I decided to keep this type of sorting since it produces much shorter encoding strings 
		// for the 0 count characters.

		// j moves from i to 0 dragging the node it points to towards the 0 index array position as far as it can until the node count at j-1 is >= than the node count at j.
		// Sort by count first, then, if counts are equal, sort by height.
		// The height parameter becomes useful when building the Huffman tree and the frequency table has already been sorted before with a new modification that needs sorting.
		// For the row of nodes with the same count (the last 100 or so nodes in the array might have a count of 0, or there might be 20 nodes with a count of 1), sort so that
		// the node with the largest height is closer to the array's 0 index position. 
		// This sorting method drastically reduces the tree's height, which also drastically reduces the encoding string length for all the 0 count characters.
		for (int j = i; j > 0 && 
				(frequencyTable[j - 1]->count < frequencyTable[j]->count || 
					(frequencyTable[j - 1]->count == frequencyTable[j]->count && frequencyTable[j - 1]->height < frequencyTable[j]->height)); j--)
		{
			// classic array swap of two values
			node *temp = frequencyTable[j - 1];
			frequencyTable[j - 1] = frequencyTable[j];
			frequencyTable[j] = temp;
		}
		
		/*
		// Same as above, but doesn't take subtree height into sorting consideration.
		// This will generate a tree with a much larger height and produce much longer encoding strings for the 0 count characters.
		for (int j = i; j > 0 && frequencyTable[j - 1]->count < frequencyTable[j]->count; j--)
		{
			// classic array swap of two values
			node *temp = frequencyTable[j - 1];
			frequencyTable[j - 1] = frequencyTable[j];
			frequencyTable[j] = temp;
		}*/
	}

}



// Recursively traverses the Huffman tree and generates the appropriate encoding 
// string for each of the 256 ASCII characters. The encoding strings this generates
// will be a variable length string consisting of a combination of 0's and 1's. A 0
// represents a left path in the tree was taken and a 1 represents a right path in 
// the tree was taken. When a leaf node is reached (also containing a character value),
// assign the generated encoding string to the appropriate slot in the encodingStrings[]
// array. The array index in encodingStrings[] represents the character's ASCII value. 
string Huffman::generateEncodingStrings(node *currentNode, string encodingString){

	// If we reached a leaf node, return the given encoding string.
	// This will return to the previous recursive call of this function,
	// which will assign the string to the encodingStrings[] array.
	if (currentNode->left_ptr == NULL && currentNode->right_ptr == NULL) return encodingString;

	string temp = generateEncodingStrings(currentNode->left_ptr, encodingString + "0");
	if (currentNode->left_ptr->character != NULL) encodingStrings[((int)currentNode->left_ptr->character) & 0xff] = temp;		// The ((int)character) & 0xff produces positive ASCII values
																																// since we're using signed chars with values of -128 to 127
	temp = generateEncodingStrings(currentNode->right_ptr, encodingString + "1");												// and calling a negative index value in an array will explode
	if (currentNode->right_ptr->character != NULL) encodingStrings[((int)currentNode->right_ptr->character) & 0xff] = temp;

	return "";
}


// Takes in an 8 character string of '1's and '0's representing a byte value 
// and returns its decimal value equivalent. Return range is from 0 to 255
int Huffman::byte_string_to_decimal(string byte_string){

	// if byte_string is anything but 8 characters long, return 0.
	if (byte_string.length() != 8) return 0;
	int sum = 0;
	for (int i = 0; i < 8; i++) if (byte_string.at(i) == '1') sum += (int)pow(2,(7 - i));
	return sum;
}



// Recursively traverses the tree to find valid padding bits that will not make it to a leaf node.
// Takes in a length for number of padding bits needed, a node from where to start, and the current 
// padding bit string. 
string Huffman::generatePaddingBitString(int length, node *currentNode, string paddingString){

	// if we generated a paddingString of the desired length that doesn't reach a leaf node
	if (length == 0 && currentNode->left_ptr != NULL && currentNode->right_ptr != NULL) return paddingString;

	// if we reached a leaf node
	if (currentNode->left_ptr == NULL && currentNode->right_ptr == NULL) return "";

	string temp = generatePaddingBitString(length - 1, currentNode->left_ptr, paddingString + "0");
	if (temp.length() >= length) return temp;

	temp = generatePaddingBitString(length - 1, currentNode->right_ptr, paddingString + "1");
	if (temp.length() >= length) return temp;

	return "";
}



// Takes in a decimal integer between 0 and 255 and generates the appropriate
// 8-bit bit-string that represents that value in binary. Returns a string
// ranging from "00000000" to "11111111".
string Huffman::decimal_to_byte_string(int decimal){

	if (decimal < 0 || decimal > 255) return "";

	string byte_str = "";
	for (int i = 0; i < 8; i++) {
		if (decimal >= pow(2, 7 - i)) {
			byte_str += "1";
			decimal -= pow(2, 7 - i);
		}
		else byte_str += "0";
	}
	return byte_str;
}