/*
	Filename:		AVL.cpp
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		B-Trees - AVL Tree - Disk-based Comparison
	Date:			April 25, 2015
	Description:	Here's where the actual work gets done for the AVL tree. All public and private
					methods are defined here and all tree parameters are calculated here.

*/

#include "AVL.h"

using namespace std;

AVL::AVL()
{
	// AVL constructor

	AVL_root_id = 0;

	// if file does not exist, create it.
	// if file does exist, do nothing with it.
	ofstream tree_file;
	tree_file.open("trees\\avl.tree", ios::binary);
	if (file.fail())
	{
		cout << "Something went wrong :(\n\nUnable to open ofstream trees\\avl.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
	tree_file.close();

	// open the tree file stream to read and write in binary
	file.open("trees\\avl.tree", ios::binary | ios::in | ios::out);

	if (file.fail())
	{
		cout << "Something went wrong :(\n\nUnable to open fstream trees\\avl.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
}

void AVL::AVL_Insert(const string &word)
{
	// This method creates and adds to an AVL tree.
	// If <word> is already IN the tree, increment the node's count.
	// If <word> is NOT in the tree, add it to the tree as a new node 
	// and balance the tree the AVL way.

	// For this project, we can only have 3 nodes in memory at one time. That's 
	// what n1, n2, and n3 are for. They each take on different roles throughout 
	// this code, but I have kept a line of code commented out above wherever n1,
	// n2, and n3 are used that helps you see what role n1, n2, or n3 is playing 
	// at that particular spot in the program.

	// To better understand this code, draw out a sample AVL tree and insert a few
	// new nodes that will hit each case (LL, LR, RL, RR). Will be helpful to follow
	// the AVL powerpoint slides as well.

	int A, B, F, Q, n3L, n3R, d;	// A = new node's closest ancestor with BF of +-1
									// B = 
									// F = A's parent
									// Q = P's parent; P = insert position of new node
									// n3L = n3's left child
									// n3R = n3's right child
									// d = displacement; used to adjust Balance Factos

	// If the tree is empty, make a root node with <word> and save it to disk
	if (AVL_root_id == 0)
	{
		n1.id = id_count++;
		AVL_root_id = n1.id;
		//n1.key = word;
		strcpy_s(n1.key, word.c_str());
		write_node_to_file(n1);
		return;
	}
	else
	{
		// Find where <word> belongs in the tree
		F = Q = 0;									// F and Q lag, so they start at null
		A = AVL_root_id;							// A and P start at the root

		//P = read_node_from_file(AVL_root_id);
		n1 = read_node_from_file(AVL_root_id);
		
		// Search for potential insertion point
		//while (P.id != 0)
		while (n1.id != 0)
		{
			//cout << "I'm stuck here!\n";
			//if (P.BF != 0)
			if (n1.BF != 0)
			{				
				// remember the last place we saw
				// a non-zero BF (and its parent)

				//A = P.id;
				A = n1.id;
				F = Q;
			}

			//switch (compare_strings(word, P.key))
			switch (compare_strings(word, n1.key))
			{
				// if <word> is less than n1.key (n1 == P)
				case -1:	//Q = P.id;
							Q = n1.id;

							//P = read_node_from_file(P.left);
							n1 = read_node_from_file(n1.left);
							break;

				// if n1 contains the <word> we're inserting, increment n1's count (n1 == P)
				case  0:	//P.count++;
							n1.count++;

							//write_node_to_file(P);
							write_node_to_file(n1);
							return;

				// if <word> is greater than n1.key (n1 == P)
				case  1:	//Q = P.id;
							Q = n1.id;

							//P = read_node_from_file(P.right);
							n1 = read_node_from_file(n1.right);
							break;
			}
		}

		// At this point, n1 (n1 = P) fell off the tree (n1 == P == NULL node) and Q is the new node's parent

		// We didn't find a node containing <word>, so make a new node
		
		// n1 will equal Q for this part (since at this point n1 == NULL node) and n2 will hold the new node.
		n1 = read_node_from_file(Q);

		//Y = new AVL_node;
		n2.id = id_count++;

		//Y.key = word;
		//n2.key = word;
		strcpy_s(n2.key, word.c_str());

		// n2 could have held other info from the last insertion, better clear it out just to be safe...
		n2.count = 1;
		n2.BF = 0;
		n2.left = 0;
		n2.right = 0;
		write_node_to_file(n2);

		// Will the new node be n1's new left or right child? (n1 == Q, n2 == Y)
		//if (compare_strings(word, Q.key) == -1)
		if (compare_strings(word, n1.key) == -1)
		{
			//Q.left_ptr = Y;			// new node is now Q's left child
			n1.left = n2.id;			// n2 is now n1's left child

			//write_node_to_file(Q);
			write_node_to_file(n1);

			AVL::left_child_pointer_change_count++;
		}
		else
		{
			//Q.right_ptr = Y;			// new node is now Q's right child
			n1.right = n2.id;			// n2 is now n1's right child

			//write_node_to_file(Q);
			write_node_to_file(n1);

			AVL::right_child_pointer_change_count++;
		}

		// Insertion complege
		// Now to see if we have to fix an imbalance 

		// Adjust Balance Factors from A to Q 
		// All nodes between A and Q must have a BF of 0 and will become +-1
		// If <word> was inserted in the LEFT subtree of A, then d = 1
		// If <word> was inserted in the RIGHT subtree of A, then d = -1

		// Q pointer is not used anymore, so reuse n1
		// n1 == A
		n1 = read_node_from_file(A);

		//if (compare_strings(word, A.key) == 1)
		if (compare_strings(word, n1.key) == 1)
		{
			// We still need A and Y (A == n1, Y == n2) so bring out n3 to hold P

			//P = A.right_ptr;
			n3 = read_node_from_file(n1.right);

			//B = P;
			B = n3.id;

			d = -1;
		}
		else
		{
			// We still need A and Y (A == n1, Y == n2) so bring out n3 to hold P

			//P = A.left_ptr;
			n3 = read_node_from_file(n1.left);

			//B = P;
			B = n3.id;

			d = 1;
		}

		// P is now one node below A
		// n3 is now one node below n1

		// n1 == A, n2 == Y, n3 == P

		//while (P != Y)
		while (n3.id != n2.id)
		{
			//if (compare_strings(word, P.key) == 1)
			if (compare_strings(word, n3.key) == 1)
			{
				//P.BF = -1;
				n3.BF = -1;

				AVL::Balance_Factor_change_count++;

				write_node_to_file(n3);

				//P = P.right_ptr;
				n3 = read_node_from_file(n3.right);
			}
			else
			{
				//P.BF = 1;
				n3.BF = 1;

				AVL::Balance_Factor_change_count++;

				write_node_to_file(n3);

				//P = P.left_ptr;
				n3 = read_node_from_file(n3.left);
			}
		}

		// Now we check the BF at n1 (n1 == A) and see if we just
		// BALANCED the tree, IMBALANCED the tree, or if
		// it is still BALANCED ENOUGH.

		//if (A.BF == 0)						// The tree was completely balanced. The
		if (n1.BF == 0)							// insert pushed it to slight imbalance.
		{										// Set the BF to +-1. This is close
			//A.BF = d;							// enough to live with, so exit now.
			n1.BF = d;

			write_node_to_file(n1);

			AVL::Balance_Factor_change_count++;	
			return;
		}

		//if (A.BF == -d)						// If the tree had a slight imbalance
		if (n1.BF == -d)						// the OTHER way, did the insertion
		{										// throw the tree INTO balance?
			//A.BF = 0;							// If so, set the BF to zero and return.
			n1.BF = 0;

			write_node_to_file(n1);

			AVL::Balance_Factor_change_count++;	
			return;
		}

		// If we took neither of the two returns just above, 
		// then the tree WAS acceptably imbalanced, and is
		// now UNACCEPTABLY IMBALANCED. So, perform a rotation.

		// At this point, we won't use Y or P anymore. Therefore, reuse n2 and n3.

		// n1 == A, n2 == B
		n2 = read_node_from_file(B);

		if (d == 1)	// if LEFT imbalance
		{
			//if (B.BF == 1)	// LL rotation
			if (n2.BF == 1)	// LL rotation
			{
				//A.left_ptr = B.right_ptr;
				n1.left = n2.right;

				AVL::left_child_pointer_change_count++;

				//B.right_ptr = A;
				n2.right = n1.id;

				AVL::right_child_pointer_change_count++;

				//A.BF = 0;
				n1.BF = 0;

				//B.BF = 0;
				n2.BF = 0;

				AVL::Balance_Factor_change_count += 2;

				write_node_to_file(n1);
				write_node_to_file(n2);
			}

			// Whatever n3 was before, it won't be used anymore... Reuse n3
			// n1 == A, n2 == B, n3 == C

			else	// LR rotation
			{
				//C = B.right_ptr;						// C is B's right child
				n3 = read_node_from_file(n2.right);		// n3 is n2's right child

				//CL = C.left_ptr;						// CL is C's left child
				n3L = n3.left;							// n3L is n3's left child

				//CR = C.right_ptr;					// CR is C's right child
				n3R = n3.right;							// n3R is n3's right child

				// Adjust child pointers of A, B, and C
				// Adjust child pointers of n1, n2, and n3

				//C.left_ptr = B;
				n3.left = n2.id;

				//C.right_ptr = A;
				n3.right = n1.id;

				//B.right_ptr = CL;
				n2.right = n3L;

				//A.left_ptr = CR;
				n1.left = n3R;

				AVL::left_child_pointer_change_count += 2;
				AVL::right_child_pointer_change_count += 2;

				// Set the new BF's at n1 (n1 == A) and n2 (n2 == B), based on the 
				// BF at n3 (n3 == C). Note: There are 3 sub-cases

				//switch (C.BF)
				switch (n3.BF)
				{

					case -1:	//A.BF = 0;
								n1.BF = 0;

								//B.BF = 1;
								n2.BF = 1;

								AVL::Balance_Factor_change_count += 2;
								break;

					case  0:	//A.BF = 0;
								n1.BF = 0;

								//B.BF = 0;
								n2.BF = 0;

								AVL::Balance_Factor_change_count += 2;
								break;

					case  1:	//A.BF = -1;
								n1.BF = -1;

								//B.BF = 0;
								n2.BF = 0;

								AVL::Balance_Factor_change_count += 2;
								break;
				}

				// n3 (n3 == C) is now balanced

				//C.BF = 0;
				n3.BF = 0;

				AVL::Balance_Factor_change_count++;

				//B = C;
				B = n3.id;

				write_node_to_file(n1);
				write_node_to_file(n2);
				write_node_to_file(n3);
			}
		}

		// n1 == A, n2 == B

		else // if RIGHT imbalance (d == -1)
		{
			//if (B->BF == -1)	// RR rotation
			if (n2.BF == -1)	// RR rotation
			{
				//A.right_ptr = B.left_ptr;
				n1.right = n2.left;

				AVL::right_child_pointer_change_count++;

				//B.left_ptr = A;
				n2.left = n1.id;

				AVL::left_child_pointer_change_count++;

				//A.BF = 0;
				n1.BF = 0;

				//B.BF = 0;
				n2.BF = 0;

				AVL::Balance_Factor_change_count += 2;
				write_node_to_file(n1);
				write_node_to_file(n2);
			}

			// Whatever n3 was before, it won't be used anymore... Reuse n3
			// n1 == A, n2 == B, n3 == C

			else	// RL rotation
			{
				//C = B.left_ptr;					// C is B's left child
				n3 = read_node_from_file(n2.left);	// n3 is n2's left child

				//CL = C.left_ptr;					// CL is C's left child
				n3L = n3.left;						// n3L is n3's left child

				//CR = C.right_ptr;					// CR is C's right child
				n3R = n3.right;						// n3R is n3's right child


				// Adjust child pointers of A, B, and C
				// Adjust child pointers of n1, n2, and n3

				//C.right_ptr = B;
				n3.right = n2.id;

				//C.left_ptr = A;
				n3.left = n1.id;

				//B.left_ptr = CR;
				n2.left = n3R;

				//A.right_ptr = CL;
				n1.right = n3L;

				AVL::left_child_pointer_change_count += 2;
				AVL::right_child_pointer_change_count += 2;

				// Set the new BF's at n1 (n1 == A) and n2 (n2 == B), based on the 
				// BF at n3 (n3 == C). Note: There are 3 sub-cases

				//switch (C.BF)
				switch (n3.BF)
				{

					case -1:	//A.BF = 1;
								n1.BF = 1;

								//B.BF = 0;
								n2.BF = 0;

								AVL::Balance_Factor_change_count += 2;
								break;

					case  0:	//A.BF = 0;
								n1.BF = 0;

								//B.BF = 0;
								n2.BF = 0;

								AVL::Balance_Factor_change_count += 2;
								break;

					case  1:	//A.BF = 0;
								n1.BF = 0;

								//B.BF = -1;
								n2.BF = -1;

								AVL::Balance_Factor_change_count += 2;
								break;
				}

				// n3 (n3 == C) is now balanced

				//C.BF = 0;
				n3.BF = 0;

				AVL::Balance_Factor_change_count++;

				//B = C;
				B = n3.id;

				write_node_to_file(n1);
				write_node_to_file(n2);
				write_node_to_file(n3);
			}
		}

		// Finish up:
		// 
		// The subtree with root n2 (n2 == B) has been rebalanced, and
		// is the new subtree of F. The original subtree
		// of F had root n1 (n1 == A).

		// From this point on, we won't use n3 == C, so reuse n3
		// n1 == A, n2 == B, n3 == F
		n3 = read_node_from_file(F);

		// did we balance the root?
		//if (F == NULL)
		if (n3.id == 0)
		{
			//AVL_root_id = B;
			AVL_root_id = B;
			return;
		}

		// otherwise, we rebalanced whatever was the
		// child (left or right) of F.
		//if (A == F.left_ptr)
		if (n1.id == n3.left)
		{
			//F.left_ptr = B;
			n3.left = B;

			AVL::left_child_pointer_change_count++;
			write_node_to_file(n3);
			return;
		}
		//if (A == F.right_ptr)
		if (n1.id == n3.right)
		{
			//F.right_ptr = B;
			n3.right = B;

			AVL::right_child_pointer_change_count++;
			write_node_to_file(n3);
			return;
		}

		// We should never get here
		char waitChar;
		cout << "\n\nSomething went wrong, you shouldn't be here...\nPress ENTER to exit.";
		cin.get(waitChar);
		exit(1);

		// End of AVL_Insert()
	}
}



void AVL::AVL_Traverse()
{
	// This is just a public method to call the recursive traverse method

	// Save start time
	clock_t START_TIME = clock();

	AVL::traverse(AVL_root_id, 1);

	// Finished traversing the tree

	// Calculate elapsed time in miliseconds.
	unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

	// Calculate elapsed time in seconds with milisecond precision
	AVL::traverse_time = ((float)ELAPSED_TIME) / 1000;
}


void AVL::AVL_Close()
{
	// closes the avl tree fstream
	file.close();

	// Reset tree parameters
	AVL_root_id = 0;
	id_count = 1;
	key_comparisons_count = 0;
	left_child_pointer_change_count = 0;
	right_child_pointer_change_count = 0;
	Balance_Factor_change_count = 0;
	distinct_words_count = 0;
	total_words_count = 0;
	tree_height = 0;
	disk_read_count = 0;
	disk_write_count = 0;
	traverse_time = 0;

	// Reset nodes n1, n2, and n3
	n1.id = n2.id = n3.id = 0;
	strcpy_s(n1.key, "");
	strcpy_s(n2.key, "");
	strcpy_s(n3.key, "");
	n1.count = n2.count = n3.count = 1;
	n1.BF = n2.BF = n3.BF = 0;
	n1.left = n2.left = n3.left = 0;
	n1.right = n2.right = n3.right = 0;
}

void AVL::AVL_New()
{
	// Same as AVL constructor

	AVL_root_id = 0;

	if (file.is_open()) return;

	// else, make a new file (if needed) and open it.

	// if file does not exist, create it
	// if file does exist, do nothing with it.
	ofstream tree_file;
	tree_file.open("trees\\avl.tree", ios::binary);
	if (file.fail())
	{
		cout << "\nSomething went wrong :(\n\nUnable to open ofstream trees\\avl.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
	tree_file.close();

	// open the tree file stream to read and write in binary
	file.open("trees\\avl.tree", ios::binary | ios::in | ios::out);

	if (file.fail())
	{
		cout << "\nSomething went wrong :(\n\nUnable to open fstream trees\\avl.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
}


void AVL::AVL_Delete()
{
	// deletes the file just in case a new tree will be created with another text
	if (remove("trees\\avl.tree") != 0) cout << "\nSomething went wrong :(\ntrees\\avl.tree was not deleted successfully\n\n";
}


// Tree Parameters GET methods
int AVL::get_key_comparisons_count() { return AVL::key_comparisons_count; }
int AVL::get_left_child_pointer_change_count() { return AVL::left_child_pointer_change_count; }
int AVL::get_right_child_pointer_change_count() { return AVL::right_child_pointer_change_count; }
int AVL::get_Balance_Factor_change_count() { return AVL::Balance_Factor_change_count; }
int AVL::get_distinct_words_count() { return AVL::distinct_words_count; }
int AVL::get_total_words_count() { return AVL::total_words_count; }
int AVL::get_tree_height() { return AVL::tree_height; }
int AVL::get_disk_read_count(){ return AVL::disk_read_count; }
int AVL::get_disk_write_count(){ return AVL::disk_write_count; }
int AVL::get_node_size(){ return sizeof(AVL_node); }
float AVL::get_traverse_time() { return AVL::traverse_time; }



// --- Helper Functions --- //


void AVL::traverse(int node, int current_height)
{
	// This method performs a recursive in-order tree traversal to calculate 
	// the number of distinct words, total words, and the tree's height.
	// If the tree is empty, return.

	if (node == 0) return;

	n1 = read_node_from_file(node);
	AVL::disk_read_count--;	// don't count this read

	AVL_node current_node = n1;

	// Go left
	//AVL::traverse(node.left_ptr, current_height++);
	AVL::traverse(n1.left, current_height + 1);

	// n1 might not be the same after that last recursive call
	if (n1.id != current_node.id)
	{
		n1 = read_node_from_file(node);
		AVL::disk_read_count--;	// don't count this read
	}
	

	// Count this node
	AVL::distinct_words_count++;

	//AVL::total_words_count += node->count;
	AVL::total_words_count += n1.count;

	if (current_height > AVL::tree_height) AVL::tree_height = current_height;

	// Go right
	//AVL::traverse(node->right_ptr, current_height++);
	AVL::traverse(n1.right, current_height + 1);
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


void AVL::write_node_to_file(AVL_node node)
{
	// Takes a node and writes its contents to the tree file.
	// If that node already exists, overwrite its contents with the new data.

	file.seekp(sizeof(node)*(node.id - 1));	// seek to location in the file where node is located
											// if node is not in file, node is appended to the end

	file.write((char*)&node, sizeof(node));	// writes node binary data to buffer

	file.flush();							// forces buffer to write to disk

	AVL::disk_write_count++;
}


AVL::AVL_node AVL::read_node_from_file(int id)
{
	// Given a node I.D., return that node's contents from the tree file.
	// If node I.D. doesn't exist, return NULL node.

	AVL_node node;							// Empty node we'll fill with what is read in.
	node.id = id;

	// If asked to return the NULL node
	if (id == 0)
	{
		strcpy_s(node.key, "");
		node.count = 1;
		node.BF = 0;
		node.left = 0;
		node.right = 0;
		return node;
	}
	
	file.seekg(sizeof(node)*(id - 1));		// Seek to location in the file where the node we want to read is located.
											// If node is not in file, node is appended to the end.

	file.read((char*)&node, sizeof(node));	// Read in the node data and place it in our node variable.

	AVL::disk_read_count++;

	return node;
}