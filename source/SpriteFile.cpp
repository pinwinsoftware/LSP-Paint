#include <fstream>

#include "SpriteFile.h"
#include "EditorState.h"
#include "Editor.h"

bool NewSprite() {
    g_spriteRows.assign(g_spriteSize, std::string(g_spriteSize, ' '));
    return true;
}

bool OpenSprite(const std::string& filename) {
    std::ifstream file(filename);

    if (!file)
        return false;

    std::vector<std::string> rows;
    std::string row;

    while (std::getline(file, row)) {
        if (!row.empty() && row.back() == '\r')
            row.pop_back();

        rows.push_back(row);
    }

    if (rows.empty())
        return false;

    int spriteSize = static_cast<int>(rows[0].size());

    if (spriteSize < MIN_SPRITE_SIZE || spriteSize > MAX_SPRITE_SIZE) {
        return false;
    }

    if (static_cast<int>(rows.size()) != spriteSize)
        return false;

    for (const std::string& currentRow : rows) {
        if (static_cast<int>(currentRow.size()) != spriteSize)
            return false;

        for (char tile : currentRow) {
            if (tile != ' ' && tile != '0' && tile != '1')
                return false;
        }
    }

    g_spriteSize = spriteSize;
    g_spriteRows = std::move(rows);

    g_undoStack.clear();
    g_redoStack.clear();

    UpdateSpriteSizeDisplay();

    return true;
}

bool SaveSprite(const std::string& filename) {
    std::ofstream file(filename);

    if (!file)
        return false;

    for (int y = 0; y < g_spriteSize; ++y) {
        if (y >= static_cast<int>(g_spriteRows.size()))
            return false;

        const std::string& row = g_spriteRows[y];

        if (static_cast<int>(row.size()) != g_spriteSize)
            return false;

        for (char tile : row) {
            if (tile != ' ' && tile != '0' && tile != '1')
                return false;
        }

        file << row << '\n';
    }

    return true;
}