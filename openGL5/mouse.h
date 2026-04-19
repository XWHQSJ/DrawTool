#pragma once

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#pragma GCC diagnostic pop
#else
#include <GL/glut.h>
#endif

#include "shape.h"
#include "app.h"
#include "hittest.h"
#include <cmath>

// ── Mouse motion (rubber-band preview) ──────────────────────────

void mouseMove(int xmove, int ymove) {
    auto& app = DrawingApp::instance();
    app.xend = xmove;
    app.yend = app.winHeight - ymove;

    // Freehand polyline: accumulate points
    if (app.toolPolyline && app.polylineActive) {
        app.polylinePts.push_back({app.xend, app.yend});
    }

    // Selection drag
    if (app.toolSelect && app.draggingSelection && g_selectedIndex >= 0) {
        int dx = app.xend - app.dragStartX;
        int dy = app.yend - app.dragStartY;
        Shape& s = g_shapes[g_selectedIndex];
        s.x1 += dx; s.y1 += dy;
        s.x2 += dx; s.y2 += dy;
        s.x3 += dx; s.y3 += dy;
        for (auto& p : s.pts) {
            p.first += dx;
            p.second += dy;
        }
        app.dragStartX = app.xend;
        app.dragStartY = app.yend;
    }

    glutPostRedisplay();
}

// ── Mouse button handler ────────────────────────────────────────

void mouseud(int btn, int state, int x, int y) {
    auto& app = DrawingApp::instance();
    int wy = app.winHeight - y;

    // ── Selection tool ──────────────────────────────────────────
    if (app.toolSelect) {
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            int idx = findShapeAt(x, wy);
            if (idx >= 0) {
                g_selectedIndex = idx;
                app.draggingSelection = true;
                app.dragStartX = x;
                app.dragStartY = wy;
            } else {
                g_selectedIndex = -1;
                app.draggingSelection = false;
            }
            glutPostRedisplay();
        }
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
            app.draggingSelection = false;
        }
        return;
    }

    // ── Text tool ───────────────────────────────────────────────
    if (app.toolText) {
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            app.textX = x;
            app.textY = wy;
            app.textInputActive = true;
            app.textBuffer.clear();
        }
        return;
    }

    // ── Triangle tool (3-click placement) ───────────────────────
    if (app.toolLTri || app.toolPTri) {
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            app.triVerts[app.triVertCount][0] = x;
            app.triVerts[app.triVertCount][1] = wy;
            app.triVertCount++;

            if (app.triVertCount == 3) {
                pushUndo();
                ShapeType t = app.toolLTri ? ShapeType::LineTri : ShapeType::FillTri;
                Shape s = app.makeShape(t);
                s.x1 = app.triVerts[0][0]; s.y1 = app.triVerts[0][1];
                s.x2 = app.triVerts[1][0]; s.y2 = app.triVerts[1][1];
                s.x3 = app.triVerts[2][0]; s.y3 = app.triVerts[2][1];
                g_shapes.push_back(s);
                app.triVertCount = 0;
                glutPostRedisplay();
            }
        }
        return;
    }

    // ── Bezier tool (4-click placement) ─────────────────────────
    if (app.toolBezier) {
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            app.bezierVerts[app.bezierVertCount][0] = x;
            app.bezierVerts[app.bezierVertCount][1] = wy;
            app.bezierVertCount++;

            if (app.bezierVertCount == 4) {
                pushUndo();
                Shape s = app.makeShape(ShapeType::Bezier);
                for (int k = 0; k < 4; ++k) {
                    s.pts.push_back({app.bezierVerts[k][0], app.bezierVerts[k][1]});
                }
                g_shapes.push_back(s);
                app.bezierVertCount = 0;
                glutPostRedisplay();
            }
        }
        return;
    }

    // ── Polyline / freehand tool ────────────────────────────────
    if (app.toolPolyline) {
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
            app.polylineActive = true;
            app.polylinePts.clear();
            app.polylinePts.push_back({x, wy});
        }
        if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
            if (app.polylineActive && app.polylinePts.size() >= 2) {
                pushUndo();
                Shape s = app.makeShape(ShapeType::Polyline);
                s.pts = app.polylinePts;
                g_shapes.push_back(s);
            }
            app.polylineActive = false;
            app.polylinePts.clear();
            glutPostRedisplay();
        }
        return;
    }

    // ── Standard 2-point shapes (Line, Rect, Circle, Arrow) ────
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        app.xstart = x;
        app.ystart = wy;
        app.xend = x;
        app.yend = wy;
    }

    if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        app.xend = x;
        app.yend = wy;
        pushUndo();

        ShapeType type = ShapeType::Line;
        if (app.toolLine)     type = ShapeType::Line;
        if (app.toolLCircle)  type = ShapeType::LineCircle;
        if (app.toolPCircle)  type = ShapeType::FillCircle;
        if (app.toolLRect)    type = ShapeType::LineRect;
        if (app.toolPRect)    type = ShapeType::FillRect;
        if (app.toolArrow)    type = ShapeType::Arrow;

        Shape s = app.makeShape(type);
        s.x1 = app.xstart; s.y1 = app.ystart;
        s.x2 = app.xend;   s.y2 = app.yend;
        g_shapes.push_back(s);
        glutPostRedisplay();
    }
}
