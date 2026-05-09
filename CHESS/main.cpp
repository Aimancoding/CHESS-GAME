
#include "ChessBoard.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
using namespace std;

//Configuration
const int    SQUARE_SIZE = 80;
const int    BOARD_SIZE = 8;
const int    WINDOW_W = SQUARE_SIZE * BOARD_SIZE;       // 640
const int    WINDOW_H = SQUARE_SIZE * BOARD_SIZE + 60;  // 700
const string FONT_PATH = "DejaVuSans.ttf";

//Colours
const sf::Color LIGHT_SQ(240, 217, 181);
const sf::Color DARK_SQ(181, 136, 99);
const sf::Color HIGHLIGHT(130, 200, 100, 160);
const sf::Color LAST_MOVE(200, 200, 80, 120);
const sf::Color CHECK_CLR(220, 60, 60, 180);
//  GUI HELPERS

// Map piece symbol
static sf::String getPieceGlyph(const string& sym)
{
    if (sym == "K") return sf::String(L"\u2654");
    if (sym == "Q") return sf::String(L"\u2655");
    if (sym == "R") return sf::String(L"\u2656");
    if (sym == "B") return sf::String(L"\u2657");
    if (sym == "N") return sf::String(L"\u2658");
    if (sym == "P") return sf::String(L"\u2659");
    if (sym == "k") return sf::String(L"\u265A");
    if (sym == "q") return sf::String(L"\u265B");
    if (sym == "r") return sf::String(L"\u265C");
    if (sym == "b") return sf::String(L"\u265D");
    if (sym == "n") return sf::String(L"\u265E");
    if (sym == "p") return sf::String(L"\u265F");
    return "";
}

//8×8 board with highlight layers
static void drawBoard(sf::RenderWindow& window,
    int selRow, int selCol,
    int lastSrcRow, int lastSrcCol,
    int lastDstRow, int lastDstCol,
    int checkRow, int checkCol)
{
    sf::RectangleShape sq(sf::Vector2f((float)SQUARE_SIZE, (float)SQUARE_SIZE));

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            
            sq.setFillColor((i + j) % 2 == 0 ? LIGHT_SQ : DARK_SQ);
            sq.setPosition(sf::Vector2f((float)(j * SQUARE_SIZE), (float)(i * SQUARE_SIZE)));
            window.draw(sq);

            
            if ((i == lastSrcRow && j == lastSrcCol) || (i == lastDstRow && j == lastDstCol))
            {
                sq.setFillColor(LAST_MOVE); window.draw(sq);
            }

            
            if (i == checkRow && j == checkCol)
            {
                sq.setFillColor(CHECK_CLR); window.draw(sq);
            }

           
            if (i == selRow && j == selCol)
            {
                sq.setFillColor(HIGHLIGHT); window.draw(sq);
            }
        }
    }
}

// all pieces
static void drawPieces(sf::RenderWindow& window, ChessBoard& board, sf::Font& font)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ChessPiece* p = board.getPieceAt(i, j);
            if (!p) continue;

            sf::Text text(font, getPieceGlyph(p->getPieceSymbol()), 52);

            if (p->getColor() == "W")
                text.setFillColor(sf::Color(255, 255, 255));
            else
                text.setFillColor(sf::Color(20, 20, 20));

            text.setOutlineThickness(1.5f);
            text.setOutlineColor(p->getColor() == "W"
                ? sf::Color(40, 40, 40)
                : sf::Color(200, 200, 200));

           
            sf::FloatRect bounds = text.getLocalBounds();
            float px = j * SQUARE_SIZE + (SQUARE_SIZE - bounds.size.x) / 2.f - bounds.position.x;
            float py = i * SQUARE_SIZE + (SQUARE_SIZE - bounds.size.y) / 2.f - bounds.position.y;
            text.setPosition(sf::Vector2f(px, py));
            window.draw(text);
        }
    }
}
static void drawStatus(sf::RenderWindow& window,
    sf::Font& font,
    const string& msg,
    sf::Color color = sf::Color(230, 230, 220))
{
    sf::RectangleShape bar(sf::Vector2f((float)WINDOW_W, 60.f));
    bar.setPosition(sf::Vector2f(0.f, (float)(WINDOW_H - 60)));
    bar.setFillColor(sf::Color(40, 40, 40));
    window.draw(bar);

    sf::Text text(font, msg, 20);
    text.setFillColor(color);

    sf::FloatRect b = text.getLocalBounds();
    text.setPosition(sf::Vector2f(
        (WINDOW_W - b.size.x) / 2.f - b.position.x,
        (float)(WINDOW_H - 60) + (60 - b.size.y) / 2.f - b.position.y
    ));
    window.draw(text);
}
//  MAIN
// ============================================================
int main()
{
    // Created window
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u((unsigned)WINDOW_W, (unsigned)WINDOW_H)),
        "Chess"
    );
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    if (!font.openFromFile(FONT_PATH))
    {
        bool loaded =
            font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") ||
            font.openFromFile("C:/Windows/Fonts/arial.ttf") ||
            font.openFromFile("/System/Library/Fonts/Helvetica.ttc");
        if (!loaded)
        {
            cout << "ERROR: Cannot load font '" << FONT_PATH << "'.\n"
                << "Place DejaVuSans.ttf next to the executable, "
                << "or update FONT_PATH in main.cpp.\n";
            return 1;
        }
    }

    // ── Game state
    ChessBoard game;

    bool      pieceSelected = false;
    int       selRow = -1, selCol = -1;
    int       lastSrcRow = -1, lastSrcCol = -1;
    int       lastDstRow = -1, lastDstCol = -1;
    bool      gameOver = false;
    bool      castlingMode = false;
    string    statusMsg = "White's turn";
    sf::Color statusColor(230, 230, 220);

    while (window.isOpen())
    {
        while (auto eventOpt = window.pollEvent())
        {
            const sf::Event& event = *eventOpt;

            // Window close
            if (event.is<sf::Event::Closed>())
                window.close();

            if (!gameOver)
            {
                if (const auto* kp = event.getIf<sf::Event::KeyPressed>())
                {
                    //castling mode
                    if (kp->code == sf::Keyboard::Key::C)
                    {
                        castlingMode = !castlingMode;
                        statusMsg = castlingMode
                            ? "Castling: click king-side (right) or queen-side (left) rook"
                            : (game.getCurrentPlayer() == "W" ? "White's turn" : "Black's turn");
                        statusColor = sf::Color(230, 230, 220);
                    }
                   
                    if (kp->code == sf::Keyboard::Key::Escape)
                    {
                        pieceSelected = castlingMode = false;
                        selRow = selCol = -1;
                        statusMsg = (game.getCurrentPlayer() == "W") ? "White's turn" : "Black's turn";
                        statusColor = sf::Color(230, 230, 220);
                    }
                }
            }

            // ── Mouse click
            if (!gameOver)
            {
                if (const auto* mb = event.getIf<sf::Event::MouseButtonPressed>())
                {
                    if (mb->button == sf::Mouse::Button::Left)
                    {
                        int col = mb->position.x / SQUARE_SIZE;
                        int row = mb->position.y / SQUARE_SIZE;

                        
                        if (row >= 8) goto next_event;

                        //Castling mode
                        if (castlingMode)
                        {
                            string side = (col >= 4) ? "right" : "left";
                            if (game.castling(game.getCurrentPlayer(), side))
                            {
                                game.switchPlayer();
                                lastSrcRow = lastDstRow = -1;
                                castlingMode = false;
                                statusColor = sf::Color(230, 230, 220);
                                statusMsg = (game.getCurrentPlayer() == "W") ? "White's turn" : "Black's turn";

                                string cur = game.getCurrentPlayer();
                                if (game.isCheckmate(cur)) {
                                    statusMsg = (cur == "W") ? "CHECKMATE! Black wins!" : "CHECKMATE! White wins!";
                                    statusColor = sf::Color(255, 100, 80);
                                    gameOver = true;
                                }
                                else if (game.isStalemate(cur)) {
                                    statusMsg = "STALEMATE — Draw!";
                                    statusColor = sf::Color(200, 200, 80);
                                    gameOver = true;
                                }
                            }
                            else
                            {
                                statusMsg = "Castling not possible! Press C to cancel.";
                                statusColor = sf::Color(255, 160, 80);
                            }
                            goto next_event;
                        }

                        if (!pieceSelected)
                        {
                            
                            ChessPiece* p = game.getPieceAt(row, col);
                            if (p && p->getColor() == game.getCurrentPlayer())
                            {
                                selRow = row; selCol = col;
                                pieceSelected = true;
                                statusMsg = "Selected — click destination";
                                statusColor = sf::Color(230, 230, 220);
                            }
                            else
                            {
                                statusMsg = (game.getCurrentPlayer() == "W") ? "White's turn" : "Black's turn";
                                statusColor = sf::Color(230, 230, 220);
                            }
                        }
                        else
                        {
                            if (row == selRow && col == selCol)
                            {
                                
                                pieceSelected = false;
                                selRow = selCol = -1;
                                statusMsg = (game.getCurrentPlayer() == "W") ? "White's turn" : "Black's turn";
                                statusColor = sf::Color(230, 230, 220);
                            }
                            else if (game.movePiece(selRow, selCol, row, col))
                            {
                                
                                lastSrcRow = selRow; lastSrcCol = selCol;
                                lastDstRow = row;    lastDstCol = col;
                                pieceSelected = false;
                                selRow = selCol = -1;
                                game.switchPlayer();

                                string cur = game.getCurrentPlayer();
                                statusColor = sf::Color(230, 230, 220);

                                if (game.isCheckmate(cur)) {
                                    statusMsg = (cur == "W") ? "CHECKMATE! Black wins!" : "CHECKMATE! White wins!";
                                    statusColor = sf::Color(255, 100, 80);
                                    gameOver = true;
                                }
                                else if (!game.isKingAlive("W")) {
                                    statusMsg = "Black wins!";
                                    statusColor = sf::Color(255, 100, 80);
                                    gameOver = true;
                                }
                                else if (!game.isKingAlive("B")) {
                                    statusMsg = "White wins!";
                                    statusColor = sf::Color(100, 200, 100);
                                    gameOver = true;
                                }
                                else if (game.isStalemate(cur)) {
                                    statusMsg = "STALEMATE — Draw!";
                                    statusColor = sf::Color(200, 200, 80);
                                    gameOver = true;
                                }
                                else if (game.getMoveCount() >= 50) {
                                    statusMsg = "DRAW — 50-move limit reached!";
                                    statusColor = sf::Color(200, 200, 80);
                                    gameOver = true;
                                }
                                else if (game.onlyKingsLeft()) {
                                    statusMsg = "DRAW — only kings remain!";
                                    statusColor = sf::Color(200, 200, 80);
                                    gameOver = true;
                                }
                                else if (game.isInCheck(cur)) {
                                    statusMsg = (cur == "W" ? "White" : "Black");
                                    statusMsg += " is in CHECK!";
                                    statusColor = sf::Color(255, 200, 80);
                                }
                                else {
                                    statusMsg = (cur == "W") ? "White's turn" : "Black's turn";
                                }
                            }
                            else
                            {
                                
                                statusMsg = "Invalid move — try again";
                                statusColor = sf::Color(255, 130, 80);
                            }
                        }
                    } 
                } 
            } 

        next_event:;
        } 

       
        int checkRow = -1, checkCol = -1;
        if (!gameOver && game.isInCheck(game.getCurrentPlayer()))
            game.getKingPos(game.getCurrentPlayer(), checkRow, checkCol);

       
        window.clear(sf::Color(30, 30, 30));

        drawBoard(window,
            selRow, selCol,
            lastSrcRow, lastSrcCol,
            lastDstRow, lastDstCol,
            checkRow, checkCol);

        drawPieces(window, game, font);
        drawStatus(window, font, statusMsg, statusColor);

        window.display();
    }
//fixed player turn bug
    return 0;
}
