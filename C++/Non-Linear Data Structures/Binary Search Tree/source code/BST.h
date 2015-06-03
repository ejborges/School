/*
Emilio Borges - Feb. 17, 2015
EECS 2510 - Non-Linear Data Structures
Project 1 - Binary Search Tree (BST)
*/


#ifndef BST_H	// start of include guard
#define BST_H

#include <string>
#include <iostream>



class BST
{
	public:
		BST(); // constructor

		// Dynamic Set Operations
		void insert(std::string const& string);
		void remove(std::string const& string);
		void search(std::string const& string);
		void min();
		void max();
		void next(std::string const& string);
		void prev(std::string const& string);
		void list();

	private:

		// node template
		struct BST_node
		{
			BST_node *parent_ptr = NULL;
			std::string key;
			unsigned int count = 1;
			BST_node *left_ptr = NULL;
			BST_node *right_ptr = NULL;
		};

		// tree's root pointer
		BST_node *BST_root;


		// --- Helper Functions --- //

		// given a string, search the set/tree for a node containing that string and 
		// returns the closest match (if node doesn't exist, returns what would be its parent)
		BST_node* internal_search(std::string const& string);

		// given two strings, returns -1 if string1 is less than string2, 0 if strings are equal,
		// 1 if string1 is greater than string2
		int compare_strings(std::string const& string1, std::string const& string2);

		// given a direction (any negative number will find minimum, 0 or any positive number will find maximum)
		// and starting position node, return its minimum or maximum node
		BST_node* min_max_finder(int direction, BST_node *position);

		// given a direction (any negative number will find predecessor, 0 or any positive number will find successor)
		// and starting position node, return its predecessor or successor node
		BST_node* next_prev_finder(int direction, BST_node *position);
};
#endif	// end of include guard
// No other code should go after this line