/*
h[elp]
d[ir]
q[uit]
hi[story]
du[mp] [start, end]
e[dit] address, value
f[ill] start, end, value
reset
opcode mnemonic
opcodelist
*/
#include <stdio.h>
#include "20161619.h"
int historyCount=0;

h_node head; 
h_node createNode(){
    h_node temp;
    temp = (h_node) malloc(sizeof(struct History_Node)); 
    temp->n = historyCount;
    temp->next = NULL;
    return temp; 
}

void insertNode(char *s){
    h_node temp, p;
    temp = createNode();
    temp->s = (char*)malloc(sizeof(char)*strlen(s));
    strcpy(temp->s,s); 
    if(head==NULL){
        head = temp;
    }else {
         p = head;
         while(p->next != NULL){
             p = p-> next;
         }
         p->next = temp;
    }
    
}

int listdir(const char *path) {
  struct dirent *entry;
  DIR *dp;

  dp = opendir(path);
  if (dp == NULL) {
    perror("opendir");
    return -1;
  }

  while((entry = readdir(dp))){
    printf("%s", entry->d_name);

    if(entry->d_type == DT_DIR){
        printf("/ ");
    }
    
    printf(" ");
  }
  printf("\n");
  closedir(dp);
  return 0;
}
void dump(){
    printf("dumping..\n");
}
void updateHistory(char * command){
    insertNode(command);
}
void showHistory(){
    h_node temp;
    temp = head;
    while(temp != NULL){
        printf("%d %s\n", temp->n, temp->s);
        temp = temp->next; 
    }
}
void printAllCommands(){
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n"); 
    printf("du[mp]\n");
    printf("e[dit] address, value");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
}
int countSpace(char *s){
    int count = 0;
    for(int i = 0; i < strlen(s); i++){
        if(s[i] == ' '){
            count++;
        }
    }
    return count; 
}
int getCommand(){
    char command[100]; 
    int wrongArg = 0; 
    char * newStr; 
    for(int i = 0 ; i <100; i++)
        command[i]='\0';
    
    printf("sicsim> "); 
    fgets(command,100,stdin);
    if(countSpace(command)==0){
        char * firstArg = strtok(command," ");
        firstArg[strlen(firstArg)-1]=0;
        if(strcmp(firstArg,"quit")==0 || strcmp(firstArg,"q")==0){
            return QUIT;
        }else if(strcmp(firstArg,"help") ==0 || strcmp(firstArg,"h")==0){
            printAllCommands();
        }else if(strcmp(firstArg,"dir")==0 || strcmp(firstArg,"d")==0){
             listdir(".");
        }else if(strcmp(firstArg,"history")==0|| strcmp(firstArg,"hi")==0){
            updateHistory(command); 
            historyCount++;
            showHistory();
            return RIGHT_COMMAND; 
        }else {
            wrongArg = 1; 
        }
    }
    else if(countSpace(command)==1){
       char * firstArg = strtok(command," ");
       if(strcmp(firstArg,"dump")==0){  
            char * secondArg = strtok(NULL," ");
            char * n1 = strtok(secondArg,",");
            char * n2 = strtok(NULL, " ");
            int d1,d2;
            if(n2=='\0'){
                d1 = atoi(n1); 
                if(d1!=0){
                    printf("dump start %d\n", d1);
                }else {
                    printf("Illegal format : dump start [number]\n");
                    wrongArg = 1;
                }
            }else {
                d1 = atoi(n1);
                d2 = atoi(n2);
                if(d1!=0 && d2!=0){
                  printf("dump start %d,end %d..\n", d1,d2);
                }else{
                  printf("Illegal format : dump start [number,number]\n");
                  wrongArg = 1;
                }
            }        
    }
    }
    if(wrongArg==0){
        updateHistory(command); 
        historyCount++;
    }
    

  
}
int main(){
    while(getCommand()!=QUIT){
    }
}