// source.h

#pragma once

#include <SFML/Graphics.hpp>
#define NOMINMAX
#include <windows.h>

class Board {
public:
    int size = 8;

    void Draw(sf::RenderWindow& window);
    void Highlight(sf::RenderWindow& window, int x, int y);
};

class Piece {
public:
    float radius = 30;
    int x = 0; // Initialize x to a default value
    int y;
    bool isAlive;
    
    sf::Color color;
    bool isKing = false;
    // Constructor
 

    void Draw(sf::RenderWindow& window);
};


class Engine {
public:
    Engine();
    ~Engine();

    void Setup(Piece* RedPieces, Piece* WhitePieces);
    Piece* FindPiece(int x, int y, Piece* RedPieces, Piece* WhitePieces);
    void KillPiece(int x, int y);
    int MovePiece(int x, int y, Piece* s_Piece);
    void HandleInput(sf::RenderWindow& window);
    void Update();
    void Render(sf::RenderWindow& window);
    bool AreAllPiecesKilled(Piece* pieces);
    void CalculateValidMoves(Piece* s_Piece, std::vector<std::pair<int, int>>& validMoves);
    std::vector<std::pair<int, int>> validMoves;

private:
    int turn;
    Piece* SelectedPiece;
    bool selected;
    bool invalidMove;
    bool isMessageBoxDisplayed;
    
    

    Board board;
    Piece* RedPieces = new Piece[12];
    Piece* WhitePieces = new Piece[12];
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Game : public Engine {
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void StartGame();
private:
    Piece* RedPieces = new Piece[12];
    Piece* WhitePieces = new Piece[12];
};
