#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "SpriteGrid.h"
#include "EditorState.h"
#include "Editor.h"

bool g_isPainting = false;
bool g_isErasing = false;
bool g_paintUndoSaved = false;

void CreateSpriteGrid(HWND hwnd) {
    g_spriteGrid =
        CreateWindowExA(
            WS_EX_CLIENTEDGE,
            "Lite Engine SpriteGrid",
            "",
            WS_CHILD |
            WS_VISIBLE |
            WS_TABSTOP,

            0,
            0,
            0,
            0,

            hwnd,
            reinterpret_cast<HMENU>(ID_MAP_GRID),
            GetModuleHandleA(nullptr),
            nullptr
        );
}

void UndoSpriteChange() {
    if (g_undoStack.empty())
        return;

    // Save the current state for redo
    SpriteState current;

    current.size = g_spriteSize;
    current.rows = g_spriteRows;

    g_redoStack.push_back(current);

    // Restore the previous state
    SpriteState previous = g_undoStack.back();

    g_undoStack.pop_back();

    g_spriteSize = previous.size;
    g_spriteRows = previous.rows;
}

void RedoSpriteChange() {
    if (g_redoStack.empty())
        return;

    // Save the current state for undo
    SpriteState current;

    current.size = g_spriteSize;
    current.rows = g_spriteRows;

    g_undoStack.push_back(current);

    // Restore the next state
    SpriteState next = g_redoStack.back();

    g_redoStack.pop_back();

    g_spriteSize = next.size;
    g_spriteRows = next.rows;
}

void SaveUndoState() {
    SpriteState state;

    state.size = g_spriteSize;
    state.rows = g_spriteRows;

    g_undoStack.push_back(state);
    g_redoStack.clear();
}

void PaintSpriteTile(HWND hwnd, int mouseX, int mouseY) {
    RECT clientRect = {};

    GetClientRect(hwnd, &clientRect);

    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    if (clientWidth <= 0 || clientHeight <= 0)
        return;

    // Convert the mouse position to sprite coordinates
    int spriteX = static_cast<int>(static_cast<double>(mouseX) * g_spriteSize / clientWidth);
    int spriteY = static_cast<int>(static_cast<double>(mouseY) * g_spriteSize / clientHeight);

    // Safety check
    if (spriteX < 0 || spriteX >= g_spriteSize ||
        spriteY < 0 || spriteY >= g_spriteSize) {
        return;
    }

    if (spriteY >= static_cast<int>(g_spriteRows.size()))
        return;

    if (spriteX >= static_cast<int>(g_spriteRows[spriteY].size()))
        return;

    // Do nothing if the tile already contains the selected value
    if (g_spriteRows[spriteY][spriteX] == g_selectedTile)
        return;

    // Save one undo state for the entire paint operation
    if (!g_paintUndoSaved) {
        SaveUndoState();
        g_paintUndoSaved = true;
    }

    g_spriteRows[spriteY][spriteX] = g_selectedTile;

    InvalidateRect(
        hwnd,
        nullptr,
        FALSE
    );
}

void EraseSpriteTile(HWND hwnd, int mouseX, int mouseY) {
    RECT clientRect = {};

    GetClientRect(hwnd, &clientRect);

    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    if (clientWidth <= 0 || clientHeight <= 0)
        return;

    // Convert the mouse position to sprite coordinates
    int spriteX = static_cast<int>(static_cast<double>(mouseX) * g_spriteSize / clientWidth);
    int spriteY = static_cast<int>(static_cast<double>(mouseY) * g_spriteSize / clientHeight);

    if (spriteX < 0 || spriteX >= g_spriteSize ||
        spriteY < 0 || spriteY >= g_spriteSize) {
        return;
    }

    if (spriteY >= static_cast<int>(g_spriteRows.size()))
        return;

    if (spriteX >= static_cast<int>(g_spriteRows[spriteY].size()))
        return;

    // Nothing to erase
    if (g_spriteRows[spriteY][spriteX] == ' ')
        return;

    // Save one undo state for the entire erase operation
    if (!g_paintUndoSaved) {
        SaveUndoState();
        g_paintUndoSaved = true;
    }

    g_spriteRows[spriteY][spriteX] = ' ';

    InvalidateRect(
        hwnd,
        nullptr,
        FALSE
    );
}

LRESULT CALLBACK SpriteGridProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    switch (message) {
    case WM_PAINT: {
        PAINTSTRUCT ps = {};

        HDC hdc = BeginPaint(hwnd, &ps);
        DrawSpriteGrid(hwnd, hdc);
        EndPaint(hwnd, &ps);

        return 0;
    }
    case WM_ERASEBKGND: {
        return 1;
    }
    case WM_LBUTTONDOWN: {
        SetFocus(hwnd);

        g_isPainting = true;
        g_isErasing = false;
        g_paintUndoSaved = false;

        SetCapture(hwnd);

        PaintSpriteTile(
            hwnd,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
        );

        return 0;
    }
    case WM_MOUSEMOVE: {
        if (g_isPainting) {
            PaintSpriteTile(
                hwnd,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
        }
        else if (g_isErasing) {
            EraseSpriteTile(
                hwnd,
                GET_X_LPARAM(lParam),
                GET_Y_LPARAM(lParam)
            );
        }

        return 0;
    }
    case WM_LBUTTONUP: {
        g_isPainting = false;
        g_paintUndoSaved = false;

        if (GetCapture() == hwnd)
            ReleaseCapture();

        return 0;
    }
    case WM_RBUTTONDOWN: {
        SetFocus(hwnd);

        g_isPainting = false;
        g_isErasing = true;
        g_paintUndoSaved = false;

        SetCapture(hwnd);

        EraseSpriteTile(
            hwnd,
            GET_X_LPARAM(lParam),
            GET_Y_LPARAM(lParam)
        );

        return 0;
    }
    case WM_RBUTTONUP: {
        g_isErasing = false;
        g_paintUndoSaved = false;

        if (GetCapture() == hwnd)
            ReleaseCapture();

        return 0;
    }
    case WM_CAPTURECHANGED: {
        g_isPainting = false;
        g_isErasing = false;
        g_paintUndoSaved = false;

        return 0;
    }
    case WM_KEYDOWN: {
        if (GetKeyState(VK_CONTROL) & 0x8000) {
            if (wParam == 'Z') {
                UndoSpriteChange();

                InvalidateRect(
                    hwnd,
                    nullptr,
                    FALSE
                );

                return 0;
            }
            if (wParam == 'Y') {
                RedoSpriteChange();

                InvalidateRect(
                    hwnd,
                    nullptr,
                    FALSE
                );

                return 0;
            }
        }
        return 0;
    }
    }

    return DefWindowProcA(
        hwnd,
        message,
        wParam,
        lParam
    );
}

void DrawSpriteGrid(HWND hwnd, HDC hdc) {
    RECT clientRect = {};

    GetClientRect(hwnd, &clientRect);

    const int clientWidth = clientRect.right - clientRect.left;
    const int clientHeight = clientRect.bottom - clientRect.top;

    if (clientWidth <= 0 || clientHeight <= 0)
        return;

    // Create a back buffer to prevent flickering
    HDC memoryDC = CreateCompatibleDC(hdc);

    if (!memoryDC)
        return;

    HBITMAP memoryBitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);

    if (!memoryBitmap) {
        DeleteDC(memoryDC);
        return;
    }

    HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(memoryDC, memoryBitmap));

    // Fill the background
    HBRUSH background = CreateSolidBrush(MAP_BLUE);

    FillRect(
        memoryDC,
        &clientRect,
        background
    );

    DeleteObject(background);

    // Sprite grid lines
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(180, 180, 180));

    HPEN oldPen = static_cast<HPEN>(SelectObject(memoryDC, gridPen));

    for (int y = 0; y < g_spriteSize; ++y) {
        if (y >= static_cast<int>(g_spriteRows.size()))
            break;

        const std::string& row = g_spriteRows[y];

        for (int x = 0; x < g_spriteSize; ++x) {
            if (x >= static_cast<int>(row.size()))
                break;

            char tile = row[x];

            int left = static_cast<int>(static_cast<double>(x) * clientWidth / g_spriteSize);
            int top = static_cast<int>(static_cast<double>(y) * clientHeight / g_spriteSize);
            int right = static_cast<int>(static_cast<double>(x + 1) * clientWidth / g_spriteSize);
            int bottom = static_cast<int>(static_cast<double>(y + 1) * clientHeight / g_spriteSize);

            RECT cellRect = {
                left,
                top,
                right,
                bottom
            };

            COLORREF color;

            switch (tile) {
            case ' ':
                // Transparent pixel
                color = RGB(230, 230, 230);
                break;

            case '0':
                // Void pixel
                color = MAP_BLUE;
                break;

            case '1':
                // Solid pixel
                color = RGB(255, 255, 255);
                break;

            default:
                color = RGB(230, 230, 230);
                break;
            }

            HBRUSH brush = CreateSolidBrush(color);

            FillRect(
                memoryDC,
                &cellRect,
                brush
            );

            DeleteObject(brush);

            // Draw the top and left grid lines
            MoveToEx(
                memoryDC,
                left,
                top,
                nullptr
            );

            LineTo(
                memoryDC,
                right,
                top
            );

            MoveToEx(
                memoryDC,
                left,
                top,
                nullptr
            );

            LineTo(
                memoryDC,
                left,
                bottom
            );
        }
    }

    SelectObject(memoryDC, oldPen);
    DeleteObject(gridPen);

    // Copy the completed back buffer to the window
    BitBlt(
        hdc,
        0,
        0,
        clientWidth,
        clientHeight,
        memoryDC,
        0,
        0,
        SRCCOPY
    );

    SelectObject(memoryDC, oldBitmap);
    DeleteObject(memoryBitmap);
    DeleteDC(memoryDC);
}