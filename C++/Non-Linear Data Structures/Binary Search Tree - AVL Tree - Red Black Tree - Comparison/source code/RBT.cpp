/*
	Filename:		RBT.cpp
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		Binary Search Tree - AVL Tree - Red Black Tree
	Date:			April 7, 2015
	Description:	Here's where the actual work gets done for the RBT tree. All public and private
					methods are defined here and all tree parameters are calculated here.
*/

#include <iostream>
#include "RBT.h"

using namespace std;

RBT::RBT(void)
{
	// RBT constructor
	
	RBT_nil = new RBT_node;
	RBT_nil->color = false;		// false = black, true = red
	RBT_nil->key = "";
	RBT_nil->parent_ptr = RBT_nil;
	RBT_nil->left_ptr = RBT_nil;
	RBT_nil->right_ptr = RBT_nil;
	RBT_root = RBT_nil;
}



void RBT::RBT_Insert(const string &word)
{
	// This method creates and adds to a RBT tree.
	// If <word> is already IN the tree, increment the node's count.
	// If <word> is NOT in the tree, add it to the tree as a new node 
	// and fix anything we broke so the tree holds up to the 5 RBT rules.

	// if tree is empty
	if (RBT_root == RBT_nil)
	{
		RBT_root = new RBT_node;
		RBT_root->key = word;
		RBT_root->parent_ptr = RBT_nil;
		RBT_root->left_ptr = RBT_nil;
		RBT_root->right_ptr = RBT_nil;
	}
	else
	{
		RBT_node *x = RBT_root;		// *x searches for insertion point
		RBT_node *y = RBT_nil;		// *y lags one step behind x (y is *x's parent)

		while (x != RBT_nil)		// Find the insertion point
		{
			y = x;
			switch (compare_strings(word, x->key))
			{
				// <word> goes somewhere in *x's left subtree
				case -1:	x = x->left_ptr;
							break;

				// we found <word> in the tree!
				case  0:	x->count++;
							return;

				// <word> goes somewhere in *x's right subtree
				case  1:	x = x->right_ptr;
							break;
			}
		}

		// <word> is not in the tree
		// make a new node for <word>

		// create new node with appropriate parameters
		RBT_node *new_node = new RBT_node;
		new_node->key = word;
		new_node->color = true;			// false = black, true = red
		new_node->left_ptr = RBT_nil;
		new_node->right_ptr = RBT_nil;

		// if new node should go to the left of *y (-1)
		if (compare_strings(word, y->key) == -1) {
			y->left_ptr = new_node;
			RBT::left_child_pointer_change_count++;
			new_node->parent_ptr = y;
		}

		// if new node should go to the right of *y (1)
		else
		{
			y->right_ptr = new_node;
			RBT::right_child_pointer_change_count++;
			new_node->parent_ptr = y;
		}

		RBT_Insert_Fixup(new_node);		// Fix anything we broke
	}
}



void RBT::RBT_Traverse()
{
	// This is just a public method to call the recursive traverse method
	
	if (RBT_root == RBT_nil || RBT_root == NULL) return;

	// Save start time
	clock_t START_TIME = clock();

	RBT::traverse(RBT_root, 1);

	// Finished traversing the tree

	// Calculate elapsed time in miliseconds.
	unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

	// Calculate elapsed time in seconds with milisecond precision
	RBT::traverse_time = ((float)ELAPSED_TIME) / 1000;
}



// Tree Parameters GET methods
unsigned int RBT::get_key_comparisons_count(){ return RBT::key_comparisons_count; }
unsigned int RBT::get_parent_pointer_change_count(){ return RBT::parent_pointer_change_count; }
unsigned int RBT::get_left_child_pointer_change_count(){ return RBT::left_child_pointer_change_count; }
unsigned int RBT::get_right_child_pointer_change_count(){ return RBT::right_child_pointer_change_count; }
unsigned int RBT::get_color_change_count(){ return RBT::color_change_count; }
unsigned int RBT::get_distinct_words_count(){ return RBT::distinct_words_count; }
unsigned int RBT::get_total_words_count(){ return RBT::total_words_count; }
unsigned int RBT::get_tree_height(){ return RBT::tree_height; }
float RBT::get_traverse_time(){ return RBT::traverse_time; }




// --- Helper Functions --- //


void RBT::RBT_Insert_Fixup(RBT::RBT_node *node)
{
	// If any new node inserted violates any of the 5 RBT rules, this will fix it.

	while (node->parent_ptr->color)	// color will either be true (red) or false (black)
	{
		if (node->parent_ptr == node->parent_ptr->parent_ptr->left_ptr)
		{
			RBT_node *y = node->parent_ptr->parent_ptr->right_ptr;

			if (y->color)	// color will either be true (red) or false (black)
			{
				// Case 1: re-color only
				node->parent_ptr->color = false;	// black
				y->color = false;	// black
				node->parent_ptr->parent_ptr->color = true;	// red
				RBT::color_change_count += 3;
				node = node->parent_ptr->parent_ptr;
			}

			else 
			{
				// Is *node the right child of its parent?
				if (node == node->parent_ptr->right_ptr)
				{
					// Case 2
					node = node->parent_ptr;
					Left_Rotate(node);
				}

				// Case 3
				node->parent_ptr->color = false;	// black
				node->parent_ptr->parent_ptr->color = true;	// red
				RBT::color_change_count += 2;
				Right_Rotate(node->parent_ptr->parent_ptr);
			}
		}

		else
		{
			RBT_node *y = node->parent_ptr->parent_ptr->left_ptr;

			if (y->color)	// color will either be true (red) or false (black)
			{
				// Case 1: re-color only
				node->parent_ptr->color = false;	// black
				y->color = false;	// black
				node->parent_ptr->parent_ptr->color = true;	// red
				RBT::color_change_count += 3;
				node = node->parent_ptr->parent_ptr;
			}

			else
			{
				// Is *node the right child of its parent?
				if (node == node->parent_ptr->left_ptr)
				{
					// Case 2
					node = node->parent_ptr;
					Right_Rotate(node);
				}

				// Case 3
				node->parent_ptr->color = false;	// black
				node->parent_ptr->parent_ptr->color = true;	// red
				RBT::color_change_count += 2;
				Left_Rotate(node->parent_ptr->parent_ptr);
			}
		}
	}

	RBT_root->color = false;	// false = black, true = red
	RBT::color_change_count++;
}



void RBT::Left_Rotate(RBT::RBT_node *node)
{
	// Performs a RBT left rotate on node

	RBT_node *y = node->right_ptr;		// *y is *node's right (non-nil) child

	// Turn *y's left subtree into *node's right subtree
	node->right_ptr = y->left_ptr;
	RBT::right_child_pointer_change_count++;

	if (y->left_ptr != RBT_nil)
	{
		y->left_ptr->parent_ptr = node;
		RBT::parent_pointer_change_count++;
	}

	y->parent_ptr = node->parent_ptr;	// link *node's parent to *y
	RBT::parent_pointer_change_count++;

	if (node->parent_ptr == RBT_nil)	// If *node has no parent, *node was the
	{									// root, so *y becomes the new root
		RBT_root = y;
	}
	else											// Otherwise (*node has a parent), the
	{												// spot *node used to occupy now
		if (node == node->parent_ptr->left_ptr)		// gets taken by *y
		{
			node->parent_ptr->left_ptr = y;
			RBT::left_child_pointer_change_count++;
		}
		else
		{
			node->parent_ptr->right_ptr = y;
			RBT::right_child_pointer_change_count++;
		}
	}

	y->left_ptr = node;			// put *node on *y's left, which
	RBT::left_child_pointer_change_count++;
	node->parent_ptr = y;		// makes *node's parent be *y
	RBT::parent_pointer_change_count++;
}



void RBT::Right_Rotate(RBT::RBT_node *node)
{
	// Performs a RBT right rotate on node

	RBT_node *y = node->left_ptr;		// *y is *node's left (non-nil) child

	// Turn *y's right subtree into *node's left subtree
	node->left_ptr = y->right_ptr;
	RBT::left_child_pointer_change_count++;

	if (y->right_ptr != RBT_nil)
	{
		y->right_ptr->parent_ptr = node;
		RBT::parent_pointer_change_count++;
	}

	y->parent_ptr = node->parent_ptr;	// link *node's parent to *y
	RBT::parent_pointer_change_count++;

	if (node->parent_ptr == RBT_nil)	// If *node has no parent, *node was the
	{									// root, so *y becomes the new root
		RBT_root = y;
	}
	else											// Otherwise (*node has a parent), the
	{												// spot *node used to occupy now
		if (node == node->parent_ptr->right_ptr)	// gets taken by *y
		{
			node->parent_ptr->right_ptr = y;
			RBT::right_child_pointer_change_count++;
		}
		else
		{
			node->parent_ptr->left_ptr = y;
			RBT::left_child_pointer_change_count++;
		}
	}

	y->right_ptr = node;		// put *node on *y's right, which
	RBT::right_child_pointer_change_count++;
	node->parent_ptr = y;		// makes *node's parent be *y
	RBT::parent_pointer_change_count++;
}



void RBT::traverse(RBT_node *node, int current_height)
{
	// This method performs a recursive in-order tree traversal to calculate 
	// the number of distinct words, total words, and the tree's height.
	// If the tree is empty, return.

	
	if (node == RBT_nil || node == NULL) return;

	// Go left
	RBT::traverse(node->left_ptr, current_height++);

	// Count this node
	RBT::distinct_words_count++;
	RBT::total_words_count += node->count;
	if (current_height > RBT::tree_height) RBT::tree_height = current_height;

	// Go right
	RBT::traverse(node->right_ptr, current_height++);
}



int RBT::compare_strings(const string &string1, const string &string2)
{
	// Takes two strings as inputs.
	// Compares both strings character by character.
	// returns -1 if string1 < string2
	// returns  0 if string1 == string2
	// returns  1 if string1 > string2

	RBT::key_comparisons_count++;
	if (string1 < string2) return -1;
	RBT::key_comparisons_count++;
	if (string1 > string2) return 1;
	else return 0;
}