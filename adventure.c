// Michael Childress
// CS 344 Program 2 - Adventure
// adventure.c

/*
 * NAME
 *  childrem.adventure - tracks and displays a user's progress through a randomly generated maze of rooms
 *SYNOPSIS
 *  childrem.adventure
 *DESCRIPTION
 *  Reads in room data that was generated by childrem.buildrooms and then prompts the user as they move through the maze looking for the
 *  END_ROOM. If the user types in time, the current time is sent to a file called currentTime.txt and then is displayed on the screen.
 *  Once the END_ROOM is reached, winning dialog is displayed along with the player's chosen path to get there.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define TRUE	1
#define FALSE	0


pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;	// Global mutex so it can be easily used by more than one function


struct room
{
	char* roomName;
	char* roomType;
	struct room* outboundConnections[6];
	char* connectionNames[6];		// We first have to read in the names of connections as strings before actually connecting
	int numConnections;
};



/*
 *NAME
 *  createRoom - read in data from a file to construct a room struct
 *SYNOPSIS
 *  createRoom(&struct_room, string)
 *DESCRIPTION
 *  Will read the data from the given file and place it into the appropriate spot of the struct. These structs are the rooms in the game.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void createRoom(struct room* roomToCreate, char* filePath) {

	// Open a stream to access the required file
	
	int numberOfConnections = 0;		// Will increment as we read in lines containing the word "CONNECTION"

	char lineFromFile[256];
	memset(lineFromFile, '\0', sizeof(lineFromFile));

	char importantData[256];		// Will store the string we're actually going to store in the struct
	memset(importantData, '\0', sizeof(importantData));


	FILE* fileToRead = fopen(filePath, "r");

	if(fileToRead == NULL) {

		printf("There was an error opening the file!\n");
		exit(1);

	}

	// File opened so start reading lines
	
	while (fgets(lineFromFile, 256, fileToRead) != NULL) {		// Stop when end of file reached

		// Need to determine which type of data this is. Look for keyword on line

		if(strstr(lineFromFile, "ROOM NAME") != NULL) {

			memset(importantData, '\0', sizeof(importantData));
			sscanf(lineFromFile, "%*s %*s %s", importantData);	// Only interested in 3rd item on the line

			roomToCreate->roomName = malloc(50*sizeof(char));
			sprintf(roomToCreate->roomName, "%s", importantData);

		}

		else if (strstr(lineFromFile, "CONNECTION") != NULL) {

			memset(importantData, '\0', sizeof(importantData));
			sscanf(lineFromFile, "%*s %*s %s", importantData);



			roomToCreate->connectionNames[numberOfConnections] = malloc(50*sizeof(char));
			sprintf(roomToCreate->connectionNames[numberOfConnections], "%s", importantData);	// Store connection name as string for now


			numberOfConnections++;

		}

		else if (strstr(lineFromFile, "ROOM TYPE") != NULL) {

			memset(importantData, '\0', sizeof(importantData));
			sscanf(lineFromFile, "%*s %*s %s", importantData);


			roomToCreate->roomType = malloc(50*sizeof(char));
			sprintf(roomToCreate->roomType, "%s", importantData);

		}



	}


	roomToCreate->numConnections = numberOfConnections;	
	
	fclose(fileToRead);	
}


/*
 *NAME
 *  start_routine - starting location for second thread execution
 *SYNOPSIS
 *  start_routine(NULL)
 *DESCRIPTION
 *  This will be the function run exclusively by the second thread. It gets the current time, puts it in a string, then sends the string to a file
 *  called currentTime.txt, and displays the string on the screen. It then hands control back to the main thread and awaits its next call.
 *  The process runs in an infinite loop and persists until the main thread exits.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/

void* start_routine(void* argument) {

	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	// Help with creating a time string obtained from https://www.geeksforgeeks.org/strftime-function-in-c/

	time_t t;
	struct tm* timeStruct;
	char timeString[1000];

	int file_descriptor;


	pthread_mutex_lock(&myMutex);	// Wait here until asked to start


	// Write time to the time file

	memset(timeString, '\0', sizeof(timeString));

	time(&t);
	timeStruct = localtime(&t);

	strftime(timeString, sizeof(timeString), "%l:%M%P, %A, %B %d, %Y\n", timeStruct);


	file_descriptor = open("./currentTime.txt", O_RDWR | O_CREAT | O_TRUNC, 777);

	write(file_descriptor, timeString, strlen(timeString) * sizeof(char));

	close(file_descriptor);	

	

	pthread_mutex_unlock(&myMutex);	// Send control back to the main thread


	return NULL;
}



/*
 *NAME
 *  playGame - the primary function that runs the adventure
 *SYNOPSIS
 *  playGame(roomArray)
 *DESCRIPTION
 *   This is the primary function that runs the game process. It receives the array of room structs that have been filled with information. It finds the
 *   START_ROOM and makes this the active room to display the information for in the prompt. In a do-while loop, it then continues to prompt the user,
 *   reassign the active room, and display the new room's information until the END_ROOM is found. It also keeps a record of the player's journey through
 *   the rooms and displays the path they took to victory at the end. 
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void playGame(struct room* arrayOfRooms) {

	struct room* activeRoom;	// This will always point to the room the player is currently in

	// First find, the start room
	
	int foundStart = FALSE;

	int scanIndex = 0;	// Will increment as we search for the start room

	while(foundStart == FALSE && scanIndex < 7) {

		if(strcmp(arrayOfRooms[scanIndex].roomType, "START_ROOM") == 0) {
			activeRoom = &arrayOfRooms[scanIndex];
			foundStart = TRUE;
		}

		else {
			scanIndex++;
		}

	}


	char firstLine[255];	// Displays current location
	char secondLine[1000];	// Displays possible moves
	char thirdLine[255];	// Displays user input prompt

	char buildMovesLine[1000];	// Will get concatenated with each possible room move
	

	size_t bufferSize;	// These variables are used for getline()
	char* userResponse;

	int roomNameToCheck;	// Will control the loop that compares user input to the room names
	int foundRoom;		// Bool that will become true if a match was found

	int stepCount = 0;
	char* pathToWin[1000];	// Contains pointers to strings of rooms visited by player
	int numItemsInPath = 0;


	int timeEntered = FALSE;	// Controls whether typing time reprompts the user

	int timeFile_descriptor;
	char timeToDisplay[1000];	// Will be read in from the currentTime.txt file

	pthread_mutex_lock(&myMutex);

	pthread_t myThreadID;
	
	pthread_create(&myThreadID, NULL, start_routine, NULL);


	do {
		// Reset all strings
		memset(firstLine, '\0', sizeof(firstLine));
		memset(secondLine, '\0', sizeof(secondLine));
		memset(thirdLine, '\0', sizeof(thirdLine));
		memset(buildMovesLine, '\0', sizeof(buildMovesLine));


		sprintf(firstLine, "CURRENT LOCATION: %s", activeRoom->roomName);
		printf("%s\n", firstLine);

		for(scanIndex=0; scanIndex < activeRoom->numConnections; scanIndex++) {

			strcat(buildMovesLine, activeRoom->connectionNames[scanIndex]);
			strcat(buildMovesLine, ", ");
		}

		buildMovesLine[strlen(buildMovesLine) - 2] = '.';	// Remove the , at the end of the string and replace with .
		buildMovesLine[strlen(buildMovesLine) - 1] = '\0';

		sprintf(secondLine, "POSSIBLE CONNECTIONS: %s", buildMovesLine);
		printf("%s", secondLine);

		do {

			timeEntered = FALSE;

			sprintf(thirdLine, "\nWHERE TO? >");
			printf("%s", thirdLine);


			// Get user input

			bufferSize = 0;
			userResponse = NULL;

			getline(&userResponse, &bufferSize, stdin);	// automatic malloc performed by getline
			userResponse[strlen(userResponse) - 1] = '\0';	// Get rid of trailing newline

			printf("\n");

			if(strcmp(userResponse, "time") == 0) {		// Hand control over to the second thread

				timeEntered = TRUE;
				pthread_mutex_unlock(&myMutex);
				pthread_join(myThreadID, NULL);		// Wait for second thread to finish			

				pthread_mutex_lock(&myMutex);		// Get command back
				pthread_create(&myThreadID, NULL, start_routine, NULL);		// Recreate thread for next run			
		
				memset(timeToDisplay, '\0', sizeof(timeToDisplay));
				timeFile_descriptor = open("./currentTime.txt", O_RDONLY);
				read(timeFile_descriptor, timeToDisplay, sizeof(timeToDisplay));
				printf("%s", timeToDisplay);
				close(timeFile_descriptor);
			}

			else {	// Check to see if valid input. If yes, switch the active room
		
				foundRoom = FALSE;

				for(roomNameToCheck=0; roomNameToCheck < activeRoom->numConnections; roomNameToCheck++) {

					if(foundRoom == FALSE) {

						if(strcmp(activeRoom->connectionNames[roomNameToCheck], userResponse) == 0) {

							foundRoom = TRUE;
				
							// We know we can move rooms now

							for(scanIndex=0; scanIndex < 7; scanIndex++) {

								if(strcmp(arrayOfRooms[scanIndex].roomName, userResponse) == 0) {

									activeRoom = &arrayOfRooms[scanIndex];


									pathToWin[numItemsInPath] = malloc(20 * sizeof(char));
									sprintf(pathToWin[numItemsInPath], "%s", activeRoom->roomName);
									numItemsInPath++;
			

									stepCount++;
								}

							}

						}
					}

				}

				if(foundRoom == FALSE) {

					printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
				}

			}


			free(userResponse);


		}while(timeEntered == TRUE);	// Ask for input again if user typed "time"


	}while(strcmp(activeRoom->roomType, "END_ROOM") != 0);		// Game stops when user enters the end room

	
	// Display winning dialog here

	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);

	int nameToPrint;

	for(nameToPrint=0; nameToPrint < numItemsInPath; nameToPrint++) {

		printf("%s\n", pathToWin[nameToPrint]);
		free(pathToWin[nameToPrint]);		// Free the memory when we're finished with it
	}


	// Remove the second thread because we no longer need it
	pthread_cancel(myThreadID);
	pthread_join(myThreadID, NULL);
}



/*
 *NAME
 *  freeMemory - free dynamic memory used by room structs
 *SYNOPSIS
 *  freeMemory(arrayOfRooms)
 *DESCRIPTION
 *  This function frees the dynamic memory used by the room structs(strings for type, name, and connections)
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/

void freeMemory(struct room* arrayOfRooms) {

	int index;

	for(index=0; index < 7; index++) {

		free(arrayOfRooms[index].roomName);
		
		int connectionIndex;
		for(connectionIndex=0; connectionIndex < arrayOfRooms[index].numConnections; connectionIndex++) {

			free(arrayOfRooms[index].connectionNames[connectionIndex]);
		}

		free(arrayOfRooms[index].roomType);
	}


}


/*
 *NAME
 *  main - overall control of the program
 *SYNOPSIS
 *  main()
 *DESCRIPTION
 *  Creates a second thread and locks a mutex. Determines the most recently created directory of rooms, loads files one at a time and sets the contents 
 *  of the room structs accordingly. Calls the playGame function, and finally frees dynamic memory.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/



int main() {
	
	// Check for latest version of the rooms directory (adapted from code by Benjamin Brewster in 2.4 Manipulating Directories Document)

	int mostRecentDirectoryTime = -1;			// Will eventually contain the time newest directory was made
	char targetDirectoryPrefix[50] = "childrem.rooms.";	// Look at all folders with room files in them
	char newestDirectoryName[255];				// Directory that will be used
	memset(newestDirectoryName, '\0', sizeof(newestDirectoryName));

	DIR* directoryToCheck;			// Gets assigned a value by opendir()
	struct dirent* fileInDirectory;		// Struct returned by readdir()
	struct stat directoryAttributes;	// Info returned by stat()

	directoryToCheck = opendir(".");	// Start in directory where adventure executable is
	
	if (directoryToCheck > 0) {

		while ((fileInDirectory = readdir(directoryToCheck)) != NULL) {

			if (strstr(fileInDirectory->d_name, targetDirectoryPrefix) != NULL) {

				stat(fileInDirectory->d_name, &directoryAttributes);

				if ((int)directoryAttributes.st_mtime > mostRecentDirectoryTime) {

					mostRecentDirectoryTime = (int)directoryAttributes.st_mtime;
					memset(newestDirectoryName, '\0', sizeof(newestDirectoryName));
					strcpy(newestDirectoryName, fileInDirectory->d_name);

				}

			}

		}

	}

	closedir(directoryToCheck);


	// Read the contents of the files in that directory into 7 room structs (stored in an array)

	struct room roomsInGame[7];
	int index = 0;			// Will increment by one each time a file is read from

		// We now know that newestDirectoryName holds the value of the most recent directory. This is the root of the filepath needed to look at files.

		DIR* directoryToRead;
		struct dirent* fileToRead;
		char fileName[255];		// Will contain the end of the file path (actual file to be read)
		memset(fileName, '\0', sizeof(fileName));

		char fullFilePath[255];		// Will contain directory name + file name for use with open()
		memset(fullFilePath, '\0', sizeof(fullFilePath));

		directoryToRead = opendir(newestDirectoryName);

		if (directoryToRead > 0) {


			while ((fileToRead = readdir(directoryToRead)) != NULL) {		// All files in this directory are relevant
	
				
				memset(fileName, '\0', sizeof(fileName));	// Prepare string for next value

				strcpy(fileName, fileToRead->d_name);		// Lift the actual filename

				memset(fullFilePath, '\0', sizeof(fullFilePath));	// Prepare string for next value

				if (strcmp(fileName, ".") != 0 && strcmp(fileName, "..") != 0) {	// Ignore the root and parent directories


					sprintf(fullFilePath, "%s/%s", newestDirectoryName, fileName);

					createRoom(&roomsInGame[index], fullFilePath);

					index++;
				}
			}

		}

		closedir(directoryToRead);



	// Start playing with read in data
	
	playGame(roomsInGame);

	freeMemory(roomsInGame);	// Free dynamic memory


	return 0;
}
