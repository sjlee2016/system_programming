#include <stdio.h>
#include "20161619.h"
void updateHistory(char *command){
    h_node temp;
    temp = (History_Node *)malloc(sizeof(History_Node) * 1);
    temp->n = historyCount;
    temp->next = NULL;
    temp->s = (char *)malloc(sizeof(char) * strlen(command));
    strcpy(temp->s, command);
    if (head == NULL)
    {
        head = temp;
        current = head;
    }
    else
    {
        current->next = temp;
        current = temp;
    }
}

void showHistory(){
    h_node temp;
    temp = head;
    while (temp != NULL)
    {
        printf("%d %s", temp->n, temp->s);
        temp = temp->next;
    }
    printf("\n");
}

int listdir(const char *path){
    struct dirent *entry;
    DIR *dp = NULL;
    struct stat buf;
    dp = opendir(path);
    if (dp == NULL)
    {
        perror("opendir");
        return -1;
    }
    while ((entry = readdir(dp)))
    {
        printf("%s", entry->d_name);
        lstat(entry->d_name, &buf);
        if (S_ISDIR(buf.st_mode))
        {
            printf("/");
        }
        else if (S_IEXEC & buf.st_mode)
        {
            printf("*");
        }
        printf(" ");
    }
    printf("\n");
    closedir(dp);
    return 0;
}

void printAllCommands(){
    printf("h[elp]\nd[ir]\nq[uit]\ndu[mp]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
}

int isSingleInst(){
    int j;
    int flag = 0;
    int found = 0; 
    for (int i = 0; i < 10; i++)
    {
        flag = 0;
        instLength = strlen(singleInsts[i]);
        if (strncmp(userInput, singleInsts[i], instLength) == 0)
        {
            for (j = instLength; j < 100; j++)
            {
                if (userInput[j] != '\t' && userInput[j] != ' ' && userInput[j] != '\0' &&  userInput[j] != '\n'){
                    flag = 1;
                    break;
                }
            }
            if(flag == 0){
                strcpy(command, singleInsts[i]);   
                found = 1; 
                flag = 0;  
            }
        }
    }
    if(found==1){
        return 1;
    }else
        return 0;
}

int isEmpty(char c){
    return (c== ' ' || c == '\t' || c == '\0' || c == '\n'); 
}
int isHexadecimal(char c){
 return ('0' <= c && c <= '9' ) || ('A' <= c && c  <= 'F') || ('a' <= c && c <= 'f'); 
}
// return -1 if not true
// number of parameters if true
int isPluralInst(){
    instLength = 0;
    for (int i = 0; i < 7; i++){
        instLength = strlen(pluralInsts[i]);
        if (strncmp(userInput, pluralInsts[i], instLength) == 0) {
            if (isEmpty(userInput[instLength]))
            { 
                strcpy(command, pluralInsts[i]);
                return 1;
            }
        }   
    }
    return 0; 
}

int checkParams(){ // return 0 for incorrect parameter 
   int n_comma = 0 ;
   char input[10] = "";
   int paramLength = 0 ; 
   int last = 0;
   int idx1,idx2,idx3,i;
   for(i  = instLength; i < 100; i++){
       if(userInput[i]==','){
            n_comma++;
            if(n_comma==1)
                idx1=i;  // 1,2
            else if(n_comma==2)
                idx2=i;  // 1,2,3
            else 
                return 0;  // 1,2,3,..,
       }
   }
   if(n_comma == 0 && (strncmp(userInput,"du",2)==0 || strncmp(userInput,"dump",4)==0)){
       for( i = instLength; i < 100; i++){
           if(!isEmpty(userInput[i])){
                break;
           }
       }
       if(i==100){
        numOfParams = 0;
        return 1; 
       }
   }
    if(n_comma>=0){
       last = 0; 
       for(i = instLength; i<100; i++){
           if(userInput[i]=='\0' || userInput[i]==','){
               break;
           }
             if(!(isEmpty(userInput[i]))){
              if(isHexadecimal(userInput[i])){
                  
                input[last++] += userInput[i]; 
                if(last>9){
                    printf("parameter length can not be bigger than 9\n");
                    return 0; 
                }
              }else{
                  printf("wrong input here!!!\n");
                  return 0;
              }
             }
           }
       
       numOfParams = 1; 
       printf("copied %s to params\n", input);
       strcpy(params[0],input);
   }

   if(n_comma>=1){
       memset(input,0,sizeof(input));
       last = 0;  
        for(i = idx1+1; i<100; i++){
           if(userInput[i]=='\0' || userInput[i]==','){
               break;
           }
           if(!(isEmpty(userInput[i]))){
              if(isHexadecimal(userInput[i])){
                  
                input[last++] += userInput[i]; 
                if(last>9){
                    printf("parameter length can not be bigger than 9\n");
                    return 0; 
                }
              }else{
                  printf("wrong input here!!!\n");
                  return 0;
              }
           }
       }
       numOfParams = 2; 
       printf("copied %s to params\n", input);
       strcpy(params[1],input);
   }

     if(n_comma==2){
       memset(input,0,sizeof(input)); 
       last = 0;  
        for(i = idx2+1; i<100; i++){
           if(userInput[i]=='\0'){
               break;
           }
           if(userInput[i]!='\t' && userInput[i]!=' '  && userInput[i]!='\n'){
               if(!('0' <= userInput[i] && userInput[i] <= '9' ) || ('A' <= userInput[i] &&userInput[i] <= 'F') || ('a' <= userInput[i] && userInput[i] <= 'f')){
                   printf("Wrong parameter input..\n");
                   return 0;
               }
                input[last++] += userInput[i]; 
                if(last>9){
                    printf("parameter length can not be bigger than 9\n");
                    return 0; 
                }
           }
       }
       numOfParams = 3; 
       printf("copied %s to params\n", input);
       strcpy(params[2],input);
   }
   return 1; 
}


void printMem(long start, long end)
{
    long rowStart = start - start % 16;
    long rowEnd = end - end % 16;
    /// print out memory
    for (long i = rowStart; i <= rowEnd; i += 16)
    {
        printf("%05lX\t", i);
        for (long j = i; j < i + 16; j++)
        {
            if (j < start || j > end)
            {
                printf("  ");
            }
            else
            {
                int val = VMemory[j];
                printf("%02X", val);
            }
            printf("  ");
        }
        printf("; ");
        for (long j = i; j < i + 16; j++)
        {
            if (VMemory[j] >= 20 && VMemory[j] <= 0x7E)
                printf("%c", VMemory[j]);
            else
                printf(".");
        }
        printf("\n");
    }
}
int dumpMemory()
{
    long start, end, rowStart, rowEnd;
    char *err;
    if (numOfParams == 0)
    {
        if (defaultStartAddr >= MAX_MEMORY_SIZE)
            defaultStartAddr = 0;
        start = defaultStartAddr + 1;
        end = start + (16 * 10 - 1);
        defaultStartAddr = end;
    }
    else if (numOfParams == 1)
    {
        start = strtol(params[0], &err, 16);
        end = start + (16 * 10 - 1);
    }
    else if (numOfParams == 2)
    {
        start = strtol(params[0], &err, 16);
        end = strtol(params[1], &err, 16);
    }
    else
    {
        printf("Wrong parameter input\n");
        return 0;
    }
    if(end >= MAX_MEMORY_SIZE){
        end = MAX_MEMORY_SIZE-1; 
    }
    if(start > MAX_MEMORY_SIZE || start < 0 ){
        printf("start address should be between 0~1MB\n");
        return 0;
    }
    if(start > end ){
        printf("start address should be less than end address\n");
        return 0;
    }
    printMem(start, end);
    return 1; 
}

int editMemory()
{
    char *err;
    if (numOfParams == 2)
    {
        long address = strtol(params[0], &err, 16);
        long value = strtol(params[1], &err, 16);
        if (address < 0 || address > MAX_MEMORY_SIZE-1)
        {
            printf("Memory out of bound\n");
            return 0;
        }else if(value > 0xFF){
            printf("Value should be between 0~0xFF\n");
            return 0;
        }

        VMemory[address] = value;
        return 1; 
    }
    else
    {
        printf("Wrong parameter input\n");
        return 0;
    }
}

void resetMemory(){
    for(int i = 0 ; i < MAX_MEMORY_SIZE; i++)
        VMemory[i] = 0; 
}
int fillMemory()
{
    char *err;
    if (numOfParams == 3)
    {
        long start = strtol(params[0], &err, 16);
        long end = strtol(params[1], &err, 16);
        long value = strtol(params[2], &err, 16);
        if (start < 0 || end < 0 || start >= MAX_MEMORY_SIZE || end >= MAX_MEMORY_SIZE)
        {
            printf("Memory out of bound\n");
            return 0;
        }
        else if (start > end)
        {
            printf("start address should be bigger than end address\n");
            return 0;
        }
        if (value > 0xFF)
        {
            printf("You can only change with a value within the range of 00~FF.\n");
            return 0;
        }
        for (long i = start; i <= end; i++)
        {
            VMemory[i] = value;
            return 1;
        }
    }
    else
    {
        printf("Wrong parameter input\n");
        return 0;
    }
}

int getCommand()
{
    int isRightArg = 1;
    char *newStr;
    int i; 
    int successful = 0; 
    for (int i = 0; i < 100; i++)
        userInput[i] = '\0';
    printf("sicsim> ");

    fgets(userInput, 100, stdin);

    if (isSingleInst())
    {
        historyCount++;
        updateHistory(userInput);
        if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0)
        {
            return QUIT;
        }
        else if (strcmp(command, "help") == 0 || strcmp(command, "h") == 0)
        {
            printAllCommands();
        }
        else if (strcmp(command, "dir") == 0 || strcmp(command, "d") == 0)
        {
            listdir(".");
        }
        else if (strcmp(command, "history") == 0 || strcmp(command, "hi") == 0)
        {
            showHistory();
        }else if(strcmp(command,"reset")==0){
            resetMemory(); 
        }
    }
    else if (isPluralInst())
    {
        instLength = strlen(command); 
        printf("inst Length : %d\n", instLength);
        if ((strcmp(command, "dump") == 0 || strcmp(command, "du") == 0) && checkParams())
        {
            successful = dumpMemory();
        }else if((strcmp(command, "edit") == 0 || strcmp(command, "e") == 0) && checkParams()){
            successful = editMemory();
        }else if((strcmp(command, "fil") == 0 || strcmp(command, "f") == 0)  && checkParams()){
            successful = fillMemory();
        }else if(strcmp(command, "opcode") == 0){
            checkParams();
            printf("opcode..\n"); 
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

    memset(VMemory, 0, sizeof(VMemory));
    while (getCommand() != QUIT)
    {
    }
    return 0;
}
