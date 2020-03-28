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
    long rowStart = start - start % 16;
    long rowEnd = end - end % 16;
    for (long i = rowStart; i <= rowEnd; i += 16) {
        printf("%05lX ", i);
        for (long j = i; j < i + 16; j++){
            if (j < start || j > end) {
                printf("  ");
            }
            else{
                int val = VMemory[j];
                printf("%02X", val);
            }
            printf(" ");
        }
        printf("; ");
        for (long j = i; j < i + 16; j++){
            if (VMemory[j] >= 20 && VMemory[j] <= 0x7E)
                printf("%c", VMemory[j]);
            else
                printf(".");
        }
        printf("\n");
    }
}
void cleanEmptyStr(){
    char temp[MAX_USER_INPUT]; 
    int i,j;
    for(i = 0 ; i < MAX_USER_INPUT; i++)
        temp[i] = ' ';
    for(i = 0 ; i < MAX_USER_INPUT; i++){
        if(!(isEmpty(userInput[i]))){
            break;
        }
    }
    for(j = 0; j < MAX_USER_INPUT; j++){
        temp[j] = userInput[j+i]; 
    }

     for(i = 0 ; i < MAX_USER_INPUT; i++)
        userInput[i] = ' ';

    strcpy(userInput,temp); 
}
int dumpMemory(){
    long start, end, rowStart, rowEnd;
    char *err;
    if (numOfParams == 0) {
        if (defaultStartAddr >= MAX_MEMORY_SIZE)
            defaultStartAddr = 0;
        start = defaultStartAddr + 1;
        end = start + (16 * 10 - 1);
        defaultStartAddr = end;
    }
    else if (numOfParams == 1){
        start = strtol(params[0], &err, 16);
        end = start + (16 * 10 - 1);
    }
    else if (numOfParams == 2) {
        start = strtol(params[0], &err, 16);
        end = strtol(params[1], &err, 16);
    }
    else{
         printErrorMessage(ERROR_INPUT_FORMAT);
         return ERROR;
    }
    if(end >= MAX_MEMORY_SIZE){
        end = MAX_MEMORY_SIZE-1; 
    }
    if(start > MAX_MEMORY_SIZE || start < 0 ){
       printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
       return ERROR;
    }
    if(start > end ){
       printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
       return ERROR;
    }
    printMem(start, end);
    return SUCCESS; 
}

int editMemory(){
    char *err;
    if (numOfParams == 2) {
        long address = strtol(params[0], &err, 16);
        long value = strtol(params[1], &err, 16);
        if (address < 0 || address > MAX_MEMORY_SIZE-1){
            printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
            return ERROR;
        }else if(value > 0xFF){
              printErrorMessage(ERROR_PARAMETER_OUT_OF_BOUND);
              return ERROR;
        }
        VMemory[address] = value;
        return SUCCESS; 
    }else{
        printErrorMessage(ERROR_PARAMETER);
        return ERROR;
    }
}

void resetMemory(){
    memset(VMemory, 0, sizeof(VMemory));
}
int fillMemory(){
    char *err;
    if (numOfParams == 3){
        long start = strtol(params[0], &err, 16);
        long end = strtol(params[1], &err, 16);
        long value = strtol(params[2], &err, 16);
        if (start < 0 || end < 0 || start >= MAX_MEMORY_SIZE || end >= MAX_MEMORY_SIZE) {
            printf("Memory out of bound\n");
            return 0;
        }
        else if (start > end){
            printf("start address should be bigger than end address\n");
            return 0;
        }
        if (value > 0xFF){
            printf("You can only change with a value within the range of 00~FF.\n");
            return 0;
        }
        for (long i = start; i <= end; i++){
            VMemory[i] = value;
        }
        return 1;
    }
    else{
        printf("Wrong parameter input\n");
        return 0;
    }
    return 1;
}
void showOpcode(){
    for(int i = 0; i < MAX_HASH_SIZE ; i++) {
        Table_Element *elem = HashTable[i];
        printf("%d :",i);
        if(elem == NULL) {
            printf("\n");
            continue;
        }
        while(elem != NULL) {
            printf(" [%s,%02X]",elem->mnemonic,elem->opcode);
            if(elem->next != NULL) {
                printf(" ->");
            }
            elem = elem->next;
        }
        printf("\n");
    }
}

int getHashKey(char * mnemonic) {
    int key = mnemonic[0] + mnemonic[strlen(mnemonic)];
    return key%MAX_HASH_SIZE;
}

int getOpcode(){
    int key = getHashKey(targetMnemonic); 
    for(Table_Element *temp = HashTable[key]; temp != NULL; temp = temp -> next) {
        if(strcmp(temp->mnemonic,targetMnemonic) == 0){
            printf("opcode is %X\n", temp->opcode);
            return SUCCESS;
        }
    }
    printf("opcode is not found for %s\n", targetMnemonic);
    return ERROR;
}
int checkParamsMnemonic(){
    char tempM[7];
    memset(tempM,0,sizeof(tempM));
    int i,idx = 0;
    int lastIndex = 0;
    int flag = 0;
    for(i = 6; i < MAX_USER_INPUT-1; i++){
        if(userInput[i]>='A'&&userInput[i]<='Z'){
            if(idx>=6){
                return ERROR;
            }
            tempM[idx++] = userInput[i]; 
            flag = 1;
        }else if(!(isEmpty(userInput[i]))){
            printf("opcode should be in capital letter\n");
            return ERROR;
        }
        if(flag){
            if(isEmpty(userInput[i]))
                break;
        }
    }
    for(int j = i; j < MAX_USER_INPUT-1; j++) {
        if(!(isEmpty(userInput[j]))){
            printf("wrong input\n");
            return ERROR;
        }
    }
    strcpy(targetMnemonic, tempM); 
    return SUCCESS;
}

void insertTableElement(int opcode, char * mnemonic, char * format ){
    Table_Element *newElem = (Table_Element*) malloc(sizeof(Table_Element)); 
    strcpy(newElem->mnemonic,mnemonic);
    strcpy(newElem->format,format);
    newElem->opcode = opcode;
    int key = getHashKey(mnemonic);
    if(HashTable[key]!=NULL){ //  해당 table key 의 첫 element 일 경우 
        newElem->next = HashTable[key];
        HashTable[key] = newElem;
    }else { // 이미 key에 element가 존재할때 
        HashTable[key] = newElem;
        newElem->next = NULL; 
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
    char format[5];
    char *err;
    ip = fopen("opcode.txt","r");
    if(ip==NULL){
        printf("opcode file does not exist\n");
        return 0;
    }
    while(1){
        fscanf(ip,"%X%s%s", &opcode, mnemonic, format);
        if(feof(ip))
            break;
        insertTableElement(opcode,mnemonic,format); 

    }
    resetMemory();
    while (getCommand() != QUIT) {
    }
    return 0;
}
