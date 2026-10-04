#pragma once
#include <windows.h>
#include <string>
#include <vector>

constexpr int MIN_SPRITE_SIZE = 1;
constexpr int MAX_SPRITE_SIZE = 128;

struct SpriteState {
    int size;
    std::vector<std::string> rows;
};

extern HWND g_mainWindow;
extern HWND g_entityList;
extern HWND g_spriteGrid;

extern std::vector<std::string> g_spriteRows;

extern std::vector<SpriteState> g_undoStack;
extern std::vector<SpriteState> g_redoStack;

extern int g_selectedEntity;

extern int g_spriteScrollX;
extern int g_spriteScrollY;

extern std::string g_currentSpriteFile;