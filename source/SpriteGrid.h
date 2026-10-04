#pragma once
#include <windows.h>

extern char g_selectedTile;

void CreateSpriteGrid(HWND hwnd);
void SaveUndoState();
void UndoSpriteChange();
void RedoSpriteChange();
void DrawSpriteGrid(HWND hwnd, HDC hdc);

LRESULT CALLBACK SpriteGridProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

constexpr COLORREF MAP_BLUE = RGB(58, 150, 221);