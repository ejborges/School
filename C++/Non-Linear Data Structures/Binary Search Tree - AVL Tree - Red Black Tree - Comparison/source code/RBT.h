/*
	Filename:		RBT.h
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		Binary Search Tree - AVL Tree - Red Black Tree
	Date:			April 7, 2015
	Description:	Header file for RBT.cpp
					This includes the public methods to insert and traverse the RBT tree as well as public access to 
					calculated tree parameters. Has private RBT_node template, root pointer, nil pointer, and helper methods.
*/

#ifndef RBT_H	// start of include guard
#define RBT_H

#include <string>
#include <ctime>

class RBT
{

	public:
		RBT();
		void RBT_Insert(const std::string &word);
		void RBT_Traverse();

		// Tree Parameters GET methods
		unsigned int get_key_comparisons_count();
		unsigned int get_parent_pointer_change_count();
		unsigned int get_left_child_pointer_change_count();
		unsigned int get_right_child_pointer_change_count();
		unsigned int get_color_change_count();
		unsigned int get_distinct_words_count();
		unsigned int get_total_words_count();
		unsigned int get_tree_height();
		float get_traverse_time();

	private:
		
		struct RBT_node
		{
			// RBT node template
			std::string key;
			unsigned int count = 1;
			bool color = false;				// false = black, true = red;
			RBT_node *parent_ptr = NULL;
			RBT_node *left_ptr = NULL;
			RBT_node *right_ptr = NULL;
		};

		RBT_node *RBT_root;
		RBT_node *RBT_nil;

		// Tree Parameters
		unsigned int key_comparisons_count = 0;
		unsigned int parent_pointer_change_count = 0;
		unsigned int left_child_pointer_change_count = 0;
		unsigned int right_child_pointer_change_count = 0;
		unsigned int color_change_count = 0;
		unsigned int distinct_words_count = 0;
		unsigned int total_words_count = 0;
		unsigned int tree_height = 0;
		float traverse_time = 0;


		// --- Helper Functions --- //

		void RBT_Insert_Fixup(RBT_node *node);

		void Left_Rotate(RBT_node *node);
		void Right_Rotate(RBT_node *node);

		void traverse(RBT_node *node, int current_height);	// Performs an in-order tree traversal to calculate the number
															// of distinct words, total words, and the tree's height.

		int compare_strings(const std::string &string1, const std::string &string2);	// Given two strings, returns:
																						//-1 if string1 < string2
																						// 0 if string1 == string2
																						// 1 if string1 > string2

};

#endif	// end of include guard
// No other code should go after this line