#include "20161619.h"

void updateHistory(char *command){
    h_node temp = (History_Node *)malloc(sizeof(History_Node) * 1); // allocate history node
    temp->n = historyCount; // store current number of history stored into n 
    temp->next = NULL;
    temp->s = (char *)malloc(sizeof(char) * strlen(command)); // dynamically allocate char array 
    if(strlen(command)>0)
        command[strlen(command)-1]='\0'; // delete enter
    strcpy(temp->s, command); // copy command to s 
    if (head == NULL){ // if there is no previous history node stored
        head = temp; // update head and current
        current = head;
    } else{
        current->next = temp; // otherwise add the new node to  the linked list
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
    return SUCCESS;
}

void printAllCommands(){ // print out all the commands
    printf("h[elp]\nd[ir]\nq[uit]\ndu[mp] [start, end] \ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\nassemble filename\ntype filename\nsymbol\n");
}

int isSimpleInst(){
    int i,j,flag,found = 0;
    for (i = 0; i < 11; i++) { // loop through simple instruction list to find out it matches the user input 
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

int readFile(char * file){
    char c; 
    FILE * in = fopen(file,"r"); ;
    if(in==NULL){
        printf("file is not found in the current directory\n");
        return ERROR; 
    }
     while ((c = getc(in)) != EOF)
        putchar(c);
    
    fclose(in);

    return SUCCESS; 
}
int isEmpty(char c){
    return (c== ' ' || c == '\t' || c == '\0' || c == '\n'); // if the character is one of the following, it is identified to be empty  
}
int isHexadecimal(char c){
 return ('0' <= c && c <= '9' ) || ('A' <= c && c  <= 'F') || ('a' <= c && c <= 'f'); // returns 1 if the character is in hexadecimal
}
void printErrorMessage(int type){ // print out error message for specific type 
        if(type==ERROR_PARAMETER) 
		printf("Wrong command format given..\n");
        else if(type== ERROR_ADDRESS_OUT_OF_BOUND)
		 printf("Address out of bound..\n");  
        else if(type==ERROR_INPUT_FORMAT)
		 printf("Wrong parameter given..\n"); 
        else if(type== ERROR_PARAMETER_LENGTH)
		  printf("Parameter length cannot be bigger than 9..\n"); 
        else if(type== ERROR_PARAMETER_OUT_OF_BOUND)
		 printf("All parameter should be in between 0~0XFF..\n");
        else
		printf("Wrong command.\n"); 
    
}

int isComplexInst(){
    instLength = 0;
    for (int i = 0; i < 9; i++){ // loop through complex instruction list
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

int checkFilename(){
    int i,j,lenF,lenE, lenT;
    int numOfPeriod = 0;
    int filenameLength = 0;
    lenF = 0; 
    lenE = 0;
    lenT = 0;
    for(int i = 0; i < 100; i++){
        fullFileName[i] = '\0';
        filename[i] = '\0'; 
        extension[i] = '\0';
    }
    for(i = instLength; i < MAX_USER_INPUT; i++){
        if(!isEmpty(userInput[i])){
            break;
        }
    }
    if(i==MAX_USER_INPUT)
        return ERROR;
    
    for(j = i; j < MAX_USER_INPUT; j++){
        if(isEmpty(userInput[j])){
            break; 
        }
        if(userInput[j]=='.'){
            fullFileName[lenT++] = userInput[j];
            numOfPeriod++;
        }
        if(numOfPeriod>1){
            return ERROR;
        }
        if(numOfPeriod==0 && userInput[j]!= '.'){
            filename[lenF++] = userInput[j]; 
            fullFileName[lenT++] = userInput[j];
        }else if(numOfPeriod==1 && userInput[j]!='.'){
            extension[lenE++] = userInput[j]; 
            fullFileName[lenT++] = userInput[j];
        }
    }
    return SUCCESS;     
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
                printf("there should not be more than 3 parameters..\n"); 
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
        if(end > MAX_MEMORY_SIZE-1 ) // if end address is bigger than MAX memory size
        end = MAX_MEMORY_SIZE-1;  // set end address to the end memory address
    }
    else if (numOfParams == 1){ // if one parameter is given
        start = strtol(params[0], &err, 16); // set start address to the given parameter
        end = start + (16 * 10 - 1); 
        if(end > MAX_MEMORY_SIZE-1 )// if end address is bigger than MAX memory size
        end = MAX_MEMORY_SIZE-1;  // set end address to the end memory address
    }else if (numOfParams == 2) { // if two parameters are given
        start = strtol(params[0], &err, 16); // set start
        end = strtol(params[1], &err, 16); // set end address 
    }
    else{
         printErrorMessage(ERROR_INPUT_FORMAT); // if more than two parameters are given, print out error message
         return ERROR;
    }

    if(start > MAX_MEMORY_SIZE || end > MAX_MEMORY_SIZE || start < 0 || end < 0){ // print error message if start is bigger than max memory or less than zero    printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
      printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
       return ERROR;
    }

    if(start > end ){ // print error message if start is bigger than end 
       printErrorMessage(ERROR_ADDRESS_OUT_OF_BOUND);
       return ERROR;
    }
    
    if(end > MAX_MEMORY_SIZE-1 ){ // if end address is bigger than MAX memory size
        end = MAX_MEMORY_SIZE-1;  // set end address to the end memory address
    }
  
    defaultStartAddr = end; // update global variable
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
        }else if(value > 0xFF|| value < 0 ){ // print out error message if value is out of bound 
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
            return ERROR;
        }
        if (value > 0xFF || value < 0){
            printErrorMessage(ERROR_PARAMETER_OUT_OF_BOUND);
            return ERROR;
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
     Table_Element *elem;
     int i = 0; 
     while(i<MAX_HASH_SIZE){
        elem = HashTable[i]; // get table element from hash table[i] 
        printf("%d :",i);
        if(elem == NULL) { // if the following table is empty, skip
            printf("\n");
            continue;
        }
        
        while(elem != NULL) { // loop through the element until it points to null 
            printf(" [%s,%02lX]",elem->mnemonic,elem->opcode); // print out the mnemonic and opcode of the table element
            if(elem->next != NULL) { // print arrow if next element exists
                printf(" ->");
            }
            elem = elem->next; // continue to next pointer
        }
        printf("\n");
        i++;
    }
    
}
void showSymbol(){ // print out all opcodes from hash table
     Symbol_Element *elem;
     int i = 0; 
     int found = 0; 
     while(i<SYMBOL_HASH_SIZE){
        elem = SymbolTable[i]; // get table element from hash table[i] 
        if(elem == NULL) { // if the following table is empty, skip
            i++;
            continue;
        }
        while(elem != NULL) { // loop through the element until it points to null 
            found = 1;
            printf("\t%s\t%04lX\n",elem->identifier,elem->address); // print out the mnemonic and opcode of the table element
            elem = elem->next; // continue to next pointer
        }
        i++;
    }
    if(!found){
        printf("Symbol Table is not found\n"); 
    }
}
int getHashKey(char * mnemonic) { // returns the hash key for the following mnemonic
    int key = 0;
    key = mnemonic[0] + mnemonic[strlen(mnemonic)-1];
    return key%MAX_HASH_SIZE;
}

int getHashKeySymbol(char * symbol) {
    int key = 0;
    key = symbol[0]-'A'; 
    return key%SYMBOL_HASH_SIZE;
}

long getAddress(char * symbol){
    int key = getHashKeySymbol(symbol); 
    for(Symbol_Element * temp = SymbolTable[key]; temp != NULL; temp= temp -> next){
        if(strcmp(temp->identifier,symbol)==0){
            return temp->address;
        }
    }
    return -1;
}
Table_Element * getElement(char * mnemonic){
    int key = getHashKey(mnemonic);  // finds hashkey 
    for(Table_Element *temp = HashTable[key]; temp != NULL; temp = temp -> next) { // loop through the following hash table
        if(strcmp(temp->mnemonic,mnemonic) == 0){ // if target mnemonic is found 
             return temp;
        }
    }
    return NULL;
}
long getOpcode(char * mnemonic, int option){  // prints out opcode for the target mnemonic
    Table_Element * elem = getElement(mnemonic); 
    if(elem==NULL){
        if(option==0)
         printf("opcode is not found for %s\n", mnemonic); // if not found
     return -1;
    }else{
        if(option==0)
          printf("opcode for %s is %02lX\n", mnemonic, elem->opcode); 
     return elem->opcode;
    }
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
    newElem->next = NULL; 
    int key = getHashKey(mnemonic); // get hash key for the following mnemonic
    if(HashTable[key]!=NULL){ //  if the element is not the first element to be inserted into the table with the specific hash key 
        newElem->next = HashTable[key]; // point to the current head 
        HashTable[key] = newElem; // update the head to new node
    }else { // if it is the first element
        HashTable[key] = newElem;  // update head node 
    }

}

Symbol_Element * getSymbol(char * identifier){
    int key = getHashKeySymbol(identifier);
    for(Symbol_Element *temp = SymbolTable[key]; temp != NULL; temp = temp -> next) { // loop through the following hash table
        if(strcmp(temp->identifier,identifier) == 0){ // if target mnemonic is found 
            return temp;
        }
    }
    return NULL; 
}
int insertSymbolElement(char * identifier, long address, char * type, char * value){
    Symbol_Element *newElem = (Symbol_Element*) malloc(sizeof(Symbol_Element)); // allocate new table element
    Symbol_Element *temp, *previous = NULL; 
    strcpy(newElem->identifier,identifier); // copy mnemonic and format string
    newElem->address= address;
    strcpy(newElem->type,type);
    strcpy(newElem->value,value);
    int key = getHashKeySymbol(identifier); // get hash key for the following mnemonic
     if(SymbolTable[key]!=NULL){ //  if the element is not the first element to be inserted into the table with the specific hash key 
        if(SymbolTable[key]->next==NULL){
             if(strcmp(SymbolTable[key]->identifier,identifier)>0){
                  newElem->next = SymbolTable[key];
                  SymbolTable[key] = newElem;
             }else{
                 SymbolTable[key]->next = newElem;
             }
             return SUCCESS; 
        }
        for(temp = SymbolTable[key];  temp->next != NULL; temp=temp->next){
            if(strcmp(temp->identifier,identifier)==0){
                printf("Symbol is already defined\n"); 
                return ERROR; 
            }else if(strcmp(temp->identifier,identifier)>0){
                if(previous==NULL){
                    newElem->next = temp; 
                    SymbolTable[key] = newElem;
                    return SUCCESS;
                }else {
                    newElem->next = previous->next;
                    previous->next = newElem; 
                    return SUCCESS;
                }
            }
            previous = temp; 
        }
        temp->next = newElem;
        return SUCCESS;
       // update the head to new node
    }else { // if it is the first element
        SymbolTable[key] = newElem;  // update head node 
        return SUCCESS;
    }
}
void freeSymbolTable(){
      for(int i = SYMBOL_HASH_SIZE-1 ; i >= 0; i--) {
        Symbol_Element *remove_node = SymbolTable[i];
        SymbolTable[i] = NULL; 
    }
}
long getByteSize(char * str){
    long count = 0;
    char * err;
    if(str[0]=='C'){
        for(int i = 1 ; i < strlen(str); i++){
            if(str[i]!='\''){
                count++;
            }
        }
    }else if(str[0]=='X'){
        count = 1; 
    }else{
            return strtol(str,&err,10);
    }
    return count; 
}
void parseLine( char * line, int option ){
    int variableOrConstant = 0;
    int numChar = 0;
    int needToIncrement = 0; 
    char * err;
    numWord = 0;
    locationOfOpcode = -1;
    format = 0;  
    isVariable = 0;
    isFormatFour = 0; 
    for(int i = 0; i < 5; i++){
        for(int j = 0 ; j < 20; j++)
          operand[i][j] = '\0'; 
    }
    if(line[0]=='.'){
        needToPrint = 0;
        return;
    }

    for(int i = 0; i < line_size-1; i++){
        if(line[i]=='\0' || line[i]=='\n'){
            break;
        }
        if(!isEmpty(line[i])&&line[i]!=','){
            operand[numWord][numChar++] = line[i]; 
            needToIncrement = 1; 
            if(isEmpty(line[i+1])|| line[i+1]==','){
                numWord++;
                numChar = 0;
            }
        }
    }
    if(getOpcode(operand[0],1)==-1 ){ // Not Opcode 
        if(strcmp(operand[0],"BASE")==0){
            strcpy(base,operand[1]);
            needToPrint = 0;
        }
        else if(strcmp(operand[1],"START")==0){
            strcpy(title,operand[1]);
            LOCCTR = strtol(operand[2], &err, 16);
            needToPrint = 0;    
        } else if(strcmp(operand[0], "END")==0){
            LOCCTR += 1;
            endFound = 1; 
            needToPrint = 0;
        }else if(strcmp(operand[1],"RESB")==0){
            LOCCTR += getByteSize(operand[2]);
            variableOrConstant = 1;
            isVariable = 1;
        }else if(strcmp(operand[1],"RESW")==0){
            LOCCTR += 3*getByteSize(operand[2]);
            variableOrConstant = 1;
            isVariable = 1;
        }else if(strcmp(operand[1],"WORD")==0){
            LOCCTR += 3*getByteSize(operand[2]);
            variableOrConstant = 1;
        }else if(strcmp(operand[1],"BYTE")==0){
            LOCCTR += getByteSize(operand[2]);
            variableOrConstant = 1;
        }else {
            if(numWord == 3 ){
                if(option==0)
                    insertSymbolElement(operand[0],previousLOCCTR,"ROUTINE","ROUTINE"); 
            locationOfOpcode = 1;
            }else
                locationOfOpcode = 0; 
        }
        if(variableOrConstant){
             if(option==0)
                insertSymbolElement(operand[0],previousLOCCTR,operand[1],operand[2]);  
             locationOfOpcode = -1;
        }
    }else{
        locationOfOpcode = 0; 
    }
    if(locationOfOpcode>=0){
        trueMnemonic = (char*)malloc(sizeof(operand[locationOfOpcode]));
        strcpy(trueMnemonic,operand[locationOfOpcode]);
    if(operand[locationOfOpcode][0]=='+'){
        isFormatFour = 1; 
        trueMnemonic = strtok(trueMnemonic ,"+");
    }
        Table_Element *elem = getElement(trueMnemonic);
        if(elem!=NULL){
        if(strcmp(elem->format,"1")==0){
            LOCCTR++;
            format = 1;
        }else if(strcmp(elem->format,"2")==0){
            LOCCTR+=2;
            format = 2;
        }else if(strcmp(elem->format,"3/4")==0){
            if(isFormatFour){
                format = 4;
                LOCCTR+=4;
            }else{
                format = 3;
                LOCCTR+=3;
            } 
        }
        }
    }
    
} 
long getRegisterNumber(char * r){
    long objcode = 0;
    if(strcmp(r,"A") == 0) {
        objcode += 0;
    } else if(strcmp(r,"X") == 0) {
        objcode += 1;
    } else if(strcmp(r,"L") == 0) {
         objcode += 2;
    } else if(strcmp(r,"B") == 0) {
        objcode += 3;
    } else if(strcmp(r,"S") == 0) {
        objcode += 4;
    } else if(strcmp(r,"T") == 0) {
        objcode += 5;
    } else if(strcmp(r,"F") == 0) {
        objcode += 6;
    } else if(strcmp(r,"PC") == 0) {
        objcode += 8;
    } else if(strcmp(r,"SW") == 0) {
        objcode += 9;
    }else 
        return -1;
    return objcode*16; 
}
void insertObjectCode(long address, char * objcode){
    Obj_Element * newNode = (Obj_Element*)malloc(sizeof(Obj_Element));
    newNode->objcode = (char*)malloc(sizeof(char)*strlen(objcode));
    strcpy(newNode->objcode,objcode);
    newNode->address = address;
    newNode->next = NULL;
    if(T_head==NULL){
        T_head = newNode;
        T_last = T_head;
    }else{
        T_last->next = newNode;
        T_last = newNode;
    }
}
void insertRelocationNode(long startingAddr, long length){
   Relocation_Element * newNode = (Relocation_Element*)malloc(sizeof(Relocation_Element));
   newNode->length = length;
   newNode->startingAddr = startingAddr;
   newNode->next = NULL;
    if(R_head==NULL){
        R_head = newNode;
        R_last = R_head;
    }else{
        R_last->next = newNode;
        R_last = newNode;
    }
}
long calculateObjectCode(char * line){
    long objectCode = 0;
    long reg = 0;
    long opcode; 
    long disp; 
    long labelLoc;
    char * err;
    int j=0;
    int constantValue=0;
    int n=0,i=0,x=0,b=0,p = 0,e = 0;
    isConstant = 0;
    isX = 0; 
    int ascii;
    char * trueOperand; 
    char v[100];
    memset(v,0,sizeof(v));
    if(locationOfOpcode==-1&&numWord>=2){
        if(strcmp(operand[numWord-2],"BYTE")==0|| strcmp(operand[numWord-2],"WORD")==0){
        if(operand[numWord-1][0]=='X'){
            for(int i = 1; i < strlen(operand[numWord-1]);i++){
                if(operand[numWord-1][i]!='\''){
                    v[j++] =operand[numWord-1][i];
                }
            }
            isX = 1;
            objectCode = strtol(v,&err,16);
        }else if(operand[numWord-1][0]=='C'){
            for(int i = 1; i < strlen(operand[numWord-1]);i++){
                if(operand[numWord-1][i]!='\''){
                    objectCode *= 256;
                    objectCode += operand[numWord-1][i];
                }
            }
            isConstant = 1;
        }
        return objectCode;  
        }else
            return -1;
    }
    opcode = getOpcode(trueMnemonic,1); 
    if(strcmp(trueMnemonic,"RSUB")==0){
        objectCode += opcode+3; 
        objectCode = objectCode << 16;
        return objectCode;
    }
    if(format==1)
        return opcode;
    
    if(format==2 && numWord - locationOfOpcode >= 1 ){
        objectCode = opcode; 
        objectCode = objectCode << 8;
        reg = getRegisterNumber(operand[locationOfOpcode+1]); 
        if(reg < 0){
            printf("Wrong register input..\n");
            return -1; 
        }
        objectCode += reg;  
        objectCode = objectCode << 4;
        if(numWord - locationOfOpcode >= 2){
            reg = getRegisterNumber(operand[locationOfOpcode+2]);
            if(reg > 0){ 
                objectCode += reg; 
            }
        }
        objectCode /= 16;
        return objectCode;
    }else{
        objectCode = opcode;
        if(numWord-locationOfOpcode > 2){
            if(strcmp(operand[locationOfOpcode+2],"X")==0) // X register used
                x = 1;
            else if(operand[locationOfOpcode+2][0]!='\0'){
                printf("3/4 format can only have X as 2nd operand\n");
                return -1; 
            }
        }
        if(format==4){
            e = 1;
        }
        if(numWord - locationOfOpcode == 0){
            printf("should be an operand!\n"); 
            return -1;
        }
        if(operand[locationOfOpcode+1][0]=='#'){ // immediate
            trueOperand = (char*)malloc(sizeof(operand[locationOfOpcode+1]));
             strcpy( trueOperand ,operand[locationOfOpcode+1]);
             trueOperand = strtok( trueOperand ,"#");
             n = 0;
             i = 1;
        }else if(operand[locationOfOpcode+1][0]=='@'){ // indirect
             trueOperand = (char*)malloc(sizeof(operand[locationOfOpcode+1]));
             strcpy( trueOperand ,operand[locationOfOpcode+1]);
             trueOperand = strtok( trueOperand ,"@");
             n = 1; 
             i = 0;
        }else { // simple 
             trueOperand = (char*)malloc(sizeof(operand[locationOfOpcode+1]));
             strcpy( trueOperand ,operand[locationOfOpcode+1]);
             n = 1;
             i = 1;
        }

        if(trueOperand[0]>='A' && trueOperand[0]<='Z'){
            labelLoc = getAddress(trueOperand);
        }else{
            if(i==1){
            constantValue = 1;
            labelLoc = strtol(trueOperand, &err, 10);
            }
        }
        if(constantValue==1)
            disp = labelLoc;
        else 
            disp = labelLoc - LOCCTR;
        
        if(format==3){
        if(-2048 <=  disp && disp <= 2047 && constantValue != 1) {
            p = 1;
            b = 0;
            if(disp < 0) 
                 disp = 0x1000 + disp;
        }else if (disp <= 4095 && constantValue != 1) {  //BASE relativ로 표현 가능한가?
            disp = labelLoc - baseLoc;
            b = 1;
            p = 0;
        }else{ // direct addressing
            b = 0;
            p = 0;
        }
        }else{ // format 4 
            disp = labelLoc; 
            b = 0;
            p = 0; 
        }
        objectCode = objectCode << 12;   
        objectCode = opcode;      
        objectCode = objectCode + n*2 + i;   
        objectCode = objectCode << 4;
        objectCode = objectCode + x*8 + b*4 + p*2 + e; 

        if(format==4){
            objectCode = objectCode << 20;
        }else
            objectCode = objectCode << 12; 
        objectCode += disp; 
    }
   return objectCode;
}
int passTwo(char * fileName){
    FILE *obj = fopen("test","w");
    FILE *in = fopen(fileName,"r");
    FILE *out = fopen(filename,"w");
    Obj_Element * rem;
    LOCCTR = 0;
    previousLOCCTR = 0;
    endFound = 0;
    char c;
    int i = 0; 
    long objectCode;
    int numOfLines = 0;
    char line[line_size];
    if(in==NULL){
        printf("file not found]\n");
        return ERROR;
    }
    while(1){
       c = fgetc(in);
      if( feof(in) ) {
         break ;
      }
      if(c=='\n'){
          numOfLines+=5;
          needToPrint = 1; 
          parseLine(line,1);
          memset(TEMP_BUFFER,0,sizeof(TEMP_BUFFER));
          if(needToPrint){
            objectCode = calculateObjectCode(line);
           if(objectCode >= 0){
               if(isX){
                    fprintf(out, "%-5d\t%04lX\t%-8s\t\t%02lX\n", numOfLines,previousLOCCTR,line,objectCode);
                    sprintf(TEMP_BUFFER,"%02lX",objectCode);
                    if(T_head!=NULL&&LOCCTR-T_head->address > 0x1E) {
                        printf("T%06lX", previousLOCCTR-T_head->address );
                         while(T_head != NULL){
                             rem = T_head;
                             T_head = T_head->next;
                             printf("%s", rem->objcode);
                             free(rem);
                         }
                     T_head = NULL;
                     T_last = NULL;
                     printf("\n");
                    }
                      insertObjectCode(previousLOCCTR,TEMP_BUFFER);
                }else if(isConstant){
                     fprintf(out, "%-5d\t%04lX\t%-8s\t\t%lX\n", numOfLines,previousLOCCTR,line,objectCode);
                     sprintf(TEMP_BUFFER,"%lX",objectCode);
                    
                    if(T_head!=NULL&&LOCCTR-T_head->address > 0x1E) {
                        printf("T%06lX", previousLOCCTR-T_head->address );
                         while(T_head != NULL){
                             rem = T_head;
                             T_head = T_head->next;
                             printf("%s", rem->objcode);
                             free(rem);
                         }
                     T_head = NULL;
                     T_last = NULL;
                     printf("\n");
                    }
                     insertObjectCode(previousLOCCTR,TEMP_BUFFER);
               }else if(format==1|| format==2 || strcmp(trueMnemonic,"RSUB")==0 ){
                        fprintf(out, "%-5d\t%04lX\t%-8s\t\t",numOfLines,previousLOCCTR,line);
                   if(numWord==2 || (numWord==3 && strcmp(operand[locationOfOpcode+1],"X")==0)||strcmp(trueMnemonic,"RSUB")==0)  
                        fprintf(out,"\t");
                
                fprintf(out,"%lX\n", objectCode);
                sprintf(TEMP_BUFFER,"%lX",objectCode);
                
               if(T_head!=NULL&&LOCCTR-T_head->address > 0x1E) {
                        printf("T%06lX", previousLOCCTR-T_head->address );
                         while(T_head != NULL){
                             rem = T_head;
                             T_head = T_head->next;
                             printf("%s", rem->objcode);
                             free(rem);
                         }
                     T_head = NULL;
                     T_last = NULL;
                     printf("\n");
                    }
                    insertObjectCode(previousLOCCTR,TEMP_BUFFER);
                }
                else{
                    fprintf(out, "%-5d\t%04lX\t%-8s\t\t%06lX\n", numOfLines,previousLOCCTR,line,objectCode);
                    sprintf(TEMP_BUFFER,"%06lX",objectCode);
                      if(T_head!=NULL && LOCCTR-T_head->address > 0x1E) {
                          printf("T%06lX", previousLOCCTR-T_head->address );
                          while(T_head != NULL){
                              rem = T_head;
                              T_head = T_head->next;
                              printf("%s", rem->objcode);
                            free(rem);
                      }
                     T_head = NULL;
                     T_last = NULL;
                     printf("\n");
                  }
                   insertObjectCode(previousLOCCTR,TEMP_BUFFER);
                }
            }else{
                if(isVariable&&T_head!=NULL&&T_last!=NULL){
                    printf("T%06lX", previousLOCCTR-T_head->address );
                 if(T_head!=NULL ) {
                          printf("T%06lX", previousLOCCTR-T_head->address );
                          while(T_head != NULL){
                              rem = T_head;
                              T_head = T_head->next;
                              printf("%s", rem->objcode);
                            free(rem);
                      }
                     T_head = NULL;
                     T_last = NULL;
                     printf("\n");
                }
                }
                fprintf(out, "%-5d\t%04lX\t%-12s\n", numOfLines,previousLOCCTR,line);
            }
        }
        else{
          fprintf(out, "%-5d\t\t%-12s\n", numOfLines,line);
        }
        previousLOCCTR = LOCCTR; 
        memset(line,0,sizeof(line)); // initialize input and last
        i = 0;
      if(endFound){
              break;
          }
      }else {
          line[i++] = c; 
      }
     
    }
   if(T_head!=NULL&&T_last!=NULL){
       printf("T%06lX", previousLOCCTR-T_head->address);
        while(T_head != NULL){
           rem = T_head;
            T_head = T_head->next;
             printf("%s", rem->objcode);
             free(rem);
        }
        T_head = NULL;
        T_last = NULL;
        printf("\n");
    }
    fclose(out);
    fclose(in); 
    return SUCCESS;
}
int passOne(char * fileName){
    char lstName [20];
    FILE *in = fopen(fileName,"r");
    FILE *out = fopen(strcat(filename,".lst"),"w");
    LOCCTR = 0; 
    previousLOCCTR = 0;
    endFound = 0;
    int numOfLines = 0;
    char line[line_size];
    char c; 
    int i = 0; 
    if(in==NULL){
        printf("file not found]\n");
        return ERROR;
    }
    while(1){
       c = fgetc(in);
      if( feof(in) ) {
         break ;
      }
      if(c=='\n'){
          numOfLines+=5;
          needToPrint = 1; 
          parseLine(line,0);
          previousLOCCTR = LOCCTR; 
          memset(line,0,sizeof(line)); // initialize input and last
          i = 0;
      if(endFound){
              break;
          }
      }else {
          line[i++] = c; 
      }
     
    }
    fclose(out);
    fclose(in); 
    return SUCCESS;
}
int assemble(char * fileName){
    char * err; 
    memset(base,0,sizeof(base));
    freeSymbolTable(); 
    if(!passOne(fileName)){
        printf("Wrong assembly code..\n");
        return ERROR; 
    } 
    if(base[0]!='\0'){
        baseLoc = getAddress(base);
    }
    passTwo(fileName); 
    printf("successfully assembled %s\n", fileName);
    return SUCCESS; 
}
int getCommand()
{
    int i; 
    int successful = 0; 
    for (i = 0; i < MAX_USER_INPUT; i++)
        userInput[i] = '\0';
    printf("sicsim> ");
    fgets(userInput, MAX_USER_INPUT, stdin);
    cleanEmptyStr();  // remove unnessary empty string 
    if (isSimpleInst()) { // user command is one of the simple insturctions 
        historyCount++;
        updateHistory(userInput);
        if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0){   // quit OR q 
            return QUIT;
        }
        else if (strcmp(command, "help") == 0 || strcmp(command, "h") == 0){ // help OR h 
            printAllCommands();
        }
        else if (strcmp(command, "dir") == 0 || strcmp(command, "d") == 0){ // dir OR d  
            listdir(".");
        }
        else if (strcmp(command, "history") == 0 || strcmp(command, "hi") == 0){ // history OR hi 
            showHistory();
        }else if(strcmp(command,"reset")==0){ // reset memory
            resetMemory(); 
        }else if(strcmp(command,"opcodelist")==0){ // print opcode list 
            showOpcode(); 
        }else if(strcmp(command,"symbol")==0){
            showSymbol();
        }
    }
    else if (isComplexInst()){   // user command is one of the complex instructions 
        instLength = strlen(command); 
        if ((strcmp(command, "dump") == 0 || strcmp(command, "du") == 0) && checkParams()){ // dump OR du
            successful = dumpMemory();
        }else if((strcmp(command, "edit") == 0 || strcmp(command, "e") == 0) && checkParams()){ // edit OR e 
            successful = editMemory();
        }else if((strcmp(command, "fill") == 0 || strcmp(command, "f") == 0)  && checkParams()){  // fill OR f
            successful = fillMemory();
        }else if(strcmp(command, "opcode") == 0 && checkParamsMnemonic()){ // opcode 
            successful = getOpcode(targetMnemonic,0);
        }else if(strcmp(command,"type")==0 && checkFilename()){
            successful = readFile(fullFileName); 
        }else if(strcmp(command,"assemble")==0 && checkFilename()){
            successful = assemble(fullFileName); 
        }
    }
    if(successful){ // insert to history list only if command was successful 
         historyCount++;
         updateHistory(userInput);
    }
    return SUCCESS;
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
