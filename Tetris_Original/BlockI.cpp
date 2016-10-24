#include "StdAfx.h"
#include "Tetris.h"
#include "TetrisDlg.h"

/* xǥ�� ���ִ� ���� pivot�̰�,      */
/* [col-1], [col], [col+1], [col+2]  */
/* �̷������� ǥ���Ǿ��ִ�.          */
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

// Block I�� ����
BlockI::BlockI(unsigned char clr, unsigned char **board) : Block(clr, board)
{
    board[0][3] = clr;
    board[0][4] = clr;
    board[0][5] = clr;
    board[0][6] = clr;
}

// �Ʒ��� �̵� ���� ���� Ȯ��
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

// �������� �̵� ���� ���� Ȯ��
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

// ���������� �̵� ���� ���� Ȯ��
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

// ȸ�� ���� ���� Ȯ��
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

// �Ʒ��� �̵�
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

// �������� �̵�
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

// ���������� �̵�
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

// Block ȸ��
void BlockI::turn()
{
	// ������¸� �����
    clear();
	// ������ �ٲ� ��
    dir ^= 1;
	// �ٽ� ������
    show();
}

// Block�� ������ �� �ִ��� ���� Ȯ��
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

// Block ���̱�
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

// Block �����
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