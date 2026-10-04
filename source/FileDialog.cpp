#include <windows.h>
#include <commdlg.h>

#include "Filedialog.h"

#pragma comment(lib, "Comdlg32.lib")

std::string OpenSpriteFile() {
    char filename[MAX_PATH] = {};

    OPENFILENAMEA dialog = {};

    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFile = filename;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrFilter = "Sprite Files (*.lsp)\0*.lsp\0" "All Files (*.*)\0*.*\0";
    dialog.nFilterIndex = 1;
    dialog.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileNameA(&dialog)) {
        return filename;
    }

    return "";
}

std::string SaveSpriteFile() {
    char filename[MAX_PATH] = {};

    OPENFILENAMEA dialog = {};

    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFile = filename;
    dialog.nMaxFile = MAX_PATH;
    dialog.lpstrFilter = "Sprite Files (*.lsp)\0*.lsp\0" "All Files (*.*)\0*.*\0";
    dialog.nFilterIndex = 1;
    dialog.lpstrDefExt = "lsp";
    dialog.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileNameA(&dialog)) {
        return filename;
    }

    return "";
}