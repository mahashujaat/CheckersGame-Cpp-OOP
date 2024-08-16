#include <iostream>
#include <SFML/Graphics.hpp>
#define NOMINMAX
#include <windows.h>
#include "source.h"
#include <WinUser.h>
sf::Font font;
const int size = 8;

LRESULT CALLBACK Game::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Board::Draw(sf::RenderWindow& window) {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(75.f, 75.f));
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                tile.setPosition(sf::Vector2f(75 * i, 75 * j));
                if ((i + j) % 2 == 0) {
                    tile.setFillColor(sf::Color::White);
                }
                else {
                    tile.setFillColor(sf::Color::Black);
                }
                window.draw(tile);
            }
        }
    }

void Board::Highlight(sf::RenderWindow& window, int x, int y) {
        sf::RectangleShape tile;
        tile.setSize(sf::Vector2f(75.f, 75.f));
        tile.setFillColor(sf::Color::Green);
        tile.setPosition(sf::Vector2f(75 * x, 75 * y));
        window.draw(tile);
    
};



void Piece::Draw(sf::RenderWindow& window) {
        if (isAlive) {
            sf::CircleShape shape(radius);
            shape.setFillColor(color);
            if (isKing) {
                shape.setOutlineThickness(3.f);
                shape.setOutlineColor(sf::Color::Yellow);
            }
            shape.setPosition(sf::Vector2f(x * 75 + (75 - radius * 2) / 2, y * 75 + (75 - 2 * radius) / 2));
            window.draw(shape);
        }
    
};


Engine::Engine() : turn(1), SelectedPiece(nullptr), selected(false), invalidMove(false), isMessageBoxDisplayed(false) {
    // Constructor code...


    RedPieces = new Piece[12];
    WhitePieces = new Piece[12];

    for (int i = 0; i < 12; i++) {
        WhitePieces[i].color = sf::Color::White;
        RedPieces[i].color = sf::Color::Red;
    }

    Setup(RedPieces, WhitePieces);

}   

Engine::~Engine() {
        delete[] RedPieces;
        delete[] WhitePieces;
        
       
    }

void Engine::Setup(Piece* RedPieces, Piece* WhitePieces) {
    int m = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = (int)!(i % 2 & 1); j < 8; j += 2) {
            WhitePieces[m].isKing = false;
            WhitePieces[m].isAlive = true;
            WhitePieces[m].x = j;
            WhitePieces[m].y = i;
            m++;
        }
    }
    m = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = (int)(i % 2 & 1); j < 8; j += 2) {
            RedPieces[m].isKing = false;
            RedPieces[m].isAlive = true;
            RedPieces[m].x = j;
            RedPieces[m].y = 7 - i;
            m++;
        }
    }


}


Piece* Engine::FindPiece(int x, int y, Piece* RedPieces, Piece* WhitePieces) {
        for (int i = 0; i < 12; i++) {
            if (RedPieces[i].x == x && RedPieces[i].y == y) {
                if (RedPieces[i].isAlive) {
                    return &RedPieces[i];
                }
            }
            if (WhitePieces[i].x == x && WhitePieces[i].y == y) {
                if (WhitePieces[i].isAlive) {
                    return &WhitePieces[i];
                }
            }
        }
        return nullptr;
    }

void Engine::KillPiece(int x, int y) {
    Piece* piece = FindPiece(x, y, RedPieces, WhitePieces);
    if (piece) {
        piece->isAlive = false;
        if (piece->color == sf::Color::Red) {
            if (AreAllPiecesKilled(RedPieces)) {
                MessageBoxW(NULL, L"WHITE WON!", L"Game Over", MB_ICONINFORMATION);
            }
        }
        else if (piece->color == sf::Color::White) {
            if (AreAllPiecesKilled(WhitePieces)) {
                MessageBoxW(NULL, L"RED WON!", L"Game Over", MB_ICONINFORMATION);
            }
        }
        turn = (turn == 1) ? 2 : 1;
    }
}


int Engine::MovePiece(int x, int y, Piece* s_Piece) {
    int dx = abs(x - s_Piece->x);
    int dy = abs(y - s_Piece->y);

    if (dx != dy || dx > 2) {
        if (!isMessageBoxDisplayed) {
            MessageBoxW(NULL, L"Invalid move! You can move diagonally forward.", L"Invalid Move", MB_ICONERROR);
            isMessageBoxDisplayed = true; // Set the flag to indicate a message box is displayed
        }
        return 0;
    }
    if (s_Piece->color == sf::Color::Red && y == 0 && !s_Piece->isKing) {
        //turn = (turn == 1) ? 2 : 1;
        s_Piece->isKing = true;
        
    }
    else if (s_Piece->color == sf::Color::White && y == size - 1 && !s_Piece->isKing) {
        //turn = (turn == 1) ? 2 : 1;
        s_Piece->isKing = true;
        
    }

    
    if (s_Piece->isKing) {
        if (dx == 1 || dx == -1) {
            if (dy == 1 || dy == -1) {
                // Check if there's a piece at the destination
                Piece* destPiece = FindPiece(x, y, RedPieces, WhitePieces);
                if (destPiece == nullptr) {
                    turn = (turn == 1) ? 2 : 1;
                    // Valid diagonal move for a king
                    s_Piece->x = x;
                    s_Piece->y = y;
                    return 1;
                }
            }
        }
        // Valid jump (capture) for a king (no change)
        if (dx == 2 || dx == -2) {
            if (dy == 2 || dy == -2) {
                int midX = (x + s_Piece->x) / 2;
                int midY = (y + s_Piece->y) / 2;
                Piece* capturedPiece = FindPiece(midX, midY, RedPieces, WhitePieces);

                // Check if there's a piece at the destination
                Piece* destPiece = FindPiece(x, y, RedPieces, WhitePieces);

                if (capturedPiece != nullptr && capturedPiece->color != s_Piece->color && destPiece == nullptr) {
                    turn = (turn == 1) ? 2 : 1;
                    // Capture the opponent's piece and move
                    KillPiece(midX, midY);
                    s_Piece->x = x;
                    s_Piece->y = y;
                    turn = (turn == 1) ? 2 : 1;
                    return 1;
                }
            }
        }
    }

    // Conditions for non-king Red pieces
    if (s_Piece->color == sf::Color::Red && !s_Piece->isKing) {
        if (x == s_Piece->x - 1 && y == s_Piece->y - 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                turn = (turn == 1) ? 2 : 1;
                s_Piece->x = x;
                s_Piece->y = y;
                
                return 1;
            }
        }
        if (x == s_Piece->x + 1 && y == s_Piece->y - 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                turn = (turn == 1) ? 2 : 1;
                s_Piece->x = x;
                s_Piece->y = y;
                
                return 1;
            }
        }
        if (x == s_Piece->x - 2 && y == s_Piece->y - 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x + 1, y + 1, RedPieces, WhitePieces) != nullptr &&
                FindPiece(x + 1, y + 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                turn = (turn == 1) ? 2 : 1;
                KillPiece(x + 1, y + 1);
                s_Piece->x = x;
                s_Piece->y = y;
                turn = (turn == 1) ? 2 : 1;
                return 1;
            }
        }
        if (x == s_Piece->x + 2 && y == s_Piece->y - 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x - 1, y + 1, RedPieces, WhitePieces) != nullptr &&
                FindPiece(x - 1, y + 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                turn = (turn == 1) ? 2 : 1;
                KillPiece(x - 1, y + 1);
                s_Piece->x = x;
                s_Piece->y = y;
                turn = (turn == 1) ? 2 : 1;
                return 1;
            }
        }
    }

    // Conditions for non-king White pieces
    if (s_Piece->color == sf::Color::White && !s_Piece->isKing) {
        if (x == s_Piece->x - 1 && y == s_Piece->y + 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                turn = (turn == 1) ? 2 : 1;
                s_Piece->x = x;
                s_Piece->y = y;
                
                return 1;
            }
        }
        if (x == s_Piece->x + 1 && y == s_Piece->y + 1) {
            if (!FindPiece(x, y, RedPieces, WhitePieces)) {
                turn = (turn == 1) ? 2 : 1;
                s_Piece->x = x;
                s_Piece->y = y;
                
                return 1;
            }
        }
        if (x == s_Piece->x - 2 && y == s_Piece->y + 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x + 1, y - 1, RedPieces, WhitePieces) != nullptr &&
                FindPiece(x + 1, y - 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                turn = (turn == 1) ? 2 : 1;
                s_Piece->x = x;
                s_Piece->y = y;
                KillPiece(x + 1, y - 1);
                turn = (turn == 1) ? 2 : 1;
                return 1;
                
            }
        }
        if (x == s_Piece->x + 2 && y == s_Piece->y + 2) {
            if (!FindPiece(x, y, RedPieces, WhitePieces) && FindPiece(x - 1, y - 1, RedPieces, WhitePieces) != nullptr &&
                FindPiece(x - 1, y - 1, RedPieces, WhitePieces)->color != s_Piece->color) {
                turn = (turn == 1) ? 2 : 1;
                s_Piece->x = x;
                s_Piece->y = y;
                KillPiece(x - 1, y - 1);
                turn = (turn == 1) ? 2 : 1;
                return 1;
            }
        }
    }

    return 0;
}



void Engine::HandleInput(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        if (!invalidMove && event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                selected = !selected;
                isMessageBoxDisplayed = false;

                if (selected) {
                    int x = sf::Mouse::getPosition(window).x / 75;
                    int y = sf::Mouse::getPosition(window).y / 75;
                    if (FindPiece(x, y, RedPieces, WhitePieces) &&
                        ((FindPiece(x, y, RedPieces, WhitePieces)->color == sf::Color::Red && turn == 1) ||
                         (FindPiece(x, y, RedPieces, WhitePieces)->color == sf::Color::White && turn == 2))) {
                        if (FindPiece(x, y, RedPieces, WhitePieces) == SelectedPiece) {
                            SelectedPiece = nullptr;
                            validMoves.clear(); // Clear valid move positions when deselecting
                        } else {
                            SelectedPiece = FindPiece(x, y, RedPieces, WhitePieces);

                            // Calculate and store valid move positions for the selected piece
                            validMoves.clear();
                            CalculateValidMoves(SelectedPiece, validMoves);
                        }
                        selected = false;
                    } else if (SelectedPiece != nullptr && MovePiece(x, y, SelectedPiece)) {
                        selected = false;
                        SelectedPiece = nullptr;
                        validMoves.clear(); // Clear valid move positions after moving
                    }
                }
            }
        }
        if (invalidMove) {
            invalidMove = false; // Reset the flag after handling the message box
        }

        if (selected) {
            int x = sf::Mouse::getPosition(window).x / 75;
            int y = sf::Mouse::getPosition(window).y / 75;
            if (FindPiece(x, y, RedPieces, WhitePieces) &&
                ((FindPiece(x, y, RedPieces, WhitePieces)->color == sf::Color::Red && turn == 1) ||
                 (FindPiece(x, y, RedPieces, WhitePieces)->color == sf::Color::White && turn == 2))) {
                if (FindPiece(x, y, RedPieces, WhitePieces) == SelectedPiece) {
                    SelectedPiece = nullptr;
                } else {
                    SelectedPiece = FindPiece(x, y, RedPieces, WhitePieces);
                }
                selected = false;
            } else if (SelectedPiece != nullptr && MovePiece(x, y, SelectedPiece)) {
                selected = false;
                SelectedPiece = nullptr;
            }
        }
    }
}


void Engine::Update() {
        // ... (unchanged)
    }

void Engine::Render(sf::RenderWindow& window) {
    window.clear();
    board.Draw(window);

     // Declare the font object outside the loop


    for (int i = 0; i < 12; i++) {
        Piece& piece = RedPieces[i]; // Use a reference for clarity

        if (piece.isAlive) {
            piece.Draw(window);
            if (piece.isKing) {
                // Display kings differently (e.g., add a 'K' text)
                sf::CircleShape kingCircle(piece.radius - 6); // Adjust the size of the circle
                kingCircle.setFillColor(sf::Color::Yellow);
                kingCircle.setPosition(piece.x * 75 + 5, piece.y * 75 + 5); // Adjust position for the circle
                window.draw(kingCircle);

                sf::Text kingText("K", font, 30);
                // Center the text within the king circle
                sf::FloatRect textBounds = kingText.getLocalBounds();
                kingText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
                kingText.setPosition(piece.x * 75 + piece.radius, piece.y * 75 + piece.radius);
                kingText.setFillColor(sf::Color::Black); // Set the text color
                window.draw(kingText);
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        Piece& piece = WhitePieces[i]; // Use a reference for clarity

        if (piece.isAlive) {
            piece.Draw(window);
            if (piece.isKing) {
                // Display kings differently (e.g., add a 'K' text)
                sf::CircleShape kingCircle(piece.radius - 6); // Adjust the size of the circle
                kingCircle.setFillColor(sf::Color::Yellow);
                kingCircle.setPosition(piece.x * 75 + 5, piece.y * 75 + 5); // Adjust position for the circle
                window.draw(kingCircle);

                sf::Text kingText("K", font, 30);
                // Center the text within the king circle
                sf::FloatRect textBounds = kingText.getLocalBounds();
                kingText.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
                kingText.setPosition(piece.x * 75 + piece.radius, piece.y * 75 + piece.radius);
                kingText.setFillColor(sf::Color::Black); // Set the text color
                window.draw(kingText);
            }
        }
    }

    window.display(); // Display the rendered content after clearing and drawing
}
void Engine::CalculateValidMoves(Piece* s_Piece, std::vector<std::pair<int, int>>& validMoves) {
    validMoves.clear(); // Clear any previous valid moves

    int x = s_Piece->x;
    int y = s_Piece->y;

    int dx[4] = { -1, 1, -1, 1 };
    int dy[4] = { -1, -1, 1, 1 }; // Consider all four diagonal directions

    if (s_Piece->isKing) {
        // Code for king pieces
        bool addedInDirection[4] = { false }; // To track whether we added an empty box in each direction

        for (int i = 0; i < 4; i++) { // Loop through all four diagonal directions
            int newX = x + dx[i];
            int newY = y + dy[i]; // Consider all four diagonal directions

            if (!addedInDirection[i] && newX >= 0 && newX < size && newY >= 0 && newY < size) {
                Piece* pieceAtPos = FindPiece(newX, newY, RedPieces, WhitePieces);

                if (pieceAtPos == nullptr) {
                    // Empty position; add it to validMoves
                    validMoves.push_back(std::make_pair(newX, newY));
                    addedInDirection[i] = true; // Mark that we added in this direction
                }
                else if (pieceAtPos->color != s_Piece->color) {
                    // There's an opponent's piece; check for an empty block in its diagonal
                    int blockX = newX + dx[i];
                    int blockY = newY + dy[i];

                    if (blockX >= 0 && blockX < size && blockY >= 0 && blockY < size) {
                        Piece* blockAtPos = FindPiece(blockX, blockY, RedPieces, WhitePieces);

                        if (blockAtPos == nullptr) {
                            // Empty block in the diagonal of the opponent's piece; add it to validMoves
                            validMoves.push_back(std::make_pair(blockX, blockY));
                        }
                    }
                }
            }
        }
    }
    else {
        // Code for regular pieces
        int dx[2] = { -1, 1 };
        int dy = (s_Piece->color == sf::Color::Red) ? -1 : 1; // Adjust the direction based on piece color

        for (int i = 0; i < 2; i++) {
            int newX = x + dx[i];
            int newY = y + dy;

            if (newX >= 0 && newX < size && newY >= 0 && newY < size) {
                Piece* pieceAtPos = FindPiece(newX, newY, RedPieces, WhitePieces);

                if (pieceAtPos == nullptr) {
                    // Immediate diagonal move (no opponent's piece in the way)
                    validMoves.push_back(std::make_pair(newX, newY));
                }
                else if (pieceAtPos->color != s_Piece->color) {
                    // There's an opponent's piece; check for a valid jump
                    int jumpX = newX + dx[i];
                    int jumpY = newY + dy;

                    if (jumpX >= 0 && jumpX < size && jumpY >= 0 && jumpY < size) {
                        Piece* capturedPiece = FindPiece(jumpX, jumpY, RedPieces, WhitePieces);

                        if (capturedPiece == nullptr) {
                            // Valid jump (capture) move
                            validMoves.push_back(std::make_pair(jumpX, jumpY));
                        }
                    }
                }
            }
        }
    }
}

bool Engine::AreAllPiecesKilled(Piece* pieces) {
    for (int i = 0; i < 12; i++) {
        if (pieces[i].isAlive) {
            return false;
        }
    }
    return true;
}

void Game::StartGame() {

    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;
    sf::RenderWindow window(sf::VideoMode(600, 600), "Checkers", sf::Style::Default, settings);
    if (!font.loadFromFile("C:/Windows/Fonts/Arial.ttf")) {
        // Handle the error if the font cannot be loaded
        std::cerr << "Error: Font file not found or couldn't be loaded." << std::endl;
    }

    while (window.isOpen()) {
        HandleInput(window);
        Update();

        window.clear(); // Clear the window only once per frame

        // Draw the game board and pieces
        Render(window);

        for (const auto& movePos : validMoves) {
            int x = movePos.first;
            int y = movePos.second;

            // Draw valid move highlights
            sf::RectangleShape highlight;
            highlight.setSize(sf::Vector2f(75.f, 75.f));
            highlight.setFillColor(sf::Color::Green);
            highlight.setPosition(sf::Vector2f(75 * x, 75 * y));
            window.draw(highlight);
            window.display();
        } 
    }
};