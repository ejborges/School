/*
	Filename:		main.cpp
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		B-Trees - AVL Tree - Disk-based Comparison
	Date:			April 25, 2015
	Description:	Starting point for the B-Tree_AVL program.
					Contains the code to read and parse all words from the text file,
					place all words into each tree (AVL, BTree) one tree at a time,
					and prints out statistics on each tree after each tree finishes.
					Contains menu code for selecting which text file to run.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <math.h>

#include "AVL.h"
#include "BTree.h"

using namespace std;

string files_location = "texts\\";	// Folder path pointing to where the texts-to-be-run are.

// Available files to run
string files[] = {"Beowulf.txt", "Hamlet.txt", "Koran.txt",
					"Shakespeare.txt", "Sherlock.txt", "Tao Teh King.txt", 
					"The King James Bible.txt", "The United States Constitution.txt", 
					"War and Peace.txt"};

int file_id;	// Holds the files[] array index value of the file to be run

string outputs[9];	// Output from files executed
string temp;		// Holds output temporarily
string temp_time;	// Holds time output temporarily

void read_file_into(const string &filename, string tree);	// Method to parse each word from <filename> and feed them to tree building methods
int return_file_id_from_menu();	// Prints the menu of available files to run and returns the selected file or -1 for exit

// Initial pass parameters
float file_read_overhead_time = 0;	// time it takes to read each word in <filename> but do nothing with them
int initial_pass_total_words = 0;

float avl_time = 0, btree_time = 0; // time it takes for each tree to be built

// Run each tree's constructor
AVL AVL_T;
BTree BTree_T;

// Which tree should run?
bool run_avl = true;
bool run_btree = true;


int main()
{
	// This is where the program begins... 

	for (int i = 0; i < 9; i++) outputs[i] = "";

	while (true)
	{
		temp = "";						// clear temp
		file_read_overhead_time = 0;	// reset variables
		initial_pass_total_words = 0;

		file_id = return_file_id_from_menu();	// Will print the menu and return an int ranging from -9 through +9.
												// The int represents the file to be run from the files[] array.
												// Positive means run the desired file.
												// Negative means view results without running, if file hasn't run before, run file.

		// if file_id is negative and that file has already run before, print its output
		if (file_id < 0 && outputs[(file_id*-1) - 1].length() > 1)
		{
			cout << outputs[(file_id*-1) - 1];
			continue;
		}

		// file_id must be positive and zero indexed
		file_id = abs(file_id) - 1;

		// check if the file is a .txt file
		if (files[file_id].length() < 5)	// if filename is too short, e.g. ".txt" is not a valid filename 
		{
			cout << "\nERR: '" << files[file_id] << "' is an invalid file\n\nPress ENTER to exit.";
			char waitchar;
			cin >> waitchar;
			exit(1);
		}
		// check to make sure the file is a ".txt" file type
		if (files[file_id][files[file_id].length() - 1] != 't' &&
			files[file_id][files[file_id].length() - 2] != 'x' &&
			files[file_id][files[file_id].length() - 3] != 't' &&
			files[file_id][files[file_id].length() - 4] != '.')
		{
			cout << "\nERR: '" << files[file_id] << "' is not of type '.txt'\n\nPress ENTER to exit.";
			char waitchar;
			cin >> waitchar;
			exit(1);
		}

		// <filename> header
		temp += "\n";
		temp += to_string(file_id + 1); // file_id is 0 indexed, it has to be 1 indexed
		temp += ") ";
		temp += files[file_id];
		temp += "\n";
		for (int i = 0; i < 59; i++) temp += "-";
		temp += "\n\7";

		// Save header and print to screen
		outputs[file_id] = temp;
		cout << temp;
		temp = "";

		// Save start time
		clock_t START_TIME = clock();

		
		// 1.	Scan the file, and parse it out one-word-at-a-time, and do nothing with the words.
		//		That will tell you how much of main’s time goes into the file-reading and parsing overhead.
		read_file_into(files[file_id], "none");

		// Header
		temp += "\n\tInitial Pass";
		temp += "\n\t------------";

		// Read Time
		temp += "\n\t\tRead Time:\t\t";
		temp_time = to_string(file_read_overhead_time);		// float to string
		temp += temp_time.erase(temp_time.length() - 3, 3); // removes unwanted "000" from the end of the time value string
		temp += " seconds";

		// Total Words
		temp += "\n\t\tTotal Words:\t\t";
		temp += to_string(initial_pass_total_words);
		temp += "\n";

		// Save results and print to screen
		outputs[file_id] += temp;
		cout << temp;
		temp = "";
		

		// Run AVL tree?
		if (run_avl)
		{
			// 2.	Make another pass through the file, and put all of the words into an AVL tree
			//
			//		This will print out the following statistics: 
			//			a.	Time it took to build the tree (actual AVL build time = read_file_and_build_tree time - read_file time)
			//			b.	Time it took to traverse the completed AVL tree (to calculate distinct words, total words, and tree height)
			//			c.	Number of key comparions made while building the tree
			//					I. comparions made between the new word being inserted vs the word in the node we're currently on in the tree
			//			d.	Number of Balance Factor changes 
			//			e.	Number of left child pointer changes
			//			f.	Number of right child pointer changes
			//			g.	Number of total child pointer changes
			//			h.	Number of distinct words in the text file
			//			i.	Number of total words in the text file
			//			j.	Number of the tree height
			//
			AVL_T.AVL_New();		// Sets everything up for a new AVL tree to be created

			read_file_into(files[file_id], "avl");

			AVL_T.AVL_Traverse();	// This method performs a recursive in-order tree traversal to calculate 
									// the number of distinct words, total words, and the tree's height.

			// Header
			temp += "\n\tAVL Tree";
			temp += "\n\t--------";

			// Build Time
			temp += "\n\t\tBuild Time:\t\t";
			temp_time = to_string(avl_time - file_read_overhead_time);	// float to string
			temp += temp_time.erase(temp_time.length() - 3, 3);			// removes unwanted "000" from the end of the time value string
			temp += " seconds";

			// Traverse Time
			temp += "\n\t\tTraverse Time:\t\t";
			temp_time = to_string(AVL_T.get_traverse_time());			// float to string
			temp += temp_time.erase(temp_time.length() - 3, 3);			// removes unwanted "000" from the end of the time value string
			temp += " seconds";

			// Distinct Words
			temp += "\n\t\tDistinct Words:\t\t";
			temp += to_string(AVL_T.get_distinct_words_count());

			// Total Words
			temp += "\n\t\tTotal Words:\t\t";
			temp += to_string(AVL_T.get_total_words_count());

			// Tree Height
			temp += "\n\t\tTree Height:\t\t";
			temp += to_string(AVL_T.get_tree_height());

			// Node Count
			temp += "\n\t\tNode Count:\t\t";
			temp += to_string(AVL_T.get_distinct_words_count());

			// Key Comparisons
			temp += "\n\t\tKey Comparisons:\t";
			temp += to_string(AVL_T.get_key_comparisons_count());

			// Balance Factor Changes
			temp += "\n\t\tBalance Factor Changes:\t";
			temp += to_string(AVL_T.get_Balance_Factor_change_count());

			// Left Pointer Modification Count
			temp += "\n\t\tLeft Ptr Mod Count:\t";
			temp += to_string(AVL_T.get_left_child_pointer_change_count());

			// Right Pointer Mofification Count
			temp += "\n\t\tRight Ptr Mod Count:\t";
			temp += to_string(AVL_T.get_right_child_pointer_change_count());

			// Total Pointer Modification Count
			temp += "\n\t\tTotal Ptr Mod Count:\t";
			temp += to_string(AVL_T.get_left_child_pointer_change_count() + AVL_T.get_right_child_pointer_change_count());

			// Disk Read Count
			temp += "\n\t\tDisk Read Count:\t";
			temp += to_string(AVL_T.get_disk_read_count());

			// Disk Write Count
			temp += "\n\t\tDisk Write Count:\t";
			temp += to_string(AVL_T.get_disk_write_count());

			// Total Disk Access Count
			temp += "\n\t\tTotal Disk Access:\t";
			temp += to_string(AVL_T.get_disk_read_count() + AVL_T.get_disk_write_count());

			// Tree File Size
			temp += "\n\t\tTree File Size:\t\t";
			temp += to_string(AVL_T.get_node_size()*AVL_T.get_distinct_words_count());	// # of bytes in each node * total number of nodes
			temp += " bytes";

			// Save results and print to screen
			outputs[file_id] += temp;
			cout << temp;
			temp = "";

			AVL_T.AVL_Close();	// Closes the avl tree fstream and resets values/parameters for a new tree.
		}
		

		// Run B-Tree?
		if (run_btree)
		{
			// 3.	Make another pass through the file, and put all of the words into a B-Tree
			//
			//		This will print out the following statistics: 
			//			a.	Time it took to build the tree (actual BTree build time = read_file_and_build_tree time - read_file time)
			//			b.	Time it took to traverse the completed BTree tree (to calculate distinct words, total words, and tree height)
			//			c.	Number of key comparions made while building the tree
			//					I. comparions made between the new word being inserted vs the word in the node we're currently on in the tree
			//			d.	Number of color changes 
			//			e.	Number of parent pointer changes
			//			f.	Number of left child pointer changes
			//			g.	Number of right child pointer changes
			//			h.	Number of total pointer changes
			//			i.	Number of distinct words in the text file
			//			j.	Number of total words in the text file
			//			k.	Number of the tree height
			//
			BTree_T.BTree_New();		// Sets everything up for a new BTree to be created

			read_file_into(files[file_id], "btree");

			BTree_T.BTree_Traverse();

			// Header
			temp += "\n\n\tB-Tree with t = ";
			temp += to_string((BTree_T.get_keys_per_node_value() + 1) / 2);
			temp += "\n\t------------------";

			// Build Time
			temp += "\n\t\tBuild Time:\t\t";
			temp_time = to_string(btree_time - file_read_overhead_time);	// float to string
			temp += temp_time.erase(temp_time.length() - 3, 3);				// removes unwanted "000" from the end of the time value string
			temp += " seconds";

			// Traverse Time
			temp += "\n\t\tTraverse Time:\t\t";
			temp_time = to_string(BTree_T.get_traverse_time());				// float to string
			temp += temp_time.erase(temp_time.length() - 3, 3);				// removes unwanted "000" from the end of the time value string
			temp += " seconds";

			// Distinct Words
			temp += "\n\t\tDistinct Words:\t\t";
			temp += to_string(BTree_T.get_distinct_words_count());

			// Total Words
			temp += "\n\t\tTotal Words:\t\t";
			temp += to_string(BTree_T.get_total_words_count());

			// Tree Height
			temp += "\n\t\tTree Height:\t\t";
			temp += to_string(BTree_T.get_tree_height());

			// Node Count
			temp += "\n\t\tNode Count:\t\t";
			temp += to_string(BTree_T.get_node_count());

			// Key Comparisions
			temp += "\n\t\tKey Comparisons:\t";
			temp += to_string(BTree_T.get_key_comparisons_count());

			// Loading Factor
			temp += "\n\t\tLoading Factor:\t\t";
			// number of key containers in use / total number of key containers available
			// distinct words / number of nodes * (2*t - 1); as a %
			temp += to_string((float)BTree_T.get_distinct_words_count() / (BTree_T.get_node_count() * BTree_T.get_keys_per_node_value()));
			temp += " %";

			// Disk Read Count
			temp += "\n\t\tDisk Read Count:\t";
			temp += to_string(BTree_T.get_disk_read_count());

			// Disk Write Count
			temp += "\n\t\tDisk Write Count:\t";
			temp += to_string(BTree_T.get_disk_write_count());

			// Total Disk Access Count
			temp += "\n\t\tTotal Disk Access:\t";
			temp += to_string(BTree_T.get_disk_read_count() + BTree_T.get_disk_write_count());

			// Tree File Size
			temp += "\n\t\tTree File Size:\t\t";
			temp += to_string(BTree_T.get_node_size()*BTree_T.get_node_count());	// # of bytes in each node * total number of nodes
			temp += " bytes";

			// Save results and print to screen
			outputs[file_id] += temp;
			cout << temp;
			temp = "";

			BTree_T.BTree_Close();	// Closes the BTree fstream and resets values/parameters for a new tree.
		}
		

		// Done processing text file

		// Calculate elapsed time in miliseconds.
		unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

		// process time in seconds with milisecond precision
		float process_time = ((float)ELAPSED_TIME) / 1000;

		// Overall Run Time
		temp += "\n\nOverall Run Time: ";
		temp_time = to_string(process_time);					// float to string
		temp += temp_time.erase(temp_time.length() - 3, 3);		// removes unwanted "000" from the end of the time value string
		temp += " seconds";

		// <filename> footer and divider
		temp += "\n";
		for (int i = 0; i < 59; i++) temp += "-";
		temp += "\n";

		// Save results and print to screen
		outputs[file_id] += temp;
		cout << temp;

		// If the process takes longer than 3 seconds to finish,
		// make two little beeps (ASCII 7) to let the user know
		// the process has finished.
		if (process_time > 3) cout << "\7\7";
	}
}



void read_file_into(const string &filename, string tree)
{
	// This method will be used to calculate the time (in seconds with milisecond precision) 
	// it takes to go through the entire file and parse out all the words. This method can
	// be run in 3 different ways:
	//
	// tree == "none" will do nothing with the words read in
	// tree == "avl" will create an AVL tree with the words read in
	// tree == "btree" will create a B-Tree with the words read in

	if (tree != "none" && tree != "avl" && tree != "btree")
	{
		cout << "\nERR: '" << tree << "' cannot be used with read_file_into();" << endl;
		return;
	}


	// Save start time
	clock_t START_TIME = clock();


	// Initial pass setup.
	// This pass will only measure the time it takes to go through all of the words in the file.
	char chari[50];		// char chari[50] assumes no word is longer than 49 characters
	char c;				// char c will hold the char currently being read from the file
	int iPtr;			// 
	bool IsDelimiter = false, WasDelimiter = false;
	for (int i = 0; i < 50; i++) chari[i] = '\0';	// set all array chars to null

	// Open the file and attempt to read it.
	ifstream InFile;
	InFile.open(files_location + filename, ios::binary);
	if (InFile.fail())
	{
		cout << "Unable to open " << filename << "\n\n"
			<< "Program Exiting\n\nPress ENTER to exit\n";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}

	// Parse all words from the file.
	iPtr = 0;
	InFile.get(c);			// need to get the first char from outside the loop to properly 
							// detect the end-of-file (eof) flag.
	while (!InFile.eof())
	{
		IsDelimiter = (c == 32 || c == 10 || c == 13 || c == 9 ||
			c == '.' || c == ',' || c == '!' || c == ';' ||
			c == ':' || c == '(' || c == ')');

		// if THIS char is a delimiter and the last char WASN'T
		if (IsDelimiter && !WasDelimiter)
		{
			WasDelimiter = true;

			if (tree == "none") initial_pass_total_words++;
			else if (tree == "avl") AVL_T.AVL_Insert(chari);
			else if (tree == "btree") BTree_T.BTree_Insert(chari);

			for (int i = 0; i<50; i++) chari[i] = '\0'; // zero the word
			iPtr = 0;
		}

		// if this isn't a delimiter, keep going
		else if (!IsDelimiter)
		{
			chari[iPtr] = c;
			iPtr++;
		}

		else if (IsDelimiter && WasDelimiter)
		{
			// Do nothing -- two consecutive delimiters.
		}

		WasDelimiter = IsDelimiter;		// for the NEXT iteration
		InFile.get(c);

		// If last char in file is not a delimiter,
		// save the current word before we quit this loop.
		if (InFile.eof())
		{
			if (tree == "none") initial_pass_total_words++;
			else if (tree == "avl") AVL_T.AVL_Insert(chari);
			else if (tree == "btree") BTree_T.BTree_Insert(chari);
		}
	}
	InFile.close();

	// Finished reading in all the words from the file.

	// Calculate elapsed time in miliseconds.
	unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

	// time in seconds with milisecond precision
	if (tree == "none") file_read_overhead_time = ((float)ELAPSED_TIME) / 1000;
	else if (tree == "avl") avl_time = ((float)ELAPSED_TIME) / 1000;
	else if (tree == "btree") btree_time = ((float)ELAPSED_TIME) / 1000;
}

int return_file_id_from_menu()
{
	// Prints the menu of available files to run
	// returns positive 1 through 9 to run the desired file
	// returns negative 1 through 9 to view the results of the desired file
	// 0 will exit
	// an 'r' after the number will force run the file

	string input;
	do
	{
		for (int i = 0; i < 9; i++)
		{
			// Prints out: <file_id>) <filename>.txt
			cout << "\n\t" << (i + 1) << ") " << files[i];
		}

		cout << "\n\t0) EXIT\n\nEnter the number for the file you wish to run: ";

		cin >> input;

		if (input.at(0) == '0')
		{
			AVL_T.AVL_Delete();		// Deletes the avl.tree file.
			BTree_T.BTree_Delete();	// Deletes the b.tree file.
			exit(0);
		}

		system("CLS");	// CLear Screen

	} while (input.at(0) < '0' || input.at(0) > '9'); // while invalid input

	// if user wants to re-run the file. 'r' for Run
	if (input.length() >= 2 && (input.at(1) == 'r' || input.at(1) == 'R')) return (input.at(0) - 48); // ASCII decimal 48 == '0'

	return (input.at(0) - 48)*-1;	// ASCII decimal 48 == '0'
}