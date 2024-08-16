#define NOMINMAX
#include <windows.h>
#include "source.h"
#include <iostream>
#include <SFML/Graphics.hpp>

HINSTANCE hInstance;
HWND hwndStartButton, hwndQuitButton, hwndWelcomeHeading, hwndHowToPlayButton;
HFONT hFont;
int nCmdShow;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        if (reinterpret_cast<HWND>(lParam) == hwndStartButton) {
            Game game;
            game.StartGame();
            PostQuitMessage(0);
            
        }
        else if (reinterpret_cast<HWND>(lParam) == hwndQuitButton) {
            PostQuitMessage(0);
        }
        else if (reinterpret_cast<HWND>(lParam) == hwndHowToPlayButton) {
            const wchar_t* message = L"HOW TO PLAY:\n\n"
                L"* First turn is Red\n"
                L"* To move, First click the piece you want to move then chose the box of right or left forward diagonal\n"
                L"* pieces move diagonally forward\n"
                L"* Capture opponent's pieces by jumping over them diagonally.\n"
                L"* Mandatory capturing if possible; multiple captures in one turn are allowed.\n"
                L"* Win by capturing all opponent's pieces or trapping them without moves.\n"
                L"* Develop strategies to control the center, promote pieces to kings, and plan multiple captures.\n\n"
                L"\"Playing checkers is like strategizing in style - where every move is a step towards triumph or a hilarious tumble!\"\n\n"
                L"HAVE A GOOD LUCK!";
            MessageBoxW(hwnd, message, L"How to Play", MB_OK);
        }
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEXW), CS_HREDRAW | CS_VREDRAW, WindowProc,
                         0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                         L"MenuApp", NULL };
    RegisterClassExW(&wcex);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowW(L"MenuApp", L"Menu Example", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        800, 800, NULL, NULL, GetModuleHandle(NULL), NULL);

    hwndStartButton = CreateWindowW(L"BUTTON", L"Start New Game", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        250, 300, 300, 70, hwnd, NULL, hInstance, NULL);

    hwndQuitButton = CreateWindowW(L"BUTTON", L"Quit", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        250, 400, 300, 70, hwnd, NULL, hInstance, NULL);

    hwndWelcomeHeading = CreateWindowW(L"STATIC", L"Welcome to the Checkers Game!",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        0, 100, 800, 80, hwnd, NULL, hInstance, NULL);

    hwndHowToPlayButton = CreateWindowW(L"BUTTON", L"How to Play", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        250, 500, 300, 70, hwnd, NULL, hInstance, NULL);

    // Create a bold and italic font
    hFont = CreateFontW(
        24, 0, 0, 0, FW_BOLD, TRUE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_SWISS, L"Arial");

    // Set the font for the static control
    SendMessage(hwndWelcomeHeading, WM_SETFONT, (WPARAM)hFont, TRUE);

    // Set background color and text color
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0)); // Black background
    SetClassLongPtr(hwndWelcomeHeading, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
    SendMessage(hwndWelcomeHeading, WM_CTLCOLORSTATIC, (WPARAM)GetDC(hwndWelcomeHeading), (LPARAM)hwndWelcomeHeading);

    // ...

    sf::RenderWindow sfmlWindow(hwnd);
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up resources
    DeleteObject(hFont);
    DeleteObject(hBrush);

    return (int)msg.wParam;
}
