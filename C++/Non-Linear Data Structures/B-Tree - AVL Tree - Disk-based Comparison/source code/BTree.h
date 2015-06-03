/*
	Filename:		BTree.h
	Author:			Emilio Borges
	Course:			EECS 2510 - Non-Linear Data Structures
	Project:		B-Trees - AVL Tree - Disk-based Comparison
	Date:			April 25, 2015
	Description:	Header file for BTree.cpp
					This is where t is definied.
					This includes the public methods to create and delete a BTree .tree file, insert and traverse
					the BTree tree, and get calculated tree parameters. Has private file stream, BTree_node template,
					key_container template, root pointer, tree parameters, and helper methods (read from/write to file, 
					traverse, compare).

*/

#ifndef BTree_H	// start of include guard
#define BTree_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

class BTree
{
	public:
		BTree();
		void BTree_Insert(const std::string &word);
		void BTree_Traverse();
		void BTree_Close();		// Closes the btree fstream and resets values/parameters for a new tree.
		void BTree_New();		// Same as constructor, used when running multiple texts.
		void BTree_Delete();	// Deletes the b.tree file

		// Tree Parameters GET methods
		int get_key_comparisons_count();
		int get_distinct_words_count();
		int get_total_words_count();
		int get_tree_height();
		int get_node_count();
		int get_keys_per_node_value();
		int get_disk_read_count();
		int get_disk_write_count();
		int get_node_size();
		float get_traverse_time();

	private:

		std::fstream file;

		static const int t = 2;
		static const int keys_per_node = 2 * t - 1;

		struct key_container
		{
			// Within each BTree_node will be (2*t - 1) of these key_containers

			char key[50];			// Max word length is 50 chars.
			int count = 1;
			int right = 0;			// Right node "pointer". value of 0 == NULL since node IDs start at 1.

			// Constructor
			key_container()
			{
				strcpy_s(key, "");
			}
		};	
		
		const key_container empty_container;	// empty_container will make it easier to clear out containers

		struct BTree_node
		{
			// B-Tree node template

			int id = 0;									// Holds this node's I.D. number. An I.D. of 0 is a NULL node

			int empty_containers = keys_per_node;		// Keeps count of the number of key_containers that are empty in this node.

			key_container key_container[keys_per_node];	// Each array slot will hold a key, count, and right "pointer".

			int left = 0;								// Left node "pointer". value of 0 == NULL since node IDs start at 1.
														// This is the left-most path we can take since each key_container holds a right path.
			
			bool leaf = true;							// true if this node is a leaf, false if this node is an internal node or root.

			BTree_node()
			{
				// Every key container starts out with an empty key
				for (int i = 0; i < keys_per_node; i++) strcpy_s(key_container[i].key, "");
			}
		} n1, n2, n3;						// The three nodes we're allowed to keep in memory.

		const BTree_node empty_node;	//	empty_node will make it easier to clear out nodes 

		int BTree_root_id;				// Holds the I.D. number of the root node.

		int id_count = 1;				// Any new node's id will be this.
										// After a new node is created, id_count is incremented.

		// Tree Parameters
		int key_comparisons_count = 0;
		int distinct_words_count = 0;
		int total_words_count = 0;
		int tree_height = 0;
		int node_count = 0;
		int disk_read_count = 0;
		int disk_write_count = 0;
		float traverse_time = 0;


		// --- Helper Functions --- //

		void traverse(int root, int current_height);	// Performs an in-order tree traversal to calculate the number
														// of distinct words, total words, and the tree's height.

		int compare_strings(const std::string &string1, const std::string &string2);	// Given two strings, returns:
																						//-1 if string1 < string2
																						// 0 if string1 == string2
																						// 1 if string1 > string2

		void write_node_to_file(BTree_node node);		// Takes a node and writes its contents to the tree file.
													// If that node already exists, overwrite its contents with 
													// the new data.

		BTree_node read_node_from_file(int id);	// Given a node I.D., return that node's contents from the tree file.
												// If node I.D. doesn't exist, return NULL node.
};

#endif	// end of include guard
// No other code should go after this line