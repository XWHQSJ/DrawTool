#pragma once

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <GLUT/glut.h>
#pragma GCC diagnostic pop
#else
#include <GL/glut.h>
#endif

#include "app.h"

// ── Shape selection menu callback ───────────────────────────────

void graphics(GLint selectedOption) {
    auto& app = DrawingApp::instance();
    switch (selectedOption) {
    case 1:  app.setTool(DrawingApp::Tool::Line);     break;
    case 2:  app.setTool(DrawingApp::Tool::LCircle);  break;
    case 3:  app.setTool(DrawingApp::Tool::PCircle);  break;
    case 4:  app.setTool(DrawingApp::Tool::LRect);    break;
    case 5:  app.setTool(DrawingApp::Tool::PRect);    break;
    case 6:  app.setTool(DrawingApp::Tool::LTri);     break;
    case 7:  app.setTool(DrawingApp::Tool::PTri);     break;
    case 8:  app.setTool(DrawingApp::Tool::Polyline); break;
    case 9:  app.setTool(DrawingApp::Tool::Arrow);    break;
    case 10: app.setTool(DrawingApp::Tool::Bezier);   break;
    case 11: app.setTool(DrawingApp::Tool::Text);     break;
    case 12: app.setTool(DrawingApp::Tool::Select);   break;
    default: break;
    }
    glutPostRedisplay();
}

// ── Color selection menu callback ───────────────────────────────

void colorSelection(GLint selectedOption) {
    auto& app = DrawingApp::instance();
    switch (selectedOption) {
    case 1: app.red = 1.0f; app.green = 0.0f; app.blue = 0.0f; break;
    case 2: app.red = 0.0f; app.green = 1.0f; app.blue = 0.0f; break;
    case 3: app.red = 0.0f; app.green = 0.0f; app.blue = 1.0f; break;
    case 4: app.red = 1.0f; app.green = 1.0f; app.blue = 0.0f; break;
    case 5: app.red = 1.0f; app.green = 0.0f; app.blue = 1.0f; break;
    case 6: app.red = 0.0f; app.green = 1.0f; app.blue = 1.0f; break;
    case 7: app.red = 1.0f; app.green = 1.0f; app.blue = 1.0f; break;
    default: break;
    }
    glutPostRedisplay();
}

// ── Polygon mode selection ──────────────────────────────────────

void polygonModeSelection(GLint selectedOption) {
    switch (selectedOption) {
    case 1: glPolygonMode(GL_FRONT, GL_FILL);  break;
    case 2: glPolygonMode(GL_FRONT, GL_LINE);  break;
    case 3: glPolygonMode(GL_FRONT, GL_POINT); break;
    case 4: glPolygonMode(GL_BACK, GL_FILL);   break;
    case 5: glPolygonMode(GL_BACK, GL_LINE);   break;
    case 6: glPolygonMode(GL_BACK, GL_POINT);  break;
    case 7: glShadeModel(GL_FLAT);             break;
    case 8: glShadeModel(GL_SMOOTH);           break;
    default: break;
    }
    glutPostRedisplay();
}

// ── Line stipple mode selection ─────────────────────────────────

void linesModeSelection(GLint selectedOption) {
    auto& app = DrawingApp::instance();
    switch (selectedOption) {
    case 1: app.stipple = 0x00FF; break;
    case 2: app.stipple = 0x00FF; break;
    case 3: app.stipple = 0x0C0F; break;
    case 4: app.stipple = 0x0C0F; break;
    case 5: app.stipple = 0xAAAA; break;
    case 6: app.stipple = 0xAAAA; break;
    case 7: app.stipple = 0;      break; // solid
    default: break;
    }
    glutPostRedisplay();
}
