#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

#define QUIT 0
#define MAX_MEMORY_SIZE 1048576 // Arraysize to store 1 MB memory 
#define MAX_HASH_SIZE 20  // max size for hash table 
#define SYMBOL_HASH_SIZE 27   // max size for symbol hash table
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

typedef struct Table_Element{ // node for storing mnemonic information
    char mnemonic[7];
    char format[5];
    long opcode;
    struct Table_Element* next;
}Table_Element;

typedef struct Symbol_Element{ // node used for symbol table
    char identifier[7];
    char type[10]; 
    long address;
    char * value;
    struct Symbol_Element* next;
}Symbol_Element;

typedef struct Obj_Element{ // node used to store object code 
    long address; 
    char * objcode;
    struct Obj_Element * next;
}Obj_Element;

typedef struct Relocation_Element{ // node used to store address of relocation
    long address;
    long length;
    struct Relocation_Element* next;
}Relocation_Element;

typedef struct ESTAB_Table{
    long address;
    long length;
    char symbol[100]; 
    struct ESTAB_Table * next; 
}ESTAB_Table; 
ESTAB_Table * ESTAB[3];
h_node head;  // points to the head of linked list of history node
h_node current; // points to the current node of history node 
Table_Element * HashTable [MAX_HASH_SIZE]; // hash table used to store opcode info 
Symbol_Element * SymbolTable [SYMBOL_HASH_SIZE]; // hash table used for symbol table
Obj_Element * T_head;
Obj_Element * T_last;
Relocation_Element * R_head;
Relocation_Element * R_last;

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
char complexInsts[13][10] = {"dump","du","edit","e","fill","f","opcode","type","assemble","bp","loader","progaddr","run"}; // list of instructions that require extra parameter input 
char userInput[100]; // stores user input 
char command[11]; // stores command from user input (excludes empty string)
char targetMnemonic[7]; // stores mnemonic from user input 
char filename[100]; // stores the file name without the extension
char fullFileName[100]; // stores the full filename
char extension[10]; // stores extension of the filename
char operand[10][100]; // used to store the operands in asm file 
long LOCCTR = 0; // stores the current LOCCTR
long previousLOCCTR = 0; // stores previous LOCCTR
int line_size = 100; // size of line for reading asm file
char title[100]; // stores the title of the assembly program
char base[100]; // stores BASE 
int needToPrint; // decides whether the object code needs to be printed
int endFound; // flag to show that file reached END symbol 
int numWord; // the number of seperate string in current line of assembly code
int locationOfMnemonic; // index of Mnemonic in assembly code 
char * trueMnemonic; // stores the mnemonic after removing  + 
int format; // stores the addressing mode of the mnemonic 
long baseLoc; // address of the BASE register
int isConstant; // flag to indicate the operand is a constant
int isVariable; // flag to indicate the operand is a variable
int startFound;  // flag to indicate that the file reached START symbol
long endLoc; // location of END in the program 
long firstExecLoc;  // location of the first executable instruction 
int isX; // flag to indicate a hexadecimal constant
char TEMP_BUFFER [1000] = {0};  // used to temporary store object code 
FILE *objFile;
char objName [200]; // store file name for object file
char lstName [200]; // store file name for immediate file 

// PROJECT 3 
long PROGADDR = 0;   //Program Load Address. Starting address in memory where the linked program is to be loaded.
long CSADDR = 0; // Control Section Address.Starting address assigned to the control section currently being scanner by the loader  
long CSLTH = 0; // Length of Control Section 
int numOfFile;
int currentFileNum; 
FILE * objf[3];  
char breakpoints[1000][8]; 
int bpNum = 0; 
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
long getOpcode(char * mnemonic, int option);

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


/******************************************************
 * getHashKeySymbol
 * returns the hash key for the given symbol
 * ****************************************************/
int getHashKeySymbol(char * symbol);


/******************************************************
 * getAddress
 * returns the address of the given symbol 
 * ****************************************************/
long getAddress(char * symbol);

/******************************************************
 * getElement
 * returns table element for the given mnemonic.
 * returns null if the given mnemonic is not found in the hash table. 
 * ****************************************************/
Table_Element * getElement(char * mnemonic);

/******************************************************
 * insertSymbolElement
 * insert new symbol to the symbol table
 * returns 1 if successfully inserted
 * returns 0 if there is already a duplicate in the table 
 * ****************************************************/
int insertSymbolElement(char * identifier, long address, char * type, char * value);

/******************************************************
 * freeSymbolTable
 * free symbol table. This is called before assemble function is called
 * in order to erase the previous symbol table
 * ****************************************************/
void freeSymbolTable();

/******************************************************
 * getByteSize
 * returns the byte size of the given string 
 * ****************************************************/
long getByteSize(char * str);

/******************************************************
 * checkFilename
 * seperates the file name and extension and store them 
 * into filename and extension array 
 * returns 0 if the file is not found in the directory
 * return 1 otherwise. 
 * ****************************************************/
int checkFilename();

/******************************************************
 * showSymbol
 * prints out the symbol table
 * ****************************************************/
void showSymbol(); 

/******************************************************
 * parseLine
 * parse the given line to calculate the location of the mnemonic,
 * addressing mode and seperate the operands 
 * if option is 0 : for the given line of assembly code, inserts symbols
 * into SYMTAB if it exists
 * if option is 1 : does not insert symbol into SYMTAB
 * returns 1 if successful
 * returns 0 if there is an error
 * ****************************************************/
int parseLine( char * line, int option);

/******************************************************
 * getRegisterNumber
 * returns the register number 
 * ****************************************************/
long getRegisterNumber(char * r);

/******************************************************
 * insertObjectCode
 * insert new object code node into the object code linked list
 * ****************************************************/
void insertObjectCode(long address, char * objcode);

/******************************************************
 * insertRelocationNode
 * insert new relocation node into the relocation linked list
 * ****************************************************/
void insertRelocationNode(long address);

/******************************************************
 * calculateObjectCode
 * returns the object code of the given line 
 * ****************************************************/
long calculateObjectCode(int numLine,char * line);

/******************************************************
 * typeFile
 * prints out the contents of the file 
 * returns 0 if the file is not found in the directory
 * return 1 otherwise. 
 * ****************************************************/
int typeFile(char * fileName);

/******************************************************
 * assemble
 * create immediate file and object file from the assembly file
 * returns 0 if there is an error in the assembly code
 * returns 1 if successful
 * ****************************************************/
int assemble(char * fileName);

/******************************************************
 * passOne
 * method used to read the assembly file for the first time
 * and calculate LOCC for each line and insert symbols into the symbol table
 * returns 0 if there is an error in the assembly code
 * returns 1 if successful
 * ****************************************************/
int passOne(char * asmFileName);

/******************************************************
 * passTwo
 * method called when reading the assembly file for the second time
 * With the symbol table created by passOne(), it now calculates the object code
 * for each line and prints out immediate file and object file 
 * returns 0 if there is an error in the assembly code
 * returns 1 successful
 * ****************************************************/
int passTwo(char * asmFileName);

/******************************************************
 * printObjectFile
 * prints the contents of the object code linked list into the
 * object file and free the linked list
 * ****************************************************/
void printObjectFile();