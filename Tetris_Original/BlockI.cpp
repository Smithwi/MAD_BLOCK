#include "StdAfx.h"
#include "Tetris.h"
#include "TetrisDlg.h"

/* x표시 되있는 곳이 pivot이고,      */
/* [col-1], [col], [col+1], [col+2]  */
/* 이런식으로 표현되어있다.          */
/*
    case dir
    when 0
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    . . . . * x * * . . . .
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    when 1
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    . . . . . . . . . . . .
    . . . . . * . . . . . . 
    . . . . . x . . . . . . 
    . . . . . * . . . . . . 
    . . . . . * . . . . . . 
    . . . . . . . . . . . .
    . . . . . . . . . . . .
*/

// Block I의 구성
BlockI::BlockI(unsigned char clr, unsigned char **board) : Block(clr, board)
{
    board[0][3] = clr;
    board[0][4] = clr;
    board[0][5] = clr;
    board[0][6] = clr;
}

// 아래로 이동 가능 여부 확인
bool BlockI::canMoveDown()
{
    switch(dir)
    {
    case 0:
        return passable(row + 1, col);
    default:
        return row < CTetrisDlg::ROW - 3 && !board[row + 3][col];
    }
}

// 왼쪽으로 이동 가능 여부 확인
bool BlockI::canMoveLeft()
{
    switch(dir)
    {
    case 0:
        return col > 1 && !board[row][col - 2];
    default:
        return passable(row, col - 1);
    }
}

// 오른쪽으로 이동 가능 여부 확인
bool BlockI::canMoveRight()
{
    switch(dir)
    {
    case 0:
        return col < CTetrisDlg::COL - 3 && !board[row][col + 3];
    default:
        return passable(row, col + 1);
    }
}

// 회전 가능 여부 확인
bool BlockI::canTurn()
{
    switch(dir)
    {
    case 0:
        return row > 0 && row < CTetrisDlg::ROW - 2 && !board[row - 1][col] && !board[row + 1][col] && !board[row + 2][col];
    default:
        if(col > 0 && col < CTetrisDlg::COL - 2)
        {
            unsigned char *thisRow = board[row];
            return !thisRow[col - 1] && !thisRow[col + 1] && !thisRow[col + 2];
        }
        return false;
    }
}

// 아래로 이동
void BlockI::moveDown()
{
    switch(dir)
    {
    case 0:
        clear();
        ++row;
        show();
        return;
    default:
        board[row - 1][col] = 0;
        ++row;
        board[row + 2][col] = clr;
    }
}

// 왼쪽으로 이동
void BlockI::moveLeft()
{
    unsigned char *thisRow = board[row];
    switch(dir)
    {
    case 0:
        thisRow[col + 2] = 0;
        --col;
        thisRow[col - 1] = clr;
        return;
    default:
        clear();
        --col;
        show();
    }
}

// 오른쪽으로 이동
void BlockI::moveRight()
{
    unsigned char *thisRow = board[row];
    switch(dir)
    {
    case 0:
        thisRow[col - 1] = 0;
        ++col;
        thisRow[col + 2] = clr;
        return;
    default:
        clear();
        ++col;
        show();
    }
}

// Block 회전
void BlockI::turn()
{
	// 현재상태를 지우고
    clear();
	// 방향을 바꾼 후
    dir ^= 1;
	// 다시 보여줌
    show();
}

// Block이 내려갈 수 있는지 여부 확인
bool BlockI::passable(char row, char col)
{
    switch(dir)
    {
    case 0:
        if(row < CTetrisDlg::ROW && col > 0 && col < CTetrisDlg::COL - 2)
        {
            unsigned char *thisRow = board[row];
            return !thisRow[col - 1] && !thisRow[col] && !thisRow[col + 1] && !thisRow[col + 2];
        }
        return false;
    default:
        if(row > 0 && row < CTetrisDlg::ROW - 2 && col > -1 && col < CTetrisDlg::COL)
        {
            return !board[row - 1][col] && !board[row][col] && !board[row + 1][col] && !board[row + 2][col];
        }
        return false;
    }
}

// Block 보이기
void BlockI::show()
{
    unsigned char *thisRow = board[row];
    switch(dir)
    {
    case 0:
        thisRow[col - 1] = clr;
        thisRow[col] = clr;
        thisRow[col + 1] = clr;
        thisRow[col + 2] = clr;
        return;
    default:
        board[row - 1][col] = clr;
        board[row][col] = clr;
        board[row + 1][col] = clr;
        board[row + 2][col] = clr;
    }
}

// Block 지우기
void BlockI::clear()
{
    unsigned char *thisRow = board[row];
    switch(dir)
    {
    case 0:
        thisRow[col - 1] = 0;
        thisRow[col] = 0;
        thisRow[col + 1] = 0;
        thisRow[col + 2] = 0;
        return;
    default:
        board[row - 1][col] = 0;
        board[row][col] = 0;
        board[row + 1][col] = 0;
        board[row + 2][col] = 0;
    }
}