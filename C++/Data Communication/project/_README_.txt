Author:	 Emilio Borges

Date:	 Oct. 8, 2015

Class:	 EECS 3150 Data Communications

Project: Socket Tx/Rx




THIS PROJECT ONLY WORKS ON WINDOWS 




To test the project on your machine:

	
	1. Hold the [Shift] key and right-click the \Debug folder

	2. Click on the "Open command window here" option

		- a Command Line Interface (CLI) should appear

	3. Repeat steps 1 and 2 to open another CLI

	4. In one CLI, type "receiver.exe 3000 output.txt" and press the [ENTER] key

	5. On the other CLI, type "transmitter.exe localhost 3000 test.txt" and press the [ENTER] key

	6. Both programs should have run successfully

	7. Go into the \Debug folder and open the test.txt and output.txt file

		- both files should be identical






To open the project solution, run VisualStudio and open the .sln file

Once opened in VisualStudio, you can compile the source code to generate your own executables.








[Folders]

	\common
		- contains library source code used throughout the project

			- txrx.h

			- txrx.cpp



	\Debug
		- contains compiled code (.exe, .lib) and sample .txt files

		- receiver.exe
		- transmitter.exe

		- common.lib



	\receiver
		- contains receiver source code

			- main.cpp



	\transmitter
 		- contains transmitter source code

			- main.cpp

			- transmitter.h

			- transmitter.cpp
