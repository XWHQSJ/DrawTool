#pragma once

#include "shape.h"
#include <cmath>

static const float HIT_TOLERANCE = 8.0f;

// Point-to-line-segment distance squared
inline float pointToSegDistSq(int px, int py, int ax, int ay, int bx, int by) {
    float dx = (float)(bx - ax);
    float dy = (float)(by - ay);
    float lenSq = dx * dx + dy * dy;
    if (lenSq < 1e-6f) {
        float ex = (float)(px - ax);
        float ey = (float)(py - ay);
        return ex * ex + ey * ey;
    }
    float t = ((px - ax) * dx + (py - ay) * dy) / lenSq;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float cx = ax + t * dx - px;
    float cy = ay + t * dy - py;
    return cx * cx + cy * cy;
}

// Point inside rectangle (axis-aligned)
inline bool pointInRect(int px, int py, int x1, int y1, int x2, int y2) {
    int minx = x1 < x2 ? x1 : x2;
    int maxx = x1 > x2 ? x1 : x2;
    int miny = y1 < y2 ? y1 : y2;
    int maxy = y1 > y2 ? y1 : y2;
    return px >= minx && px <= maxx && py >= miny && py <= maxy;
}

// Point inside triangle
inline bool pointInTriangle(int px, int py, int x1, int y1, int x2, int y2, int x3, int y3) {
    auto sign = [](int ax, int ay, int bx, int by, int cx, int cy) -> float {
        return (float)(ax - cx) * (by - cy) - (float)(bx - cx) * (ay - cy);
    };
    float d1 = sign(px, py, x1, y1, x2, y2);
    float d2 = sign(px, py, x2, y2, x3, y3);
    float d3 = sign(px, py, x3, y3, x1, y1);
    bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(has_neg && has_pos);
}

// Line-line intersection test
inline bool linesIntersect(int a1x, int a1y, int a2x, int a2y,
                           int b1x, int b1y, int b2x, int b2y) {
    float d1x = (float)(a2x - a1x), d1y = (float)(a2y - a1y);
    float d2x = (float)(b2x - b1x), d2y = (float)(b2y - b1y);
    float cross = d1x * d2y - d1y * d2x;
    if (std::fabs(cross) < 1e-10f) return false;
    float t = ((b1x - a1x) * d2y - (b1y - a1y) * d2x) / cross;
    float u = ((b1x - a1x) * d1y - (b1y - a1y) * d1x) / cross;
    return t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f;
}

// Hit test a single shape at pixel (px, py)
inline bool hitTestShape(const Shape& s, int px, int py) {
    float tol = HIT_TOLERANCE;
    float tolSq = tol * tol;

    switch (s.type) {
    case ShapeType::Line:
    case ShapeType::Arrow:
        return pointToSegDistSq(px, py, s.x1, s.y1, s.x2, s.y2) <= tolSq;

    case ShapeType::LineCircle:
    case ShapeType::FillCircle: {
        float radius = std::sqrt(
            (float)(s.x1 - s.x2) * (s.x1 - s.x2) +
            (float)(s.y1 - s.y2) * (s.y1 - s.y2)) / 2.0f;
        float cx = (s.x1 + s.x2) / 2.0f;
        float cy = (s.y1 + s.y2) / 2.0f;
        float dist = std::sqrt((px - cx) * (px - cx) + (py - cy) * (py - cy));
        if (s.type == ShapeType::FillCircle) return dist <= radius + tol;
        return std::fabs(dist - radius) <= tol;
    }

    case ShapeType::LineRect:
        // Near any edge
        if (pointToSegDistSq(px, py, s.x1, s.y1, s.x2, s.y1) <= tolSq) return true;
        if (pointToSegDistSq(px, py, s.x2, s.y1, s.x2, s.y2) <= tolSq) return true;
        if (pointToSegDistSq(px, py, s.x2, s.y2, s.x1, s.y2) <= tolSq) return true;
        if (pointToSegDistSq(px, py, s.x1, s.y2, s.x1, s.y1) <= tolSq) return true;
        return false;

    case ShapeType::FillRect:
        return pointInRect(px, py, s.x1, s.y1, s.x2, s.y2);

    case ShapeType::LineTri:
        if (pointToSegDistSq(px, py, s.x1, s.y1, s.x2, s.y2) <= tolSq) return true;
        if (pointToSegDistSq(px, py, s.x2, s.y2, s.x3, s.y3) <= tolSq) return true;
        if (pointToSegDistSq(px, py, s.x3, s.y3, s.x1, s.y1) <= tolSq) return true;
        return false;

    case ShapeType::FillTri:
        return pointInTriangle(px, py, s.x1, s.y1, s.x2, s.y2, s.x3, s.y3);

    case ShapeType::Polyline: {
        for (size_t i = 1; i < s.pts.size(); ++i) {
            if (pointToSegDistSq(px, py,
                    s.pts[i - 1].first, s.pts[i - 1].second,
                    s.pts[i].first, s.pts[i].second) <= tolSq)
                return true;
        }
        return false;
    }

    case ShapeType::Bezier: {
        // Approximate: test against polyline segments between control points
        if (s.pts.size() < 4) return false;
        // Sample 20 points on the bezier
        for (int i = 1; i <= 20; ++i) {
            float t0 = (i - 1) / 20.0f;
            float t1 = i / 20.0f;
            auto bez = [&](float t) -> std::pair<float, float> {
                float u = 1 - t;
                float x = u*u*u*s.pts[0].first + 3*u*u*t*s.pts[1].first +
                           3*u*t*t*s.pts[2].first + t*t*t*s.pts[3].first;
                float y = u*u*u*s.pts[0].second + 3*u*u*t*s.pts[1].second +
                           3*u*t*t*s.pts[2].second + t*t*t*s.pts[3].second;
                return {x, y};
            };
            auto [ax, ay] = bez(t0);
            auto [bx, by] = bez(t1);
            if (pointToSegDistSq(px, py, (int)ax, (int)ay, (int)bx, (int)by) <= tolSq)
                return true;
        }
        return false;
    }

    case ShapeType::Text: {
        // Simple bounding box around text origin
        int tw = (int)s.text.size() * 9;
        return px >= s.x1 && px <= s.x1 + tw && py >= s.y1 - 18 && py <= s.y1;
    }
    }
    return false;
}

// Find top-most (last-drawn) shape at point, return index or -1
inline int findShapeAt(int px, int py) {
    for (int i = (int)g_shapes.size() - 1; i >= 0; --i) {
        if (hitTestShape(g_shapes[i], px, py)) return i;
    }
    return -1;
}
