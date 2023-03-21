/*
Author: Jared Gogel
CS470
01/20/2023
*/
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>

using namespace std;

//Function to tokenize command from input
void tokenCommand(char* command, int &argc, char** argv)
{
    argc = 0;
    char* tokenCMD = strtok(command, " \t");
    while(tokenCMD != nullptr)
    {
        *argv++ = tokenCMD;
        argc++;
        tokenCMD = strtok(nullptr, " \t");
    }
    *argv = nullptr;
}

//Function to execute commands from input
void exeCommand(char **argv)
{
    int status;
    pid_t pid = fork();
    if(pid < 0)
    {
        cout << "Forking child process failed" << endl;
        exit(1);
    }
    else if(pid == 0)
    {
        if(execvp(*argv, argv) < 0)
        {
            cout << "Unknown Command: " << argv[0] << endl;
            cout << "For a list of commands, please type: manual" << endl;
            exit(1);
        }
    }
    else
        waitpid(pid, &status, 0);
}

//Function to print the manual for cwushell
void printManual()
{
    cout << "List of All Commands and Input Examples\n" << endl;

    cout << "manual" << endl;
    cout << "Prints out a list of all commands included with this cwushell as well as usage examples." << endl;
    cout << "Input: manual\n" << endl;

    cout << "exit [n]" << endl;
    cout << "Terminates the shell. [n] is an optional switch." << endl;
    cout << "input: exit or exit 3\n" << endl;

    cout << "prompt <new_prompt>" << endl;
    cout << "Changes the prompt name if one is provided or sets the prompt to the default if one is not provided." << endl;
    cout << "Input: prompt or prompt HelloWorld\n" << endl;

    cout << "cpuinfo -switch" << endl;
    cout << "Shows the cpu information based on the switch provided." << endl;
    cout << "There are three switches: -c (cpu clock), -t (cpu type), -n (number of cores)" << endl;
    cout << "Input: cpuinfo -c" << endl;
    cout << "       cpuinfo -t" << endl;
    cout << "       cpuinfo -n\n" << endl;

    cout << "meminfo -switch" << endl;
    cout << "Show the memory information based on the switch provided." << endl;
    cout << "There are three possible switches: -t (total ram), -u (used ram), -c (size of L2 cache)" << endl;
    cout << "Examples: meminfo -t" << endl;
    cout << "          meminfo -u" << endl;
    cout << "          meminfo -c\n" << endl;

    cout << "All other linux shell commands are available on this cwushell." << endl;
    cout << "example commands: man, ls, cat, etc." << endl;
    cout << "Note: The command man is used to print the manual of already existing commands." << endl;
    cout << "Input: man ls or man cat" << endl;
}

int main(int argc, char* argv[])
{
    char input[300];
    string prompt = "cwushell";

    while(true)
    {
        cout << prompt << "> ";
        cin.getline(input, 300);
        tokenCommand(input, argc, argv);

        if(argc == 0)
            continue;
        if(strcmp(input, "exit") == 0)
        {
            if(argc >= 2)
            {
                string sNum = argv[1];
                stringstream sSNum(sNum);
                int num = 0;
                sSNum >> num;

                return num;
            }

            return 0;
        }
        else if(strcmp(input, "manual") == 0)
        {
            printManual();
        }
        else if(strcmp(input, "prompt") == 0)
        {
            if(argc >= 2)
            {
                prompt = argv[1];
                continue;
            }
            prompt = "cwushell";
        }
        else if(strcmp(input, "cpuinfo") == 0)
        {
            string line;
            ifstream cpu("/proc/cpuinfo");

            if(argc >= 2 && (strcmp(argv[1], "-t") == 0))
            {
                for(int i = 0; i < 5 && !cpu.eof(); i++)
                {
                    getline(cpu, line);
                }
                cout << line << endl;
            }
            else if(argc >= 2 && (strcmp(argv[1], "-c") == 0))
            {
                for(int i = 0; i < 8 && !cpu.eof(); i++)
                {
                    getline(cpu, line);
                }
                cout << line << endl;
            }
            else if(argc >= 2 && (strcmp(argv[1], "-n") == 0))
            {
                long num_cores = sysconf(_SC_NPROCESSORS_CONF);
                cout << "Number of Cores: " << num_cores << endl;

            }
            else if(argc >= 2 && (strcmp(argv[1], "-h") == 0) || argc >= 2 && (strcmp(argv[1], "-help") == 0))
            {
                cout << "cpuinfo has three switches:" << endl;
                cout << "-c: Prints the CPU clock speed." << endl;
                cout << "-t: Prints the CPU type." << endl;
                cout << "-n: Prints the number of CPU cores.\n" << endl;
                cout << "For a full list of commands and examples type: manual" << endl;
            }
            else
            {
                cout << "Error: incorrect usage of command: cpuinfo -switch\n" << endl;
                cout << "cpuinfo has three switches:" << endl;
                cout << "-c: Prints the CPU clock speed." << endl;
                cout << "-t: Prints the CPU type." << endl;
                cout << "-n: Prints the number of CPU cores.\n" << endl;
                cout << "For a full list of commands and examples type: manual" << endl;
            }
        }
        else if(strcmp(input, "meminfo") == 0)
        {
            if(argc >= 2 && (strcmp(argv[1], "-t") == 0))
            {
                long t_ram = sysconf(_SC_PAGESIZE) * sysconf(_SC_PHYS_PAGES);
                cout <<"Total ram in bytes: " << t_ram << endl;
            }
            else if(argc >= 2 && (strcmp(argv[1], "-u") == 0))
            {
                long u_ram = sysconf(_SC_PAGESIZE) * sysconf(_SC_AVPHYS_PAGES);
                cout << "Used ram in bytes: " << u_ram << endl;
            }
            else if(argc >= 2 && (strcmp(argv[1], "-c") == 0))
            {
                long l2 = sysconf(_SC_LEVEL2_CACHE_SIZE);
                cout << "Size of L2 cache: " << l2 << endl;

            }
            else if(argc >= 2 && (strcmp(argv[1], "-h") == 0) || argc >= 2 && (strcmp(argv[1], "-help") == 0))
            {
                cout << "meminfo has the following three switches:" << endl;
                cout << "-t: Prints the total available RAM." << endl;
                cout << "-u: Prints the used ammount of RAM." << endl;
                cout << "-c: Prints the size of the L2 Cache.\n" << endl;
                cout << "For a full list of commands and examples type: manual" << endl;
            }
            else
            {
                cout << "Incorrect usage of command: meminfo -switch\n" << endl;
                cout << "meminfo has the following three switches:" << endl;
                cout << "-t: Prints the total available RAM." << endl;
                cout << "-u: Prints the used ammount of RAM." << endl;
                cout << "-c: Prints the size of the L2 Cache.\n" << endl;
                cout << "For a full list of commands and examples type: manual" << endl;
            }
        }
        else
        {
            exeCommand(argv);
        }
    }
}