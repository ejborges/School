/*
Emilio Borges - Feb. 17, 2015
EECS 2510 - Non-Linear Data Structures
Project 1 - Binary Search Tree (BST)
*/


#include "BST.h"

using namespace std;

// BST constructor
BST::BST(){
	BST_root = NULL;
}



// Insert
// If <string> is already IN the set, increment the node's count
// If <string> is NOT in the set, add it to the set
// Output: <string> <count>
void BST::insert(string const& string){
	
	// If the set/tree is empty, make a root node with <string> and output it's key and count
	if (BST_root == NULL){
		BST_root = new BST_node;
		BST_root->key = string;
		cout << BST_root->key << " " << BST_root->count << endl;
	}
	else {
		// this will give us the node containing <string> or give us its parent node
		// if closest_match is the parent node, we know <string> is a new word and must be added
		// we'll know if closest_match is the parent if compare(string, closest_match->key) doesn't return 0
		BST_node *closest_match = internal_search(string);

		// one last comparision to see if we have the parent node or the node containing <string>
		// if <string> is less than or greater than closest_match->key, then we create a new node for <string>
		// else, we increment closest_match->count
		switch (compare_strings(string, closest_match->key)){

			// closest_match happened to be the parent and we have to insert a new left node for <string>
			case -1: closest_match->left_ptr = new BST_node;
					 closest_match->left_ptr->key = string;
					 closest_match->left_ptr->parent_ptr = closest_match;
					 cout << closest_match->left_ptr->key << " " << closest_match->left_ptr->count << endl;
					 break;

			// closest_match happened to be the node containing <string>, so ++ it's count
			case  0: closest_match->count++;
					 cout << closest_match->key << " " << closest_match->count << endl;
					 break;

			// closest_match happened to be the parent and we have to insert a new right node for <string>
			case  1: closest_match->right_ptr = new BST_node;
					 closest_match->right_ptr->key = string;
					 closest_match->right_ptr->parent_ptr = closest_match;
					 cout << closest_match->right_ptr->key << " " << closest_match->right_ptr->count << endl;
					 break;
		}
	}
}


// Delete
// If <string> is NOT in the set, output <string> <-1>
// If <string> IS in the set, and has a count of more than 1, decrement the count 
// corresponding to the string, and output <string> <nnn>, where <nnn> is the new count
// If <string> IS in the set, and has a count of exactly 1, delete it from the set, and ouput <string> <0>
void BST::remove(string const& string){
	
	// this will give us the node containing <string> or give us its parent node
	// if closest_match is the parent node, we know <string> is not in the set/tree
	// we'll know if closest_match is the parent if compare(string, closest_match->key) doesn't return 0
	BST_node *closest_match = internal_search(string);

	// one last comparision to see if we have the parent node or the node containing <string>
	// if internal_search returned NULL, we know <string> is definitely not in the set/tree
	// else, if the comparison returns 0, we have found <string> in the set/tree
	// else, we couldn't find <string> in the set/tree
	if (closest_match == NULL) cout << string << " -1\n";

	// if we found <string> in the set/tree
	else if (compare_strings(string, closest_match->key) == 0){
		
		// If <string> IS in the set, and has a count of more than 1, decrement the count 
		// corresponding to the string, and output <string> <nnn>, where <nnn> is the new count
		if (closest_match->count > 1) cout << closest_match->key << " " << --closest_match->count << endl;

		// If <string> IS in the set, and has a count of exactly 1, delete it from the set, and ouput <string> <0>
		else {

			// If we're at the root node
			if (closest_match->parent_ptr == NULL){

				// If the root has no children, set root pointer to null and delete the root node
				if (closest_match->left_ptr == NULL && closest_match->right_ptr == NULL){
					BST_root = NULL;
					delete closest_match;
				}

				// else if root has left child and no right child
				else if(closest_match->left_ptr != NULL && closest_match->right_ptr == NULL){

					// set the root pointer to root's left child, set root's left's child parent pointer to null, and delete root
					BST_root = closest_match->left_ptr;
					closest_match->left_ptr->parent_ptr = NULL;
					delete closest_match;
				}

				// else if root has right child and no left child
				else if (closest_match->right_ptr != NULL && closest_match->left_ptr == NULL){

					// set the root pointer to root's right child, set root's right's child parent pointer to null, and delete root
					BST_root = closest_match->right_ptr;
					closest_match->right_ptr->parent_ptr = NULL;
					delete closest_match;
				}

				// else, root has both left and right children
				else {

					// find the root's successor
					BST_node *successor = next_prev_finder(1, closest_match);

					// make root's data equal its successor's data
					closest_match->key = successor->key;
					closest_match->count = successor->count;

					// if successor has no children (since successor went through every possible left pointer, we only check the right)
					if (successor->right_ptr == NULL){

						// set the appropriate parent child pointer to null and delete successor node
						if (successor->parent_ptr->right_ptr == successor) successor->parent_ptr->right_ptr = NULL;
						else successor->parent_ptr->left_ptr = NULL;
						delete successor;
					}

					// if successor has a right child
					else{

						// set the appropriate parent child pointer to point to the successor's right child, adjust the child's parent pointer, and delete successor
						if (successor->parent_ptr->right_ptr == successor) successor->parent_ptr->right_ptr = successor->right_ptr;
						else successor->parent_ptr->left_ptr = successor->right_ptr;
						successor->right_ptr->parent_ptr = successor->parent_ptr;
						delete successor;
					}
				}
			}

			// else if we're on a leaf
			else if (closest_match->left_ptr == NULL && closest_match->right_ptr == NULL) {

				// set leaf's parent left/right node to NULL, and delete the leaf node
				if (closest_match->parent_ptr->left_ptr == closest_match) closest_match->parent_ptr->left_ptr = NULL;
				else closest_match->parent_ptr->right_ptr = NULL;
				delete closest_match;
			}

			// else, we are on an internal node
			else {
				
				// if node only has left child and no right child
				if (closest_match->left_ptr != NULL && closest_match->right_ptr == NULL){

					// set node's parent's left/righ pointer to point to node's left child, set node's left child's parent pointer to point to node's parent, and delete node
					if (closest_match->parent_ptr->left_ptr == closest_match) closest_match->parent_ptr->left_ptr = closest_match->left_ptr;
					else closest_match->parent_ptr->right_ptr = closest_match->left_ptr;
					closest_match->left_ptr->parent_ptr = closest_match->parent_ptr;
					delete closest_match;
				}

				// else if node only has right child and no left child
				else if (closest_match->right_ptr != NULL && closest_match->left_ptr == NULL){

					// set node's parent's left/righ pointer to point to node's right child, set node's right child's parent pointer to point to node's parent, and delete node
					if (closest_match->parent_ptr->left_ptr == closest_match) closest_match->parent_ptr->left_ptr = closest_match->right_ptr;
					else closest_match->parent_ptr->right_ptr = closest_match->right_ptr;
					closest_match->right_ptr->parent_ptr = closest_match->parent_ptr;
					delete closest_match;
				}

				// else, node has both left and right children
				else {

					// find the root's successor
					BST_node *successor = next_prev_finder(1, closest_match);

					// make root's data equal its successor's data
					closest_match->key = successor->key;
					closest_match->count = successor->count;

					// if successor has no children (since successor went through every possible left pointer, we only check the right)
					if (successor->right_ptr == NULL){

						// set the appropriate parent child pointer to null and delete successor node
						if (successor->parent_ptr->right_ptr == successor) successor->parent_ptr->right_ptr = NULL;
						else successor->parent_ptr->left_ptr = NULL;
						delete successor;
					}

					// if successor has a right child
					else{

						// set the appropriate parent child pointer to point to the successor's right child, adjust the child's parent pointer, and delete successor
						if (successor->parent_ptr->right_ptr == successor) successor->parent_ptr->right_ptr = successor->right_ptr;
						else successor->parent_ptr->left_ptr = successor->right_ptr;
						successor->right_ptr->parent_ptr = successor->parent_ptr;
						delete successor;
					}
				}
			}

			// Once we "deleted" the node, print out <string> <0>
			cout << string << " 0\n";
		}
	}

	// else, compare_strings(string, closest_match->key) returned -1 or 1, which means <string> is not in the set/tree
	else cout << string << " -1\n";
}


// Search
// If <string> is in the set, output <string> <nnn>, where <nnn> is the count for the <string>
// If <string> is not in the set, output <string> <0>
void BST::search(string const& string){
	
	// this will give us the node containing <string> or give us its parent node
	// if closest_match is the parent node, we know <string> is not in the set/tree
	// we'll know if closest_match is the parent if compare(string, closest_match->key) doesn't return 0
	BST_node *closest_match = internal_search(string);

	// one last comparision to see if we have the parent node or the node containing <string>
	// if internal_search returned NULL, we know <string> is definitely not in the set/tree
	// else, if the comparison returns 0, we have found <string> in the set/tree
	// else, we couldn't find <string> in the set/tree
	if (closest_match == NULL) cout << string << " 0\n";
	else if (compare_strings(string, closest_match->key) == 0) cout << string << " " << closest_match->count << endl;
	else cout << string << " 0\n";
}


// Minimum
// Output <string>, where <string> is the minimum value of the set.
// If the set is empty, output a blank line.
void BST::min(){

	// if the root is NULL, then the set/tree is empty
	if (BST_root == NULL) cout << endl;

	// tell the min_max_finder() function to return the minimum
	else cout << min_max_finder(-1, BST_root)->key << endl;
}


// Maximum
// Output <string>, where <string> is the maximum value of the set.
// If the set is empty, output a blank line.
void BST::max(){

	// if the root is NULL, then the set/tree is empty
	if (BST_root == NULL) cout << endl;

	// tell the min_max_finder() function to return the maximum
	else cout << min_max_finder(1, BST_root)->key << endl;
}


// Next
// If <string> is in the set, output its successsor <string>.
// If <string> doesn't have a successor, or if it isn't in the set, output a blank line
void BST::next(string const& string){
	
	// this will give us the node containing <string> or give us its parent node
	// if closest_match is the parent node, we know <string> is not in the set/tree
	// we'll know if closest_match is the parent if compare(string, closest_match->key) doesn't return 0
	BST_node *closest_match = internal_search(string);

	// one last comparision to see if we have the parent node or the node containing <string>
	// if internal_search returned NULL, we know <string> is definitely not in the set/tree
	// else, if the comparison returns 0, we have found <string> in the set/tree
	// else, we couldn't find <string> in the set/tree
	if (closest_match == NULL) cout << endl;

	// if we found <string> in the set/tree, try to find its successor
	else if (compare_strings(string, closest_match->key) == 0) {

		// successor will hold the successor string (if found) or NULL (if not found)
		BST_node *successor = next_prev_finder(1, closest_match);

		if (successor != NULL) cout << successor->key << endl;
		else cout << endl;
	}

	// else, closest_match returned -1 or 1, which means <string> is not in the set/tree
	else cout << endl;
}


// Prev
// If <string> is in the set, output its predecessor <string>.
// If <string> doesn't have a predecessor, or if it isn't in the set, output a blank line
void BST::prev(string const& string){
	
	// this will give us the node containing <string> or give us its parent node
	// if closest_match is the parent node, we know <string> is not in the set/tree
	// we'll know if closest_match is the parent if compare(string, closest_match->key) doesn't return 0
	BST_node *closest_match = internal_search(string);

	// one last comparision to see if we have the parent node or the node containing <string>
	// if internal_search returned NULL, we know <string> is definitely not in the set/tree
	// else, if the comparison returns 0, we have found <string> in the set/tree
	// else, we couldn't find <string> in the set/tree
	if (closest_match == NULL) cout << endl;

	// if we found <string> in the set/tree, try to find its predecessor
	else if (compare_strings(string, closest_match->key) == 0) {

		// predecessor will hold the predecessor string (if found) or NULL (if not found)
		BST_node *predecessor = next_prev_finder(-1, closest_match);

		if (predecessor != NULL) cout << predecessor->key << endl;
		else cout << endl;
	}

	// else, closest_match returned -1 or 1, which means <string> is not in the set/tree
	else cout << endl;
}


// Does an in-order traversal, listing all of the strings in the tree in ascending order with their count.
// If set/tree is empty, output a blank line
void BST::list(){
	
	// If the tree is empty
	if (BST_root == NULL) cout << endl;

	// else, find set's minimum and loop through its successors
	else {

		// find set's minimum
		BST_node *position = min_max_finder(-1, BST_root);

		// print position's string and count, then set position to be its successor
		// when position equals NULL, we've reached the set/tree's maximum
		do{
			cout << position->key << " " << position->count << endl;
			position = next_prev_finder(1, position);
		} while (position != NULL);
	}
}





// --- Helper Functions --- //


// Search tree for <string>
// If <string> is found, return pointer for node containing <string>
// If <string> is not found, return pointer for what would be <string>'s parent
// If tree is empty return NULL
// This search will get you to the <string> you're looking for or at least get you to what its parent should be
// Just do a simple key/<string> comparison with the pointer this function returns
BST::BST_node *BST::internal_search(string const& string){

	// If the tree is empty
	if (BST_root == NULL) return NULL;

	BST_node *position = BST_root; // set our starting point

	do{
		switch (compare_strings(string, position->key)){

			// <string> should be somewhere down position's left child
			case -1: if (position->left_ptr != NULL) position = position->left_ptr;
					 else return position;
					 break;

			// <string> == position->key
			case  0: return position; break;

			// <string> should be somewhere down position's right child
			case  1: if (position->right_ptr != NULL) position = position->right_ptr;
					 else return position;
					 break;
		}
	} while (true);
}


// Takes two strings as inputs
// Compares both strings character by character
// returns -1 if string1 < string2
// returns  0 if string1 == string2
// returns  1 if string1 > string2
int BST::compare_strings(string const& string1, string const& string2){

	int check_length;
	
	// Obligatory note that this is a C++ "Conditional Operator", (condition) ? if_true : if_false
	// check_length has to be the smallest length of the two strings or else we risk running out of bounds with one of the two strings
	(string1.length() < string2.length()) ? check_length = string1.length() : check_length = string2.length();

	// check character by character which one has a smaller/greater ASCII value
	// cast a char into ASCII int
	for (int i = 0; i < check_length; i++){
		if ((int)string1[i] < (int)string2[i]) return -1;
		if ((int)string1[i] > (int)string2[i]) return 1;
	}

	// if they have the same characters throughout the check_length range, check which one's .length is smaller/larger
	if (string1.length() < string2.length()) return -1;
	else if (string1.length() > string2.length()) return 1;
	else return 0; // string1 must equal string2
}


// Find the tree's minimum or maximum
// Given a starting <position> node,
// if <direction> is any negative number, find the minimum
// if <direction> is 0 or any positive number, find the maximum
// return pointer to minimum/maximum node
// if tree is empty or given position is NULL, return NULL
BST::BST_node *BST::min_max_finder(int direction, BST_node *position){

	if (BST_root == NULL || position == NULL ) return NULL;
	else {

		// find minimum
		if (direction < 0){
			// Follow as many left nodes as possible
			while (position->left_ptr != NULL){
				position = position->left_ptr;
			}
			return position;
		}

		// find maximum
		else {
			// Follow as many right nodes as possible
			while (position->right_ptr != NULL){
				position = position->right_ptr;
			}
			return position;
		}
	}
}


// Find a node's successor or predecessor
// Given a starting <position> node,
// if <direction> is any negative number, find the predecessor
// if <direction> is 0 or any positive number, find the successor
// return pointer to predecessor/successor node
// return NULL if predecessor/successor doesn't exist
// if tree is empty or given position is NULL, return NULL
BST::BST_node* BST::next_prev_finder(int direction, BST_node *position) {

	if (BST_root == NULL || position == NULL) return NULL;
	else {

		// find predecessor
		if (direction < 0){
			
			// if current <position> has a left node, <position>'s predecessor is that left node's maximum
			if (position->left_ptr != NULL) return min_max_finder(1, position->left_ptr);

			// if current <position> doesn't have a left node
			else {

				// try to move <position> up so that it points at its parent, if such a parent exists and <position> is its parent's left child
				// quit moving <position> up if it's its parent's right child, then the successor will be <position>'s parent
				// if <position> points to the set's minimum (<position> doesn't have a predecessor), then this will return the parent of the tree's root, which is NULL
				while (position->parent_ptr != NULL && position == position->parent_ptr->left_ptr) position = position->parent_ptr;
				return position->parent_ptr;
			}
		}

		// find successor
		else {

			// if current <position> has a right node, <position>'s successor is that right node's minimum
			if (position->right_ptr != NULL) return min_max_finder(-1, position->right_ptr);

			// if current <position> doesn't have a right node
			else {

				// try to move <position> up so that it points at its parent, if such a parent exists and <position> is its parent's right child
				// quit moving <position> up if it's its parent's left child, then the successor will be <position>'s parent
				// if <position> points to the set's maximum (<position> doesn't have a successor), then this will return the parent of the tree's root, which is NULL
				while (position->parent_ptr != NULL && position == position->parent_ptr->right_ptr) position = position->parent_ptr;
				return position->parent_ptr;
			}
		}
	}
}