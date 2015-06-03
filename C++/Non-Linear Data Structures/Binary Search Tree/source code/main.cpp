/*
Emilio Borges - Feb. 17, 2015
EECS 2510 - Non-Linear Data Structures
Project 1 - Binary Search Tree (BST)
*/

#include "BST.h"

using namespace std;

BST tree;				// creates an empty binary search tree by running its constructor
string command, argument;

void getCommand();		// gets user's command and converts to lowercase
void executeCommand();	// calls the appropriate BST method (using command) with appropriate argument


// program starts here in main()
int main()
{
	do{
		getCommand();
		executeCommand();
	} while (true);

	return 0;
}


// Asks the user for a command and converts that command to a lowercase string
void getCommand(){

	cout << "\nCommand: ";
	cin >> command;

	// goes through each input_command character and converts it to lowercase
	for (unsigned int i = 0; i < command.length(); i++) {
		command[i] = tolower(command[i]);
	}
}

// Determins if the command is known and runs the appropriate function
void executeCommand(){

	// insert
	if (command == "insert") {
		cin >> argument;
		tree.insert(argument);
	}

	// delete 
	else if (command == "delete"){
		cin >> argument;
		tree.remove(argument);
	}

	// search
	else if (command == "search"){
		cin >> argument;
		tree.search(argument);
	}

	// min
	else if (command == "min"){
		tree.min();
	}

	// max
	else if (command == "max"){
		tree.max();
	}

	// next
	else if (command == "next"){
		cin >> argument;
		tree.next(argument);
	}

	// prev
	else if (command == "prev"){
		cin >> argument;
		tree.prev(argument);
	}

	// list
	else if (command == "list"){
		tree.list();
	}

	// help
	else if (command == "help"){
		cout << "\nList of available commands:\n"
				<< "\tinsert <string>\n"
				<< "\tdelete <string>\n"
				<< "\tsearch <string>\n"
				<< "\tmin\n"
				<< "\tmax\n"
				<< "\tnext <string>\n"
				<< "\tprev <string>\n"
				<< "\tlist\n"
				<< "\thelp\n"
				<< "\texit\n" << endl;
	}

	// exit
	else if (command == "exit") exit(0);

	// invalid command
	else {
		cout << "'" << command << "' is not a command. Type 'help' for a list of available commands.\n" << endl;
	}
}
