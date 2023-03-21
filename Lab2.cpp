/*
Author: Jared Gogel
CS470
02/01/2023
"I pledge that I have neither given nor received help
from anyone other than the instructor/TA for all program
components included here!"
*/
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

//Function to print board
void makeBoard(vector<vector<string>> board, int row, int col)
{
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            cout << board[i][j] << " ";
        }
    cout << endl;
    }
}

//Function to handle invalid inputs for player moves
bool invalidMove(vector<vector<string>> &board, int x, int y)
{
    if (x < 0 || y < 0 || x >= board.size() || y >= board.size())
    {
        cout << "Invalid move. Coordinates out of bounds." << endl;
        cout << "Please enter two numbers for x and y coordinates within bounds of the grid i.e. 1 0." << endl;
        return true;
    }
    if (board[x][y] != "|")
    {
        cout << "Invalid move. The space is already taken." << endl;
        cout << "Please enter two numbers for x and y coordinates within bounds of the grid i.e. 1 0." << endl;
        return true;
    }
    return false;
}

//Function to check for a win along the rows of the board
bool rowVic(vector<vector<string>> &board, const string& playerID)
{
    bool vic = true;
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board.size(); j++)
        {
            if (board[j][i] != playerID)
            {
                vic = false;
            }
        }
        if (vic)
        {
            return true;
        }
        else
        {
            vic = true;
        }
    }
    return false;
}

//Function to check for a win along the columns of the board
bool colVic(vector<vector<string>> &board, const string& playerID)
{
    bool vic = true;

    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board.size(); j++)
        {
            if (board[i][j] != playerID)
            {
                vic = false;
            }
        }
        if (vic)
        {
            return true;
        }
        else
        {
            vic = true;
        }
    }
    return false;
}

//Function to check for wins along the diagonals of the board
bool diaVic(vector<vector<string>> &board, const string& playerID)
{
    bool vic = true;
    for (int i = 0; i < board.size(); i++)
    {
        if (board[i][i] != playerID)
        {
            vic = false;
        }
    }
    if (vic)
    {
        return true;
    }
    else
    {
        vic = true;
    }

    for (int i = board.size() - 1; i > -1; i--)
    {
        if (board[board.size() - i - 1][i] != playerID)
        {
            vic = false;
        }
    }
    if (vic)
    {
        return true;
    }
    else
    {
        vic = true;
    }
    return false;
}

//Function that runs the win condition functions and outputs the winner
bool win(vector<vector<string>> &board, const string& playerID)
{
    if (rowVic(board, playerID) || colVic(board, playerID) || diaVic(board, playerID))
    {
        if (playerID == "X")
        {
            cout << "Player 1 wins!\n" << endl;
        }
        else
        {
            cout << "Player 2 wins!\n" << endl;
        }
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {

    //Error checking the command line arguments for no parameters and limiting grids to equal numbers above 3
    if (argc == 1 )
    {
        cout << "The program was called with no parameters." << endl;
        cout << "Please enter two equal numbers for the grid size i.e. 3 3 or 9 9" << endl;
        exit(1);
    }
    else if (atoi(argv[1]) != atoi(argv[2]) || argc != 3 || atoi(argv[1]) < 3)
    {
        cout << "Invalid input for grid size entered." << endl;
        cout << "Please enter two equal numbers greater than 2 for the grid size i.e. 3 3 or 9 9" << endl;
        exit(1);
    }

    //Creating 2d vector of board with size from command line arguments
    int row = atoi(argv[1]);
    int col = atoi(argv[2]);
    vector<vector<string>> board(row, vector<string> (col, "|"));

    makeBoard(board, row, col);

    //Creating players and running game
    string playerID = "X";
    int counter = 0;

    do
    {
        int x;
        int y;
        cout << playerID << "'s turn. Please enter in a valid coordinate to place your symbol: ";
        cin >> x >> y;

        //Checking for invalid moves from player input
        if (invalidMove(board, x, y)){
            continue;
        }

        //updating board with correct player input
        counter++;
        board[x][y] = playerID;

        //Checking for win conditions
        if (win(board, playerID))
        {
         break;
        }

        //Changing to next player
        playerID = (playerID == "X") ? "O" : "X";

        //Printing updated board
        makeBoard(board, row, col);

        //Checking for draw
        if (counter == (x * y))
        {
            cout << "Draw!" << endl;
            break;
        }
    } while (true);

    //Printing final board after win or draw and clearing memory
    makeBoard(board, row, col);
    board.clear();
    return 0;
}



