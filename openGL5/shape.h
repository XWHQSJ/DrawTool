#pragma once

#include <utility>
#include <vector>

enum class ShapeType {
    Line,
    LineCircle,
    FillCircle,
    LineRect,
    FillRect,
    LineTri,
    FillTri,
    Polyline,
    Arrow,
    Bezier,
    Text
};

struct Shape {
    ShapeType type;
    int x1 = 0, y1 = 0;
    int x2 = 0, y2 = 0;
    int x3 = 0, y3 = 0;
    float r = 1.0f, g = 1.0f, b = 1.0f;
    int lineWidth = 1;
    int stipple = 0;
    int layer = 0;
    bool selected = false;
    std::vector<std::pair<int, int>> pts;
    std::string text;
};

// Global shape storage
inline std::vector<Shape> g_shapes;

// Undo / Redo stacks
inline std::vector<std::vector<Shape>> g_undoStack;
inline std::vector<std::vector<Shape>> g_redoStack;

// Selection index (-1 = none)
inline int g_selectedIndex = -1;

// Push current state onto undo stack before modification
inline void pushUndo() {
    g_undoStack.push_back(g_shapes);
    g_redoStack.clear();
}

inline void performUndo() {
    if (g_undoStack.empty()) return;
    g_redoStack.push_back(g_shapes);
    g_shapes = g_undoStack.back();
    g_undoStack.pop_back();
    g_selectedIndex = -1;
}

inline void performRedo() {
    if (g_redoStack.empty()) return;
    g_undoStack.push_back(g_shapes);
    g_shapes = g_redoStack.back();
    g_redoStack.pop_back();
    g_selectedIndex = -1;
}
