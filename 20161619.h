#define QUIT 0
#define RIGHT_COMMAND 1;
/*
#define HELP 1
#define DIRECTORY 2
#define HISTORY 3
#define DUMP 4
#define EDIT 5
#define FILL 6
#define RESET 7
#define OPCODE 8
#define OPCODELIST 9 
*/
#define MAX_MEMORY_SIZE 1048576 // 1 MB memory 
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
unsigned char VMemory [MAX_MEMORY_SIZE]; 
char params[3][10]; 
int numOfParams=0;
long defaultStartAddr = -1; 
int instEndInx = 0; 
int instLength = 0;
typedef struct History_Node * h_node;
typedef struct History_Node{ 
    char * s; 
    int n; 
    struct History_Node *next; 
}History_Node;
h_node head; 
h_node current;
int historyCount = 0;
char singleInsts [10][10] = {"help", "h", "dir", "d", "history","hi","quit","q", "reset","opcodelist"};
char pluralInsts[7][10] = {"dump","du","edit","e","fill","f","opcode"};
char userInput[100]; 
char command[10];
void updateHistory(char *command);
int listdir(const char *path);
void showHistory();
void printAllCommands();