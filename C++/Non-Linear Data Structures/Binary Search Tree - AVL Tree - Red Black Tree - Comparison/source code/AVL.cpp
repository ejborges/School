/*
	Filename:		AVL.cpp
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		Binary Search Tree - AVL Tree - Red Black Tree
	Date:			April 7, 2015
	Description:	Here's where the actual work gets done for the AVL tree. All public and private
					methods are defined here and all tree parameters are calculated here.
*/

#include "AVL.h"

using namespace std;

AVL::AVL()
{
	// AVL constructor

	AVL_root = NULL;
}

void AVL::AVL_Insert(const string &word)
{
	// This method creates and adds to an AVL tree.
	// If <word> is already IN the tree, increment the node's count.
	// If <word> is NOT in the tree, add it to the tree as a new node 
	// and balance the tree the AVL way.

	// To better understand this code, draw out a sample AVL tree and insert a few
	// new nodes that will hit each case (LL, LR, RL, RR). Will be helpful to follow
	// the AVL powerpoint slides as well.

	AVL_node *Y;					// Y = potential new node to insert
	AVL_node *A, *B, *F, *P, *Q;	// A = *Y's closest ancestor with BF of +-1
									// B = 
									// F = *A's parent
									// P = insert position of new node
									// Q = *P's parent

	AVL_node *C, *CL, *CR;			// C = *B's right child
									// CL = *C's left child
									// CR = *C's right child

	int d;							// displacement; used to adjust Balance Factos

	// If the tree is empty, make a root node with <word>
	if (AVL_root == NULL){
		AVL_root = new AVL_node;
		AVL_root->key = word;
		return;
	}
	else
	{
		// Find where <word> belongs in the tree
		F = Q = NULL;		// *F and *Q lag, so they start at null
		A = P = AVL_root;	// *A and *P start at the root

		// Search for potential insertion point
		while (P != NULL)
		{
			if (P->BF != 0)	// remember the last place we saw
			{				// a non-zero BF (and its parent)
				A = P;
				F = Q;
			}

			switch (compare_strings(word, P->key))
			{
				// if <word> is less than *P
				case -1:	Q = P;
							P = P->left_ptr;
							break;

				// if *P contains the <word> we're inserting, increment *P's count
				case  0:	P->count++;
							return;

				// if <word> is greater than *P
				case  1:	Q = P;
							P = P->right_ptr;
							break;
			}
		}
		
		// We didn't find a node containing <word>, so make a new node
		Y = new AVL_node;
		Y->key = word;

		// Will the new node be Q's new left or right child
		if (compare_strings(word, Q->key) == -1)
		{
			Q->left_ptr = Y;	// new node is now Q's left child
			AVL::left_child_pointer_change_count++;
		}
		else
		{
			Q->right_ptr = Y;	// new node is now Q's right child
			AVL::right_child_pointer_change_count;
		}
		
		// Insertion complege
		// Now to see if we have to fix an imbalance 
		
		// Adjust Balance Factors from *A to *Q
		// All nodes between *A and *Q must have a BF of 0 and will become +-1
		// If <word> was inserted in the LEFT subtree of *A, then d = 1
		// If <word> was inserted in the RIGHT subtree of *A, then d = -1
		
		if (compare_strings(word, A->key) == 1)
		{
			P = A->right_ptr;
			B = P;
			d = -1;
		}
		else
		{
			P = A->left_ptr;
			B = P;
			d = 1;
		}
		
		while (P != Y)	// P is now one node below A
		{
			if (compare_strings(word, P->key) == 1)
			{
				P->BF = -1;
				AVL::Balance_Factor_change_count++;
				P = P->right_ptr;
			}
			else
			{
				P->BF = 1;
				AVL::Balance_Factor_change_count++;
				P = P->left_ptr;
			}
		}
		
		// Now we check the BF at *A and see if we just
		// BALANCED the tree, IMBALANCED the tree, or if
		// it is still BALANCED ENOUGH.

		if (A->BF == 0)							// The tree was completely balanced. The
		{										// insert pushed it to slight imbalance.
			A->BF = d;							// Set the BF to +-1. This is close
			AVL::Balance_Factor_change_count++;	// enough to live with, so exit now.
			return;								
		}

		if (A->BF == -d)						// If the tree had a slight imbalance
		{										// the OTHER way, did the insertion
			A->BF = 0;							// throw the tree INTO balance?
			AVL::Balance_Factor_change_count++;	// If so, set the BF to zero and return.
			return;								
		}
		
		// If we took neither of the two returns just above, 
		// then the tree WAS acceptably imbalanced, and is
		// now UNACCEPTABLY IMBALANCED. So, perform a rotation.

		if (d == 1)	// if LEFT imbalance
		{
			if (B->BF == 1)	// LL rotation
			{
				A->left_ptr = B->right_ptr;
				AVL::left_child_pointer_change_count++;
				B->right_ptr = A;
				AVL::right_child_pointer_change_count++;
				A->BF = 0;
				B->BF = 0;
				AVL::Balance_Factor_change_count += 2;
			}
			else	// LR rotation
			{
				C = B->right_ptr;	// *C is *B's right child
				CL = C->left_ptr;	// *CL is *C's left child
				CR = C->right_ptr;	// *CR is *C's right child

				// Adjust child pointers of *A, *B, and *C
				C->left_ptr = B;
				C->right_ptr = A;
				B->right_ptr = CL;
				A->left_ptr = CR;
				AVL::left_child_pointer_change_count += 2;
				AVL::right_child_pointer_change_count += 2;

				// Set the new BF's at *A and *B, based on the 
				// BF at *C. Note: There are 3 sub-cases
				switch (C->BF)
				{

					case -1:	A->BF = 0;
								B->BF = 1;
								AVL::Balance_Factor_change_count += 2;
								break;

					case  0:	A->BF = 0;
								B->BF = 0;
								AVL::Balance_Factor_change_count += 2;
								break;

					case  1:	A->BF = -1;
								B->BF = 0;
								AVL::Balance_Factor_change_count += 2;
								break;
				}

				// *C is now balanced
				C->BF = 0;
				AVL::Balance_Factor_change_count++;
				B = C;
			}
		}
		else // if RIGHT imbalance (d == -1)
		{
			if (B->BF == -1)	// RR rotation
			{
				A->right_ptr = B->left_ptr;
				AVL::right_child_pointer_change_count++;
				B->left_ptr = A;
				AVL::left_child_pointer_change_count++;
				A->BF = 0;
				B->BF = 0;
				AVL::Balance_Factor_change_count += 2;
			}
			else	// RL rotation
			{
				C = B->left_ptr;	// *C is *B's left child
				CL = C->left_ptr;	// *CL is *C's left child
				CR = C->right_ptr;	// *CR is *C's right child

				// Adjust child pointers of *A, *B, and *C
				C->right_ptr = B;
				C->left_ptr = A;
				B->left_ptr = CR;
				A->right_ptr = CL;
				AVL::left_child_pointer_change_count += 2;
				AVL::right_child_pointer_change_count += 2;

				// Set the new BF's at *A and *B, based on the 
				// BF at *C. Note: There are 3 sub-cases
				switch (C->BF)
				{

					case -1:	A->BF = 1;
								B->BF = 0;
								AVL::Balance_Factor_change_count += 2;
								break;

					case  0:	A->BF = 0;
								B->BF = 0;
								AVL::Balance_Factor_change_count += 2;
								break;

					case  1:	A->BF = 0;
								B->BF = -1;
								AVL::Balance_Factor_change_count += 2;
								break;
				}

				// *C is now balanced
				C->BF = 0;
				AVL::Balance_Factor_change_count++;
				B = C;
			}
		}

		// Finish up:
		// 
		// The subtree with root *B has been rebalanced, and
		// is the new subtree of *F. The original subtree
		// of *F had root *A.

		// did we balance the root?
		if (F == NULL)
		{
			AVL_root = B;
			return;
		}

		// otherwise, we rebalanced whatever was the
		// child (left or right) of *F.
		if (A == F->left_ptr)
		{
			F->left_ptr = B;
			AVL::left_child_pointer_change_count++;
			return;
		}
		if (A == F->right_ptr)
		{
			F->right_ptr = B;
			AVL::right_child_pointer_change_count++;
			return;
		}
		
		// We should never get here
		exit(1);

		// End of AVL_Insert()
	}
}



void AVL::AVL_Traverse()
{
	// This is just a public method to call the recursive traverse method
	
	if (AVL_root == NULL) return;

	// Save start time
	clock_t START_TIME = clock();

	AVL::traverse(AVL_root, 1);

	// Finished traversing the tree

	// Calculate elapsed time in miliseconds.
	unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

	// Calculate elapsed time in seconds with milisecond precision
	AVL::traverse_time = ((float)ELAPSED_TIME) / 1000;
}




// Tree Parameters GET methods
unsigned int AVL::get_key_comparisons_count() { return AVL::key_comparisons_count; }
unsigned int AVL::get_left_child_pointer_change_count() { return AVL::left_child_pointer_change_count; }
unsigned int AVL::get_right_child_pointer_change_count() { return AVL::right_child_pointer_change_count; }
unsigned int AVL::get_Balance_Factor_change_count() { return AVL::Balance_Factor_change_count; }
unsigned int AVL::get_distinct_words_count() { return AVL::distinct_words_count; }
unsigned int AVL::get_total_words_count() { return AVL::total_words_count; }
unsigned int AVL::get_tree_height() { return AVL::tree_height; }
float AVL::get_traverse_time() { return AVL::traverse_time; }



// --- Helper Functions --- //


void AVL::traverse(AVL_node *node, int current_height)
{
	// This method performs a recursive in-order tree traversal to calculate 
	// the number of distinct words, total words, and the tree's height.
	// If the tree is empty, return.

	if (node == NULL) return;

	// Go left
	AVL::traverse(node->left_ptr, current_height++);

	// Count this node
	AVL::distinct_words_count++;
	AVL::total_words_count += node->count;
	if (current_height > AVL::tree_height) AVL::tree_height = current_height;

	// Go right
	AVL::traverse(node->right_ptr, current_height++);
}


int AVL::compare_strings(const string &string1, const string &string2)
{
	// Takes two strings as inputs.
	// Compares both strings character by character.
	// returns -1 if string1 < string2
	// returns  0 if string1 == string2
	// returns  1 if string1 > string2

	AVL::key_comparisons_count++;
	if (string1 < string2) return -1;
	AVL::key_comparisons_count++;
	if (string1 > string2) return 1;
	else return 0;
}