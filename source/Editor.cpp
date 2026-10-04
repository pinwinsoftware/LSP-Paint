#include <windows.h>
#include <string>

#include "Editor.h"
#include "EditorState.h"
#include "Filedialog.h"
#include "SpriteGrid.h"
#include "SpriteFile.h"
#include "resource.h"

HWND  g_spriteSizeLeft = nullptr;
HWND  g_spriteSizeEdit = nullptr;
HWND  g_spriteSizeRight = nullptr;

HWND g_tileSpaceButton = nullptr;
HWND g_tile0Button = nullptr;
HWND g_tile1Button = nullptr;

char g_selectedTile = '0';

WNDPROC g_oldLeftButtonProc = nullptr;
WNDPROC g_oldRightButtonProc = nullptr;

HACCEL g_accelTable = nullptr;

int  g_spriteSize = 16;
int g_resizeDirection = 0;

bool  g_spriteSizeButtonHeld = false;
bool  g_spriteSizeRepeating = false;

LRESULT CALLBACK spriteSizeButtonProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

void CreateEmptySprite() {
    g_spriteRows.clear();

    for (int y = 0; y < g_spriteSize; ++y) {
        g_spriteRows.emplace_back(g_spriteSize, ' ');
    }
}

void UpdateSpriteSizeDisplay() {
    if (! g_spriteSizeEdit)
        return;

    char text[32];

    wsprintfA(
        text,
        "%dx%d",
         g_spriteSize,
         g_spriteSize
    );

    SetWindowTextA(
         g_spriteSizeEdit,
        text
    );
}

void CreateTileControls(HWND hwnd) {
    g_tileSpaceButton =
        CreateWindowExA(
            0,
            "BUTTON",
            "Transparent",
            WS_CHILD |
            WS_VISIBLE |
            BS_PUSHBUTTON,

            0, 0,
            180, 24,

            hwnd,
            reinterpret_cast<HMENU>(
                ID_TILE_SPACE
                ),
            GetModuleHandleA(nullptr),
            nullptr
        );

    g_tile0Button =
        CreateWindowExA(
            0,
            "BUTTON",
            "Void",
            WS_CHILD |
            WS_VISIBLE |
            BS_PUSHBUTTON,

            0, 0,
            180, 24,

            hwnd,
            reinterpret_cast<HMENU>(
                ID_TILE_0
                ),
            GetModuleHandleA(nullptr),
            nullptr
        );

    g_tile1Button =
        CreateWindowExA(
            0,
            "BUTTON",
            "Solid",
            WS_CHILD |
            WS_VISIBLE |
            BS_PUSHBUTTON,

            0, 0,
            180, 24,

            hwnd,
            reinterpret_cast<HMENU>(
                ID_TILE_1
                ),
            GetModuleHandleA(nullptr),
            nullptr
        );
}

void CreateSpriteSizeControls(HWND hwnd) {
     g_spriteSizeLeft =
        CreateWindowExA(
            0,
            "BUTTON",
            "<",
            WS_CHILD |
            WS_VISIBLE |
            BS_PUSHBUTTON,

            0, 0,
            30, 24,

            hwnd,
            reinterpret_cast<HMENU>(
                ID_SPRITE_SIZE_LEFT
                ),
            GetModuleHandleA(nullptr),
            nullptr
        );

     g_spriteSizeEdit =
        CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "EDIT",
            "16x16",
            WS_CHILD |
            WS_VISIBLE |
            ES_READONLY |
            ES_CENTER,

            0, 0,
            120, 24,

            hwnd,
            reinterpret_cast<HMENU>(
                ID_SPRITE_SIZE_EDIT
                ),
            GetModuleHandleA(nullptr),
            nullptr
        );

     g_spriteSizeRight =
        CreateWindowExA(
            0,
            "BUTTON",
            ">",
            WS_CHILD |
            WS_VISIBLE |
            BS_PUSHBUTTON,

            0, 0,
            30, 24,

            hwnd,
            reinterpret_cast<HMENU>(
                ID_SPRITE_SIZE_RIGHT
                ),
            GetModuleHandleA(nullptr),
            nullptr
        );

    // Subclass the two resize buttons
    g_oldLeftButtonProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA( g_spriteSizeLeft, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(spriteSizeButtonProc)));
    g_oldRightButtonProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrA( g_spriteSizeRight, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(spriteSizeButtonProc)));
}

void ResizeSpriteData(int newSize) {
    if (newSize < MIN_SPRITE_SIZE || newSize > MAX_SPRITE_SIZE)
        return;

    g_spriteRows.resize(newSize);

    for (std::string& row : g_spriteRows) {
        row.resize(newSize, ' ');
    }
     g_spriteSize = newSize;
}

void ResizeEditor(HWND hwnd) {
    RECT rect = {};

    GetClientRect(hwnd, &rect);

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    const int controlX = width - ENTITY_LIST_WIDTH - EDITOR_MARGIN;

    const int tileButtonWidth = 180;
    const int tileSpaceButtonWidth = 180;
    const int tileButtonHeight = 24;
    const int tileButtonGap = 6;

    const int tileSpaceX = controlX + (ENTITY_LIST_WIDTH - tileSpaceButtonWidth) / 2;
    const int tileSmallX = controlX + (ENTITY_LIST_WIDTH - tileButtonWidth) / 2;

    const int tileY = EDITOR_MARGIN;

    MoveWindow(
        g_tileSpaceButton,
        tileSpaceX,
        tileY,
        tileSpaceButtonWidth,
        tileButtonHeight,
        TRUE
    );

    MoveWindow(
        g_tile0Button,
        tileSmallX,
        tileY +
        tileButtonHeight +
        tileButtonGap,
        tileButtonWidth,
        tileButtonHeight,
        TRUE
    );

    MoveWindow(
        g_tile1Button,
        tileSmallX,
        tileY +
        (tileButtonHeight + tileButtonGap) * 2,
        tileButtonWidth,
        tileButtonHeight,
        TRUE
    );

    // Sprite size controls

    const int spriteSizeButtonWidth = 30;
    const int spriteSizeEditWidth = 120;
    const int spriteSizeControlHeight = 24;
    const int spriteSizeControlGap = 10;

    const int spriteSizeTotalWidth = spriteSizeButtonWidth + spriteSizeEditWidth + spriteSizeButtonWidth;
    const int spriteSizeX = controlX + (ENTITY_LIST_WIDTH - spriteSizeTotalWidth) / 2;
    const int spriteSizeY = tileY + (tileButtonHeight * 3) + (tileButtonGap * 2) + spriteSizeControlGap;

    MoveWindow(
         g_spriteSizeLeft,
        spriteSizeX,
        spriteSizeY,
        spriteSizeButtonWidth,
        spriteSizeControlHeight,
        TRUE
    );

    MoveWindow(
         g_spriteSizeEdit,
        spriteSizeX +
        spriteSizeButtonWidth,
        spriteSizeY,
        spriteSizeEditWidth,
        spriteSizeControlHeight,
        TRUE
    );

    MoveWindow(
         g_spriteSizeRight,
        spriteSizeX +
        spriteSizeButtonWidth +
        spriteSizeEditWidth,
        spriteSizeY,
        spriteSizeButtonWidth,
        spriteSizeControlHeight,
        TRUE
    );

    int availableWidth = width - ENTITY_LIST_WIDTH - EDITOR_MARGIN * 3;
    int availableHeight = height - EDITOR_MARGIN * 2;

    if (availableWidth < 0)
        availableWidth = 0;

    if (availableHeight < 0)
        availableHeight = 0;

    int spriteViewportSize = min(availableWidth, availableHeight);

    if (spriteViewportSize < 0)
        spriteViewportSize = 0;

    if (g_spriteGrid) {
        MoveWindow(
            g_spriteGrid,
            EDITOR_MARGIN,
            EDITOR_MARGIN,
            spriteViewportSize,
            spriteViewportSize,
            TRUE
        );

        InvalidateRect(
            g_spriteGrid,
            nullptr,
            TRUE
        );
    }
}

void ResizeMapByStep(HWND hwnd) {
    if (g_resizeDirection < 0) {
        if ( g_spriteSize > MIN_SPRITE_SIZE) {
            ResizeSpriteData( g_spriteSize - 1);
        }
    }
    else if (g_resizeDirection > 0) {
        if ( g_spriteSize < MAX_SPRITE_SIZE) {
            ResizeSpriteData( g_spriteSize + 1);
        }
    }

    UpdateSpriteSizeDisplay();

    ResizeEditor(hwnd);

    InvalidateRect(
        g_spriteGrid,
        nullptr,
        TRUE
    );
}

LRESULT CALLBACK spriteSizeButtonProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {
    int id = GetDlgCtrlID(hwnd);

    WNDPROC oldProc = nullptr;

    if (id == ID_SPRITE_SIZE_LEFT) {
        oldProc = g_oldLeftButtonProc;
    }
    else if (id == ID_SPRITE_SIZE_RIGHT) {
        oldProc = g_oldRightButtonProc;
    }

    if (message == WM_LBUTTONDOWN) {
        HWND parent = GetParent(hwnd);

        SaveUndoState();

        if (id == ID_SPRITE_SIZE_LEFT) {
            g_resizeDirection = -1;
        }
        else if (id == ID_SPRITE_SIZE_RIGHT) {
            g_resizeDirection = 1;
        }

         g_spriteSizeButtonHeld = true;

        ResizeMapByStep(parent);

        // Delay auto-repeat after the initial click.
        SetTimer(
            parent,
            ID_SPRITE_RESIZE_TIMER,
            400,
            nullptr
        );

        SetCapture(hwnd);

        if (oldProc) {
            return CallWindowProcA(
                oldProc,
                hwnd,
                message,
                wParam,
                lParam
            );
        }
        return 0;
    }

    if (message == WM_LBUTTONUP) {
        HWND parent = GetParent(hwnd);

         g_spriteSizeButtonHeld = false;
         g_spriteSizeRepeating = false;
        g_resizeDirection = 0;

        KillTimer(parent, ID_SPRITE_RESIZE_TIMER);

        ReleaseCapture();

        if (oldProc) {
            return CallWindowProcA(
                oldProc,
                hwnd,
                message,
                wParam,
                lParam
            );
        }

        return 0;
    }

    if (oldProc) {
        return CallWindowProcA(
            oldProc,
            hwnd,
            message,
            wParam,
            lParam
        );
    }

    return DefWindowProcA(
        hwnd,
        message,
        wParam,
        lParam
    );
}


LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        ACCEL accelerators[] =
        {
            { FVIRTKEY | FCONTROL, 'N', ID_NEW_SPRITE  },
            { FVIRTKEY | FCONTROL, 'O', ID_OPEN_SPRITE },
            { FVIRTKEY | FCONTROL, 'S', ID_SAVE_SPRITE }
        };

        g_accelTable = CreateAcceleratorTableA(accelerators, ARRAYSIZE(accelerators));

        HMENU menuBar = CreateMenu();
        HMENU fileMenu = CreatePopupMenu();
        HMENU helpMenu = CreatePopupMenu();

        HBITMAP hNew = LoadBitmapA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDB_FILE));
        HBITMAP hOpen = LoadBitmapA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDB_FILE));
        HBITMAP hSave = LoadBitmapA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDB_SAVE));
        HBITMAP hSaveAs = LoadBitmapA(GetModuleHandleA(nullptr), MAKEINTRESOURCEA(IDB_SAVE));

        MENUITEMINFOA mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_ID | MIIM_STRING | MIIM_BITMAP;
        mii.fType = MFT_STRING;
        mii.wID = ID_NEW_SPRITE;
        mii.dwTypeData = const_cast<LPSTR>("New\tCtrl+N");
        mii.hbmpItem = hNew;

        InsertMenuItemA(
            fileMenu,
            GetMenuItemCount(fileMenu),
            TRUE,
            &mii
        );

        mii.wID = ID_OPEN_SPRITE;
        mii.dwTypeData = const_cast<LPSTR>("Open...\tCtrl+O");
        mii.hbmpItem = hOpen;

        InsertMenuItemA(
            fileMenu,
            GetMenuItemCount(fileMenu),
            TRUE,
            &mii
        );

        // Save
        mii.wID = ID_SAVE_SPRITE;
        mii.dwTypeData = const_cast<LPSTR>("Save\tCtrl+S");
        mii.hbmpItem = hSave;

        InsertMenuItemA(
            fileMenu,
            GetMenuItemCount(fileMenu),
            TRUE,
            &mii
        );

        // Save As...
        mii.wID = ID_SAVE_SPRITE_AS;
        mii.dwTypeData = const_cast<LPSTR>("Save As...");
        mii.hbmpItem = hSaveAs;

        InsertMenuItemA(
            fileMenu,
            GetMenuItemCount(fileMenu),
            TRUE,
            &mii
        );

        // Separator
        AppendMenuA(
            fileMenu,
            MF_SEPARATOR,
            0,
            nullptr
        );

        // Exit
        AppendMenuA(
            fileMenu,
            MF_STRING,
            ID_EXIT,
            "Exit"
        );

        HMENU editMenu =
            CreatePopupMenu();

        AppendMenuA(
            menuBar,
            MF_POPUP,
            reinterpret_cast<UINT_PTR>(fileMenu),
            "File"
        );

        AppendMenuA(
            editMenu,
            MF_STRING,
            ID_UNDO,
            "Undo\tCtrl+Z"
        );

        AppendMenuA(
            editMenu,
            MF_STRING,
            ID_REDO,
            "Redo\tCtrl+Y"
        );

        AppendMenuA(
            editMenu,
            MF_SEPARATOR,
            0,
            nullptr
        );

        AppendMenuA(
            editMenu,
            MF_STRING,
            ID_CLEAR_SPRITE,
            "Clear Sprite"
        );

        AppendMenuA(
            menuBar,
            MF_POPUP,
            reinterpret_cast<UINT_PTR>(editMenu),
            "Edit"
        );

        AppendMenuA(
            helpMenu, 
            MF_STRING, 
            ID_HELP_CONTENTS, 
            "Creating Sprites"
        );

        AppendMenuA(
            helpMenu, 
            MF_SEPARATOR,
            0,
            nullptr
        );

        AppendMenuA(
            helpMenu,
            MF_STRING,
            ID_HELP_ABOUT,
            "About"
        );

        AppendMenuA(
            menuBar,
            MF_POPUP,
            (UINT_PTR)
            helpMenu,
            "Help"
        );

        SetMenu(hwnd, menuBar);
        CreateEmptySprite();
        CreateSpriteGrid(hwnd);
        CreateSpriteSizeControls(hwnd);
        CreateTileControls(hwnd);

        break;
    }
    case WM_SIZE: {
        ResizeEditor(hwnd);
        break;
    }
    case WM_TIMER: {
        if (wParam == ID_SPRITE_RESIZE_TIMER) {
            if (! g_spriteSizeButtonHeld) {
                KillTimer(hwnd, ID_SPRITE_RESIZE_TIMER);

                return 0;
            }

            if (! g_spriteSizeRepeating) {
                // Initial 400 ms delay has expired
                 g_spriteSizeRepeating = true;

                SetTimer(
                    hwnd,
                    ID_SPRITE_RESIZE_TIMER,
                    50,
                    nullptr
                );

                return 0;
            }

            ResizeMapByStep(hwnd);

            return 0;
        }

        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_TILE_SPACE: {
            g_selectedTile = ' ';
            break;
        }
        case ID_TILE_0: {
            g_selectedTile = '0';
            break;
        }
        case ID_TILE_1: {
            g_selectedTile = '1';
            break;
        }
        case ID_NEW_SPRITE: {
            SaveUndoState();

             NewSprite();

            g_currentSpriteFile.clear();

            InvalidateRect(
                g_spriteGrid,
                nullptr,
                TRUE
            );

            break;
        }
        case ID_OPEN_SPRITE: {
            std::string filename = OpenSpriteFile();

            if (filename.empty())
                break;

            SaveUndoState();

            if (!OpenSprite(filename)) {
                // Remove the undo state because the open operation failed
                if (!g_undoStack.empty()) {
                    g_undoStack.pop_back();
                }

                MessageBoxA(
                    hwnd,
                    "Failed to open sprite file",
                    "Open Sprite Error",
                    MB_OK | MB_ICONERROR
                );

                break;
            }


            g_currentSpriteFile = filename;

            InvalidateRect(
                g_spriteGrid,
                nullptr,
                TRUE
            );

            std::string fileNameOnly = filename.substr(filename.find_last_of("\\/") + 1);

            std::string title = "LSP Paint - " + fileNameOnly;

            SetWindowTextA(hwnd, title.c_str());

            break;
        }
        case ID_SAVE_SPRITE: {
            if (g_currentSpriteFile.empty()) {
                std::string filename = SaveSpriteFile();

                if (filename.empty())
                    break;

                if (!SaveSprite(filename)) {
                    MessageBoxA(
                        hwnd,
                        "Failed to save sprite file",
                        "Error",
                        MB_OK | MB_ICONERROR
                    );

                    break;
                }

                g_currentSpriteFile = filename;
            }
            else {
                if (!SaveSprite(g_currentSpriteFile)) {
                    MessageBoxA(
                        hwnd,
                        "Failed to save sprite file",
                        "Error",
                        MB_OK | MB_ICONERROR
                    );

                    break;
                }
            }

            break;
        }
        case ID_SAVE_SPRITE_AS: {
            std::string filename = SaveSpriteFile();

            if (filename.empty())
                break;

            if (!SaveSprite(filename)) {
                MessageBoxA(
                    hwnd,
                    "Failed to save sprite file",
                    "Error",
                    MB_OK | MB_ICONERROR
                );

                break;
            }

            g_currentSpriteFile = filename;

            SetWindowTextA(hwnd, filename.c_str());

            break;
        }
        case ID_UNDO: {
            UndoSpriteChange();

            InvalidateRect(
                g_spriteGrid,
                nullptr,
                TRUE
            );

            break;
        }
        case ID_REDO: {
            RedoSpriteChange();

            InvalidateRect(
                g_spriteGrid,
                nullptr,
                TRUE
            );

            break;
        }
        case ID_CLEAR_SPRITE: {
            bool hasContent = false;

            for (const std::string& row : g_spriteRows) {
                for (char tile : row) {
                    if (tile != ' ') {
                        hasContent = true;
                        break;
                    }
                }

                if (hasContent)
                    break;
            }

            if (!hasContent)
                break;

            // Save the map before clearing it
            SaveUndoState();

            // Clear map
            CreateEmptySprite();

            // Redraw
            InvalidateRect(
                g_spriteGrid,
                nullptr,
                TRUE
            );

            break;
        }
        case ID_HELP_CONTENTS: {
            STARTUPINFOA si = {};
            si.cb = sizeof(si);

            PROCESS_INFORMATION pi = {};

            CreateProcessA(
                nullptr,
                (LPSTR)"hh.exe \"LSP Paint.chm::/CreatingSprites.htm\"",
                nullptr,
                nullptr,
                FALSE,
                0,
                nullptr,
                nullptr,
                &si,
                &pi
            );

            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);

            break;
        }
        case ID_HELP_ABOUT: {
            MessageBoxA(
                hwnd,
                "LSP Paint\nVersion 1.0.0.0\nDeveloped by Pinwin",
                "About",
                MB_OK | MB_ICONINFORMATION);

            break;
        }
        case ID_EXIT: {
            DestroyWindow(hwnd);
            break;
        }
        }

        break;
    }
    case WM_DESTROY: {
        if (g_accelTable) {
            DestroyAcceleratorTable(g_accelTable);
            g_accelTable = nullptr;
        }

        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProcA(
            hwnd,
            message,
            wParam,
            lParam
        );
    }

    return 0;
}