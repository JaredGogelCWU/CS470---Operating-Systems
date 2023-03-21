/*
Author: Jared Gogel
CS470
Lab3
"I pledge that I have neither given nor received help
from anyone other than the instructor/TA for all program
components included here!"
 */
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <pthread.h>
#define DEFAULT "3"
using namespace std;

//Structure to hold problem one data
struct p1Data
{
    int size;
    int** array;
    bool isDone;
};

//Structure to hold problem two data
struct p2Data
{
    int size;
    int* array;
    bool isDone;
};

struct p1Data p1D;
struct p2Data p2D;
pthread_mutex_t lock;

//Function to check if the input is an integer
bool isNumber(string input)
{
    for(int i = 0; i < (int)input.length(); i++)
    {
        if (!isdigit(input[i]))
        {
            return false;
        }
    }
    return true;
}

//Function that checks if the neighbors of the matrix element is zero or one
int p1CheckNeighbors(int x, int y)
{
    int size = p1D.size;
    int zeros = 0;
    int ones = 0;

    //Checking top rows
    if(x == 0)
    {
        if(y == 0)
        {
            if(p1D.array[x][y + 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y + 1] == 0) zeros++;
            else ones++;
        }
        if(y == size - 1)
        {
            if(p1D.array[x][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y - 1] == 0) zeros++;
            else ones++;
        }
        else
        {
            if(p1D.array[x][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x][y + 1] == 0)  zeros++;
            else ones++;
            if(p1D.array[x + 1][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y + 1] == 0) zeros++;
            else ones++;
        }
    }
    //Checking middle rows
    if(x > 0 && x < size - 1)
    {
        if(y == 0)
        {
            if(p1D.array[x][y + 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y + 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y + 1] == 0) zeros++;
            else ones++;
        }
        if(y == size - 1)
        {
            if(p1D.array[x][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y - 1] == 0) zeros++;
            else ones++;
        }

        if(y > 0 && y < size - 1)
        {
            if(p1D.array[x - 1][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y + 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x][y + 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x + 1][y + 1] == 0) zeros++;
            else ones++;
        }
    }
    //Checking bottom rows
    if (x == size - 1)
    {
        if(y == 0)
        {
            if(p1D.array[x][y + 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y + 1] == 0) zeros++;
            else ones++;
        }
        if(y == size - 1)
        {
            if(p1D.array[x][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y - 1] == 0) zeros++;
            else ones++;
        }
        else
        {
            if(p1D.array[x][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x][y + 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y - 1] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y] == 0) zeros++;
            else ones++;
            if(p1D.array[x - 1][y + 1] == 0) zeros++;
            else ones++;
        }
    }
    if (zeros > ones){return 0;}
    if (ones > zeros){return 1;}
    return -1;
}

//Function to flip the value of the element depending on value of neighbors
void p1Flip(int x, int y)
{
    if (p1D.array[x][y] == 1) {
        p1D.array[x][y] = 0;
    }
    else
    {
        p1D.array[x][y] = 1;
    }

}

//Function to check if the problem one process has completed
void* p1IsDone(void *arg)
{
    int total = p1D.size * p1D.size;
    int counter = 0;
    while(!p1D.isDone)
    {
        for(int i = 0; i < p1D.size; i++)
        {
            for(int j = 0; j < p1D.size; j++)
                if(p1D.array[i][j] == p1D.array[0][0]) counter++;
        }
        if(counter == total)
        {
            p1D.isDone = true;
            pthread_mutex_unlock(&lock);
            break;
        }
        else counter = 0;
    }
    return nullptr;
}

//Function that creates the problem one subroutine to be executed
void* p1Threads(void *arg)
{
    while(!p1D.isDone)
    {
        int x = rand() % p1D.size;
        int y = rand() % p1D.size;
        pthread_mutex_lock(&lock);
        printf("Thread #%lu generated (%d, %d).\n",pthread_self(), x, y);
        int test = p1CheckNeighbors(x, y);

        if(test == -1 && !p1D.isDone)
        {
            printf("flipping not required for thread #%lu\n",pthread_self());
        }

        if(test != p1D.array[x][y] && test != -1 && !p1D.isDone)
        {
            printf("Thread #%lu flipping the element at (%d, %d).\n",pthread_self(), x, y);
            p1Flip(x, y);
            printf("After flipping: \n");

            for(int i = 0; i < p1D.size; i++)
            {
                for(int j = 0; j < p1D.size; j++)
                    printf("%d ", p1D.array[i][j]);
            }
            printf("\n");
            pthread_mutex_unlock(&lock);
            continue;
        }

        if(p1D.isDone)
        {
            printf("Program completed.\n");
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}

//Function that runs problem one
void problem1(int size)
{
    srand(time(nullptr));
    p1D.isDone = false;
    string threadCount;

    cout << "Problem 1 chosen. Please enter the number of threads to use: ";
    cin >> threadCount;
    if(!isNumber(threadCount) && stoi(threadCount) <= 0)
    {
        cout << "Please enter a valid integer." << endl;
        return;
    }

    p1D.array = new int *[size];
    for(int i = 0; i < size; i++)
    {
        p1D.array[i] = new int[size];
    }
    int temp;

    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            temp = rand();
            p1D.array[i][j] = (temp % 2);
        }
    }

    cout << "Original matrix: " << endl;
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            cout << p1D.array[i][j] << " ";
        }
    }
    cout << endl;

    pthread_t super;
    pthread_create(&super, nullptr, &p1IsDone, nullptr);
    pthread_t threads[stoi(threadCount)];
    for(int i = 0; i < stoi(threadCount); i++)
    {
        pthread_create(&threads[i], nullptr, &p1Threads, (void *) (long) i);
    }

    for(int i = 0; i < stoi(threadCount); i++)
    {
        pthread_join(threads[i], nullptr);
    }

    pthread_join(super, nullptr);
    cout << "Completed matrix: \n";
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            cout << p1D.array[i][j] << " ";
        }
        printf("\n");
    }
    cout << endl;
}

//Function that sorts problem 2
bool p2Sorted()
{
    for(int i = 1; i < p2D.size; i++)
    {
        if (p2D.array[i - 1] > p2D.array[i]){return false;}
    }
    return true;
}

//function that checks if problem two is done
void* p2IsDone(void* arg)
{
    while(!p2D.isDone)
    {
        if(p2Sorted())
        {
            pthread_mutex_unlock(&lock);
            p2D.isDone = true;
            break;
        }
    }
    return nullptr;
}

//Function for problem 2 that moves the index
bool p2MoveIndex(int loc)
{
    bool retval = false;
    int replace = -1;
    int value = p2D.array[loc];
    if(loc == 0){return false;}

    for(int i = 0; i < loc; i++)
    {
        if(p2D.array[i] > p2D.array[loc])
        {
            replace = i;
            retval = true;
            break;
        }
    }

    if(retval)
    {
        for(int i = loc; i > replace; i--)
        {
            p2D.array[i] = p2D.array[i - 1];
        }
        p2D.array[replace] = value;
    }
    return retval;
}

//Function that creates problem two subroutine
void* p2Threads(void *arg)
{
    while(!p2D.isDone)
    {
        int var = rand() % p2D.size;
        pthread_mutex_lock(&lock);
        printf("Thread #%lu generated index: %d\n", pthread_self(), var);

        if(p2D.isDone)
        {
            pthread_mutex_unlock(&lock);
            break;
        }

        bool check = p2MoveIndex(var);
        if(check && !p2D.isDone)
        {
            printf("Array adjusted at location %d. The new array: ",var);

            for(int i = 0; i < p2D.size; i++)
            {
                printf("%d ", p2D.array[i]);
            }
            cout << endl;
        }
        pthread_mutex_unlock(&lock);
    }
    return nullptr;
}

//Function to run problem 2
void problem2(int size)
{
    srand(time(nullptr));
    string threadCount;
    p2D.isDone = false;

    cout << "Problem 2 Chosen. Please enter the number of threads to use: ";
    cin >> threadCount;
    if(!isNumber(threadCount) || stoi(threadCount) <= 0)
    {
        cout << "Please enter a valid integer." << endl;
        return;
    }
    p2D.array = new int[size];
    for(int i = 0; i < size; i++)
    {
        p2D.array[i] = rand() % 100 + 1;
    }

    cout << "Original array: \n";
    for(int i = 0; i < size; i++)
    {
        cout << p2D.array[i] << " ";
    }
    cout << endl;

    pthread_t super;
    pthread_create(&super, nullptr, &p2IsDone, nullptr);
    pthread_t threads[stoi(threadCount)];
    for (int i = 0; i < stoi(threadCount); i++)
    {
        pthread_create(&threads[i], nullptr, &p2Threads, (void*)(long)i);
    }

    for (int i = 0; i < stoi(threadCount); i++)
    {
        pthread_join(threads[i], nullptr);
    }
    pthread_join(super, nullptr);

    cout << "Completed array: \n";
    for (int i = 0; i < size; i++)
    {
        cout << p2D.array[i] << " ";
    }
    cout << endl;
}

//Function to create the menu for selecting either problem and exiting program
void menu(const string& argument)
{
    if(!isNumber(argument) || stoi(argument) <= 0)
    {
        cout << "Please enter a valid integer." << endl;
        return;
    }

    int size = stoi(argument);
    p1D.size = size;
    p2D.size = size;
    string input;

    while(true)
    {
        cout << "[1] Problem 1\n[2] Problem 2\n[3] Exit\n" << endl;
        cout << "Enter: ";
        cin >> input;
        input = input.substr(input.find_first_not_of(' '));

        if (input == "1")
        {
            problem1(size);
            continue;
        }
        if (input == "2")
        {
            problem2(size);
            continue;
        }
        if (input == "3"){break;}
        cout << "Valid inputs: '1' for problem 1 \n"
             << "'2' for problem 2, or '3' to exit." << endl;
    }
}

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        cout << "Enter a valid number of arguments." << endl;
        cout << "example: /a.out 6 " << endl;
        cout << "Where 6 represents the size of the matrix or the array" << endl;
    }

    if (argc == 1)
        menu(DEFAULT);

    menu(argv[1]);
    return 0;
}