/*
	Filename:		BST.cpp
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		Binary Search Tree - AVL Tree - Red Black Tree
	Date:			April 7, 2015
	Description:	Here's where the actual work gets done for the BST tree. All public and private
					methods are defined here and all tree parameters are calculated here. 
*/

#include "BST.h"

using namespace std;

BST::BST()
{
	// BST constructor

	BST_root = NULL;
}



void BST::BST_Insert(const string &word)
{
	// This method creates and adds to a binary search tree.
	// If <word> is already IN the tree, increment the node's count.
	// If <word> is NOT in the tree, add it to the tree as a new node.


	// If the tree is empty, make a root node with <word>
	if (BST_root == NULL){
		BST_root = new BST_node;
		BST_root->key = word;
	}
	else {
		
		BST_node *closest_match = internal_search_for(word);// internal_search() will give us the node containing <word> or give us its parent node.
															// If *closest_match is the parent node, we know <word> is a new word and must be added.
															// We'll know if *closest_match is the parent if compare_strings(word, closest_match->key) doesn't return 0

		switch (compare_strings(word, closest_match->key))	// One last comparision to see if we have the parent node or the node containing <word>
		{													// if <word> is less than or greater than closest_match->key, then we create a new node for <word>
															// else, we increment closest_match->count

			// *closest_match happened to be the parent and we have to insert a new left node for <word>
			case -1:	closest_match->left_ptr = new BST_node;
						closest_match->left_ptr->key = word;
						closest_match->left_ptr->parent_ptr = closest_match;
						break;

			// *closest_match happened to be the node containing <word>, so ++ it's count
			case  0:	closest_match->count++;
						break;

			// *closest_match happened to be the parent and we have to insert a new right node for <string>
			case  1:	closest_match->right_ptr = new BST_node;
						closest_match->right_ptr->key = word;
						closest_match->right_ptr->parent_ptr = closest_match;
						break;
		}
	}
}



void BST::BST_Traverse()
{
	// This method performs an in-order tree traversal to calculate 
	// the number of distinct words, total words, and the tree's height.
	// If the tree is empty, return.
	// Traversal is done this way (instead of using recursion) just in case
	// for the unlikely event that the tree's height gets too large (e.g. 1,000,000+
	// from inserting from a sorted list of 1,000,000+ items) and the computer has to
	// keep track of a large call stack, which may run the computer out of memory!

	if (BST_root == NULL) return;
	else
	{
		// Save start time
		clock_t START_TIME = clock();


		// Start at root with heiht of 1
		BST_node *position = BST_root;
		BST::tree_height_count++;
		unsigned int current_height_level = 1;

		// Start at the tree's minimum node (leftmost node)
		while (position->left_ptr != NULL)
		{
			position = position->left_ptr;
			current_height_level++;
			BST::tree_height_count++;
		}

		// We're at the minimum node, start counting distinct words and total words.
		BST::distinct_words_count++;
		BST::total_words_count += position->count;

		// Find *position's successor
		do{

			// if current *position has a right node, then *position's successor is that right node's minimum.
			if (position->right_ptr != NULL)
			{
				// Find *position->right_ptr's minimum
				position = position->right_ptr;
				current_height_level++;
				while (position->left_ptr != NULL)
				{
					position = position->left_ptr;
					current_height_level++;
				}

				// We're at the successor node, update distinct words, total words, and height counts.
				BST::distinct_words_count++;
				BST::total_words_count += position->count;
				if (current_height_level > BST::tree_height_count) BST::tree_height_count = current_height_level;
			}

			// if current *position doesn't have a right node
			else
			{
				// Keep moving *position up a level in the tree while it has a parent and it is its parent's right child.
				while (position->parent_ptr != NULL && position == position->parent_ptr->right_ptr)
				{
					position = position->parent_ptr;
					current_height_level--;
				}
				// At this point, the successor should be *position's parent.
				position = position->parent_ptr;
				current_height_level--;

				if (position != NULL)
				{
					// We're at the successor node, update distinct words, total words, and height counts.
					BST::distinct_words_count++;
					BST::total_words_count += position->count;
					if (current_height_level > BST::tree_height_count) BST::tree_height_count = current_height_level;
				}
			}
		} while (position != NULL);

		// Finished traversing the tree

		// Calculate elapsed time in miliseconds.
		unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

		// Calculate elapsed time in seconds with milisecond precision
		BST::traverse_time = ((float)ELAPSED_TIME) / 1000;
	}
}



// Tree Parameters GET methods
unsigned int BST::get_key_comparisons_count(){ return BST::key_comparisons_count; }
unsigned int BST::get_distinct_words_count(){ return BST::distinct_words_count; }
unsigned int BST::get_total_words_count(){ return BST::total_words_count; }
unsigned int BST::get_tree_height_count(){ return BST::tree_height_count; }
float BST::get_traverse_time(){ return BST::traverse_time; }




// --- Helper Functions --- //


BST::BST_node *BST::internal_search_for(const string &word)
{
	// Search tree for <word>.
	// If <word> is found, return pointer for node containing <word>.
	// If <word> is not found, return pointer for what would be <word>'s parent.
	// If tree is empty return NULL.
	// This search will get you to the <word> you're looking for or at least get you to what its parent should be.
	// Just do a simple key/<word> comparison with the pointer this function returns to check if you have the node
	// containing <word> or what would be its parent.


	// If the tree is empty
	if (BST_root == NULL) return NULL;

	BST_node *position = BST_root; // set our starting point

	do{
		switch (compare_strings(word, position->key)){

			// <word> should be somewhere down position's left child
			case -1:	if (position->left_ptr != NULL) position = position->left_ptr;
						else return position;
						break;

			// <word> == position->key
			case  0:	return position;
						break;

			// <word> should be somewhere down position's right child
			case  1:	if (position->right_ptr != NULL) position = position->right_ptr;
						else return position;
						break;
		}
	} while (true);
}



int BST::compare_strings(const string &string1, const string &string2)
{
	// Takes two strings as inputs.
	// Compares both strings character by character.
	// returns -1 if string1 < string2
	// returns  0 if string1 == string2
	// returns  1 if string1 > string2
	
	BST::key_comparisons_count++;
	if (string1 < string2) return -1;
	BST::key_comparisons_count++;
	if (string1 > string2) return 1;
	else return 0;
}