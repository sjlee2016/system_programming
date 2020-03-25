#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#define QUIT 0
#define MAX_MEMORY_SIZE 1048576 // 1 MB memory 
#define MAX_HASH_SIZE 20
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
typedef struct Table_Element{
    char mnemonic[7];
    char format[5];
    int opcode;
    struct Table_Element* next;
}Table_Element;
Table_Element * HashTable [MAX_HASH_SIZE];

h_node head; 
h_node current;

int historyCount = 0;
int hashSize = 0; 
char singleInsts [10][10] = {"help", "h", "dir", "d", "history","hi","quit","q", "reset","opcodelist"};
char pluralInsts[7][10] = {"dump","du","edit","e","fill","f","opcode"};
char userInput[100]; 
char command[10];
void updateHistory(char *command);
void showHistory();
int listdir(const char *path);
void printAllCommands();
int isSingleInst();
int isEmpty(char c);
int isHexadecimal(char c);
int isPluralInst();
int checkParams();
void printMem(long start, long end);
int dumpMemory();
int editMemory();
int create_hash(int opcode, char * mnemonic, char * format);
void resetMemory();
int fillMemory();
void showOpcode();
int opcode();
int getCommand();