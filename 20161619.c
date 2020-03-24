
#include <stdio.h>
#include "20161619.h"
int historyCount = 0;

void updateHistory(char *command)
{
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

void showHistory()
{
    h_node temp;
    temp = head;
    while (temp != NULL)
    {
        printf("%d %s", temp->n, temp->s);
        temp = temp->next;
    }
    printf("\n");
}

int listdir(const char *path)
{
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

void printAllCommands()
{
    printf("h[elp]\nd[ir]\nq[uit]\ndu[mp]\ne[dit] address, value\nf[ill] start, end, value\nreset\nopcode mnemonic\nopcodelist\n");
}

int isSingleInst()
{
    int instLength;
    int j;
    for (int i = 0; i < 10; i++)
    {
        instLength = strlen(singleInsts[i]);
        if (strncmp(userInput, singleInsts[i], instLength) == 0)
        {
            for (j = instLength + 1; j < 100; j++)
            {
                if (userInput[j] != '\t' && userInput[j] != ' ' && userInput[j] != '\0')
                {
                    break;
                }
            }
            if (j == 100)
            {
                strcpy(command, singleInsts[i]);
                return 1;
            }
        }
    }
    return 0;
}

// return -1 if not true
// number of parameters if true
int isPluralInst()
{
    int instLength = 0;
    for (int i = 0; i < 7; i++)
    {
        instLength = strlen(pluralInsts[i]);
        if (strncmp(userInput, pluralInsts[i], instLength) == 0)
        {
            if (userInput[instLength] == ' ' || userInput[instLength] == '\t' || userInput[instLength] == '\0')
            {
                strcpy(command, pluralInsts[i]);
                return 1;
            }
        }
    }
}
void checkParams()
{
   
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
void dump()
{
    long start, end, rowStart, rowEnd;
    char *err;
    checkParams();
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
        return;
    }
    printMem(start, end);
}

void editMemory()
{
    checkParams();
    char *err;
    if (numOfParams == 2)
    {
        long address = strtol(params[0], &err, 16);
        long value = strtol(params[1], &err, 16);
        if (address < 0 || address >= MAX_MEMORY_SIZE)
        {
            printf("Memory out of bound\n");
            return;
        }
        VMemory[address] = value;
    }
    else
    {
        printf("Wrong parameter input\n");
        return;
    }
}

void fillMemory()
{
    checkParams();
    char *err;
    if (numOfParams == 3)
    {
        long start = strtol(params[0], &err, 16);
        long end = strtol(params[1], &err, 16);
        long value = strtol(params[2], &err, 16);
        if (start < 0 || end < 0 || start >= MAX_MEMORY_SIZE || end >= MAX_MEMORY_SIZE)
        {
            printf("Memory out of bound\n");
            return;
        }
        else if (start > end)
        {
            printf("start address should be bigger than end address\n");
        }
        if (value > 0xFF)
        {
            printf("00~FF 사이의 값으로만 변경할 수 있습니다.\n");
            return;
        }
        for (long i = start; i <= end; i++)
        {
            VMemory[i] = value;
        }
    }
    else
    {
        printf("Wrong parameter input\n");
        return;
    }
}

int getCommand()
{
    int isRightArg = 1;
    char *newStr;
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
        }
    }
    else if (isPluralInst())
    {
        historyCount++;
        updateHistory(userInput);
        if (strcmp(command, "dump") == 0)
        {
        }
    }
    return 1;
}
int main()
{

    memset(VMemory, 0, sizeof(VMemory));
    numOfParams = 3;
    strcpy(params[0], "4");
    strcpy(params[1], "10");
    strcpy(params[2], "16");
    fillMemory();
    numOfParams=0;
    dump();
    while (getCommand() != QUIT)
    {
    }
    return 0;
}
