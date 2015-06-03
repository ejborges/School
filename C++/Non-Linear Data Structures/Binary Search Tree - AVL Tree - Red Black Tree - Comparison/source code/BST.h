/*
	Filename:		BST.h
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		Binary Search Tree - AVL Tree - Red Black Tree
	Date:			April 7, 2015
	Description:	Header file for BST.cpp
					This includes the public methods to insert and traverse the BST as well as public access to 
					calculated tree parameters. Has private BST_node template, root pointer, and helper methods.
*/

#ifndef BST_H	// start of include guard
#define BST_H

#include <string>
#include <ctime>

class BST
{

	public:
		BST();
		void BST_Insert(const std::string &word);
		void BST_Traverse();

		// Tree Parameters GET methods
		unsigned int get_key_comparisons_count();
		unsigned int get_distinct_words_count();
		unsigned int get_total_words_count();
		unsigned int get_tree_height_count();
		float get_traverse_time();

	private:

		struct BST_node
		{
			// BST node template
			std::string key;
			unsigned int count = 1;
			BST_node *parent_ptr = NULL;
			BST_node *left_ptr = NULL;
			BST_node *right_ptr = NULL;
		};

		BST_node *BST_root;

		// Tree Parameters
		unsigned int key_comparisons_count = 0;
		unsigned int distinct_words_count = 0;
		unsigned int total_words_count = 0;
		unsigned int tree_height_count = 0;
		float traverse_time = 0;


		// --- Helper Functions --- //
				
		BST_node* internal_search_for(const std::string &word);	// Given a word, search the tree for a node containing that word.
																// Returns the closest match (if node doesn't exist, returns what would be its parent)

		int compare_strings(const std::string &string1, const std::string &string2);	// Given two strings, returns:
																						//-1 if string1 < string2
																						// 0 if string1 == string2
																						// 1 if string1 > string2
};

#endif	// end of include guard
// No other code should go after this line