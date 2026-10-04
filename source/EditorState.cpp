#include "EditorState.h"

// Globals

HWND g_mainWindow = nullptr;
HWND g_entityList = nullptr;
HWND g_spriteGrid = nullptr;

std::vector<std::string> g_spriteRows;

std::vector<SpriteState> g_undoStack;
std::vector<SpriteState> g_redoStack;

int g_selectedEntity = -1;

int g_spriteScrollX = 0;
int g_spriteScrollY = 0;

std::string g_currentSpriteFile;