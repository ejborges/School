/*
	Filename:		main.cpp
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		Binary Search Tree - AVL Tree - Red Black Tree
	Date:			April 7, 2015
	Description:	Starting point for the BST_AVL_RBT program. 
					Contains the code to read and parse all words from the text file,
					place all words into each tree (BST, AVL, RBT) one tree at a time,
					and prints out statistics on each tree after each tree finishes.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#include "AVL.h"
#include "BST.h"
#include "RBT.h"

using namespace std;

string files_location = "texts\\";	// Folder path pointing to where the texts-to-be-run are.

// File used to build the trees
string filename = "Shakespeare.txt";

void read_file_into(const string &filename, string tree);	// Method to parse each word from <filename> and feed them to tree building methods

// Initial pass parameters
float file_read_overhead_time = 0;	// time it takes to read each word in <filename> but do nothing with them
unsigned int initial_pass_total_words = 0;

float bst_time = 0, avl_time = 0, rbt_time = 0; // time it takes for each tree to be built

// Run each tree's constructor
BST BST_T;
AVL AVL_T;
RBT RBT_T;


int main()
{
	// Set the output float numbers to have 3 decimal place precision.
	// Used to display elapsed time correctly.
	// If elapsed time = 1.8 seconds, this will make it output 1.800 seconds.
	cout.setf(ios::fixed, ios::floatfield);
	cout.precision(3);

	// check if the file is a .txt file
	if (filename.length() < 5)			// if filename is too short, e.g. ".txt" is not a valid filename 
	{
		cout << "\nERR: '" << filename << "' is an invalid file\n\nPress ENTER to exit.";
		char waitchar;
		cin.get(waitchar);
		exit(1);
	}
	// check to make sure the file is a ".txt" file type
	if (filename[filename.length() - 1] != 't' &&
		filename[filename.length() - 2] != 'x' &&
		filename[filename.length() - 3] != 't' &&
		filename[filename.length() - 4] != '.')
	{
		cout << "\nERR: '" << filename << "' is not of type '.txt'\n\nPress ENTER to exit.";
		char waitchar;
		cin.get(waitchar);
		exit(1);
	}

	// <filename> header
	cout << endl << filename << endl;
	for (int i = 0; i < 59; i++) cout << "-";
	cout << endl;

	// Save start time
	clock_t START_TIME = clock();

	// 1.	Scan the file, and parse it out one-word-at-a-time, and do nothing with the words.
	//		That will tell you how much of main’s time goes into the file-reading and parsing overhead.
	read_file_into(filename, "none");
	cout << "\n\tInitial Pass" <<
			"\n\t------------" <<
			"\n\t\tRead Time:\t\t" << file_read_overhead_time << " seconds" <<
			"\n\t\tTotal Words:\t\t" << initial_pass_total_words <<
			"\7" << endl;


	// 2.	Make another pass through the file, and put all of the words into a Binary Search Tree
	//
	//		This will print out the following statistics: 
	//			a.	Time it took to build the tree (actual BST build time = read_file_and_build_tree time - read_file time)
	//			b.	Time it took to traverse the completed BST tree (to calculate distinct words, total words, and tree height)
	//			c.	Number of key comparions made while building the tree
	//					I. comparions made between the new word being inserted vs the word in the node we're currently on in the tree
	//			d.	No parent or child nodes were modified, so print 0 for them
	//			e.	Number of distinct words in the text file
	//			f.	Number of total words in the text file
	//			g.	Number of the tree height
	//
	read_file_into(filename, "bst");
	BST_T.BST_Traverse();	// This method performs an in-order tree traversal to calculate 
	// the number of distinct words, total words, and the tree's height.
	cout << "\n\tBinary Search Tree" <<
			"\n\t------------------" <<
			"\n\t\tBuild Time:\t\t" << (bst_time - file_read_overhead_time) << " seconds" <<
			"\n\t\tTraverse Time:\t\t" << BST_T.get_traverse_time() << " seconds" <<
			"\n\t\tDistinct Words:\t\t" << BST_T.get_distinct_words_count() <<
			"\n\t\tTotal Words:\t\t" << BST_T.get_total_words_count() <<
			"\n\t\tTree Height:\t\t" << BST_T.get_tree_height_count() <<
			"\n\t\tKey Comparisons:\t" << BST_T.get_key_comparisons_count() << endl;

	// If the tree takes longer than 5 seconds to build,
	// make a little beep (ASCII 7) to let the user know
	// the tree has finished.
	if (bst_time > 5) cout << "\7";


	// 3.	Make another pass through the file, and put all of the words into an AVL tree
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
	read_file_into(filename, "avl");
	AVL_T.AVL_Traverse();	// This method performs a recursive in-order tree traversal to calculate 
	// the number of distinct words, total words, and the tree's height.

	cout << "\n\tAVL Tree" <<
			"\n\t--------" <<
			"\n\t\tBuild Time:\t\t" << (avl_time - file_read_overhead_time) << " seconds" <<
			"\n\t\tTraverse Time:\t\t" << AVL_T.get_traverse_time() << " seconds" <<
			"\n\t\tDistinct Words:\t\t" << AVL_T.get_distinct_words_count() <<
			"\n\t\tTotal Words:\t\t" << AVL_T.get_total_words_count() <<
			"\n\t\tTree Height:\t\t" << AVL_T.get_tree_height() <<
			"\n\t\tKey Comparisons:\t" << AVL_T.get_key_comparisons_count() <<
			"\n\t\tBalance Factor Changes:\t" << AVL_T.get_Balance_Factor_change_count() <<
			"\n\t\tLeft Ptr Mod Count:\t" << AVL_T.get_left_child_pointer_change_count() <<
			"\n\t\tRight Ptr Mod Count:\t" << AVL_T.get_right_child_pointer_change_count() <<
			"\n\t\tTotal Ptr Mod Count:\t" << (AVL_T.get_left_child_pointer_change_count()
												+ AVL_T.get_right_child_pointer_change_count()) << endl;

	// If the tree takes longer than 5 seconds to build,
	// make a little beep (ASCII 7) to let the user know
	// the tree has finished.
	if (avl_time > 5) cout << "\7";


	// 4.	Make another pass through the file, and put all of the words into a Red Black Tree
	//
	//		This will print out the following statistics: 
	//			a.	Time it took to build the tree (actual RBT build time = read_file_and_build_tree time - read_file time)
	//			b.	Time it took to traverse the completed RBT tree (to calculate distinct words, total words, and tree height)
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
	read_file_into(filename, "rbt");
	RBT_T.RBT_Traverse();

	cout << "\n\tRed Black Tree" <<
			"\n\t--------------" <<
			"\n\t\tBuild Time:\t\t" << (rbt_time - file_read_overhead_time) << " seconds" <<
			"\n\t\tTraverse Time:\t\t" << RBT_T.get_traverse_time() << " seconds" <<
			"\n\t\tDistinct Words:\t\t" << RBT_T.get_distinct_words_count() <<
			"\n\t\tTotal Words:\t\t" << RBT_T.get_total_words_count() <<
			"\n\t\tTree Height:\t\t" << RBT_T.get_tree_height() <<
			"\n\t\tKey Comparisons:\t" << RBT_T.get_key_comparisons_count() <<
			"\n\t\tColor Changes:\t\t" << RBT_T.get_color_change_count() <<
			"\n\t\tParent Ptr Mod Count:\t" << RBT_T.get_parent_pointer_change_count() <<
			"\n\t\tLeft Ptr Mod Count:\t" << RBT_T.get_left_child_pointer_change_count() <<
			"\n\t\tRight Ptr Mod Count:\t" << RBT_T.get_right_child_pointer_change_count() <<
			"\n\t\tTotal Ptr Mod Count:\t" << (RBT_T.get_parent_pointer_change_count() 
												+ RBT_T.get_left_child_pointer_change_count()
												+ RBT_T.get_right_child_pointer_change_count()) << endl;

	// If the tree takes longer than 5 seconds to build,
	// make a little beep (ASCII 7) to let the user know
	// the tree has finished.
	if (rbt_time > 5) cout << "\7";


	// Done processing text file

	// Calculate elapsed time in miliseconds.
	unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

	// process time in seconds with milisecond precision
	float process_time = ((float)ELAPSED_TIME) / 1000;

	cout << "\nOverall Run Time: " << process_time << " seconds";

	// <filename> footer and divider
	cout << endl;
	for (int i = 0; i < 59; i++) cout << "-";
	cout << endl;

	// If the process takes longer than 5 seconds to finish,
	// make two little beeps (ASCII 7) to let the user know
	// the process has finished.
	if (process_time > 5) cout << "\7\7";
	
	// Done processing ALL text files

	cout << "\n\nProcess complete. Press ENTER to exit.";
	char waitChar;
	cin.get(waitChar);
	return 0;
}



void read_file_into(const string &filename, string tree)
{
	// This method will be used to calculate the time (in seconds with milisecond precision) 
	// it takes to go through the entire file and parse out all the words. This method can
	// be run in 4 different ways:
	//
	// tree == "none" will do nothing with the words read in
	// tree == "bst" will create a Binary Search Tree with the words read in
	// tree == "avl" will create an AVL tree with the words read in
	// tree == "rbt" will create a Red Black Tree with the words read in

	if (tree != "none" && tree != "bst" && tree != "avl" && tree != "rbt")
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
		cin.get(waitChar);
		exit(1);
	}

	// Parse all words from the file.
	iPtr = 0;
	InFile.get(c);			// need to get the first char from outside the loop to properly 
							// detect the end-of-file (eof) flag.
	while (!InFile.eof())
	{
		IsDelimiter =  (c == 32 || c == 10 || c == 13 || c == 9 ||
						c == '.' || c == ',' || c == '!' || c == ';' ||
						c == ':' || c == '(' || c == ')');

		// if THIS char is a delimiter and the last char WASN'T
		if (IsDelimiter && !WasDelimiter)
		{
			WasDelimiter = true;

			if (tree == "none") initial_pass_total_words++;
			else if (tree == "bst") BST_T.BST_Insert(chari);
			else if (tree == "avl") AVL_T.AVL_Insert(chari);
			else if (tree == "rbt") RBT_T.RBT_Insert(chari);

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
			else if (tree == "bst") BST_T.BST_Insert(chari);
			else if (tree == "avl") AVL_T.AVL_Insert(chari);
			else if (tree == "rbt") RBT_T.RBT_Insert(chari);
		}
	}
	InFile.close();

	// Finished reading in all the words from the file.

	// Calculate elapsed time in miliseconds.
	unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

	// time in seconds with milisecond precision
	if (tree == "none") file_read_overhead_time = ((float)ELAPSED_TIME) / 1000;
	else if (tree == "bst") bst_time = ((float)ELAPSED_TIME) / 1000;
	else if (tree == "avl") avl_time = ((float)ELAPSED_TIME) / 1000;
	else if (tree == "rbt") rbt_time = ((float)ELAPSED_TIME) / 1000;
}