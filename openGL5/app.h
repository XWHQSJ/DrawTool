#pragma once

#include "shape.h"
#include <string>

// Encapsulates all mutable drawing state.
// GLUT callbacks access via DrawingApp::instance().
class DrawingApp {
public:
    static DrawingApp& instance() {
        static DrawingApp app;
        return app;
    }

    // Window dimensions
    int winWidth = 800;
    int winHeight = 600;

    // Current drawing color
    float red = 0.0f, green = 0.0f, blue = 1.0f;

    // Line width
    int lineWidth = 1;

    // Current stipple (0 = solid)
    int stipple = 0;

    // Active tool booleans (legacy compat — exactly one true)
    bool toolLine = false;
    bool toolLCircle = false;
    bool toolPCircle = false;
    bool toolLRect = false;
    bool toolPRect = false;
    bool toolLTri = false;
    bool toolPTri = false;
    bool toolPolyline = false;
    bool toolArrow = false;
    bool toolBezier = false;
    bool toolText = false;
    bool toolSelect = false;

    // Current active tool as enum for convenience
    enum class Tool {
        Line, LCircle, PCircle, LRect, PRect,
        LTri, PTri, Polyline, Arrow, Bezier,
        Text, Select
    };
    Tool currentTool = Tool::Line;

    // Mouse drag state
    int xstart = 0, ystart = 0;
    int xend = 0, yend = 0;

    // Triangle placement state
    int triVertCount = 0;
    int triVerts[3][2] = {};

    // Bezier placement state
    int bezierVertCount = 0;
    int bezierVerts[4][2] = {};

    // Freehand polyline accumulator
    std::vector<std::pair<int, int>> polylinePts;
    bool polylineActive = false;

    // Dragging selection state
    bool draggingSelection = false;
    int dragStartX = 0, dragStartY = 0;

    // Text input buffer
    std::string textBuffer;
    bool textInputActive = false;
    int textX = 0, textY = 0;

    // Current layer for new shapes
    int currentLayer = 0;

    void clearAllTools() {
        toolLine = toolLCircle = toolPCircle = toolLRect = toolPRect = false;
        toolLTri = toolPTri = toolPolyline = toolArrow = toolBezier = false;
        toolText = toolSelect = false;
        triVertCount = 0;
        bezierVertCount = 0;
    }

    void setTool(Tool t) {
        clearAllTools();
        currentTool = t;
        switch (t) {
            case Tool::Line:     toolLine = true; break;
            case Tool::LCircle:  toolLCircle = true; break;
            case Tool::PCircle:  toolPCircle = true; break;
            case Tool::LRect:    toolLRect = true; break;
            case Tool::PRect:    toolPRect = true; break;
            case Tool::LTri:     toolLTri = true; break;
            case Tool::PTri:     toolPTri = true; break;
            case Tool::Polyline: toolPolyline = true; break;
            case Tool::Arrow:    toolArrow = true; break;
            case Tool::Bezier:   toolBezier = true; break;
            case Tool::Text:     toolText = true; break;
            case Tool::Select:   toolSelect = true; break;
        }
    }

    // Make a shape with current color/line settings
    Shape makeShape(ShapeType type) const {
        Shape s{};
        s.type = type;
        s.r = red;
        s.g = green;
        s.b = blue;
        s.lineWidth = lineWidth;
        s.stipple = stipple;
        s.layer = currentLayer;
        return s;
    }

private:
    DrawingApp() = default;
    DrawingApp(const DrawingApp&) = delete;
    DrawingApp& operator=(const DrawingApp&) = delete;
};
