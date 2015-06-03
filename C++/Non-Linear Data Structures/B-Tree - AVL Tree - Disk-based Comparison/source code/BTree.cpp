/*
	Filename:		BTree.cpp
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		B-Trees - AVL Tree - Disk-based Comparison
	Date:			April 25, 2015
	Description:	Here's where the actual work gets done for the BTree. All public and private
					methods are defined here and all tree parameters are calculated here.

*/

#include "BTree.h"

using namespace std;

BTree::BTree()
{
	// BTree constructor

	BTree_root_id = 0;

	// if file does not exist, create it.
	// if file does exist, do nothing with it.
	ofstream tree_file;
	tree_file.open("trees\\b.tree", ios::binary);
	if (file.fail())
	{
		cout << "Something went wrong :(\n\nUnable to open ofstream trees\\b.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
	tree_file.close();

	// open the tree file stream to read and write in binary
	file.open("trees\\b.tree", ios::binary | ios::in | ios::out);

	if (file.fail())
	{
		cout << "Something went wrong :(\n\nUnable to open fstream trees\\b.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
}


void BTree::BTree_Insert(const string &word)
{
	// This method creates and adds to a BTree.
	// If <word> is already IN the tree, increment the key's count.
	// If <word> is NOT in the tree, add it to the tree.

	// For this project, we can only have 3 nodes in memory at one time. That's 
	// what n1, n2, and n3 are for. n1 will traverse the tree finding the insertion
	// point. If n1 needs to split up, n2 and n3 will each contain half of n1 and n1
	// will only contain its middle key container before the split.

	// Reset nodes n1, n2, and n3
	n1 = n2 = n3 = empty_node;

	// If the tree is empty, make a root node with <word> in one of the key slots and save it to disk
	if (BTree_root_id == 0)
	{
		n1.id = id_count++;
		BTree_root_id = n1.id;
		strcpy_s(n1.key_container[0].key, word.c_str());
		n1.empty_containers--;
		write_node_to_file(n1);
		return;
	}

	// Start at the root.
	n1 = read_node_from_file(BTree_root_id);

	// if root is a leaf node
	if (n1.leaf)
	{
		// Insert <word> into this node and rearrange they key_containers ASCII-ly
		// If the root fills up, split it up into two nodes with a new root for both nodes.

		// i = insertion_index in node's key_container[] array (zero indexed)
		for (int i = 0; n1.empty_containers > 0 && i <= keys_per_node - n1.empty_containers - 1; i++)
		{
			switch (compare_strings(word, n1.key_container[i].key))
			{
				// <word> goes before this key
				case -1:	for (int j = keys_per_node - n1.empty_containers; j > 0 && j > i; j--) 
							{
								// j starts at the first empty node available and moves backwards (towards array[0])
								// this loop moves the appropriate key containers over one spot
								n1.key_container[j] = n1.key_container[j - 1];
							}
							n1.key_container[i] = empty_container;
							strcpy_s(n1.key_container[i].key, word.c_str());	// put <word> in container
							n1.empty_containers--;
							write_node_to_file(n1);
							return;

				// <word> was found to already be in the tree. Increment its count and return.
				case 0:		n1.key_container[i].count++;
							write_node_to_file(n1);
							return;

				// <word> goes after this key
				case 1:		if (i + 1 > keys_per_node - n1.empty_containers - 1) // if the next key_container is empty 
							{
								n1.key_container[i + 1] = empty_container;
								strcpy_s(n1.key_container[i + 1].key, word.c_str());	// put <word> in container
								n1.empty_containers--;
								write_node_to_file(n1);
								return;
							}
							break;
			}
		}

		// if node is full
		if (n1.empty_containers == 0)
		{
			// n2 will be the new root/parent holding only n1's middle key_container and 
			// n3 will take the second half of n1's key containers

			// "create" the two new nodes and set the tree's new root to be n2
			n2.id = id_count++;
			BTree_root_id = n2.id;
			n2.leaf = false;
			n3.id = id_count++;

			// put n1's middle key into n2 and clear/reset n1's middle key
			n2.key_container[0] = n1.key_container[keys_per_node/2];
			n2.empty_containers--;
			n1.key_container[keys_per_node / 2] = empty_container;
			n1.empty_containers++;

			// Set the new root's left and right pointers
			n2.left = n1.id;
			n2.key_container[0].right = n3.id;

			// Cut the second haf of n1's key containers and paste them into the first half of n3.
			// i will be n1's key_container[] index and j will be n3's
			for (int i = (keys_per_node / 2) + 1, j = 0; i < keys_per_node; i++, j++)
			{
				n3.key_container[j] = n1.key_container[i];
				n3.empty_containers--;
				n1.key_container[i] = empty_container;
				n1.empty_containers++;
			}

			// Add <word> into the tree
			// This is brute forced, pay no attention to the mess...
			switch (compare_strings(word, n2.key_container[0].key))
			{
				// if <word> goes before n2[0]
				case -1:	switch (compare_strings(word, n1.key_container[0].key))
							{
								// if <word> goes before n1[0]; n1 is n2's left node
								case -1:	n1.key_container[1] = n1.key_container[0];
											n1.key_container[0] = empty_container;
											strcpy_s(n1.key_container[0].key, word.c_str());
											n1.empty_containers--;
											break;

								// if <word> == n1[0]
								case 0:		n1.key_container[0].count++;
											break;

								// if <word> goes after n1[0]
								case 1:		strcpy_s(n1.key_container[1].key, word.c_str());
											n1.empty_containers--;
											break;
							}
							break;

				// if <word> == n2[0]
				case 0:		n2.key_container[0].count++;
							break;

				// if <word> goes after n2[0]
				case 1:		switch (compare_strings(word, n3.key_container[0].key))
							{
								// if <word> goes before n1[0]; n3 is n2's right node
								case -1:	n3.key_container[1] = n3.key_container[0];
											n3.key_container[0] = empty_container;
											strcpy_s(n3.key_container[0].key, word.c_str());
											n3.empty_containers--;
											break;

								// if <word> == n1[0]
								case 0:		n3.key_container[0].count++;
											break;

								// if <word> goes after n1[0]
								case 1:		strcpy_s(n3.key_container[1].key, word.c_str());
											n3.empty_containers--;
											break;
							}
							break;
			}

			// Save nodes to file
			write_node_to_file(n1);
			write_node_to_file(n2);
			write_node_to_file(n3);

			return;	// We inserted <word> into the tree and split the node, so return to get next word.
		}

		write_node_to_file(n1);

		return; // We inserted <word> into the tree but didn't have to split the node, so return to get next word.
	}

	// Root now has children
	// Traverse the tree until insertion point is found, split nodes if needed.
	n2 = n1;	// n2 will lag behind n1 as its parent
	
	// find insertion point inside this node, go left or right if needed
	for (int i = 0; i < keys_per_node - n1.empty_containers;)
	{
		// if n1 is full, split it
		if (n1.empty_containers == 0)
		{
			// Are we at the root?
			if (n1.id == BTree_root_id)
			{
				// n2 will be the new root/parent holding only n1's middle key_container and 
				// n3 will take the second half of n1's key containers

				// Reset nodes n2 and n3
				n2 = n3 = empty_node;

				// "create" the two new nodes and set the tree's new root to be n2
				n2.id = id_count++;
				BTree_root_id = n2.id;
				n2.leaf = false;
				n3.id = id_count++;
				n3.leaf = n1.leaf;

				// put n1's middle key into n2 and clear/reset n1's middle key
				n2.key_container[0] = n1.key_container[keys_per_node / 2];
				n2.empty_containers--;
				n3.left = n1.key_container[keys_per_node / 2].right;	// set n3's left pointer
				n1.key_container[keys_per_node / 2] = empty_container;
				n1.empty_containers++;

				// Set the new root's left and right pointers
				n2.left = n1.id;
				n2.key_container[0].right = n3.id;

				// Cut the second haf of n1's key containers and paste them into the first half of n3.
				// i will be n1's key_container[] index and j will be n3's
				for (int i = (keys_per_node / 2) + 1, j = 0; i < keys_per_node; i++, j++)
				{
					n3.key_container[j] = n1.key_container[i];
					n3.empty_containers--;
					n1.key_container[i] = empty_container;
					n1.empty_containers++;
				}

				// Save nodes to file
				write_node_to_file(n1);
				write_node_to_file(n2);
				write_node_to_file(n3);

				n1 = n2; // for use inside the switch()
				i = 0;
			}
			else // n1 is not the root
			{
				// n2 is n1's parent and n1 is full; split n1
				// n2 is guaranteed to not be full

				// if n2 is not n1's parent, start from root again
				// we might be trying to split a node and n2 HAS TO BE
				// n1's parent
				if (n1.id == n2.id) 
				{
					n1 = n2 = read_node_from_file(BTree_root_id);
					i = 0;
					continue; // start from beginning of for() loop
				}

				// reset n3 and make a new node out of it
				n3 = empty_node;
				n3.id = id_count++;
				n3.leaf = n1.leaf;
				n3.left = n1.key_container[keys_per_node / 2].right;

				// Cut the second haf of n1's key containers and paste them into the first half of n3.
				// i will be n1's key_container[] index and j will be n3's
				for (int i = (keys_per_node / 2) + 1, j = 0; i < keys_per_node; i++, j++)
				{
					n3.key_container[j] = n1.key_container[i];
					n3.empty_containers--;
					n1.key_container[i] = empty_container;
					n1.empty_containers++;
				}

				// Move n1's middle key container into n2
				int j = keys_per_node - n2.empty_containers;
				for (; j > 0 && compare_strings(n1.key_container[keys_per_node / 2].key, n2.key_container[j - 1].key) == -1; j--)
				{
					// j starts at the first empty node available in n2 and moves backwards (towards array[0])
					// this loop moves the appropriate key containers over one spot to make room for the new one
					n2.key_container[j] = n2.key_container[j - 1];
				}
				n2.key_container[j] = n1.key_container[keys_per_node / 2];
				n2.empty_containers--;
				n2.key_container[j].right = n3.id;
				n1.key_container[keys_per_node / 2] = empty_container;
				n1.empty_containers++;

				// Add <word> into the tree
				// This is brute forced, pay no attention to the mess...
				//switch (compare_strings(word, n2.key_container[0].key))
				//{
				//	// if <word> goes before n2[0]
				//	case -1:	switch (compare_strings(word, n1.key_container[0].key))
				//				{
				//					// if <word> goes before n1[0]; n1 is n2's left node
				//					case -1:	n1.key_container[1] = n1.key_container[0];
				//								n1.key_container[0] = empty_container;
				//								strcpy_s(n1.key_container[0].key, word.c_str());
				//								n1.empty_containers--;
				//								break;

				//					// if <word> == n1[0]
				//					case 0:		n1.key_container[0].count++;
				//								break;

				//					// if <word> goes after n1[0]
				//					case 1:		strcpy_s(n1.key_container[1].key, word.c_str());
				//								n1.empty_containers--;
				//								break;
				//				}
				//				break;

				//	// if <word> == n2[0]
				//	case 0:		n2.key_container[0].count++;
				//				break;

				//	// if <word> goes after n2[0]
				//	case 1:		switch (compare_strings(word, n3.key_container[0].key))
				//				{
				//					// if <word> goes before n1[0]; n3 is n2's right node
				//					case -1:	n3.key_container[1] = n3.key_container[0];
				//								n3.key_container[0] = empty_container;
				//								strcpy_s(n3.key_container[0].key, word.c_str());
				//								n3.empty_containers--;
				//								break;

				//					// if <word> == n1[0]
				//					case 0:		n3.key_container[0].count++;
				//								break;

				//					// if <word> goes after n1[0]
				//					case 1:		strcpy_s(n3.key_container[1].key, word.c_str());
				//								n3.empty_containers--;
				//								break;
				//				}
				//				break;
				//}

				// Save nodes to file
				write_node_to_file(n1);
				write_node_to_file(n2);
				write_node_to_file(n3);

				n1 = n2; // for use inside the switch()
				i = 0;
			}
		}

		switch (compare_strings(word, n1.key_container[i].key))
		{
			// <word> goes before this key
			case -1:	if (i == 0 && !n1.leaf)
						{
							n2 = n1;
							n1 = read_node_from_file(n1.left);
							continue;
						}
						else if (i > 0 && !n1.leaf)
						{
							n2 = n1;
							n1 = read_node_from_file(n1.key_container[i - 1].right);
							i = 0;
							continue;
						}
						else // we're at a leaf node
						{
							// we have to insert <word> into this node
							for (int j = keys_per_node - n1.empty_containers; j > 0 && j > i; j--)
							{
								// j starts at the first empty node available and moves backwards (towards array[0])
								// this loop moves the appropriate key containers over one spot
								n1.key_container[j] = n1.key_container[j - 1];
							}
							n1.key_container[i] = empty_container;
							strcpy_s(n1.key_container[i].key, word.c_str());	// put <word> in container
							n1.empty_containers--;
							write_node_to_file(n1);
							return;
						}
						break;

			// <word> was found to already be in the tree. Increment its count and return.
			case 0:		n1.key_container[i].count++;
						write_node_to_file(n1);
						return;

			// <word> goes after this key
			case 1:		if (i == (keys_per_node - n1.empty_containers - 1) && !n1.leaf)
						{
							// if we're at the last non-empty key container in an internal node
							n2 = n1;
							n1 = read_node_from_file(n1.key_container[i].right);
							i = 0;
							continue;
						}
						else if (i == (keys_per_node - n1.empty_containers - 1) && n1.leaf)
						{
							// if we're at the last non-empty key container in a leaf node
							// we have to insert <word> into this node
							n1.key_container[i + 1] = empty_container;
							strcpy_s(n1.key_container[i + 1].key, word.c_str());	// put <word> in container
							n1.empty_containers--;
							write_node_to_file(n1);
							return;
						}
						else i++;
						break;
		}
	}
}


void BTree::BTree_Traverse()
{
	// This is just a public method to call the recursive traverse method

	// Reset n1, n2, and n3
	n1 = n2 = n3 = empty_node;

	// Save start time
	clock_t START_TIME = clock();

	BTree::traverse(BTree_root_id, 1);

	// Finished traversing the tree

	// Calculate elapsed time in miliseconds.
	unsigned ELAPSED_TIME = (clock() - START_TIME) / (CLOCKS_PER_SEC / 1000);

	// Calculate elapsed time in seconds with milisecond precision
	BTree::traverse_time = ((float)ELAPSED_TIME) / 1000;
}


void BTree::BTree_Close()
{
	// closes the avl tree fstream
	file.close();

	// Reset tree parameters
	BTree_root_id = 0;
	id_count = 1;
	key_comparisons_count = 0;
	distinct_words_count = 0;
	total_words_count = 0;
	tree_height = 0;
	node_count = 0;
	disk_read_count = 0;
	disk_write_count = 0;
	traverse_time = 0;

	// Reset nodes root_n1, n2, and n3
	n1.id = n2.id = n3.id = 0;
	n1.left = n2.left = n3.left = 0;
	n1.empty_containers = n2.empty_containers = n3.empty_containers = keys_per_node;
	n1.leaf = n2.leaf = n3.leaf = true;
	for (int i = 0; i < keys_per_node; i++)
	{
		strcpy_s(n1.key_container[i].key, "");
		strcpy_s(n2.key_container[i].key, "");
		strcpy_s(n3.key_container[i].key, "");

		n1.key_container[i].count = n2.key_container[i].count = n3.key_container[i].count = 1;
		n1.key_container[i].right = n2.key_container[i].right = n3.key_container[i].right = 0;
	}
}

void BTree::BTree_New()
{
	// Same as AVL constructor

	BTree_root_id = 0;

	if (file.is_open()) return;

	// else, make a new file (if needed) and open it.

	// if file does not exist, create it
	// if file does exist, do nothing with it.
	ofstream tree_file;
	tree_file.open("trees\\b.tree", ios::binary);
	if (file.fail())
	{
		cout << "\nSomething went wrong :(\n\nUnable to open ofstream trees\\b.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
	tree_file.close();

	// open the tree file stream to read and write in binary
	file.open("trees\\b.tree", ios::binary | ios::in | ios::out);

	if (file.fail())
	{
		cout << "\nSomething went wrong :(\n\nUnable to open fstream trees\\b.tree\n\nPress ENTER to exit.";
		char waitChar;
		cin >> waitChar;
		exit(1);
	}
}


void BTree::BTree_Delete()
{
	// deletes the file just in case a new tree will be created with another text
	if (remove("trees\\b.tree") != 0) cout << "\nSomething went wrong :(\ntrees\\b.tree was not deleted successfully\n\n";
}


// Tree Parameters GET methods
int BTree::get_key_comparisons_count() { return BTree::key_comparisons_count; }
int BTree::get_distinct_words_count() { return BTree::distinct_words_count; }
int BTree::get_total_words_count() { return BTree::total_words_count; }
int BTree::get_tree_height() { return BTree::tree_height; }
int BTree::get_node_count() { return BTree::node_count; }
int BTree::get_keys_per_node_value(){ return BTree::keys_per_node; }
int BTree::get_disk_read_count(){ return BTree::disk_read_count; }
int BTree::get_disk_write_count(){ return BTree::disk_write_count; }
int BTree::get_node_size(){ return sizeof(BTree_node); }
float BTree::get_traverse_time() { return BTree::traverse_time; }



// --- Helper Functions --- //


void BTree::traverse(int node_id, int current_height)
{
	// This method performs a recursive in-order tree traversal to calculate 
	// the number of distinct words, total words, node count, and the tree's height.
	// If the tree is empty, return.

	if (node_id == 0) return;

	n1 = read_node_from_file(node_id);
	BTree::disk_read_count--;	// don't count this read

	BTree_node current_node = n1;

	// Go left 
	BTree::traverse(n1.left, current_height + 1);

	// n1 might not be the same after that last recursive call
	if (n1.id != current_node.id) 
	{
		n1 = read_node_from_file(node_id);
		BTree::disk_read_count--;	// don't count this read
	}

	// Count this node
	BTree::node_count++;

	// Get tree height
	if (current_height > BTree::tree_height) BTree::tree_height = current_height;

	for (int i = 0; i < keys_per_node - n1.empty_containers; i++)
	{
		// Get distinct words and total words
		BTree::distinct_words_count++;
		BTree::total_words_count += n1.key_container[i].count;
		BTree::traverse(n1.key_container[i].right, current_height + 1);
		// n1 might not be the same after that last recursive call
		if (n1.id != current_node.id)
		{
			n1 = read_node_from_file(node_id);
			BTree::disk_read_count--;	// don't count this read
		}
	}
}


int BTree::compare_strings(const string &string1, const string &string2)
{
	// Takes two strings as inputs.
	// Compares both strings character by character.
	// returns -1 if string1 < string2
	// returns  0 if string1 == string2
	// returns  1 if string1 > string2

	BTree::key_comparisons_count++;
	if (string1 < string2) return -1;
	BTree::key_comparisons_count++;
	if (string1 > string2) return 1;
	else return 0;
}


void BTree::write_node_to_file(BTree_node node)
{
	// Takes a node and writes its contents to the tree file.
	// If that node already exists, overwrite its contents with the new data.

	file.seekp(sizeof(node)*(node.id - 1));	// seek to location in the file where node is located
											// if node is not in file, node is appended to the end

	file.write((char*)&node, sizeof(node));	// writes node binary data to buffer

	file.flush();							// forces buffer to write to disk

	BTree::disk_write_count++;
}


BTree::BTree_node BTree::read_node_from_file(int id)
{
	// Given a node I.D., return that node's contents from the tree file.
	// If node I.D. doesn't exist, return NULL node.

	BTree_node node;	// Empty node we'll fill with what is read in.
	node.id = id;

	// If asked to return the NULL node
	/*if (id == 0)
	{
		strcpy_s(node.key, "");
		node.count = 1;
		node.left = 0;
		node.right = 0;
		return node;
	}*/

	file.seekg(sizeof(node)*(id - 1));		// Seek to location in the file where the node we want to read is located.
											// If node is not in file, node is appended to the end.

	file.read((char*)&node, sizeof(node));	// Read in the node data and place it in our node variable.

	BTree::disk_read_count++;

	return node;
}