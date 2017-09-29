/****************************************************************
 Lauren Egerton
 CS 344 / Fall 2016
 Program 2
 Description : A program that uses an array of 10 strings and 7 struct rooms to randomly
 populate 7 different files to play a simple text-based game.
 Cite : Office hours with Prof. Brewster, 10/26/16, who offered helpful examples
 of how I could break up my program into functions for the randomization
 of connecting the rooms.
 Cite : Ignoring "." files
 http://stackoverflow.com/questions/20265328/readdir-beginning-with-dots-instead-of-//files
 Cite : Creating directories https://linux.die.net/man/3/chdir
 Cite : Used often as a reference, Programming in C by Stephan Kochan, 4th ed.
 ***************************************************************/

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

/****************************************************************
            Function and struct definitions
 ***************************************************************/

//define struct room
struct room{
    char name[10];
    char type[12];
    int numConnections;
    struct room *connections[6];
};

//will populate newRoom with data from the files
struct newRoom{
    char name[12];
    char type[12];
    int numberConnections;
    char *connectRoom[6];
};

//returns a pointer to a random room from the array of struct rooms
struct room *getRandomRoom(struct room array[]){
    int random;
    struct room *A;
    
    random = rand() % 7;
    
    A = &array[random];
    return A;
}

//returns true if the rooms have the same name
int isSameRoom(struct room *A, struct room *B){
    if(strcmp(A->name, B->name) == 0){
        return 0;
    }
    return 1;
}

//returns true if less than 6 connections exist (can have max of 6)
int canAddConnectionFrom(struct room *A){
    if(A->numConnections < 6){
        return 0;
    }
    else
        return 1;
}

//returns true when all connections are between 3-6
int isGraphFull(struct room array[]){
    int x, flag;
    flag = 0;
    
    //check connections for each room in array
    for(x=0; x<7; x++){
        if((array[x].numConnections > 2) && (array[x].numConnections < 7)){
            flag = 0;
        }
        else{
            flag = 1;
            break;
        }
    }
    return flag;
}

//check if the rooms are already connected
int isConnected(struct room *A, struct room *B){
    int x, y;
    y = A->numConnections;
    
    for(x=0; x<y; x++){
        if(A->connections[x] == B){
            //return as soon as a connection is found
            return 0;
        }
    }
    //no connection found
    return 1;
}

//creates a one-way connection between two rooms
void connectRoom(struct room *A, struct room *B){
    int first;
    first = A->numConnections;
    
    //next available spot in room's connections arrays
    A->connections[first] = B;
    A->numConnections++;
}

//returns 0 if number is already in array
int containsNum(int array[], int value){
    int i;
    
    for (i=0; i < 7; i++) {
        if (array[i] == value)
            return 0;
    }
    return 1;
}

//returns 0 if the name is one of the connections for the room
int hasConnection(struct newRoom *ptr, char name[10]){
    int i;
    
    for(i=0; i<ptr->numberConnections; i++){
        if(strncmp(ptr->connectRoom[i], name, 10) == 0)
            return 0;
    }
    return 1;
}

/****************************************************************
                Main function begins here
 ***************************************************************/
int main(void){
    //seed random number
    srand(time(0));
    
    //create directory name using pid
    char directoryName[20];
    int pid = getpid();
    sprintf(directoryName, "%s%d", "./egertonl.rooms.", pid);
    
    //make a directory
    mkdir(directoryName, 0755);
 
    //create directory
    char *directory = directoryName;
    chdir (directory);
    
    //use this array for indices of room names
    int numbers[7];
    int random, i, j;
    
    //generate 7 random numbers between 0-9 and add to numbers array
    for(i=0; i<7; i++){
        do{
            random = rand() % 10;
            j = containsNum(numbers, random);
            //keep looping until the number is unique
        }while(j == 0);
        
        //Add unique number to the array
        numbers[i] = random;
    }
    
    //create a list of 10 names to use for struct rooms
    char *roomNames[10] = { "Tokyo", "Jakarta", "Delhi", "Manila", "Chengdu", "Lima",
        "London", "Madrid", "Mumbai", "Cairo" };
    
    //Create an array 7 room structs
    struct room allRooms[7];
    
    //set all room connections to 0 to start
    for(i=0; i<7; i++){
        allRooms[i].numConnections = 0;
    }
    
    int x;
    //1. For each struct room in array allRooms :
    for(x=0; x<7; x++){
        //Set room types
        //first one is start_room
        if(x == 0){
            strncpy(allRooms[x].type, "START_ROOM", 12);
        }
        //last one is end_room
        else if(x == 6){
            strncpy(allRooms[x].type, "END_ROOM", 10);
        }
        //all other rooms to mid_room
        else{
            strncpy(allRooms[x].type, "MID_ROOM", 10);
        }
        
        //set name to indice of 'numbers' using roomNames array
        strncpy(allRooms[x].name, roomNames[numbers[x]], 10);
    }
    
    //Set random connections between rooms
    //loop until all rooms have 3-6 connections
    do{
        struct room *A, *B;
        
        do{
            A = getRandomRoom(allRooms);
        }while(canAddConnectionFrom(A) == 1);
        
        do{
            B = getRandomRoom(allRooms);
            //keep looping until a room has a free connection OR is the same room
            //OR already have a connection
        }while((canAddConnectionFrom(B) == 1) ||
               (isSameRoom(A, B) == 0) || (isConnected(A, B) == 0));
        
        connectRoom(A, B);
        connectRoom(B, A);
    }while(isGraphFull(allRooms) == 1);
    
    //Put 7 rooms into a 7 different files, inside the Room directory
    //For each struct room in allRooms
    for(x=0; x<7; x++){
        int w, z;
        w = allRooms[x].numConnections;
        
        //get the name of the room and save it in a string
        char currName [10];
        strncpy(currName, allRooms[x].name, 10);
        
        FILE *inputFile;
        
        inputFile = fopen(currName, "w");
        
        //check that file is created
        if (inputFile == 0){
            fprintf(stderr, "Could not open %s\n", currName);
            perror("Error in main()\n");
            exit(1);
        }
        else{
            //printf("File created and opened!\n");
        }
        
        fprintf(inputFile, "ROOM NAME: %s\n", currName);
        for(z=0; z<w; z++){
            //convert int to string for parsing later
            char num[2];
            snprintf(num, 2, "%d", (z+1));
            fprintf(inputFile, "CONNECTION %s: %s\n", num, allRooms[x].connections[z]->name);
        }
        fprintf(inputFile, "ROOM TYPE: %s\n", allRooms[x].type);
        
        fclose(inputFile);
    }
    
    //Read all data from files into a new struct room array
    //create an array of 7 struct newRoom
    struct newRoom gameRoom[7];
    
    //set each newRoom connectRoom pointer to NULL ?
    int w, c;
    int length;
    length = 10;
    for(w=0; w<7; w++){
        for(c=0; c<6; c++){
            //printf("Do we get here?\n");
            gameRoom[w].connectRoom[c] = malloc( length * sizeof(char) );
        }
    }
    
/*** Open Directory and read from each file into an array of structs for use in Game ****/
    DIR *dir;
    struct dirent *dirName;
    char * file_name;
    FILE *outFile;
    dir = opendir(".");
    
    char buffer[512];
    memset(buffer, '\0', 512);
    
    char check1[12];    //to check for Connection
    memset(check1, '\0', 12);
    
    char check2[12];    //tp check for Type or Name
    memset(check2, '\0', 12);
    
    char checkName[12]; //to use to save name or type
    memset(checkName, '\0', 12);
    
    int count;          //to increment through array of struct rooms
    count = 0;
 
    //for each file
    //ignore "." files
    while ((dirName=readdir(dir)) != NULL){
        int t;
        t=0;
        //skipping the dot files
        if ( (strcmp(dirName->d_name, ".") == 0) || (strcmp(dirName->d_name, "..") == 0) ){
        }
        //get info from each file
        else{
            //open file
            outFile = fopen(dirName->d_name, "r");
            
            //check that file is open
            if(outFile == 0){
                fprintf(stderr, "Could not open %s\n", dirName->d_name);
                perror("Error in main()\n");
                exit(1);
            }
                else{
                    //printf("Current file: %s \n", dirName->d_name);
                }

            //get each line and parse
            while(fgets(buffer, sizeof buffer, outFile) != NULL){
                //printf("Getting the line now!\n");
                //printf("Current line is: %s\n", buffer);
                //parse line
                sscanf(buffer, "%s%s%*c%s\n", check1, check2, checkName);
                //printf("check1 is: %s check2 is: %s checkName is: %s\n", check1, check2, checkName);
                
                //save in gameRoom array
                //set name
                if(strcmp(check2, "NAME:") == 0){
                    //printf("Check2 = Name\n");
                    strncpy(gameRoom[count].name, checkName, 12);
                    //printf("Current struct in game is name: %s\n", gameRoom[count].name);
                    memset(check2, '\0', 12);
                    memset(checkName, '\0', 12);
                }
                //set connections
                if(strcmp(check1, "CONNECTION") == 0){
                    //printf("t = %d\n", t);
                    //printf("Check1 = CONNECTION\n");
                    strncpy(gameRoom[count].connectRoom[t], checkName, 12);
                    t++;
                    gameRoom[count].numberConnections = t;
                    memset(check1, '\0', 10);
                    memset(checkName, '\0', 12);
                }
                
                //set type
                if(strcmp(check2, "TYPE:") == 0){
                    //printf("Check2 = Type\n");
                    strncpy(gameRoom[count].type, checkName, 12);
                    //printf("Current struct in game is type: %s\n", gameRoom[count].type);
                    memset(check2, '\0', 10);
                    memset(checkName, '\0', 12);
                }
            }
            
            //close file
            fclose(outFile);
    
            //move to next struct in gameRoom array
            count++;
            //make sure char array is clear for next struct's info
            memset(buffer, '\0', 512);
        }
    }
    
    //close directory
    closedir(dir);

/********************************************************************
                    Game Interface Starts Here
*********************************************************************/
    
    //number of steps taken during game
    int steps;
    steps = 0;
    //array of strings to track path taken
    char *path[200];
    //error message
    char bigError[] = "HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.";

    //printf("Error is: %s\n", bigError);
    
    char choice[10];
    char here[10];
    struct newRoom *current;
    
    //start game in START_ROOM
    printf("\n");
    
    for(i=0; i<7; i++){
        if(strcmp(gameRoom[i].type, "START_ROOM") == 0){
            //pointer to current room
            current = &gameRoom[i];
        }
    }
  
    do{
        printf("CURRENT LOCATION: %s\n", current->name);
        printf("POSSIBLE CONNECTIONS:");
        //print connections
        for(x=0; x<current->numberConnections; x++){
            printf(" %s", current->connectRoom[x]);
            if(x < (current->numberConnections - 1)){
                printf(",");
            }
            if(x == (current->numberConnections - 1)){
                printf(".\n");
            }
        }

        printf("WHERE TO? >");
        //memset(choice, '\0', 10);
        fgets(choice, 10, stdin);
        //remove trailing newline char
        strtok(choice, "\n");
        //printf("Your current choice is: %s\n", choice);
        printf("\n");
    
        int g;
        g = hasConnection(current, choice);
        
        //if use has entered a correct connection choice
        if(g == 0){
            for(i=0; i<7; i++){
                if(strncmp(gameRoom[i].name, choice, 10) == 0){
                    //allocate mem for a spot in path array
                    path[steps] = malloc( length * sizeof(char) );
                    //add current room to path
                    strncpy(path[steps], current->name, 10);
                    //change current pointer to next room
                    current = &gameRoom[i];
                    //add a step
                    steps++;
                    break;
                }
            }
        }
        //user has entered an incorrect user choice
        else if(g == 1){
            printf("%s\n", bigError);
            printf("\n");
        }
    }while(strncmp(current->type, "END_ROOM", 12) != 0);

    //User ends up in the END_ROOM, game is over
    if(strncmp(current->type, "END_ROOM", 12) == 0){
        printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    }
    
    //add final room to path / steps count
    for(i=0; i<7; i++){
        if(strncmp(gameRoom[i].name, choice, 10) == 0){
            //allocate mem for a spot in path array
            path[steps] = malloc( length * sizeof(char) );
            //add current room to path
            strncpy(path[steps], current->name, 10);
            //change current pointer to next room
            current = &gameRoom[i];
            //add a step
            steps++;
        }
    }
    
    //print steps
    if(steps == 2){
        printf("YOU TOOK %d STEP. YOUR PATH TO VICTORY WAS:\n", (steps-1));
    }
    else if(steps > 2){
        printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", (steps-1));
    }
    
    //print path taken
    for(x=1; x<steps; x++){
        printf("%s\n", path[x]);
    }
    
    printf("\n");

    //free mem for struct room pointers to connections
    for(w=0; w<7; w++){
        for(c=0; c<6; c++){
            //printf("Do we free?\n");
            free(gameRoom[w].connectRoom[c]);
        }
    }
    
    //free mem allocated for pointers for game path array
    for(w=0; w<steps; w++){
        free(path[w]);
    }
    
    //terminate and return 0 if program runs successfully
    return 0;
}











