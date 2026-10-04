#pragma once
#include <windows.h>

#define ID_NEW_SPRITE 1001
#define ID_OPEN_SPRITE 1002
#define ID_SAVE_SPRITE 1003
#define ID_SAVE_SPRITE_AS 1004
#define ID_OPEN_LED 1005
#define ID_EXIT 1006

#define ID_UNDO 1010
#define ID_REDO 1011
#define ID_CLEAR_SPRITE 1012

#define ID_HELP_CONTENTS 1013
#define ID_HELP_ABOUT 1014

#define ID_ENTITY_LIST 1020
#define ID_MAP_GRID 1030

constexpr int EDITOR_MARGIN = 10;
constexpr int ENTITY_LIST_WIDTH = 180;

#define ID_SPRITE_SIZE_LEFT      2001
#define ID_SPRITE_SIZE_EDIT      2002
#define ID_SPRITE_SIZE_RIGHT     2003

#define ID_TILE_SPACE         2010
#define ID_TILE_0             2011
#define ID_TILE_1             2012

#define ID_SPRITE_RESIZE_TIMER   3001

extern int  g_spriteSize;

extern char g_selectedTile;

extern HACCEL g_accelTable;

void CreateEmptySprite();
void UpdateSpriteSizeDisplay();
void CreateSpriteSizeControls(HWND hwnd);
void ResizeEditor(HWND hwnd);
void ResizeMapByStep(HWND hwnd);

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
);