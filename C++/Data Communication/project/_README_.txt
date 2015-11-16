Author:	 Emilio Borges

Date:	 15 November 2015

Class:	 EECS 3150 Data Communications

Project: Socket Tx/Rx





THIS PROJECT ONLY WORKS ON WINDOWS 








To open the project solution, run Visual Studio and open the .sln file

Once opened in Visual Studio, you can compile the source code to generate your own executables.
If there are any issues, look at the [Known Issues] section below.

Once compiled, the executables will be inside the \Debug folder (if compiled under debug) or the \Production folder (if compiled under production).

Next, move\copy the \texts folder into the \Debug or \Production folder.

To test the project on your machine:

	
	1. Hold the [Shift] key and right-click the \Debug folder
 
		- (or \Production folder if compiled under production)
	2. Click on the "Open command window here" option

		- a Command Line Interface (CLI) should appear

	3. Repeat steps 1 and 2 to open another CLI

	4. In one CLI, type "receiver.exe 3000 output.txt none" and press the [ENTER] key

	5. On the other CLI, type "transmitter.exe localhost 3000 texts\test.txt none 0" and press the [ENTER] key

	6. Both programs should have run successfully

	7. Go into the \Debug folder and open the texts\test.txt and output.txt file

		- both files should be identical

	8. For help on what arguments to use for each executable, type the executable name followed by 'help'
		- e.g. "receiver.exe help" or "transmitter.exe help"
	




[Known Issues while Compiling]

While compiling, the following error appears: "error MSB8020: The build tools for Visual Studio 20xx (Platform Toolset='vxxx') cannot be found. ..." 

This is because visual studio is trying to compile the project with a build tool that is not installed. 

To fix this, in Visual Studio, go to the Solution Explorer and, if you have this project's solution open, you should see three projects inside named "common", "receiver", and "transmitter". For each of these projects, do the following:

	1. Right click on the project and select "Properties" (last option)
	2. On the left panel, under "Configuration Properties" select "General"
	3. Now, on the right panel, look for the setting named "Platform Toolset"
		- edit its value to be for the Visual Studio version you have installed
		- (e.g. "Visual Studio 2013 (v120)")
	4. Do this for all other projects in this solution
	5. The solution should compile now :)










[Folders]

	\common
		- contains library source code used throughout the project

			- txrx.h

			- txrx.cpp


	\receiver
		- contains receiver source code

			- main.cpp



	\texts
		- contains sample text documents used to test the executables
		- after compiling the VS solution, copy/move this folder into the folder containing the executables
			- this will make it easier to locate the sample text files in the command line interface

	\transmitter
 		- contains transmitter source code

			- main.cpp

			- transmitter.h

			- transmitter.cpp
