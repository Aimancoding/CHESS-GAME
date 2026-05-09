
#pragma once

#include <string>
#include <cmath>
using namespace std;
class ChessPiece
{
protected:
    string pieceColor;
    int rowPos, colPos;
public:
    ChessPiece() : pieceColor(" "), rowPos(0), colPos(0) {}
    ChessPiece(string pieceColor, int rowPos, int colPos)
        : pieceColor(pieceColor), rowPos(rowPos), colPos(colPos) {}

    string getColor() { return pieceColor; }
    int    getRowPos() { return rowPos; }
    int    getColPos() { return colPos; }
    void   updatePosition(int r, int c) { rowPos = r; colPos = c; }

    virtual bool   checkValidMove(int destRow, int destCol,
        ChessPiece* gameBoard[8][8],
        int epRow = -1, int epCol = -1) = 0;
    virtual string getPieceSymbol() = 0;
    virtual ~ChessPiece() {}
};
//  PAWN
class Pawn : public ChessPiece {
public:
    Pawn(string c, int r, int col) : ChessPiece(c, r, col) {}
    string getPieceSymbol() override { return (pieceColor == "W") ? "P" : "p"; }

    bool checkValidMove(int destRow, int destCol,
        ChessPiece* gameBoard[8][8],
        int epRow = -1, int epCol = -1) override
    {
        int srcRow = rowPos, srcCol = colPos;
        if (pieceColor == "W")
        {
            if (destRow == srcRow - 1 && destCol == srcCol && !gameBoard[destRow][destCol])
                return true;
            if (srcRow == 6 && destRow == srcRow - 2 && destCol == srcCol
                && !gameBoard[destRow][destCol] && !gameBoard[srcRow - 1][srcCol])
                return true;
            if (destRow == srcRow - 1 && (destCol == srcCol + 1 || destCol == srcCol - 1))
                if (gameBoard[destRow][destCol] && gameBoard[destRow][destCol]->getColor() == "B")
                    return true;
        }
        else
        {
            if (destRow == srcRow + 1 && destCol == srcCol && !gameBoard[destRow][destCol])
                return true;
            if (srcRow == 1 && destRow == srcRow + 2 && destCol == srcCol
                && !gameBoard[destRow][destCol] && !gameBoard[srcRow + 1][srcCol])
                return true;
            if (destRow == srcRow + 1 && (destCol == srcCol + 1 || destCol == srcCol - 1))
                if (gameBoard[destRow][destCol] && gameBoard[destRow][destCol]->getColor() == "W")
                    return true;
        }
        if (pieceColor == "W" && srcRow == 3)
            if (epRow == 3 && (epCol == srcCol + 1 || epCol == srcCol - 1))
                if (destRow == 2 && destCol == epCol) return true;
        if (pieceColor == "B" && srcRow == 4)
            if (epRow == 4 && (epCol == srcCol + 1 || epCol == srcCol - 1))
                if (destRow == 5 && destCol == epCol) return true;
        return false;
    }
};
//  ROOK
class Rook : public ChessPiece {
    bool rookMoved = false;
public:
    Rook(string c, int r, int col) : ChessPiece(c, r, col) {}
    string getPieceSymbol() override { return (pieceColor == "W") ? "R" : "r"; }
    void markAsMoved() { rookMoved = true; }
    bool hasMoved() { return rookMoved; }

    bool checkValidMove(int destRow, int destCol,
        ChessPiece* gameBoard[8][8],
        int epRow = -1, int epCol = -1) override
    {
        int srcRow = rowPos, srcCol = colPos;
        if (destRow != srcRow && destCol != srcCol) return false;
        if (destRow == srcRow) {
            int step = (destCol > srcCol) ? 1 : -1;
            for (int j = srcCol + step; j != destCol; j += step)
                if (gameBoard[srcRow][j]) return false;
        }
        else {
            int step = (destRow > srcRow) ? 1 : -1;
            for (int i = srcRow + step; i != destRow; i += step)
                if (gameBoard[i][srcCol]) return false;
        }
        if (gameBoard[destRow][destCol] && gameBoard[destRow][destCol]->getColor() == pieceColor)
            return false;
        return true;
    }
};
//implemented rook logic

//  KNIGHT
class Knight : public ChessPiece {
public:
    Knight(string c, int r, int col) : ChessPiece(c, r, col) {}
    string getPieceSymbol() override { return (pieceColor == "W") ? "N" : "n"; }

    bool checkValidMove(int destRow, int destCol,
        ChessPiece* gameBoard[8][8],
        int epRow = -1, int epCol = -1) override
    {
        int dr = abs(destRow - rowPos), dc = abs(destCol - colPos);
        if (!((dr == 2 && dc == 1) || (dr == 1 && dc == 2))) return false;
        if (gameBoard[destRow][destCol] && gameBoard[destRow][destCol]->getColor() == pieceColor)
            return false;
        return true;
    }
};


//  BISHOP
class Bishop : public ChessPiece {
public:
    Bishop(string c, int r, int col) : ChessPiece(c, r, col) {}
    string getPieceSymbol() override { return (pieceColor == "W") ? "B" : "b"; }

    bool checkValidMove(int destRow, int destCol,
        ChessPiece* gameBoard[8][8],
        int epRow = -1, int epCol = -1) override
    {
        int srcRow = rowPos, srcCol = colPos;
        if (abs(destRow - srcRow) != abs(destCol - srcCol)) return false;
        int rs = (destRow > srcRow) ? 1 : -1, cs = (destCol > srcCol) ? 1 : -1;
        int r = srcRow + rs, c = srcCol + cs;
        while (r != destRow) { if (gameBoard[r][c]) return false; r += rs; c += cs; }
        if (gameBoard[destRow][destCol] && gameBoard[destRow][destCol]->getColor() == pieceColor)
            return false;
        return true;
    }
};
// fixed movement bugs
//  QUEEN
class Queen : public ChessPiece {
public:
    Queen(string c, int r, int col) : ChessPiece(c, r, col) {}
    string getPieceSymbol() override { return (pieceColor == "W") ? "Q" : "q"; }

    bool checkValidMove(int destRow, int destCol,
        ChessPiece* gameBoard[8][8],
        int epRow = -1, int epCol = -1) override
    {
        int srcRow = rowPos, srcCol = colPos;
        int dr = abs(destRow - srcRow), dc = abs(destCol - srcCol);
        if (destRow == srcRow || destCol == srcCol) {
            if (destRow == srcRow) {
                int step = (destCol > srcCol) ? 1 : -1;
                for (int j = srcCol + step; j != destCol; j += step)
                    if (gameBoard[srcRow][j]) return false;
            }
            else {
                int step = (destRow > srcRow) ? 1 : -1;
                for (int i = srcRow + step; i != destRow; i += step)
                    if (gameBoard[i][srcCol]) return false;
            }
        }
        else if (dr == dc) {
            int rs = (destRow > srcRow) ? 1 : -1, cs = (destCol > srcCol) ? 1 : -1;
            int r = srcRow + rs, c = srcCol + cs;
            while (r != destRow) { if (gameBoard[r][c]) return false; r += rs; c += cs; }
        }
        else return false;
        if (gameBoard[destRow][destCol] && gameBoard[destRow][destCol]->getColor() == pieceColor)
            return false;
        return true;
    }
};
//  KING
class King : public ChessPiece {
    bool kingMoved = false;
public:
    King(string c, int r, int col) : ChessPiece(c, r, col) {}
    string getPieceSymbol() override { return (pieceColor == "W") ? "K" : "k"; }
    void markAsMoved() { kingMoved = true; }
    bool hasMoved() { return kingMoved; }

    bool checkValidMove(int destRow, int destCol,
        ChessPiece* gameBoard[8][8],
        int epRow = -1, int epCol = -1) override
    {
        if (abs(destRow - rowPos) > 1 || abs(destCol - colPos) > 1) return false;
        if (gameBoard[destRow][destCol] && gameBoard[destRow][destCol]->getColor() == pieceColor)
            return false;
        return true;
    }
};
//Added valid move logic
inline bool isSquareUnderAttack(int targetRow, int targetCol,
    const string& attackerColor,
    ChessPiece* gameBoard[8][8])
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (gameBoard[i][j] && gameBoard[i][j]->getColor() == attackerColor)
                if (gameBoard[i][j]->checkValidMove(targetRow, targetCol, gameBoard))
                    return true;
    return false;
}
//implememted pawn movement
