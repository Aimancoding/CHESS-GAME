

#include "chessboard.h"

//Constructor and Destructor

ChessBoard::ChessBoard()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            grid[i][j] = nullptr;
    initializePieces();
}

ChessBoard::~ChessBoard()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            delete grid[i][j];
}

//Board setup

void ChessBoard::initializePieces()
{
   
    grid[0][0] = new Rook("B", 0, 0); grid[0][1] = new Knight("B", 0, 1);
    grid[0][2] = new Bishop("B", 0, 2); grid[0][3] = new Queen("B", 0, 3);
    grid[0][4] = new King("B", 0, 4); grid[0][5] = new Bishop("B", 0, 5);
    grid[0][6] = new Knight("B", 0, 6); grid[0][7] = new Rook("B", 0, 7);
    for (int j = 0; j < 8; j++) grid[1][j] = new Pawn("B", 1, j);

    for (int i = 2; i <= 5; i++)
        for (int j = 0; j < 8; j++)
            grid[i][j] = nullptr;

    for (int j = 0; j < 8; j++) grid[6][j] = new Pawn("W", 6, j);
    grid[7][0] = new Rook("W", 7, 0); grid[7][1] = new Knight("W", 7, 1);
    grid[7][2] = new Bishop("W", 7, 2); grid[7][3] = new Queen("W", 7, 3);
    grid[7][4] = new King("W", 7, 4); grid[7][5] = new Bishop("W", 7, 5);
    grid[7][6] = new Knight("W", 7, 6); grid[7][7] = new Rook("W", 7, 7);
}

ChessPiece* ChessBoard::getPieceAt(int row, int col)
{
    return grid[row][col];
}

string ChessBoard::getCurrentPlayer() const
{
    return activePlayer;
}

int ChessBoard::getMoveCount()
{
    return totalMoves;
}

void ChessBoard::switchPlayer()
{
    activePlayer = (activePlayer == "W") ? "B" : "W";
}
bool ChessBoard::isKingAlive(const string& color)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j] && grid[i][j]->getColor() == color && dynamic_cast<King*>(grid[i][j]))
                return true;
    return false;
}

bool ChessBoard::isInCheck(const string& color)
{
    int kr = -1, kc = -1;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j] && grid[i][j]->getColor() == color && dynamic_cast<King*>(grid[i][j]))
            {
                kr = i; kc = j;
            }
    if (kr == -1) return false;
    string opp = (color == "W") ? "B" : "W";
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j] && grid[i][j]->getColor() == opp)
                if (grid[i][j]->checkValidMove(kr, kc, grid))
                    return true;
    return false;
}

void ChessBoard::getKingPos(const string& color, int& outRow, int& outCol)
{
    outRow = outCol = -1;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j] && grid[i][j]->getColor() == color && dynamic_cast<King*>(grid[i][j]))
            {
                outRow = i; outCol = j;
            }
}

bool ChessBoard::isCheckmate(const string& color)
{
    if (!isInCheck(color)) return false;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j] && grid[i][j]->getColor() == color)
                for (int r = 0; r < 8; r++)
                    for (int c = 0; c < 8; c++)
                        if (grid[i][j]->checkValidMove(r, c, grid))
                        {
                            ChessPiece* tmp = grid[r][c];
                            grid[r][c] = grid[i][j];
                            grid[i][j] = nullptr;
                            grid[r][c]->updatePosition(r, c);
                            bool still = isInCheck(color);
                            grid[i][j] = grid[r][c];
                            grid[r][c] = tmp;
                            grid[i][j]->updatePosition(i, j);
                            if (!still) return false;
                        }
    return true;
}

bool ChessBoard::isStalemate(const string& color)
{
    if (isInCheck(color)) return false;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j] && grid[i][j]->getColor() == color)
                for (int r = 0; r < 8; r++)
                    for (int c = 0; c < 8; c++)
                        if (grid[i][j]->checkValidMove(r, c, grid))
                            return false;
    return true;
}

bool ChessBoard::onlyKingsLeft()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            if (grid[i][j] && !dynamic_cast<King*>(grid[i][j]))
                return false;
    return true;
}

bool ChessBoard::movePiece(int srcRow, int srcCol, int destRow, int destCol)
{
    if (!grid[srcRow][srcCol]) return false;
    ChessPiece* sel = grid[srcRow][srcCol];
    if (sel->getColor() != activePlayer) return false;
    if (!sel->checkValidMove(destRow, destCol, grid, enPassantRow, enPassantCol))
        return false;

    ChessPiece* captured = grid[destRow][destCol];
    grid[destRow][destCol] = sel;
    grid[srcRow][srcCol] = nullptr;
    sel->updatePosition(destRow, destCol);
    bool stillCheck = isInCheck(activePlayer);
    grid[srcRow][srcCol] = sel;
    sel->updatePosition(srcRow, srcCol);
    grid[destRow][destCol] = captured;
    if (stillCheck) return false;

    if (grid[destRow][destCol]) delete grid[destRow][destCol];
    grid[destRow][destCol] = sel;
    grid[srcRow][srcCol] = nullptr;
    sel->updatePosition(destRow, destCol);
    totalMoves++;
    if (dynamic_cast<Pawn*>(sel) && abs(destRow - srcRow) == 2)
    {
        enPassantRow = destRow; enPassantCol = destCol;
    }
    else
        enPassantRow = enPassantCol = -1;

    if (dynamic_cast<Pawn*>(sel) && destCol != srcCol && captured == nullptr)
        if (grid[srcRow][destCol] && grid[srcRow][destCol]->getColor() != activePlayer)
        {
            delete grid[srcRow][destCol]; grid[srcRow][destCol] = nullptr;
        }

    
    if (dynamic_cast<Pawn*>(sel))
    {
        if (sel->getColor() == "W" && destRow == 0)
        {
            delete grid[destRow][destCol]; grid[destRow][destCol] = new Queen("W", destRow, destCol);
        }
        if (sel->getColor() == "B" && destRow == 7)
        {
            delete grid[destRow][destCol]; grid[destRow][destCol] = new Queen("B", destRow, destCol);
        }
    }
    if (auto* k = dynamic_cast<King*>(grid[destRow][destCol])) k->markAsMoved();
    if (auto* r = dynamic_cast<Rook*>(grid[destRow][destCol])) r->markAsMoved();

    return true;
}

bool ChessBoard::castling(const string& color, const string& side)
{
    int kingRow = (color == "W") ? 7 : 0;
    King* kp = dynamic_cast<King*>(grid[kingRow][4]);
    if (!kp || kp->hasMoved() || isInCheck(color)) return false;
    string opp = (color == "W") ? "B" : "W";

    if (side == "right")
    {
        Rook* rp = dynamic_cast<Rook*>(grid[kingRow][7]);
        if (!rp || rp->hasMoved()) return false;
        if (grid[kingRow][5] || grid[kingRow][6]) return false;
        if (isSquareUnderAttack(kingRow, 5, opp, grid) ||
            isSquareUnderAttack(kingRow, 6, opp, grid)) return false;
        grid[kingRow][6] = kp; grid[kingRow][5] = rp;
        grid[kingRow][4] = nullptr; grid[kingRow][7] = nullptr;
        kp->updatePosition(kingRow, 6); kp->markAsMoved();
        rp->updatePosition(kingRow, 5); rp->markAsMoved();
    }
    else if (side == "left")
    {
        Rook* rp = dynamic_cast<Rook*>(grid[kingRow][0]);
        if (!rp || rp->hasMoved()) return false;
        if (grid[kingRow][1] || grid[kingRow][2] || grid[kingRow][3]) return false;
        if (isSquareUnderAttack(kingRow, 2, opp, grid) ||
            isSquareUnderAttack(kingRow, 3, opp, grid)) return false;
        grid[kingRow][2] = kp; grid[kingRow][3] = rp;
        grid[kingRow][4] = nullptr; grid[kingRow][0] = nullptr;
        kp->updatePosition(kingRow, 2); kp->markAsMoved();
        rp->updatePosition(kingRow, 3); rp->markAsMoved();
    }
    else return false;
    return true;
}