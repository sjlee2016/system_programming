#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

#define QUIT 0
#define MAX_MEMORY_SIZE 1048576 // Arraysize to store 1 MB memory 
#define MAX_HASH_SIZE 20  // max size for hash table 
#define SYMBOL_HASH_SIZE 27 
#define MAX_USER_INPUT 100 // max user input
#define ERROR 0; // used for return value of function
#define SUCCESS 1; 

/////////// DATA STRUCTURE ////////////
typedef struct History_Node * h_node;
typedef struct History_Node{  // node for storing history information
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


typedef struct Symbol_Element{
    char identifier[7];
    char type[20]; 
    long address;
    char value[20];
    struct Symbol_Element* next;
}Symbol_Element;

h_node head;  // points to the head of linked list of history node
h_node current; // points to the current node of history node 
Table_Element * HashTable [MAX_HASH_SIZE]; // hash table used to store opcode info 
Symbol_Element * SymbolTable [SYMBOL_HASH_SIZE]; 

/// CONSTANTS used for printing out errors in printErrorMessage function 
const int ERROR_PARAMETER = -1;
const int ERROR_ADDRESS_OUT_OF_BOUND = -2;
const int ERROR_INPUT_FORMAT = -3;
const int ERROR_PARAMETER_LENGTH = -4 ;
const int ERROR_PARAMETER_OUT_OF_BOUND = -5; 
const int ERROR_PARAMETER_NOT_HEX = -6 ;

///// VARIABLES 
unsigned char VMemory [MAX_MEMORY_SIZE];  // Virtual memory
char params[3][10];  // used to parse parameters for commands 
int numOfParams=0; // global variable to store number of parameters 
long defaultStartAddr = -1;   // default starting address for memory for dump command 
int instLength = 0; // length of the instruction (excluding unnessary inputs from user like empty space and parameters)
int historyCount = 0; // total number of history stored 
char simpleInsts [11][20] = {"help", "h", "dir", "d", "history","hi","quit","q", "reset","symbol","opcodelist"}; // list of instructions that does not require any parameter
char complexInsts[9][10] = {"dump","du","edit","e","fill","f","opcode","type","assemble"}; // list of instructions that require extra parameter input 
char userInput[100]; // stores user input 
char command[11]; // stores command from user input (excludes empty string)
char targetMnemonic[7]; // stores mnemonic from user input 
char filename[100]; 
char fullFileName[100];
char extension[10];
char tempStorage[5][20];
long LOCCTR = 0;
long previousLOCCTR = 0;
int line_size = 50;
char title[100];
char base[100];
int needToPrint;
int endFound;
//// USER-DEFINED FUNCTIONS 

/******************************************************
 * updateHistory
 * adds new history node, storing user command, into history linked list.
 * this function is only called when user puts in the properly working command
 * ****************************************************/
void updateHistory(char *command);
/******************************************************
 * showHistory
 * prints out the history linked list, showing all the previous commands
 * ****************************************************/
void showHistory();
/******************************************************
 * listdir
 * prints out the files in the path
 * folders and output files can be identified with '/' and '*' added at the end of the filename   
 * ****************************************************/
int listdir(const char *path);
/******************************************************
 * printAllCommands
 * prints out all the commands working for this SIC machine 
 * ****************************************************/
void printAllCommands();
/******************************************************
 * cleanEmptyStr
 * removes any empty characters in the userInput before the first character  
 * ****************************************************/
void cleanEmptyStr();
/******************************************************
 * isSimpleInst
 * returns 1 if the user input is a single instruction ( instruction requiring no extra parameters)
 * returns 0 otherwise 
 * ****************************************************/
int isSimpleInst();
/******************************************************
 * isComplexInst
 * returns 1 if the user input is a multiple instruction ( instruction requiring no extra parameters)
 * returns 0 otherwise 
 * ****************************************************/
int isComplexInst();
/******************************************************
 * isEmpty
 * returns 1 if the character is empty, 0 otherwise 
 * ****************************************************/
int isEmpty(char c);
/******************************************************
 * isHexadecimal
 * returns 1 if the character is hexadecimal, 0 otherwise 
 * ****************************************************/
int isHexadecimal(char c);
/******************************************************
 * cleanEmptyStr
 * removes any empty string in front of the first non-empty letter
 * in the command. 
 * *****************************************************/
void cleanEmptyStr();
/******************************************************
 * checkParams
 * checks the number of parameters in the user input
 * and stores them into params array. the number of parameter is stored in the global variable, numOfParams
 * returns 0(ERROR_CODE) if parameter is in the wrong format for the specific command
 * returns 1(SUCCESS_CODE) if parameter is in the right format
 * ****************************************************/
int checkParams();
/******************************************************
 * checkParamsMnemonic
 * checks the format of parameter for opcode command
 * returns 0(ERROR_CODE) if parameter isn't in capital letter or exceeds maximum length 
 * parses mnemonic from user input and stores it to global variable, targetMnemonic 
 * ****************************************************/
int checkParamsMnemonic();
/******************************************************
 * printMem
 * prints out virtual memory from start address to end address
 * ****************************************************/
void printMem(long start, long end);
/******************************************************
 * dumpMemory
 * prints out virtual memory
 * if no parameter is given, it prints out 160 from defaultStartAddr
 * if one parameter is given(start), it prints out 160 from start address
 * if two parameters are given(start,end), it prints out from start to end address 
 * returns 0 if command is given in the wrong format 
 * returns 1 if the command worked successfully 
 * ****************************************************/
int dumpMemory();
/******************************************************
 * editMemory
 * changes the memory at the given address to the value given.
 * returns 1 if successful
 * returns 0 if unsuccessful (wrong format)
 * ****************************************************/
int editMemory();
/******************************************************
 * resetMemory
 * resets the entire virtual memory with value 0 
 * ****************************************************/
void resetMemory();
/******************************************************
 * fillMemory
 * fills the virtual memory from start to end address with the given value
 * returns 1 if successful
 * returns 0 if unsuccessful (wrong format)
 * ****************************************************/
int fillMemory();
/******************************************************
 * showOpcode
 * prints out the list of opcodes stored in the hash table 
 * ****************************************************/
void showOpcode();
/******************************************************
 * getOpcode
 * prints out the opcode of given mnemonic 
 * returns 1 if successful
 * returns 0 if failed to find the mnemonic
 * ****************************************************/
int getOpcode();

/******************************************************
 * insertTableElement
 * insert new hash table element into the hash table
 * ****************************************************/
void insertTableElement(int opcode, char * mnemonic, char * format );

/******************************************************
 * getHashKey
 * returns the hash key for the given mnemonic
 * ****************************************************/
int getHashKey(char * mnemonic);

/******************************************************
 * getCommand
 * get user input from user and store it into global variable, userInput
 * by parsing the userInput, it finds out which command should be called 
 * returns 0 if user puts quit
 * returns 1 otherwise
 * ****************************************************/
int getCommand();
int checkFilename();
void showSymbol(); 
int readFile(char * fileName);
int assemble(char * fileName);
int passOne();
