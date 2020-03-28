#include <stdio.h>
#include "20161619.h"
void updateHistory(char *command){
    h_node temp = (History_Node *)malloc(sizeof(History_Node) * 1); // allocate history node
    temp->n = historyCount; // store current number of history stored into n 
    temp->next = NULL;
    temp->s = (char *)malloc(sizeof(char) * strlen(command)); // dynamically allocate char array 
    command[strlen(command)-1]='\0'; // store null value at the end of the array
    strcpy(temp->s, command); // copy command to s 
    if (head == NULL){ // if there is no previous history node stored
        head = temp; // update head and current
        current = head;
    }
    else{
        current->next = temp; // otherwise add the new node to the linked list
        current = temp;
    }
}

void showHistory(){
    h_node temp;
    temp = head; 
    while (temp != NULL){ // starting from head node, until it reaches the end
        printf("%d %s\n", temp->n, temp->s); // print out the history info
        temp = temp->next;
    }
}

int listdir(const char *path){
    struct dirent *ent;
    struct stat buf;
    DIR *dp = opendir(path);
    if (dp == NULL) { // print error if directory cannot be opened 
        printf("directory cannot be opened\n");
        return ERROR;
    }
    while ((ent = readdir(dp))) { // read the directory 
        printf("%s", ent->d_name); // print out the name of the file 
        lstat(ent->d_name, &buf);
        if (S_ISDIR(buf.st_mode)){ // if the file is a folder 
            printf("/");
        }else if (buf.st_mode & S_IEXEC ){ // if the file is output file
            printf("*");   
        }
        printf(" ");
    }
    printf("\n"); 
    closedir(dp); // close the directory
    return 0;
}

void printAllCommands(){ // print out all the commands
    printf("h[elp]\nd[ir]\nq[uit]\ndu[mp]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
}

int isSimpleInst(){
    int i,j,flag,found = 0;
    for (i = 0; i < 10; i++) { // loop through simple instruction list to find out it matches the user input 
        flag = 0; // initialize flag value
        instLength = strlen(simpleInsts[i]); // get the length of the instruction 
        if (strncmp(userInput, simpleInsts[i], instLength) == 0){ // if user input contains the following instruction
            for (j = instLength; j < MAX_USER_INPUT; j++) { // loop through the rest of user input
                if (!isEmpty(userInput[j])){ // if other non empty character exists 
                    flag = 1; // flag is set to show that the user input is not the following instruction 
                    break;
                }
            }
            if(flag == 0){ // if the user input contains only the instruction
                strcpy(command, simpleInsts[i]);    // copy the instruction to command
                found = 1;  // set found to 1 
                flag = 0;  
            }
        }
    }
    if(found){ // if the user input is simple instruction in the right format
        return SUCCESS; // return 1
    }else
        return ERROR; // return 0 otherwise 
}

int isEmpty(char c){
    return (c== ' ' || c == '\t' || c == '\0' || c == '\n'); // if the character is one of the following, it is identified to be empty  
}
int isHexadecimal(char c){
 return ('0' <= c && c <= '9' ) || ('A' <= c && c  <= 'F') || ('a' <= c && c <= 'f'); // returns 1 if the character is in hexadecimal
}
void printErrorMessage(int type){ // print out error message for specific type 
    switch(type){
        case ERROR_PARAMETER : printf("Wrong parameter given..\n"); break;
        case ERROR_ADDRESS_OUT_OF_BOUND : printf("Address out of bound..\n"); break; 
        case ERROR_INPUT_FORMAT : printf("Wrong parameter given..\n"); break;
        case ERROR_PARAMETER_LENGTH :  printf("Parameter length cannot be bigger than 9..\n"); break;
        case ERROR_PARAMETER_OUT_OF_BOUND : printf("All parameter should be in between 0~0XFF..\n");break;
        default : printf("Wrong command.\n"); 
    }
}

int isComplexInst(){
    instLength = 0;
    for (int i = 0; i < 7; i++){ // loop through complex instruction list
        instLength = strlen(complexInsts[i]); // get length of the instruction 
        if (strncmp(userInput, complexInsts[i], instLength) == 0) { // if the user input includes following instruction
            if (isEmpty(userInput[instLength])) // check if the next character is empty
            { 
                strcpy(command, complexInsts[i]); // copy the instruction to command
                return SUCCESS; 
            }
        }   
    }
    return 0;  
}
int checkParams(){ 
   int comma = 0; 
   char input[10] = "";
   int paramLength = 0 ; 
   int last = 0;
   int idx1,idx2,idx3,i;
   for(i  = instLength; i < MAX_USER_INPUT; i++){ // search the string after the instruction 
       if(userInput[i]==','){
            comma++; // keep track of number of comma in user input
            if(comma==1){  
                idx1=i;  // store the location of the first comma in idx1 eg) 1,2    
             } else if(comma==2){ // if there is 2 commas 
                idx2=i;  // store the location of second comma in idx2 eg) 1,2,3
             }else {  // returns 0 because there should not be more than 3 commas 
                return ERROR;  // eg) 1,2,3,4
             }
       }
   }
   if(comma == 0 && (strncmp(userInput,"du",2)==0 || strncmp(userInput,"dump",4)==0)){ // for dump without any parameter
       for( i = instLength; i < MAX_USER_INPUT; i++){
           if(!isEmpty(userInput[i])){ // check the rest of string 
                break;
           }
       }
       if(i==MAX_USER_INPUT){ // if the string only contains dump 
        numOfParams = 0; // update numOfParams and return success code 
        return SUCCESS;  
       }
   }
   if(comma>=0){ // initial start 
       last = 0; 
       for(i = instLength; i<MAX_USER_INPUT; i++){
           if(userInput[i]=='\0' || userInput[i]==','){ // skip if there is second comma or it's end of string
               break;
           }
             if(!(isEmpty(userInput[i]))){ 
              if(isHexadecimal(userInput[i])){   
                input[last++] += userInput[i]; // store the parameter into input 
                if(last>9){ // if parameter is too long print out error message 
                   printErrorMessage(ERROR_PARAMETER_LENGTH);
                   return ERROR; 
                }
              }else{  // if non-hexadecimal is given as parameter, print out error
                  printErrorMessage(ERROR_INPUT_FORMAT); 
                  return ERROR;
              }
             }
           } 
       numOfParams = 1; // update numOfParams  
       strcpy(params[0],input); // store first parameter to params 
   }
   if(comma>=1){ // if there is at least one comma 
       memset(input,0,sizeof(input)); // initialize input array and last 
       last = 0;  
        for(i = idx1+1; i<MAX_USER_INPUT; i++){ // search from character next to the first comma 
           if(userInput[i]=='\0' || userInput[i]==','){ // skip if end of string or second comma
               break;
           }
           if(!(isEmpty(userInput[i]))){
              if(isHexadecimal(userInput[i])){     // store the hexadecimal value into input
                input[last++] += userInput[i]; 
                if(last>9){ // if parameter is too long, print out error message
                    printErrorMessage(ERROR_PARAMETER_LENGTH);
                    return ERROR; 
                }
              }else{ // print error for non-hexadecimal value 
                printErrorMessage(ERROR_INPUT_FORMAT);
                return ERROR;
              }
           }
       }
       numOfParams = 2; 
       strcpy(params[1],input); // store second parameter into params 
   }
   if(comma==2){ // if there are two commas
       memset(input,0,sizeof(input)); // initialize input and last
       last = 0;  
        for(i = idx2+1; i<MAX_USER_INPUT; i++){ // search from character next to second comma 
           if(userInput[i]=='\0'){ // break if end of string
               break;
           }
            if(!(isEmpty(userInput[i]))){
              if(isHexadecimal(userInput[i])){     // store the hexadecimal value into input
                input[last++] += userInput[i]; 
                if(last>9){ // if parameter is too long, print out error message
                    printErrorMessage(ERROR_PARAMETER_LENGTH);
                    return ERROR; 
                }
              }else{ // print error for non-hexadecimal value 
                printErrorMessage(ERROR_INPUT_FORMAT);
                return ERROR;
        }
       }
        }
       numOfParams = 3; 
       strcpy(params[2],input); // store third parameter into params
   }
   return 1; 
}

void printMem(long start, long end){
    long rowStart = start - start % 16; // calculate the first row address 
    long rowEnd = end - end % 16; // calulcate the last row address
    int value; 
    long i,j;
    for (i = rowStart; i <= rowEnd; i += 16) { // loop through rowStart until rowEnd, each incrementing by 16 
        printf("%05lX ", i); 
        for (j = i; j < i + 16; j++){
            if (j >= start && j <= end) { // if the address is within the start and end address
                value = VMemory[j];  // print out the memory 
                printf("%02X", value);
            }
            else{
                printf("  "); // print out empty space otherwise
            }
            printf(" ");
        }
        printf("; ");
        for (j = i; j < i + 16; j++){
            if (VMemory[j] >= 20 && VMemory[j] <= 0x7E) // if the value is within the range
                printf("%c", VMemory[j]); // print out the memory in character value 
            else
                printf("."); // print dot otherwise
        }
        printf("\n");
    }
}
void cleanEmptyStr(){ // converts userInput so it starts with non-empty character 
    char temp[MAX_USER_INPUT]; // create temporary array
    int i,j;
    for(i = 0 ; i < MAX_USER_INPUT; i++) // initialize temp with empty character
        temp[i] = ' ';

    for(i = 0 ; i < MAX_USER_INPUT; i++){ // loop through user input
        if(!(isEmpty(userInput[i]))){ // break if non-empty character 
            break;
        }
    }

    if(i==MAX_USER_INPUT) // if the entire string is empty, return 
        return;

    for(j = 0; j < MAX_USER_INPUT; j++){
        if(j+i<MAX_USER_INPUT)
           temp[j] = userInput[j+i]; // copy the content of userInput to temp, starting index of the first non-empty character
    }

     for(i = 0 ; i < MAX_USER_INPUT; i++) // erase the previous userInput
        userInput[i] = ' ';

    strcpy(userInput,temp);  // copy the temp content to userInput
}
int dumpMemory(){
    long start, end, rowStart, rowEnd;
    char *err;
    if (numOfParams == 0) { // if no parameter is given
        if (defaultStartAddr >= MAX_MEMORY_SIZE-1) // set defaultStartAddr to -1 if it exceeds MAX_MEMORY_SIZE 
            defaultStartAddr = -1;
        start = defaultStartAddr + 1;  // increment start value 
        end = start + (16 * 10 - 1); // set the end address to be 159 bigger than start addresss
        defaultStartAddr = end; // update global variable
    }
    else if (numOfParams == 1){ // if one parameter is given
        start = strtol(params[0], &err, 16); // set start address to the given parameter
        end = start + (16 * 10 - 1); 
    }
    else if (numOfParams == 2) { // if two parameters are given
        start = strtol(params[0], &err, 16); // set start
        end = strtol(params[1], &err, 16); // set end address 
    }
    else{
         printErrorMessage(ERROR_INPUT_FORMAT); // if more than two parameters are given, print out error message
         return ERROR;
    }
    if(end >= MAX_MEMORY_SIZE){ // if end address is bigger than MAX memory size
        end = MAX_MEMORY_SIZE-1;  // set end address to the end memory address
    }
    if(start > MAX_MEMORY_SIZE || start < 0 ){ // print error message if start is bigger than max memory or less than zero
       printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
       return ERROR;
    }
    if(start > end ){ // print error message if start is bigger than end 
       printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
       return ERROR;
    }
    printMem(start, end); // print out memory 
    return SUCCESS; 
}

int editMemory(){
    char *err;
    if (numOfParams == 2) { // if two parameter values are given
        long address = strtol(params[0], &err, 16); // convert each parameter to long type
        long value = strtol(params[1], &err, 16);
        if (address < 0 || address > MAX_MEMORY_SIZE-1){ // print error message if address is out of bound 
            printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
            return ERROR;
        }else if(value > 0xFF){ // print out error message if value is out of bound 
              printErrorMessage(ERROR_PARAMETER_OUT_OF_BOUND);
              return ERROR;
        }
        VMemory[address] = value; // change memory to the value 
        return SUCCESS; 
    }else{
        printErrorMessage(ERROR_PARAMETER); // if wrong number of parameter is given 
        return ERROR;
    }
}

void resetMemory(){
    memset(VMemory, 0, sizeof(VMemory)); // reset all virtual memory to zero
}
int fillMemory(){
    char *err;
    if (numOfParams == 3){ // if three parameters are given 
        long start = strtol(params[0], &err, 16);
        long end = strtol(params[1], &err, 16); // convert three parameters into long type
        long value = strtol(params[2], &err, 16);
        if (start < 0 || end < 0 || start >= MAX_MEMORY_SIZE || end >= MAX_MEMORY_SIZE) { // print out error if address is not in range 
            printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
            return ERROR;
        }
        else if (start > end){
            printf("start address should be bigger than end address\n");
            return 0;
        }
        if (value > 0xFF){
            printErrorMessage(ERROR_PARAMETER_OUT_OF_BOUND);
            return 0;
        }
        for (long i = start; i <= end; i++){ // update the memory value from start to end address 
            VMemory[i] = value;
        }
        return SUCCESS;
    }
    else{ 
        printErrorMessage(ERROR_PARAMETER); // if wrong number of parameter is given 
        return ERROR;
    }
    
}
void showOpcode(){ // print out all opcodes from hash table
    for(int i = 0; i < MAX_HASH_SIZE ; i++) { // loop through hash table
        Table_Element *elem = HashTable[i]; // get table element from hash table[i] 
        printf("%d :",i);
        if(elem == NULL) { // if the following table is empty, skip
            printf("\n");
            continue;
        }
        while(elem != NULL) { // loop through the element until it points to null 
            printf(" [%s,%02X]",elem->mnemonic,elem->opcode); // print out the mnemonic and opcode of the table element
            if(elem->next != NULL) { // print arrow if next element exists
                printf(" ->");
            }
            elem = elem->next; // continue to next pointer
        }
        printf("\n");
    }
}

int getHashKey(char * mnemonic) { // returns the hash key for the following mnemonic
    int key = mnemonic[0] + mnemonic[strlen(mnemonic)-1];
    return key%MAX_HASH_SIZE;
}

int getOpcode(){  // prints out opcode for the target mnemonic
    int key = getHashKey(targetMnemonic);  // finds hashkey 
    for(Table_Element *temp = HashTable[key]; temp != NULL; temp = temp -> next) { // loop through the following hash table
        if(strcmp(temp->mnemonic,targetMnemonic) == 0){ // if target mnemonic is found 
            printf("opcode is %X\n", temp->opcode); // print out the opcode
            return SUCCESS;
        }
    }
    printf("opcode is not found for %s\n", targetMnemonic); // if not found
    return ERROR;
}
int checkParamsMnemonic(){
    char tempM[10]; 
    memset(tempM,0,sizeof(tempM)); // intialize 
    int i,idx = 0;
    int lastIndex = 0;
    int flag = 0;
    for(i = 6; i < MAX_USER_INPUT; i++){ // loop through the rest of user input after 'opcode' 
        if(idx==10){
            printf("Opcode should not be longer than 9 letter\n");
            return ERROR; 
        }
        if(userInput[i]>='A'&&userInput[i]<='Z'){ // if the character is a capital letter
            tempM[idx++] = userInput[i];  // copy the letter to tempM
            flag = 1;
        }else if(!(isEmpty(userInput[i]))){ // if the character is not in capital letter 
            printf("opcode should be in capital letter\n"); // print error message
            return ERROR;
        }
        if(flag){
            if(isEmpty(userInput[i])) // if there has been a letter and currently it is empty, break 
                break;
        }
    }
    for(int j = i; j < MAX_USER_INPUT; j++) { // loop through to make sure the rest of the string is empty after the mnemonic
        if(!(isEmpty(userInput[j]))){  // if other characters exist after mnemonic
            printErrorMessage(ERROR_INPUT_FORMAT);
            return ERROR;
        }
    }
    strcpy(targetMnemonic, tempM); // copy tempM to targetMnemonic
    return SUCCESS;
}

void insertTableElement(int opcode, char * mnemonic, char * format ){
    Table_Element *newElem = (Table_Element*) malloc(sizeof(Table_Element)); // allocate new table element
    strcpy(newElem->mnemonic,mnemonic); // copy mnemonic and format string
    strcpy(newElem->format,format);
    newElem->opcode = opcode;
    int key = getHashKey(mnemonic); // get hash key for the following mnemonic
    if(HashTable[key]!=NULL){ //  if the element is not the first element to be inserted into the table with the specific hash key 
        newElem->next = HashTable[key]; // point to the current head 
        HashTable[key] = newElem; // update the head to new node
    }else { // if it is the first element
        HashTable[key] = newElem;  // update head node 
        newElem->next = NULL;  // point to null 
    }
}
int getCommand()
{
    int i; 
    int successful = 0; 
    for (i = 0; i < MAX_USER_INPUT; i++)
        userInput[i] = '\0';
    printf("sicsim> ");
    fgets(userInput, MAX_USER_INPUT, stdin);
    cleanEmptyStr();
    if (isSimpleInst()) {
        historyCount++;
        updateHistory(userInput);
        if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0){
            return QUIT;
        }
        else if (strcmp(command, "help") == 0 || strcmp(command, "h") == 0){
            printAllCommands();
        }
        else if (strcmp(command, "dir") == 0 || strcmp(command, "d") == 0){
            listdir(".");
        }
        else if (strcmp(command, "history") == 0 || strcmp(command, "hi") == 0){
            showHistory();
        }else if(strcmp(command,"reset")==0){
            resetMemory(); 
        }else if(strcmp(command,"opcodelist")==0){
            showOpcode(); 
        }
    }
    else if (isComplexInst()){
        instLength = strlen(command); 
        if ((strcmp(command, "dump") == 0 || strcmp(command, "du") == 0) && checkParams()){
            successful = dumpMemory();
        }else if((strcmp(command, "edit") == 0 || strcmp(command, "e") == 0) && checkParams()){
            successful = editMemory();
        }else if((strcmp(command, "fill") == 0 || strcmp(command, "f") == 0)  && checkParams()){
            successful = fillMemory();
        }else if(strcmp(command, "opcode") == 0 && checkParamsMnemonic()){
            successful = getOpcode();
        }
    }
    if(successful){
         historyCount++;
         updateHistory(userInput);
    }
    return 1;
}


int main()
{
    FILE * ip;
    int opcode;
    char mnemonic[7];
    char format[5];  // temporary storage for storing opcode info 
    char *err;
    ip = fopen("opcode.txt","r");
    if(ip==NULL){ // print error message if opcode text file is not found 
        printf("opcode file does not exist\n");
        return 0;
    }
    while(1){
        fscanf(ip,"%X%s%s", &opcode, mnemonic, format); // read each line from opcode text file
        if(feof(ip)) // break if reached end of file
            break;
        insertTableElement(opcode,mnemonic,format); // insert opcode info into hash table

    }
    resetMemory(); // initialize virtual memory 
    while (getCommand() != QUIT) { // get command from user and process, until quit command is given 
    }
    return 0;
}
