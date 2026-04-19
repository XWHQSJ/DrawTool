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
#include <algorithm>
#include <cmath>

static const float PI_CONST = 3.14159265358979323846f;
static const int CIRCLE_SEGMENTS = 360;

// ── Render a single shape ───────────────────────────────────────

static inline void renderShape(const Shape& s, bool highlight) {
    glColor3f(s.r, s.g, s.b);

    if (highlight) {
        // Draw a white dashed outline around selected shape
        glColor3f(1.0f, 1.0f, 1.0f);
        glLineStipple(1, 0xAAAA);
        glEnable(GL_LINE_STIPPLE);
    }

    if (s.stipple != 0 && !highlight) {
        glLineStipple(1, (GLushort)s.stipple);
        glEnable(GL_LINE_STIPPLE);
    }

    glLineWidth((GLfloat)s.lineWidth);

    switch (s.type) {
    case ShapeType::Line: {
        glBegin(GL_LINES);
        glVertex2i(s.x1, s.y1);
        glVertex2i(s.x2, s.y2);
        glEnd();
        break;
    }

    case ShapeType::LineCircle:
    case ShapeType::FillCircle: {
        float radius = std::sqrt(
            (float)(s.x1 - s.x2) * (s.x1 - s.x2) +
            (float)(s.y1 - s.y2) * (s.y1 - s.y2)) / 2.0f;
        float cx = (s.x1 + s.x2) / 2.0f;
        float cy = (s.y1 + s.y2) / 2.0f;

        GLenum mode = (s.type == ShapeType::FillCircle) ? GL_POLYGON : GL_LINE_LOOP;
        glBegin(mode);
        for (int i = 0; i < CIRCLE_SEGMENTS; ++i) {
            float angle = 2.0f * PI_CONST * i / CIRCLE_SEGMENTS;
            glVertex2f(cx + radius * std::sin(angle),
                       cy + radius * std::cos(angle));
        }
        glEnd();
        break;
    }

    case ShapeType::LineRect: {
        glBegin(GL_LINE_LOOP);
        glVertex2i(s.x1, s.y1);
        glVertex2i(s.x1, s.y2);
        glVertex2i(s.x2, s.y2);
        glVertex2i(s.x2, s.y1);
        glEnd();
        break;
    }

    case ShapeType::FillRect: {
        glBegin(GL_POLYGON);
        glVertex2i(s.x1, s.y1);
        glVertex2i(s.x1, s.y2);
        glVertex2i(s.x2, s.y2);
        glVertex2i(s.x2, s.y1);
        glEnd();
        break;
    }

    case ShapeType::LineTri: {
        glBegin(GL_LINE_LOOP);
        glVertex2i(s.x1, s.y1);
        glVertex2i(s.x2, s.y2);
        glVertex2i(s.x3, s.y3);
        glEnd();
        break;
    }

    case ShapeType::FillTri: {
        glBegin(GL_TRIANGLES);
        glVertex2i(s.x1, s.y1);
        glVertex2i(s.x2, s.y2);
        glVertex2i(s.x3, s.y3);
        glEnd();
        break;
    }

    case ShapeType::Polyline: {
        if (s.pts.size() < 2) break;
        glBegin(GL_LINE_STRIP);
        for (const auto& p : s.pts) {
            glVertex2i(p.first, p.second);
        }
        glEnd();
        break;
    }

    case ShapeType::Arrow: {
        // Shaft
        glBegin(GL_LINES);
        glVertex2i(s.x1, s.y1);
        glVertex2i(s.x2, s.y2);
        glEnd();

        // Arrowhead
        float dx = (float)(s.x2 - s.x1);
        float dy = (float)(s.y2 - s.y1);
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 1.0f) break;
        float ux = dx / len;
        float uy = dy / len;
        float headLen = 12.0f;
        float headW = 5.0f;
        float bx = s.x2 - ux * headLen;
        float by = s.y2 - uy * headLen;
        glBegin(GL_TRIANGLES);
        glVertex2i(s.x2, s.y2);
        glVertex2f(bx + uy * headW, by - ux * headW);
        glVertex2f(bx - uy * headW, by + ux * headW);
        glEnd();
        break;
    }

    case ShapeType::Bezier: {
        if (s.pts.size() < 4) break;
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 50; ++i) {
            float t = i / 50.0f;
            float u = 1.0f - t;
            float x = u*u*u*s.pts[0].first + 3*u*u*t*s.pts[1].first +
                       3*u*t*t*s.pts[2].first + t*t*t*s.pts[3].first;
            float y = u*u*u*s.pts[0].second + 3*u*u*t*s.pts[1].second +
                       3*u*t*t*s.pts[2].second + t*t*t*s.pts[3].second;
            glVertex2f(x, y);
        }
        glEnd();
        // Draw control points as small squares
        glPointSize(6.0f);
        glBegin(GL_POINTS);
        for (const auto& p : s.pts) {
            glVertex2i(p.first, p.second);
        }
        glEnd();
        break;
    }

    case ShapeType::Text: {
        glRasterPos2i(s.x1, s.y1);
        for (char c : s.text) {
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
        }
        break;
    }
    }

    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.0f);
}

// ── Draw all shapes (sorted by layer) ───────────────────────────

inline void drawObjects(GLenum mode) {
    // Sort indices by layer
    std::vector<int> indices(g_shapes.size());
    for (int i = 0; i < (int)g_shapes.size(); ++i) indices[i] = i;
    std::sort(indices.begin(), indices.end(), [](int a, int b) {
        return g_shapes[a].layer < g_shapes[b].layer;
    });

    for (int idx : indices) {
        if (mode == GL_SELECT) {
            glLoadName(idx);
        }
        bool highlight = (idx == g_selectedIndex);
        renderShape(g_shapes[idx], highlight);
    }
}

// ── Legacy pick processing (kept for compat) ────────────────────

inline void processSelect(GLint hits, GLuint buffer[]) {
    if (hits <= 0) return;
    GLuint* ptr = buffer;
    GLuint minZ = 0xffffffff;
    GLuint closestName = 0;
    for (int i = 0; i < hits; ++i) {
        GLuint names = *ptr++;
        GLuint zMin = *ptr++;
        ptr++; // zMax
        if (zMin < minZ && names > 0) {
            minZ = zMin;
            closestName = *ptr;
        }
        ptr += names;
    }
    g_selectedIndex = (int)closestName;
}
