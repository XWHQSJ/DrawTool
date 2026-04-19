#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#pragma GCC diagnostic pop
#else
#include <GL/glut.h>
#endif

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "app.h"
#include "shape.h"
#include "mouse.h"
#include "process.h"
#include "selection.h"
#include "save.h"
#include "png_export.h"
#include "script.h"

// ── Init ────────────────────────────────────────────────────────

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    auto& app = DrawingApp::instance();
    gluOrtho2D(0.0, (GLdouble)app.winWidth, 0.0, (GLdouble)app.winHeight);
}

// ── Draw rubber-band preview (not committed to g_shapes) ────────

static void drawPreview() {
    auto& app = DrawingApp::instance();
    glColor3f(app.red, app.green, app.blue);
    glLineWidth((GLfloat)app.lineWidth);

    // Triangle partial preview
    if ((app.toolLTri || app.toolPTri) && app.triVertCount > 0) {
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        for (int k = 0; k < app.triVertCount; ++k) {
            glVertex2i(app.triVerts[k][0], app.triVerts[k][1]);
        }
        glEnd();
        if (app.triVertCount >= 2) {
            glBegin(GL_LINE_STRIP);
            for (int k = 0; k < app.triVertCount; ++k) {
                glVertex2i(app.triVerts[k][0], app.triVerts[k][1]);
            }
            glEnd();
        }
    }

    // Bezier partial preview
    if (app.toolBezier && app.bezierVertCount > 0) {
        glPointSize(6.0f);
        glBegin(GL_POINTS);
        for (int k = 0; k < app.bezierVertCount; ++k) {
            glVertex2i(app.bezierVerts[k][0], app.bezierVerts[k][1]);
        }
        glEnd();
    }

    // Polyline live preview
    if (app.toolPolyline && app.polylineActive && app.polylinePts.size() >= 2) {
        glBegin(GL_LINE_STRIP);
        for (const auto& p : app.polylinePts) {
            glVertex2i(p.first, p.second);
        }
        glEnd();
    }

    // Rubber-band for 2-point shapes
    if (!app.toolLTri && !app.toolPTri && !app.toolBezier &&
        !app.toolPolyline && !app.toolText && !app.toolSelect) {
        if (app.xstart != app.xend || app.ystart != app.yend) {
            glEnable(GL_COLOR_LOGIC_OP);
            glLogicOp(GL_XOR);

            if (app.toolLine || app.toolArrow) {
                glBegin(GL_LINES);
                glVertex2i(app.xstart, app.ystart);
                glVertex2i(app.xend, app.yend);
                glEnd();
            } else if (app.toolLRect || app.toolPRect) {
                glBegin(GL_LINE_LOOP);
                glVertex2i(app.xstart, app.ystart);
                glVertex2i(app.xstart, app.yend);
                glVertex2i(app.xend, app.yend);
                glVertex2i(app.xend, app.ystart);
                glEnd();
            } else if (app.toolLCircle || app.toolPCircle) {
                float radius = std::sqrt(
                    (float)(app.xstart - app.xend) * (app.xstart - app.xend) +
                    (float)(app.ystart - app.yend) * (app.ystart - app.yend)) / 2.0f;
                float cx = (app.xstart + app.xend) / 2.0f;
                float cy = (app.ystart + app.yend) / 2.0f;
                glBegin(GL_LINE_LOOP);
                for (int k = 0; k < 360; ++k) {
                    float angle = 2.0f * 3.14159265f * k / 360.0f;
                    glVertex2f(cx + radius * std::sin(angle),
                               cy + radius * std::cos(angle));
                }
                glEnd();
            }

            glDisable(GL_COLOR_LOGIC_OP);
        }
    }

    // Text input cursor
    if (app.toolText && app.textInputActive) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2i(app.textX, app.textY);
        std::string display = app.textBuffer + "_";
        for (char c : display) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
    }

    glLineWidth(1.0f);
}

// ── HUD overlay ─────────────────────────────────────────────────

static const char* toolName() {
    auto& app = DrawingApp::instance();
    switch (app.currentTool) {
        case DrawingApp::Tool::Line:     return "Line";
        case DrawingApp::Tool::LCircle:  return "Circle (outline)";
        case DrawingApp::Tool::PCircle:  return "Circle (fill)";
        case DrawingApp::Tool::LRect:    return "Rect (outline)";
        case DrawingApp::Tool::PRect:    return "Rect (fill)";
        case DrawingApp::Tool::LTri:     return "Triangle (outline)";
        case DrawingApp::Tool::PTri:     return "Triangle (fill)";
        case DrawingApp::Tool::Polyline: return "Freehand Pen";
        case DrawingApp::Tool::Arrow:    return "Arrow";
        case DrawingApp::Tool::Bezier:   return "Bezier Curve";
        case DrawingApp::Tool::Text:     return "Text";
        case DrawingApp::Tool::Select:   return "Select / Move";
    }
    return "?";
}

static void drawHUD() {
    auto& app = DrawingApp::instance();

    // Semi-transparent bar at the top
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2i(0, app.winHeight);
    glVertex2i(app.winWidth, app.winHeight);
    glVertex2i(app.winWidth, app.winHeight - 22);
    glVertex2i(0, app.winHeight - 22);
    glEnd();
    glDisable(GL_BLEND);

    // Current color swatch
    glColor3f(app.red, app.green, app.blue);
    glBegin(GL_QUADS);
    glVertex2i(5, app.winHeight - 3);
    glVertex2i(19, app.winHeight - 3);
    glVertex2i(19, app.winHeight - 19);
    glVertex2i(5, app.winHeight - 19);
    glEnd();

    // Tool name and shape count
    char info[128];
    std::snprintf(info, sizeof(info), "Tool: %s | Shapes: %d | Layer: %d",
                  toolName(), (int)g_shapes.size(), app.currentLayer);

    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(25, app.winHeight - 15);
    for (const char* c = info; *c; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }
}

// ── Display callback ────────────────────────────────────────────

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawObjects(GL_RENDER);
    drawPreview();
    drawHUD();
    glutSwapBuffers();
}

// ── Reshape callback ────────────────────────────────────────────

void reshape(GLint newWidth, GLint newHeight) {
    auto& app = DrawingApp::instance();
    app.winWidth = newWidth;
    app.winHeight = newHeight;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, newWidth, newHeight);
    gluOrtho2D(0.0, (GLdouble)newWidth, 0.0, (GLdouble)newHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ── Menu callbacks ──────────────────────────────────────────────

void fillOption(GLint s) {
    switch (s) {
    case 6:
        pushUndo();
        g_shapes.clear();
        g_selectedIndex = -1;
        glutPostRedisplay();
        break;
    case 7:
        exit(0);
    default:
        break;
    }
}

void layerMenuCallback(GLint s) {
    DrawingApp::instance().currentLayer = s;
    glutPostRedisplay();
}

void lineWidthCallback(GLint s) {
    DrawingApp::instance().lineWidth = s;
    glutPostRedisplay();
}

// ── Keyboard callback ───────────────────────────────────────────

void keyboardFunc(unsigned char key, int /*x*/, int /*y*/) {
    auto& app = DrawingApp::instance();

    // Text input mode: capture characters
    if (app.toolText && app.textInputActive) {
        if (key == 13 || key == 10) { // Enter
            if (!app.textBuffer.empty()) {
                pushUndo();
                Shape s = app.makeShape(ShapeType::Text);
                s.x1 = app.textX;
                s.y1 = app.textY;
                s.text = app.textBuffer;
                g_shapes.push_back(s);
            }
            app.textInputActive = false;
            app.textBuffer.clear();
            glutPostRedisplay();
            return;
        }
        if (key == 27) { // Escape cancels
            app.textInputActive = false;
            app.textBuffer.clear();
            glutPostRedisplay();
            return;
        }
        if (key == 8 || key == 127) { // Backspace
            if (!app.textBuffer.empty()) app.textBuffer.pop_back();
            glutPostRedisplay();
            return;
        }
        if (key >= 32 && key < 127) {
            app.textBuffer += (char)key;
            glutPostRedisplay();
            return;
        }
        return;
    }

    int mod = glutGetModifiers();

    // Ctrl+Z  -> undo
    if (key == 26 || (key == 'z' && (mod & GLUT_ACTIVE_CTRL))) {
        performUndo();
        glutPostRedisplay();
        return;
    }
    // Ctrl+Y  -> redo
    if (key == 25 || (key == 'y' && (mod & GLUT_ACTIVE_CTRL))) {
        performRedo();
        glutPostRedisplay();
        return;
    }
    // Ctrl+S  -> save PNG
    if (key == 19 || (key == 's' && (mod & GLUT_ACTIVE_CTRL))) {
        savePNG(app.winWidth, app.winHeight);
        return;
    }
    // Ctrl+E  -> save SVG
    if (key == 5 || (key == 'e' && (mod & GLUT_ACTIVE_CTRL))) {
        save_svg("drawing.svg", g_shapes, app.winWidth, app.winHeight);
        std::printf("[DrawTool] Saved SVG: drawing.svg\n");
        return;
    }
    // Ctrl+O  -> load SVG
    if (key == 15 || (key == 'o' && (mod & GLUT_ACTIVE_CTRL))) {
        pushUndo();
        g_shapes.clear();
        load_svg("drawing.svg", g_shapes, app.winHeight);
        std::printf("[DrawTool] Loaded SVG: drawing.svg\n");
        glutPostRedisplay();
        return;
    }

    // Delete key (127 or backspace 8) -> clear all / delete selected
    if (key == 127 || key == 8) {
        if (g_selectedIndex >= 0 && g_selectedIndex < (int)g_shapes.size()) {
            pushUndo();
            g_shapes.erase(g_shapes.begin() + g_selectedIndex);
            g_selectedIndex = -1;
        }
        glutPostRedisplay();
        return;
    }

    // Number keys for tool selection
    switch (key) {
    case '1': app.setTool(DrawingApp::Tool::Line);     break;
    case '2': app.setTool(DrawingApp::Tool::LCircle);  break;
    case '3': app.setTool(DrawingApp::Tool::PCircle);  break;
    case '4': app.setTool(DrawingApp::Tool::LRect);    break;
    case '5': app.setTool(DrawingApp::Tool::PRect);    break;
    case '6': app.setTool(DrawingApp::Tool::LTri);     break;
    case '7': app.setTool(DrawingApp::Tool::PTri);     break;
    case '8': app.setTool(DrawingApp::Tool::Polyline); break;
    case '9': app.setTool(DrawingApp::Tool::Arrow);    break;
    case '0': app.setTool(DrawingApp::Tool::Select);   break;
    default: break;
    }
    glutPostRedisplay();
}

// ── Special keys (arrow keys for moving selection) ──────────────

void specialFunc(int key, int /*x*/, int /*y*/) {
    if (g_selectedIndex < 0 || g_selectedIndex >= (int)g_shapes.size()) return;

    int step = 5;
    int dx = 0, dy = 0;
    switch (key) {
    case GLUT_KEY_LEFT:  dx = -step; break;
    case GLUT_KEY_RIGHT: dx = step;  break;
    case GLUT_KEY_UP:    dy = step;  break;
    case GLUT_KEY_DOWN:  dy = -step; break;
    default: return;
    }

    pushUndo();
    Shape& s = g_shapes[g_selectedIndex];
    s.x1 += dx; s.y1 += dy;
    s.x2 += dx; s.y2 += dy;
    s.x3 += dx; s.y3 += dy;
    for (auto& p : s.pts) {
        p.first += dx;
        p.second += dy;
    }
    glutPostRedisplay();
}

// ── Main ────────────────────────────────────────────────────────

int main(int argc, char** argv) {
    auto& app = DrawingApp::instance();

    // ── Headless script mode: --script file.dtxt ────────────────
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--script" && i + 1 < argc) {
            return run_script(argv[i + 1]);
        }
    }

    std::printf("DrawTool v2.0\n");
    std::printf("Enter line width (1-10): ");
    std::scanf("%d", &app.lineWidth);
    if (app.lineWidth < 1) app.lineWidth = 1;
    if (app.lineWidth > 10) app.lineWidth = 10;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(app.winWidth, app.winHeight);
    glutCreateWindow("DrawTool - by XWHQSJ");

    init();
    glutDisplayFunc(display);

    // ── Shape submenu ───────────────────────────────────────────
    GLint subShape = glutCreateMenu(graphics);
    glutAddMenuEntry("Line [1]",              1);
    glutAddMenuEntry("Circle outline [2]",    2);
    glutAddMenuEntry("Circle fill [3]",       3);
    glutAddMenuEntry("Rect outline [4]",      4);
    glutAddMenuEntry("Rect fill [5]",         5);
    glutAddMenuEntry("Triangle outline [6]",  6);
    glutAddMenuEntry("Triangle fill [7]",     7);
    glutAddMenuEntry("Freehand Pen [8]",      8);
    glutAddMenuEntry("Arrow [9]",             9);
    glutAddMenuEntry("Bezier Curve",          10);
    glutAddMenuEntry("Text",                  11);
    glutAddMenuEntry("Select / Move [0]",     12);

    // ── Color submenu ───────────────────────────────────────────
    GLint subColor = glutCreateMenu(colorSelection);
    glutAddMenuEntry("Red",    1);
    glutAddMenuEntry("Green",  2);
    glutAddMenuEntry("Blue",   3);
    glutAddMenuEntry("Yellow", 4);
    glutAddMenuEntry("Purple", 5);
    glutAddMenuEntry("Cyan",   6);
    glutAddMenuEntry("White",  7);

    // ── Polygon mode submenu ────────────────────────────────────
    GLint subPoly = glutCreateMenu(polygonModeSelection);
    glutAddMenuEntry("Front Fill",   1);
    glutAddMenuEntry("Front Lines",  2);
    glutAddMenuEntry("Front Points", 3);
    glutAddMenuEntry("Back Fill",    4);
    glutAddMenuEntry("Back Lines",   5);
    glutAddMenuEntry("Back Points",  6);

    // ── Shade mode submenu ──────────────────────────────────────
    GLint subShade = glutCreateMenu(polygonModeSelection);
    glutAddMenuEntry("Flat",   7);
    glutAddMenuEntry("Smooth", 8);

    // ── Line style submenu ──────────────────────────────────────
    GLint subLine = glutCreateMenu(linesModeSelection);
    glutAddMenuEntry("0x00FF",  1);
    glutAddMenuEntry("0x0C0F",  3);
    glutAddMenuEntry("0xAAAA",  5);
    glutAddMenuEntry("Solid",   7);

    // ── Line width submenu ──────────────────────────────────────
    GLint subWidth = glutCreateMenu(lineWidthCallback);
    glutAddMenuEntry("1 px", 1);
    glutAddMenuEntry("2 px", 2);
    glutAddMenuEntry("3 px", 3);
    glutAddMenuEntry("5 px", 5);
    glutAddMenuEntry("8 px", 8);

    // ── Layer submenu ───────────────────────────────────────────
    GLint subLayer = glutCreateMenu(layerMenuCallback);
    glutAddMenuEntry("Layer 0 (back)",  0);
    glutAddMenuEntry("Layer 1",         1);
    glutAddMenuEntry("Layer 2",         2);
    glutAddMenuEntry("Layer 3 (front)", 3);

    // ── Main menu ───────────────────────────────────────────────
    glutCreateMenu(fillOption);
    glutAddSubMenu("Shapes",       subShape);
    glutAddSubMenu("Color",        subColor);
    glutAddSubMenu("Polygon Mode", subPoly);
    glutAddSubMenu("Shade Mode",   subShade);
    glutAddSubMenu("Line Style",   subLine);
    glutAddSubMenu("Line Width",   subWidth);
    glutAddSubMenu("Layer",        subLayer);
    glutAddMenuEntry("Clear All",  6);
    glutAddMenuEntry("Exit",       7);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // ── Register callbacks ──────────────────────────────────────
    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseud);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialFunc);

    std::printf("\n--- Keyboard Shortcuts ---\n");
    std::printf("1-9, 0 : Select tool\n");
    std::printf("Ctrl+Z : Undo\n");
    std::printf("Ctrl+Y : Redo\n");
    std::printf("Ctrl+S : Save PNG\n");
    std::printf("Ctrl+E : Save SVG\n");
    std::printf("Ctrl+O : Load SVG\n");
    std::printf("Delete : Delete selected shape\n");
    std::printf("Arrows : Move selected shape\n");
    std::printf("Right-click : Context menu\n\n");

    glutMainLoop();
    return 0;
}
