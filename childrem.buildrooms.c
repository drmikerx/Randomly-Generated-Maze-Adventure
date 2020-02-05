// Michael Childress
// CS 344 Program 2 - Adventure
// childrem.buildrooms.c

/*
 *NAME
 *  childrem.buildrooms.c - randomly creates and connects a maze of 7 rooms
 *SYNOPSIS
 *  childrem.buildrooms.c
 *DESCRIPTION
 *  Randomly creates and connects a maze of 7 rooms. Each room has a unique but randomly chosen name and between 3 to 6 randomly
 *  created room connections. There is one START_ROOM and one END_ROOM chosen at random for the maze.
 *  Inspiration for the room connection procedures was obtained from a skeleton code written by Benjamin Brewster.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


#include <stdio.h>
#include <time.h>	// So we can generate random numbers
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>	// For open()
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#define TRUE	1	// Will allow me to use simulated bools throughout program
#define FALSE	0


struct room
{
	int id;			// Each room struct will be assigned its own ID number for comparison when adding connections
	int numConnections;	// Will increment as more room connections are made
	char* roomName;		// "String" used to represent the name of that particular room
	char* roomType;		// "String" used to represent the type of room
	struct room* outboundConnections[6];
};



/*
 *NAME
 *  randomNumber1To10 - randomly generate an int between 1 and 10 inclusive
 *SYNOPSIS
 *  randomNumber1To10()
 *DESCRIPTION
 *  This function is used to randomly select rooms to use in game
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/

int randomNumber1To10() {

	int randomNumber;

	int MIN_VALUE = 1;
	int MAX_VALUE = 10;


	randomNumber = (rand() % (MAX_VALUE - MIN_VALUE + 1)) + 1;

	return randomNumber;

}


/*
 *NAME
 *  randomNumber0To6 - generate a random int between 0 and 6 inclusive
 *SYNOPSIS
 *  randomNumber0To6()
 *DESCRIPTION
 *  Used to randomly asign which room is start room, and which is end room (numbers correspond to array indexes)
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


int randomNumber0To6() {
	int randomNumber;

	int MIN_VALUE = 0;
	int MAX_VALUE = 6;

	randomNumber = (rand() % (MAX_VALUE - MIN_VALUE + 1));	// 0 is ok number so don't need extra + 1

	return randomNumber;

}



/*
 *NAME
 *  addRoomName - adds a room name to the passed in struct room
 *SYNOPSIS
 *  addRoomName(struct room* roomToFill, int nameNumber)
 *DESCRIPTION
 *  Receives an int which allows function to determine which name to assign to the passed in room struct
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void addRoomName(struct room* roomToFill, int nameNumber) {

	roomToFill->roomName = calloc(8, sizeof(char));

	if(nameNumber == 1) {
		strcpy(roomToFill->roomName, "Mario");
	}

	else if(nameNumber == 2) {
		strcpy(roomToFill->roomName, "Zelda");
	}

	else if(nameNumber == 3) {
		strcpy(roomToFill->roomName, "Ruby");
	}

	else if(nameNumber == 4) {
		strcpy(roomToFill->roomName, "Sonic");
	}

	else if(nameNumber == 5) {
		strcpy(roomToFill->roomName, "Geno");
	}

	else if(nameNumber == 6) {
		strcpy(roomToFill->roomName, "Nash");
	}

	else if(nameNumber == 7) {
		strcpy(roomToFill->roomName, "Lucia");
	}

	else if(nameNumber == 8) {
		strcpy(roomToFill->roomName, "Alex");
	}

	else if(nameNumber == 9) {
		strcpy(roomToFill->roomName, "Erza");
	}

	else {		// nameNumber was 10
		strcpy(roomToFill->roomName, "Natsu");
	}
	
}



/*
 *NAME
 *  createUniqueNumberArray - creates an array of 7 unique ints between 1 and 10 inclusive
 *SYNOPSIS
 *  createUniqueNumberArray(int* arrayOfNums)
 *DESCRIPTION
 *  Receives an empty int array, and proceeds to fill the array with 7 randomly generated unique ints between 1 and 10 inclusive. Each number is used
 *  to determine which name to add to the room struct in a later function.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void createUniqueNumberArray(int* arrayOfNums) {

	int count;		// Controls ensuring all positions in final array filled
	int innerCount;		// Controls cycling through curruent array looking for unique values
	int numToAdd;
	int keepGoing;

	for(count=0; count < 7; count++) {
		keepGoing = 1;		// Treating like BOOL. This will keep loop going until unique number found

		while(keepGoing == 1) {

			numToAdd = randomNumber1To10();
			
			keepGoing = 0;	// Will become true again if the number is found anywhere in array

			for(innerCount=0; innerCount < 7; innerCount++) {

				if(arrayOfNums[innerCount] == numToAdd) {
					keepGoing = 1;

				}

			}

		}

		// At this point we know the number is unique
		
		arrayOfNums[count] = numToAdd;


	}

	// We should now have a unique array of ints

}



/*
 *NAME
 *  createRoomTypeArray - Creates an int array with two unique numbers that correspond to index numbers from the rooms array.
 *SYNOPSIS
 *  createRoomTypeArray(int* arrayOfNums)
 *DESCRIPTION
 *  Creates an int array with two unique numbers that correspond to index numbers from the rooms array. The first selected index will become the
 *  START_ROOM and the second selected index will become the END_ROOM.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void createRoomTypeArray(int* arrayOfNums) {

	int numToAdd;	// will become second number included

	arrayOfNums[0] = randomNumber0To6();

	int keepGoing = TRUE;

	while(keepGoing == TRUE) {
		numToAdd = randomNumber0To6();

		if(numToAdd != arrayOfNums[0]) {
			keepGoing = FALSE;
			arrayOfNums[1] = numToAdd;
		}

	}

}



/*
 *NAME
 *  addId - Add IDs to the room structs
 *SYNOPSIS
 *  addId(struct room* arrayOfRooms)
 *DESCRIPTION
 *  Add IDs to the room structs
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void addId(struct room* arrayOfRooms) {

	int index;

	for(index=0; index<7; index++) {
		arrayOfRooms[index].id = index + 1;

	}

}



/*
 *NAME
 *  initNumConnections - Start all numConnections at 0
 *SYNOPSIS
 *  initNumConnections(struct room* arrayOfRooms)
 *DESCRIPTION
 *  Start all numConnections at 0 in each struct room
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void initNumConnections(struct room* arrayOfRooms) {
	
	int index;

	for(index=0; index<7; index++) {
		arrayOfRooms[index].numConnections = 0;
	}

}



/*
 *NAME
 *  addRoomType - assign each room struct one of three possible room types
 *SYNOPSIS
 *  addRoomType(struct room* arrayOfRooms, int startRoomIndex, int endRoomIndex)
 *DESCRIPTION
 *  Add the room type to each struct in the room array. One room is assigned the START_ROOM type and another is assigned END_ROOM.
 *  All remaining rooms are assigned MID_ROOM type.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void addRoomType(struct room* arrayOfRooms, int startRoomIndex, int endRoomIndex) {

	int index;

	for(index=0; index<7; index++) {
		arrayOfRooms[index].roomType = calloc(20, sizeof(char));

		if(index == startRoomIndex) {
			strcpy(arrayOfRooms[index].roomType, "START_ROOM");
		}

		else if(index == endRoomIndex) {
			strcpy(arrayOfRooms[index].roomType, "END_ROOM");
		}

		else {
			strcpy(arrayOfRooms[index].roomType, "MID_ROOM");
		}

	}

}



/*
 *NAME
 *  IsGraphFull - determine if all rooms have the appropriate number of connections
 *SYNOPSIS
 *  IsGraphFull(struct room* arrayOfRooms)
 *DESCRIPTION
 *  Returns true (1) if all rooms have 3 to 6 outbound connections, false (0) otherwise
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


int IsGraphFull(struct room* arrayOfRooms) {
	
	int index;
	int connectionCheck;

	int result = TRUE;	// Will be set to false if even one connection number found too low

	for(index=0; index<7; index++) {
		connectionCheck = arrayOfRooms[index].numConnections;

		if(connectionCheck < 3 || connectionCheck > 6) {
			result = FALSE;
		}
		
	}

	return result;
}



/*
 *NAME
 *  GetRandomRoom - retreive the address of a random room out of the 7 structs in the array of rooms
 *SYNOPSIS
 *  GetRandomRoom(struct room* arrayOfRooms)
 *DESCRIPTION
 *  Returns a pointer to a random room, does NOT validate if connection can be added
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


struct room* GetRandomRoom(struct room* arrayOfRooms) {
	int indexToReturn;

	indexToReturn = randomNumber0To6();

	return &arrayOfRooms[indexToReturn];

}



/*
 *NAME
 *  CanAddConnectionFrom - find out if the argument room can have another connection added to it
 *SYNOPSIS
 *  CanAddConnectionFrom(struct room roomToCheck)
 *DESCRIPTION
 *  Returns true if a connection can be added from argument room (< 6 outbound connections) false otherwise
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


int CanAddConnectionFrom(struct room roomToCheck) {

	int result;

	if(roomToCheck.numConnections < 6) {
		result = TRUE;
	}

	else {
		result = FALSE;
	}

	return result;
}



/*
 *NAME
 *  IsSameRoom - find out if the two argument rooms are actually the same room
 *SYNOPSIS
 *  IsSameRoom(struct room room1, struct room room2)
 *DESCRIPTION
 *  Returns true if both argument rooms are the same room, false otherwise
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


int IsSameRoom(struct room room1, struct room room2) {

	int result;

	int room1Id = room1.id;
	int room2Id = room2.id;

	if(room1Id == room2Id) {	// The two rooms are the same
		result = TRUE;
	}

	else {
		result = FALSE;
	}

	return result;
}



/*
 *NAME
 *  ConnectionAlreadyExists - find out if two rooms are already connected
 *SYNOPSIS
 *  ConnectionAlreadyExists(struct room room1, struct room room2)
 *DESCRIPTION
 *  Returns true if a connection from the first room argument to the second room argument already exists, false otherwise
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


int ConnectionAlreadyExists(struct room room1, struct room room2) {

	int index;

	for(index=0; index < room1.numConnections; index++) {
		if(room1.outboundConnections[index]->id == room2.id) {
			return TRUE;
		}
	}

	return FALSE;	// If we made it through the array without triggering true, the connection wasn't there
}



/*
 *NAME
 *  ConnectRoom - make an outbound connection from room1 to room2
 *SYNOPSIS
 *  ConnectRoom(struct room* room1, struct room* room2)
 *DESCRIPTION
 *  Connects the first room argument to the second room argument
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void ConnectRoom(struct room* room1, struct room* room2) {

	int index = room1->numConnections;

	room1->outboundConnections[index] = room2;	// room2 contains the address of the struct in question
	room1->numConnections++;		// Added a room connection to the first argument room so increment
}



/*
 *NAME
 *  AddRandomConnection - finds a pair of random rooms to make a connection between
 *SYNOPSIS
 *  AddRandomConnection(struct room* arrayOfRooms)
 *DESCRIPTION
 *  Adds a random, valid outbound connection from a room to another room
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void AddRandomConnection(struct room* arrayOfRooms) {

	struct room* room1;		// These will point to the actual rooms that will be included in the final result
	struct room* room2;

	while(TRUE) {
		
		room1 = GetRandomRoom(arrayOfRooms);

		if(CanAddConnectionFrom(*room1) == TRUE) {
			break;
		}

	}

	do {

		room2 = GetRandomRoom(arrayOfRooms);

	}while(CanAddConnectionFrom(*room2) == FALSE || IsSameRoom(*room1, *room2) == TRUE || ConnectionAlreadyExists(*room1, *room2) == TRUE);


	// We now have two viable rooms for connecting. Passing the address of actual room ensures the real rooms are affected
	
	ConnectRoom(room1, room2);
	ConnectRoom(room2, room1);

}



/*
 *NAME
 *  createFile - generates a file of the correct name based on the room struct passed in as an argument. sends data about that room 
 *  to the file after creation.
 *SYNOPSIS
 *  createFile(char* basePath, struct room roomToWrite)
 *DESCRIPTION
 *  Will create one file per room struct and send in data 
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void createFile(char* basePath, struct room roomToWrite) {

	// These will be used solely for strcmp function in the if statements

	char* Mario = "Mario";
	char* Zelda = "Zelda";
	char* Ruby = "Ruby";
	char* Sonic = "Sonic";
	char* Geno = "Geno";
	char* Nash = "Nash";
	char* Lucia = "Lucia";
	char* Alex = "Alex";
	char* Erza = "Erza";
	char* Natsu = "Natsu";

	int file_descriptor;

	char filePath[255];
	memset(filePath, '\0', 255);	// Make sure final string ends in a null terminator

	if(strcmp(roomToWrite.roomName, Mario) == 0) {

		sprintf(filePath, "./%s/Mario_room", basePath);
	}

	
	else if(strcmp(roomToWrite.roomName, Zelda) == 0) {

		sprintf(filePath, "./%s/Zelda_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Ruby) == 0) {

		sprintf(filePath, "./%s/Ruby_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Sonic) == 0) {

		sprintf(filePath, "./%s/Sonic_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Geno) == 0) {

		sprintf(filePath, "./%s/Geno_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Nash) == 0) {

		sprintf(filePath, "./%s/Nash_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Lucia) == 0) {

		sprintf(filePath, "./%s/Lucia_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Alex) == 0) {

		sprintf(filePath, "./%s/Alex_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Erza) == 0) {

		sprintf(filePath, "./%s/Erza_room", basePath);
	}

	else if(strcmp(roomToWrite.roomName, Natsu) == 0) {

		sprintf(filePath, "./%s/Natsu_room", basePath);
	}

	file_descriptor = open(filePath, O_WRONLY | O_CREAT, S_IWUSR | S_IRUSR);

	// Data gets sent after if/thens
	
	char stringToSend[255];
	memset(stringToSend, '\0', 255);

	sprintf(stringToSend, "ROOM NAME: %s\n", roomToWrite.roomName);	// Lift the name of the room to send

	write(file_descriptor, stringToSend, strlen(stringToSend) * sizeof(char));	// Send string through to file

	// Send each connection name to the file

	int indexToSend;

	for(indexToSend=0; indexToSend < roomToWrite.numConnections; indexToSend++) {

		memset(stringToSend, '\0', 255);	// Reset string for next line
		sprintf(stringToSend, "CONNECTION %d: %s\n", indexToSend+1, roomToWrite.outboundConnections[indexToSend]->roomName);
		write(file_descriptor, stringToSend, strlen(stringToSend) * sizeof(char));
	}

	memset(stringToSend, '\0', 255);	// Reset string for next line

	sprintf(stringToSend, "ROOM TYPE: %s\n", roomToWrite.roomType);		// Now sending room type to file

	write(file_descriptor, stringToSend, strlen(stringToSend) * sizeof(char));

	close(file_descriptor);
}



/*
 *NAME
 *  freeDynamicMemory - frees the dynamic memory used by the c strings holding the room name and room type in each room struct
 *SYNOPSIS
 *  freeDynamicMemory(struct room* arrayToClear)
 *DESCRIPTION
 *  Will be called at the end of main to free dynamic memory created and avoid memory leaks 
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/


void freeDynamicMemory(struct room* arrayToClear) {
	int loopCounter;

	for(loopCounter=0; loopCounter < 7; loopCounter++) {
		free(arrayToClear[loopCounter].roomName);
		free(arrayToClear[loopCounter].roomType);

	}


}



/*
 *NAME
 *  main - drives the main procedure of the program
 *SYNOPSIS
 *  main()
 *DESCRIPTION
 *  Seeds srand() with time so that random numbers can be generated. Creates 7 random rooms and connects them. Makes a directory for the files, and
 *  has files created and sent to it. Then frees dynamic memory.
 *AUTHOR
 *  Written by Michael Childress (childrem@oregonstate.edu)
*/

int main() {

	srand(time(NULL));	// Seed rand with time so we get random nums

	struct room roomsToInclude[7];		// This array will have 7 room struct in it and will be populated with random values
 
	addId(roomsToInclude);			// Give each room a unique ID number

	initNumConnections(roomsToInclude);	// Set all structs connection number to 0

	int loopCounter;

	// We first need an array of 7 unique random numbers between 1 and 10
	// This will be used to make sure each room included in final map is unique

	int randomNumberArray[7];

	createUniqueNumberArray(randomNumberArray);


	for(loopCounter=0; loopCounter<7; loopCounter++) {

		addRoomName(&roomsToInclude[loopCounter], randomNumberArray[loopCounter]);  // This will add in the appropriate name to each struct in the array
	}

	// Now we need to generate two unique random numbers (index values) that will be assigned start room and end room
	
	int roomTypeArray[2];

	createRoomTypeArray(roomTypeArray);

	addRoomType(roomsToInclude, roomTypeArray[0], roomTypeArray[1]);	// roomTypeArray has two integers that represent indexes in roomsToInclude
										// roomTypeArray[0] will be start room, roomTypeArray[1] will be end room
	// Create all connections in the graph
	
	while(IsGraphFull(roomsToInclude) == FALSE) {

		AddRandomConnection(roomsToInclude);

	}

	
	// Create a directory to house the 7 room files

	int processId = getpid();
	
	char fileName[255];

	memset(fileName, '\0', 255);
	sprintf(fileName, "childrem.rooms.%d", processId);

	int makeDirectoryResult;

	makeDirectoryResult = mkdir(fileName, 0700);

	if(makeDirectoryResult != 0) {
		printf("There was an error making the directory!\n");
	}

	// Create the files with room data
	
	int roomToProcess;

	for(roomToProcess=0; roomToProcess < 7; roomToProcess++) {
		createFile(fileName, roomsToInclude[roomToProcess]);	
	}
	

	// Free dynamic memory to avoid memory leaks
	
	freeDynamicMemory(roomsToInclude);


	return 0;

}
