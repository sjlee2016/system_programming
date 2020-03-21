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
#define UNDEFINED 10
#include <string.h>
#include <dirent.h>
typedef struct History_Node *h_node;
typedef struct History_Node{ 
    char * s; 
    int n; 
    struct History_Node *next; 
};