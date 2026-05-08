
#pragma once

#include "chesspieces.h"
#include <string>
using namespace std;

class ChessBoard
{
    ChessPiece* grid[8][8];
    string activePlayer = "W";
    int    totalMoves = 0;
    int    enPassantRow = -1;
    int    enPassantCol = -1;

    void initializePieces();

public:
    ChessBoard();
    ~ChessBoard();

    ChessPiece* getPieceAt(int row, int col);
    string      getCurrentPlayer() const;
    int         getMoveCount();

    void switchPlayer();

    bool isKingAlive(const string& color);
    bool isInCheck(const string& color);
    bool isCheckmate(const string& color);
    bool isStalemate(const string& color);
    bool onlyKingsLeft();
    void getKingPos(const string& color, int& outRow, int& outCol);

    bool movePiece(int srcRow, int srcCol, int destRow, int destCol);
    bool castling(const string& color, const string& side);
};