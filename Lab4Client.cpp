/*
Author: Jared Gogel
CS470
Lab 4
"I pledge that I have neither given nor received help
 * from anyone other than the instructor/TA for all program
 * components included here!"
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <regex.h>
#include <ctime>

#define BUFFER_SIZE 1024
#define CONNECTION_ATTEMPTS 5

//Function to generate a random time.
time_t getRandTime()
{
    srand(time(nullptr) * getpid());
    switch (rand() % 3)
    {
        case 0:
            return (time_t)1;
        case 1:
            return (time_t)3;
        case 2:
            return (time_t)5;
        default:
            return (time_t)1;
    }
}

//Function to handle socket errors.
int check(int exp, const char *msg)
{
    if (exp < 0)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}

//Function to error check command line arguments.
bool validateArgs(int argc, char **argv)
{
    bool deathFlag = false;
    if (argc != 4)
    {
        printf("\n%s: Please enter the ip address, port number, and number of burgers to eat\n", argv[0]);
        return false;
    }
    regex_t ipRegex, portRegex;
    int ipCode = regcomp(&ipRegex, "(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])", REG_EXTENDED);
    int ptCode = regcomp(&portRegex, "^([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$", REG_EXTENDED);
    if (ipCode != 0 || ptCode != 0)
    {
        printf("\n%s regex compilation failed.\n", argv[0]);
        regfree(&ipRegex);
        regfree(&portRegex);
        return false;
    }
    if (regexec(&ipRegex, argv[1], 0, nullptr, 0))
    {
        printf("\n%s: invalid IP address %s provided.\n", argv[0], argv[1]);
        deathFlag = true;
    }
    if (regexec(&portRegex, argv[2], 0, nullptr, 0))
    {
        printf("\n%s: invalid port %s provided.\n", argv[0], argv[2]);
        deathFlag = true;
    }
    if (atoi(argv[3]) <= 0)
    {
        printf("\n%s: invalid number of burgers %s provided.\n", argv[0], argv[3]);
        deathFlag = true;
    }
    regfree(&ipRegex);
    regfree(&portRegex);
    if (deathFlag)
        return false;
    else
        return true;
}

int main(int argc, char **argv)
{
    char ipAddr[32];
    int portNum;
    int burgerLimit;

    if (argc == 1)
    {
        strcpy(ipAddr, "127.0.0.1");
        portNum = 5437;
        burgerLimit = 10;
    }
    else if (validateArgs(argc, argv))
    {
        strcpy(ipAddr, argv[1]);
        portNum = atoi(argv[2]);
        burgerLimit = atoi(argv[3]);
    }
    else
    {
        exit(EXIT_FAILURE);
    }

    int serverSocket = 0;
    char recvBuff[BUFFER_SIZE];
    struct sockaddr_in servAddr{};

    memset(recvBuff, '0', sizeof(recvBuff));
    check((serverSocket = socket(AF_INET, SOCK_STREAM, 0)), "Socket Creation Error");

    memset(&servAddr, '0', sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(portNum);

    check(inet_pton(AF_INET, ipAddr, &servAddr.sin_addr), "inet_pton error");

    for (int attempts = 0; attempts < CONNECTION_ATTEMPTS + 1; attempts++)
    {
        int code = connect(serverSocket, (struct sockaddr *)&servAddr, sizeof(servAddr));
        if (code == -1 && attempts < CONNECTION_ATTEMPTS)
        {
            printf("Connection failed. Trying again.\n");
            sleep(3);
        }
        else if (code == -1 && attempts == CONNECTION_ATTEMPTS)
        {
            printf("Connection timed out.\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            break;
        }
    }
    printf("Connected to server.\n");
    int burgersAte = 0;
    char buf[32];

    while (burgersAte != burgerLimit)
    {
        int n = recv(serverSocket, buf, sizeof(buf) - 1, 0);
        if (n == -1)
        {
            perror("recv");
            exit(1);
        }
        else if (n == 0)
        {
            printf("Client was removed\n");
            exit(0);
        }
        buf[n] = '\0';

        time_t time = getRandTime();
        sleep(time);

        burgersAte += 1;
        printf("Ate burger %d of %d in %li seconds.\n", burgersAte, burgerLimit, time);

        if (burgersAte == burgerLimit)
        {
            sprintf(buf, "finished");
        }
        else
        {
            sprintf(buf, "still eating");
        }
        check(send(serverSocket, buf, strlen(buf), 0), "Sending Error");
    }

    printf("Clint is done.\n");
    close(serverSocket);
    return 0;
}
