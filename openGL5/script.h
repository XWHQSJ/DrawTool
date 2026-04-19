#pragma once

#include "shape.h"
#include "save.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Headless script interpreter for DrawTool.
// Reads a .dtxt file and executes drawing commands without OpenGL.
//
// Supported commands:
//   line x1 y1 x2 y2
//   circle cx cy radius [fill]
//   rect x1 y1 x2 y2 [fill]
//   triangle x1 y1 x2 y2 x3 y3 [fill]
//   arrow x1 y1 x2 y2
//   text x y "content"
//   color r g b           (floats 0.0-1.0)
//   linewidth w           (integer 1-10)
//   layer n               (integer 0-3)
//   undo
//   redo
//   clear
//   save path.svg
//   save path.png         (PNG requires OpenGL context; headless emits warning)
//   width w               (canvas width)
//   height h              (canvas height)
//   quit

struct ScriptState {
    float r = 1.0f, g = 1.0f, b = 1.0f;
    int lineWidth = 1;
    int stipple = 0;
    int layer = 0;
    int canvasW = 800;
    int canvasH = 600;
    int commandsRun = 0;
    int errors = 0;
};

inline bool run_script_line(const std::string& line, ScriptState& st) {
    // Strip leading whitespace
    size_t start = line.find_first_not_of(" \t");
    if (start == std::string::npos) return true; // blank
    std::string trimmed = line.substr(start);

    // Skip comments and empty lines
    if (trimmed.empty() || trimmed[0] == '#') return true;

    std::istringstream iss(trimmed);
    std::string cmd;
    iss >> cmd;

    auto make = [&](ShapeType type) -> Shape {
        Shape s{};
        s.type = type;
        s.r = st.r;
        s.g = st.g;
        s.b = st.b;
        s.lineWidth = st.lineWidth;
        s.stipple = st.stipple;
        s.layer = st.layer;
        return s;
    };

    if (cmd == "line") {
        int x1, y1, x2, y2;
        if (!(iss >> x1 >> y1 >> x2 >> y2)) {
            std::fprintf(stderr, "[script] line: expected 4 ints\n");
            st.errors++;
            return false;
        }
        pushUndo();
        Shape s = make(ShapeType::Line);
        s.x1 = x1; s.y1 = y1; s.x2 = x2; s.y2 = y2;
        g_shapes.push_back(s);

    } else if (cmd == "circle") {
        int cx, cy, radius;
        if (!(iss >> cx >> cy >> radius)) {
            std::fprintf(stderr, "[script] circle: expected cx cy radius\n");
            st.errors++;
            return false;
        }
        std::string fill_flag;
        iss >> fill_flag;
        bool filled = (fill_flag == "fill");
        pushUndo();
        Shape s = make(filled ? ShapeType::FillCircle : ShapeType::LineCircle);
        s.x1 = cx - radius; s.y1 = cy - radius;
        s.x2 = cx + radius; s.y2 = cy + radius;
        g_shapes.push_back(s);

    } else if (cmd == "rect") {
        int x1, y1, x2, y2;
        if (!(iss >> x1 >> y1 >> x2 >> y2)) {
            std::fprintf(stderr, "[script] rect: expected 4 ints\n");
            st.errors++;
            return false;
        }
        std::string fill_flag;
        iss >> fill_flag;
        bool filled = (fill_flag == "fill");
        pushUndo();
        Shape s = make(filled ? ShapeType::FillRect : ShapeType::LineRect);
        s.x1 = x1; s.y1 = y1; s.x2 = x2; s.y2 = y2;
        g_shapes.push_back(s);

    } else if (cmd == "triangle") {
        int x1, y1, x2, y2, x3, y3;
        if (!(iss >> x1 >> y1 >> x2 >> y2 >> x3 >> y3)) {
            std::fprintf(stderr, "[script] triangle: expected 6 ints\n");
            st.errors++;
            return false;
        }
        std::string fill_flag;
        iss >> fill_flag;
        bool filled = (fill_flag == "fill");
        pushUndo();
        Shape s = make(filled ? ShapeType::FillTri : ShapeType::LineTri);
        s.x1 = x1; s.y1 = y1; s.x2 = x2; s.y2 = y2; s.x3 = x3; s.y3 = y3;
        g_shapes.push_back(s);

    } else if (cmd == "arrow") {
        int x1, y1, x2, y2;
        if (!(iss >> x1 >> y1 >> x2 >> y2)) {
            std::fprintf(stderr, "[script] arrow: expected 4 ints\n");
            st.errors++;
            return false;
        }
        pushUndo();
        Shape s = make(ShapeType::Arrow);
        s.x1 = x1; s.y1 = y1; s.x2 = x2; s.y2 = y2;
        g_shapes.push_back(s);

    } else if (cmd == "text") {
        int x, y;
        if (!(iss >> x >> y)) {
            std::fprintf(stderr, "[script] text: expected x y \"content\"\n");
            st.errors++;
            return false;
        }
        // Read rest of line as text content (strip leading space)
        std::string content;
        std::getline(iss, content);
        size_t cstart = content.find_first_not_of(" \t");
        if (cstart != std::string::npos) {
            content = content.substr(cstart);
        }
        // Strip optional surrounding quotes
        if (content.size() >= 2 && content.front() == '"' && content.back() == '"') {
            content = content.substr(1, content.size() - 2);
        }
        pushUndo();
        Shape s = make(ShapeType::Text);
        s.x1 = x; s.y1 = y;
        s.text = content;
        g_shapes.push_back(s);

    } else if (cmd == "color") {
        float cr, cg, cb;
        if (!(iss >> cr >> cg >> cb)) {
            std::fprintf(stderr, "[script] color: expected r g b (0.0-1.0)\n");
            st.errors++;
            return false;
        }
        st.r = cr; st.g = cg; st.b = cb;

    } else if (cmd == "linewidth") {
        int w;
        if (!(iss >> w)) {
            std::fprintf(stderr, "[script] linewidth: expected integer\n");
            st.errors++;
            return false;
        }
        if (w < 1) w = 1;
        if (w > 10) w = 10;
        st.lineWidth = w;

    } else if (cmd == "layer") {
        int n;
        if (!(iss >> n)) {
            std::fprintf(stderr, "[script] layer: expected integer\n");
            st.errors++;
            return false;
        }
        if (n < 0) n = 0;
        if (n > 3) n = 3;
        st.layer = n;

    } else if (cmd == "undo") {
        performUndo();

    } else if (cmd == "redo") {
        performRedo();

    } else if (cmd == "clear") {
        pushUndo();
        g_shapes.clear();
        g_selectedIndex = -1;

    } else if (cmd == "save") {
        std::string path;
        if (!(iss >> path)) {
            std::fprintf(stderr, "[script] save: expected path\n");
            st.errors++;
            return false;
        }
        if (path.size() >= 4 && path.substr(path.size() - 4) == ".svg") {
            bool ok = save_svg(path.c_str(), g_shapes, st.canvasW, st.canvasH);
            if (ok) {
                std::printf("[script] saved SVG: %s (%d shapes)\n",
                            path.c_str(), (int)g_shapes.size());
            } else {
                std::fprintf(stderr, "[script] failed to save SVG: %s\n", path.c_str());
                st.errors++;
                return false;
            }
        } else if (path.size() >= 4 && path.substr(path.size() - 4) == ".png") {
            std::fprintf(stderr,
                "[script] PNG export requires OpenGL context (skipped in headless mode): %s\n",
                path.c_str());
        } else {
            std::fprintf(stderr, "[script] save: unsupported format: %s\n", path.c_str());
            st.errors++;
            return false;
        }

    } else if (cmd == "width") {
        int w;
        if (!(iss >> w)) {
            std::fprintf(stderr, "[script] width: expected integer\n");
            st.errors++;
            return false;
        }
        st.canvasW = w;

    } else if (cmd == "height") {
        int h;
        if (!(iss >> h)) {
            std::fprintf(stderr, "[script] height: expected integer\n");
            st.errors++;
            return false;
        }
        st.canvasH = h;

    } else if (cmd == "quit") {
        return true; // handled by caller

    } else {
        std::fprintf(stderr, "[script] unknown command: %s\n", cmd.c_str());
        st.errors++;
        return false;
    }

    st.commandsRun++;
    return true;
}

// Run an entire script file. Returns 0 on success, 1 on error.
inline int run_script(const char* path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::fprintf(stderr, "[script] cannot open: %s\n", path);
        return 1;
    }

    // Reset global state
    g_shapes.clear();
    g_undoStack.clear();
    g_redoStack.clear();
    g_selectedIndex = -1;

    ScriptState st{};
    std::string line;
    int lineNum = 0;

    while (std::getline(in, line)) {
        lineNum++;

        // Check for quit command
        size_t start = line.find_first_not_of(" \t");
        if (start != std::string::npos) {
            std::string trimmed = line.substr(start);
            if (trimmed.substr(0, 4) == "quit") {
                std::printf("[script] quit at line %d\n", lineNum);
                break;
            }
        }

        if (!run_script_line(line, st)) {
            std::fprintf(stderr, "[script] error at line %d: %s\n", lineNum, line.c_str());
        }
    }

    in.close();
    std::printf("[script] finished: %d commands, %d errors, %d shapes\n",
                st.commandsRun, st.errors, (int)g_shapes.size());
    return st.errors > 0 ? 1 : 0;
}
