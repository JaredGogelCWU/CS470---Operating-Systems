/*
Author: Jared Gogel
CS470
Lab 4
"I pledge that I have neither given nor received help
 * from anyone other than the instructor/TA for all program
 * components included here!"
*/
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <climits>

#define SCHEDULE_SWAP 30
#define SLEEP_TIME 1000000

//Structure for the PCB
struct process_control_block
{
    char priority;
    long processLocation;
    char processName[16];
    int32_t processId;
    char activityStatus;
    int32_t burstTime;
    int32_t baseRegister;
    long limitRegister;
    int numFiles;
};


int activeProcesses = 0;
int totalMemory = 0;
int openFiles = 0;
int fileSize = 0;
int quantumTime = 0;
int scheduling = 0;


//Function to copy original file
void createFileCopy(FILE * sourceFile)
{
    FILE *file;
    file = fopen("fileCopy.bin", "w+b");
    if (!file)
    {
        printf("Error Opening New File!");
    }

    fseek(sourceFile, 0L, SEEK_END);
    long size = ftell(sourceFile);
    rewind(sourceFile);

    char buffer[size];
    fread(buffer, sizeof(buffer), 1, sourceFile);
    fwrite(buffer, sizeof(buffer), 1, file);

    fclose(file);
    fclose(sourceFile);
}

//Function read command line arguments
int parseCLI(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("File Name not provided. Please provide the name of the file.\n");
        return 0;
    }

    char * fileName = argv[1];
    FILE * file = fopen(fileName, "rb");
    if (!file)
    {
        printf("Error Opening File!");
        return 0;
    }

    printf("\nCreating a copy of %s as fileCopy.bin\n\n", fileName);
    createFileCopy(file);

    return 1;
}

//Print Function for the PCB
void printPCB(struct process_control_block *pcb)
{
    printf("Priority: %d\n", pcb->priority);
    printf("Process Name: %s\n", pcb->processName);
    printf("Process ID: %d\n", pcb->processId);
    printf("Activity Status: %d\n", pcb->activityStatus);
    printf("CPU Burst Time: %d\n", pcb->burstTime);
    printf("Base Register: %d\n", pcb->baseRegister);
    printf("Limit Register: %li\n", pcb->limitRegister);
    printf("Number of Files: %d\n", pcb->numFiles);

}

//Function to read the file and store in the PCB structure
void parsePCB(FILE *file, struct process_control_block *pcb)
{
    pcb->processLocation = ftell(file);
    fread(&(pcb->priority), sizeof(char), 1, file);
    fread(pcb->processName, sizeof(char) * 16, 1, file);
    fread(&(pcb->processId), sizeof(int32_t), 1, file);
    fread(&(pcb->activityStatus), sizeof(char), 1, file);
    fread(&(pcb->burstTime), sizeof(int32_t), 1, file);
    fread(&(pcb->baseRegister), sizeof(int32_t), 1, file);
    fread(&(pcb->limitRegister), sizeof(long), 1, file);
    fread(&(pcb->numFiles), sizeof(long), 1, file);
}

//Function to remove process once CPU burst is zero
void removeProcess(FILE *file, struct process_control_block *pcb)
{
    char newActivityStatus = 0;
    int32_t newCPUBurst = 0;
    char newPriority = CHAR_MAX;

    fseek(file, pcb->processLocation + 20, SEEK_SET);
    fwrite(&newActivityStatus, sizeof(char), 1, file);
    fwrite(&newCPUBurst, sizeof(int32_t), 1, file);
    fseek(file, pcb->processLocation + 37, SEEK_SET);
    fwrite(&newPriority, sizeof(char), 1, file);

    pcb->activityStatus = newActivityStatus;
    pcb->burstTime = newCPUBurst;
    pcb->priority = newPriority;

    activeProcesses--;
    openFiles--;
}

//Function to run a process
void executeProcess(FILE *file, struct process_control_block *pcb)
{
    long endOfProcess = ftell(file);
    fseek(file, pcb->processLocation + 21, SEEK_SET);

    int32_t newBurstTime = pcb->burstTime - 1;

    if (newBurstTime <= 0)
        removeProcess(file, pcb);
    else
    {
        fwrite(&newBurstTime, sizeof(int32_t), 1, file);
        pcb->burstTime = newBurstTime;
    }

    fseek(file, endOfProcess, SEEK_SET);
}

//Function to create PCB structure and get a PCB from file
struct process_control_block *getPCB(FILE *file)
{
    struct process_control_block *pcb;
    pcb = static_cast<process_control_block *>(calloc(1, sizeof(struct process_control_block)));

    parsePCB(file, pcb);

    return pcb;
}

//Function to age processes excluding current process
void ageProcesses(FILE *file, int32_t currentProcessID)
{
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);

    while (ftell(file) != size)
    {
        struct process_control_block *pcb;

        pcb = getPCB(file);

        long endOfProcess = ftell(file);

        if (pcb->activityStatus == 0 || pcb->processId == currentProcessID)
            continue;

        fseek(file, pcb->processLocation + 37, SEEK_SET);

        char newPriority = pcb->priority > 0 ? (pcb->priority) - 1 : 0;
        fwrite(&newPriority, sizeof(char), 1, file);

        free(pcb);

        fseek(file, endOfProcess, SEEK_SET);
    }
}

//Function to parse file
void parseFile(FILE *file, int flag)
{
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);

    int numActiveProcesses = 0;
    int totalProcessMemory = 0;
    int numOpenFiles = 0;

    while (ftell(file) != size)
    {
        struct process_control_block *pcb;

        pcb = getPCB(file);

        if (flag == 1)
        {
            printPCB(pcb);
            printf("\n");
        }


        if (pcb->activityStatus > 0)
        {
            numActiveProcesses++;
            numOpenFiles++;
        }

        totalProcessMemory += (pcb->limitRegister - pcb->baseRegister);
        free(pcb);
    }

    if (flag == 1)
    {
        printf("Total number of active processes: %d\n", numActiveProcesses);
        printf("Total memory used by active processes: %d\n", totalProcessMemory);
        printf("Total number of open files: %d\n", numOpenFiles);

    }


    activeProcesses = numActiveProcesses;
    totalMemory = totalProcessMemory;
    openFiles = numOpenFiles;
    fileSize = size;

    rewind(file);
}

//Function to get the highest priority
struct process_control_block *getHighestPriority(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    rewind(file);

    struct process_control_block *highestPriorityPCB = getPCB(file);

    while (ftell(file) != size)
    {
        struct process_control_block *pcb;

        pcb = getPCB(file);

        if (pcb->activityStatus == 1 && pcb->priority < highestPriorityPCB->priority)
        {
            free(highestPriorityPCB);
            highestPriorityPCB = pcb;
        }
    }

    return highestPriorityPCB;
}

//Function to do priority scheduling
void priorityScheduling(FILE *file)
{
    struct process_control_block *priorityPCB;

    priorityPCB = getHighestPriority(file);

    printf("\033[0m");
    printf("\n-----------------------\n");
    printf("\nPriority Scheduling\n\n");
    printf("-----------------------\n\n");
    printf("\033[0;36m");

    while (quantumTime % SCHEDULE_SWAP != 0 && activeProcesses > 0)
    {
        if (quantumTime % 2 == 0)
        {
            printf("Aging all processes...\n\n");
            ageProcesses(file, priorityPCB->processId);
            free(priorityPCB);
            priorityPCB = getHighestPriority(file);
        }

        printPCB(priorityPCB);
        printf("\n");
        executeProcess(file, priorityPCB);

        if (priorityPCB->activityStatus == 0)
        {
            free(priorityPCB);
            priorityPCB = getHighestPriority(file);
        }

        quantumTime++;
        usleep(SLEEP_TIME);
    }
}

//Function to do round-robin scheduling
void rrScheduling(FILE *file)
{
    printf("\033[0m");
    printf("\n-----------------------\n");
    printf("\nRound Robin Scheduling\n\n");
    printf("-----------------------\n\n");
    printf("\033[0;31m");

    rewind(file);

    while (quantumTime % SCHEDULE_SWAP != 0 && activeProcesses > 0)
    {
        struct process_control_block *pcb = getPCB(file);

        while (pcb->activityStatus == 0)
        {
            if (ftell(file) >= fileSize)
                rewind(file);
            free(pcb);
            pcb = getPCB(file);
        }

        printPCB(pcb);
        printf("\n");
        executeProcess(file, pcb);
        free(pcb);

        quantumTime++;
        usleep(SLEEP_TIME);
    }
}

//Function to run the scheduling algorithms
void runSchedulers(FILE *file)
{
    printf("Running Scheduler...\n");
    do
    {
        quantumTime++;
        if (scheduling == 0)
        {
            rrScheduling(file);
            scheduling = 1;
        }
        else
        {
            priorityScheduling(file);
            scheduling = 0;
        }
    } while (activeProcesses > 0);
}

int main(int argc, char *argv[])
{
    printf("\033[0m");

    int isValid = parseCLI(argc, argv);
    if (isValid == 0)
    {
        return 1;
    }

    FILE *file;
    file = fopen("fileCopy.bin", "r+b");
    if (!file)
    {
        printf("Error!");
    }
    parseFile(file, 0);

    printf("This file has %d processes...\n\n", activeProcesses);
    printf("Total memory used by active processes: %d\n\n", totalMemory);
    printf("Total number of files: %d\n\n", openFiles);

    runSchedulers(file);
    printf("\033[0;33m");
    parseFile(file, 1);
    printf("\033[0m");
    return 0;
}