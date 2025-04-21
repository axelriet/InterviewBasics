/*++

Module Name:

    Sudoku.cpp

Abstract:

    9x9 Sudoku solver.

Repo:

    https://github.com/axelriet/InterviewBasics.git

Questions / Remarks:

    axelriet@gmail.com

--*/

#include <iostream>

constexpr unsigned SQUARE_BOARD_SIZE{ 9 };
constexpr unsigned SQUARE_BOARD_CORNER_MOD{ SQUARE_BOARD_SIZE / 3 };

using SQUARE_BOARD = int[SQUARE_BOARD_SIZE][SQUARE_BOARD_SIZE];

static inline bool TestRow(SQUARE_BOARD Board, int Value, int Row)
{
    for (int Col = 0; Col < SQUARE_BOARD_SIZE; Col++)
    {
        if (Board[Row][Col] == Value)
        {
            return false;
        }
    }

    return true;
}

static inline bool TestCol(SQUARE_BOARD Board, int Value, int Col)
{
    for (int Row = 0; Row < SQUARE_BOARD_SIZE; Row++)
    {
        if (Board[Row][Col] == Value)
        {
            return false;
        }
    }

    return true;
}

static inline bool TestBox(SQUARE_BOARD Board, int Value, int Row, int Col)
{
    const int CornerRow = Row - (Row % SQUARE_BOARD_CORNER_MOD);
    const int CornerCol = Col - (Col % SQUARE_BOARD_CORNER_MOD);

    for (int RowOffset = 0; RowOffset < SQUARE_BOARD_CORNER_MOD; RowOffset++)
    {
        for (int ColOffset = 0; ColOffset < SQUARE_BOARD_CORNER_MOD; ColOffset++)
        {
            if (Board[CornerRow + RowOffset][CornerCol + ColOffset] == Value)
            {
                return false;
            }
        }
    }

    return true;
}

static inline bool TestValue(SQUARE_BOARD Board, int Value, int Row, int Col)
{
    return TestRow(Board, Value, Row) &&
           TestCol(Board, Value, Col) &&
           TestBox(Board, Value, Row, Col);
}

bool SolveBoard(SQUARE_BOARD Board)
{
    for (int Row = 0; Row < SQUARE_BOARD_SIZE; Row++)
    {
        for (int Col = 0; Col < SQUARE_BOARD_SIZE; Col++)
        {
            if (Board[Row][Col] == 0)
            {
                for (int Value = 1; Value <= SQUARE_BOARD_SIZE; Value++)
                {
                    if (TestValue(Board, Value, Row, Col))
                    {
                        Board[Row][Col] = Value;

                        if (SolveBoard(Board))
                        {
                            return true;
                        }
                        else
                        {
                            //
                            // The board cannot be solved, keep trying...
                            //

                            Board[Row][Col] = 0;
                        }
                    }
                }

                return false;
            }
        }
    }

    return true;
}

bool VerifyBoard(SQUARE_BOARD Board)
{
#ifndef NDEBUG
    //
    // Check the sum of all rows and columns
    //

    for (int Row = 0; Row < SQUARE_BOARD_SIZE; Row++)
    {
        int RowSum{};
        int ColSum{};

        for (int Col = 0; Col < SQUARE_BOARD_SIZE; Col++)
        {
            RowSum += Board[Row][Col];
            ColSum += Board[Col][Row];
        }

        if ((RowSum != ColSum) || (RowSum != (SQUARE_BOARD_SIZE * (SQUARE_BOARD_SIZE + 1) / 2)))
        {
            return false;
        }
    }
#endif

    return true;
}

void PrintBoard(SQUARE_BOARD Board)
{
    for (int Row = 0; Row < SQUARE_BOARD_SIZE; Row++)
    {
        for (int Col = 0; Col < SQUARE_BOARD_SIZE; Col++)
        {
            if (Col && !(Col % SQUARE_BOARD_CORNER_MOD))
            {
                std::cout << " | ";
            }

            std::cout << Board[Row][Col];
        }

        std::cout << "\n---------------\n";
    }
}

int main()
{
    std::cout << "Sudoku!\n\n";

    static SQUARE_BOARD Board =
    {
        { 0, 8, 0,  0, 0, 0,  0, 0, 0 },
        { 7, 0, 0,  0, 0, 2,  0, 0, 1 },
        { 0, 0, 0,  4, 5, 0,  3, 0, 0 },

        { 0, 0, 8,  0, 0, 0,  0, 4, 7 },
        { 0, 5, 0,  7, 0, 4,  0, 0, 0 },
        { 2, 0, 0,  5, 0, 9,  0, 0, 0 },

        { 0, 6, 0,  2, 0, 0,  0, 0, 8 },
        { 0, 0, 0,  0, 0, 7,  0, 2, 0 },
        { 1, 0, 9,  0, 0, 5,  0, 0, 0 }
    };

    PrintBoard(Board);

    if (SolveBoard(Board))
    {
        if (VerifyBoard(Board))
        {
            std::cout << "\nSolved!\n\n";
        }
        else
        {
            std::cout << "\nInvalid solution!\n\n";
        }

        PrintBoard(Board);
    }
    else
    {
        std::cout << "\nNo solution :(\n";
    }
}
