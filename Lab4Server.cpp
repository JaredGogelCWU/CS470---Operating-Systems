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
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <ctime>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT_NUMBER 54321

//Structure for the staff
struct staffParams
{
    int amountToCook;
    int *availableBurgers;
    int *totalCooked;
    pthread_mutex_t *chefLock;
    pthread_mutex_t *burgLock;
    pthread_cond_t *mngrCond;
};

//Structure for the clients
struct clientParams
{
    int clientSocket;
    int *availableBurgers;
    pthread_mutex_t *clientLock;
    pthread_mutex_t *burgLock;
};

//Function to error check the command line arguments
bool validateArgs(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("\n%s: Please provide the number of burgers to be made and the number of chefs \n", argv[0]);
        return false;
    }

    int numBurgers = atoi(argv[1]);
    int numChefs = atoi(argv[2]);

    if (numBurgers <= 0 || numChefs <= 0)
    {
        printf("\n%s: Parameters need to be an integer larger than 0. \n", argv[0]);
        return false;
    }
    return true;
}

//Function to handle socket errors
int check(int exp, const char *msg)
{
    if (exp < 0)
    {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return exp;
}

//Function to track when all the clients are done and then it removes them
void *managerThread(void *arg)
{
    struct staffParams *p = (struct staffParams *)arg;
    int *availableBurgers = p->availableBurgers;
    int *totalCooked = p->totalCooked;
    int amountToCook = p->amountToCook;
    pthread_mutex_t *chefLock = p->chefLock;
    pthread_cond_t *mngrCond = p->mngrCond;
    pthread_mutex_lock(chefLock);
    while (*totalCooked != amountToCook)
    {
        pthread_cond_wait(mngrCond, chefLock);
    }
    pthread_mutex_unlock(chefLock);
    printf("Manager [%d] is in.\n", gettid());
    while (*availableBurgers != 0)
    {
        sleep(1);
    }
    printf("Out of burgers. Clients are being removed\n");
    exit(EXIT_SUCCESS);
}

//Function to run the chef thread
void *chefThread(void *arg)
{
    struct staffParams *p = (struct staffParams *)arg;
    int *availableBurgers = p->availableBurgers;
    int *totalCooked = p->totalCooked;
    int amountToCook = p->amountToCook;
    pthread_mutex_t *chefLock = p->chefLock;
    pthread_mutex_t *burgLock = p->burgLock;
    pthread_cond_t *mngrCond = p->mngrCond;
    char chefId[16];
    sprintf(chefId, "Chef [%d]", gettid());
    printf("%s is starting work.\n", chefId);

    while (*totalCooked < amountToCook)
    {
        if (pthread_mutex_trylock(chefLock) == 0)
        {
            *totalCooked += 1;
            pthread_mutex_lock(burgLock);
            *availableBurgers += 1;
            pthread_mutex_unlock(burgLock);
            srand(time(nullptr) * gettid());
            time_t time = rand() % 2 == 0 ? 2 : 4;
            printf("%s finished cooking burger %d/%d in %li seconds.\n", chefId, *totalCooked, amountToCook, time);
            pthread_mutex_unlock(chefLock);
            sleep(time);
        }
    }
    pthread_cond_signal(mngrCond);
    printf("%s is done.\n", chefId);
    return nullptr;
}

//Function to run the client thread
void *clientThread(void *arg)
{
    struct clientParams *p = (struct clientParams *)arg;
    int *availableBurgers = p->availableBurgers;
    pthread_mutex_t *clientLock = p->clientLock;
    pthread_mutex_t *burgLock = p->burgLock;

    int clientSocket = *((int *)arg);
    char clientId[16];
    sprintf(clientId, "Client [%d]", gettid());

    while (true)
    {
        pthread_mutex_lock(clientLock);
        if (*availableBurgers == 0)
        {
            pthread_mutex_unlock(clientLock);
            continue;
        }
        else
        {
            pthread_mutex_lock(burgLock);
            *availableBurgers -= 1;
            pthread_mutex_unlock(burgLock);

            char sendMsg[16];
            sprintf(sendMsg, "eat");
            check(send(clientSocket, sendMsg, strlen(sendMsg), 0), "Sending Error");
            pthread_mutex_unlock(clientLock);
        }
        char recvMsg[16];
        int n = recv(clientSocket, recvMsg, sizeof(recvMsg) - 1, 0);
        if (n == -1)
        {
            perror("recv");
            return nullptr;
        }
        else if (n == 0)
        {
            printf("%s disconnected.\n", clientId);
            return nullptr;
        }
        recvMsg[n] = '\0';
        printf("%s ate a burger. There are %d burgers available.\n", clientId, *availableBurgers);

        if (strcmp(recvMsg, "finished") == 0)
        {
            printf("%s is done eating!\n", clientId);
            return nullptr;
        }
    }
}

int main(int argc, char **argv) {
    int availableBurgers = 0;
    int burgersToMake;
    int numOfChefs;

    if (argc == 1)
    {
        burgersToMake = 25;
        numOfChefs = 2;
    }
    else if (validateArgs(argc, argv))
    {
        burgersToMake = atoi(argv[1]);
        numOfChefs = atoi(argv[2]);
    }
    else
    {
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servAddr{};
    char sendBuff[BUFFER_SIZE];
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servAddr, '0', sizeof(servAddr));
    memset(sendBuff, '0', sizeof(sendBuff));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(PORT_NUMBER);
    check(bind(serverSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)), "Error in binding port.");
    check(listen(serverSocket, 10), "Error in listening to port.");

    pthread_mutex_t chefMtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t clientMtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t burgMtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t mngrCond = PTHREAD_COND_INITIALIZER;

    struct staffParams staffParams{};
    staffParams.amountToCook = burgersToMake;
    staffParams.availableBurgers = &availableBurgers;
    staffParams.totalCooked = (int *) malloc(sizeof(int));
    staffParams.chefLock = &chefMtx;
    staffParams.burgLock = &burgMtx;
    staffParams.mngrCond = &mngrCond;


    struct clientParams clientParams{};
    clientParams.availableBurgers = &availableBurgers;
    clientParams.clientLock = &clientMtx;
    clientParams.burgLock = &burgMtx;
    printf("Burger Joint is open!\n");

    pthread_t m;
    pthread_create(&m, nullptr, managerThread, &staffParams);

    pthread_t threadArray[numOfChefs];
    for (int i = 0; i < numOfChefs; i++)
    {
        if (pthread_create(&threadArray[i], nullptr, chefThread, &staffParams) != 0)
        {
            printf("Failed to create chef thread [%d]\n", i);
        }
    }

    while (true)
    {
        int clientSocket = accept(serverSocket, (struct sockaddr *) nullptr, nullptr);
        clientParams.clientSocket = clientSocket;
        pthread_t t;
        pthread_create(&t, nullptr, clientThread, &clientParams);
        pthread_detach(t);
    }
}
