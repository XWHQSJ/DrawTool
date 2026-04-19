#pragma once

#include "shape.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

static const float PI_F = 3.14159265358979323846f;

// ── Save SVG ────────────────────────────────────────────────────────

inline bool save_svg(const char* path, const std::vector<Shape>& shapes,
                     int w, int h) {
    std::ofstream out(path);
    if (!out.is_open()) return false;

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        << "width=\"" << w << "\" height=\"" << h << "\" "
        << "viewBox=\"0 0 " << w << " " << h << "\">\n";
    out << "<rect width=\"100%\" height=\"100%\" fill=\"black\"/>\n";

    for (const auto& s : shapes) {
        char col[32];
        std::snprintf(col, sizeof(col), "rgb(%d,%d,%d)",
                      (int)(s.r * 255), (int)(s.g * 255), (int)(s.b * 255));

        int fy1 = h - s.y1;
        int fy2 = h - s.y2;
        int fy3 = h - s.y3;

        switch (s.type) {
        case ShapeType::Line:
            out << "  <line x1=\"" << s.x1 << "\" y1=\"" << fy1
                << "\" x2=\"" << s.x2 << "\" y2=\"" << fy2
                << "\" stroke=\"" << col
                << "\" stroke-width=\"" << s.lineWidth
                << "\" data-type=\"Line\"/>\n";
            break;

        case ShapeType::LineCircle:
        case ShapeType::FillCircle: {
            float radius = std::sqrt(
                (float)(s.x1 - s.x2) * (s.x1 - s.x2) +
                (float)(s.y1 - s.y2) * (s.y1 - s.y2)) / 2.0f;
            int cx = (s.x1 + s.x2) / 2;
            int cy = h - (s.y1 + s.y2) / 2;
            const char* fill =
                s.type == ShapeType::FillCircle ? col : "none";
            out << "  <circle cx=\"" << cx << "\" cy=\"" << cy
                << "\" r=\"" << (int)radius
                << "\" fill=\"" << fill
                << "\" stroke=\"" << col
                << "\" stroke-width=\"" << s.lineWidth
                << "\" data-type=\""
                << (s.type == ShapeType::LineCircle ? "LineCircle"
                                                    : "FillCircle")
                << "\"/>\n";
            break;
        }

        case ShapeType::LineRect:
        case ShapeType::FillRect: {
            int rx = std::min(s.x1, s.x2);
            int ry = std::min(fy1, fy2);
            int rw = std::abs(s.x2 - s.x1);
            int rh = std::abs(fy2 - fy1);
            const char* fill =
                s.type == ShapeType::FillRect ? col : "none";
            out << "  <rect x=\"" << rx << "\" y=\"" << ry
                << "\" width=\"" << rw << "\" height=\"" << rh
                << "\" fill=\"" << fill
                << "\" stroke=\"" << col
                << "\" stroke-width=\"" << s.lineWidth
                << "\" data-type=\""
                << (s.type == ShapeType::LineRect ? "LineRect" : "FillRect")
                << "\"/>\n";
            break;
        }

        case ShapeType::LineTri:
        case ShapeType::FillTri: {
            const char* fill =
                s.type == ShapeType::FillTri ? col : "none";
            out << "  <polygon points=\""
                << s.x1 << "," << fy1 << " "
                << s.x2 << "," << fy2 << " "
                << s.x3 << "," << fy3
                << "\" fill=\"" << fill
                << "\" stroke=\"" << col
                << "\" stroke-width=\"" << s.lineWidth
                << "\" data-type=\""
                << (s.type == ShapeType::LineTri ? "LineTri" : "FillTri")
                << "\"/>\n";
            break;
        }

        case ShapeType::Polyline: {
            if (s.pts.empty()) break;
            out << "  <polyline points=\"";
            for (size_t i = 0; i < s.pts.size(); ++i) {
                if (i > 0) out << " ";
                out << s.pts[i].first << "," << (h - s.pts[i].second);
            }
            out << "\" fill=\"none\" stroke=\"" << col
                << "\" stroke-width=\"" << s.lineWidth
                << "\" data-type=\"Polyline\"/>\n";
            break;
        }

        case ShapeType::Arrow:
            out << "  <line x1=\"" << s.x1 << "\" y1=\"" << fy1
                << "\" x2=\"" << s.x2 << "\" y2=\"" << fy2
                << "\" stroke=\"" << col
                << "\" stroke-width=\"" << s.lineWidth
                << "\" marker-end=\"url(#arrowhead)\""
                << " data-type=\"Arrow\"/>\n";
            break;

        case ShapeType::Bezier: {
            if (s.pts.size() < 4) break;
            out << "  <path d=\"M " << s.pts[0].first << " " << (h - s.pts[0].second)
                << " C " << s.pts[1].first << " " << (h - s.pts[1].second)
                << " " << s.pts[2].first << " " << (h - s.pts[2].second)
                << " " << s.pts[3].first << " " << (h - s.pts[3].second)
                << "\" fill=\"none\" stroke=\"" << col
                << "\" stroke-width=\"" << s.lineWidth
                << "\" data-type=\"Bezier\"/>\n";
            break;
        }

        case ShapeType::Text:
            out << "  <text x=\"" << s.x1 << "\" y=\"" << fy1
                << "\" fill=\"" << col
                << "\" font-size=\"18\" data-type=\"Text\">"
                << s.text << "</text>\n";
            break;
        }
    }

    out << "</svg>\n";
    out.close();
    return true;
}

// ── Helper: parse "rgb(r,g,b)" or named color to float ──────────

static inline void parse_color(const std::string& s, float& r, float& g, float& b) {
    r = g = b = 1.0f;
    if (s.find("rgb(") == 0) {
        int ri = 255, gi = 255, bi = 255;
        std::sscanf(s.c_str(), "rgb(%d,%d,%d)", &ri, &gi, &bi);
        r = ri / 255.0f;
        g = gi / 255.0f;
        b = bi / 255.0f;
    }
}

static inline std::string get_attr(const std::string& tag, const char* name) {
    std::string needle = std::string(name) + "=\"";
    auto pos = tag.find(needle);
    if (pos == std::string::npos) return "";
    pos += needle.size();
    auto end = tag.find('"', pos);
    if (end == std::string::npos) return "";
    return tag.substr(pos, end - pos);
}

static inline int get_int_attr(const std::string& tag, const char* name) {
    std::string v = get_attr(tag, name);
    if (v.empty()) return 0;
    return std::atoi(v.c_str());
}

// ── Load SVG (minimal round-trip) ───────────────────────────────

inline bool load_svg(const char* path, std::vector<Shape>& shapes, int h) {
    std::ifstream in(path);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        std::string dtype = get_attr(line, "data-type");
        if (dtype.empty()) continue;

        Shape s{};
        std::string stroke = get_attr(line, "stroke");
        parse_color(stroke, s.r, s.g, s.b);
        std::string sw = get_attr(line, "stroke-width");
        s.lineWidth = sw.empty() ? 1 : std::atoi(sw.c_str());

        if (dtype == "Line" || dtype == "Arrow") {
            s.type = dtype == "Arrow" ? ShapeType::Arrow : ShapeType::Line;
            s.x1 = get_int_attr(line, "x1");
            s.y1 = h - get_int_attr(line, "y1");
            s.x2 = get_int_attr(line, "x2");
            s.y2 = h - get_int_attr(line, "y2");
            shapes.push_back(s);
        } else if (dtype == "LineCircle" || dtype == "FillCircle") {
            s.type = dtype == "LineCircle" ? ShapeType::LineCircle
                                           : ShapeType::FillCircle;
            int cx = get_int_attr(line, "cx");
            int cy = get_int_attr(line, "cy");
            int r  = get_int_attr(line, "r");
            s.x1 = cx - r;
            s.y1 = h - (cy - r);
            s.x2 = cx + r;
            s.y2 = h - (cy + r);
            shapes.push_back(s);
        } else if (dtype == "LineRect" || dtype == "FillRect") {
            s.type = dtype == "LineRect" ? ShapeType::LineRect
                                         : ShapeType::FillRect;
            int rx = get_int_attr(line, "x");
            int ry = get_int_attr(line, "y");
            int rw = get_int_attr(line, "width");
            int rh = get_int_attr(line, "height");
            s.x1 = rx;
            s.y1 = h - ry;
            s.x2 = rx + rw;
            s.y2 = h - (ry + rh);
            shapes.push_back(s);
        } else if (dtype == "LineTri" || dtype == "FillTri") {
            s.type = dtype == "LineTri" ? ShapeType::LineTri
                                        : ShapeType::FillTri;
            std::string pts = get_attr(line, "points");
            int vals[6] = {};
            std::sscanf(pts.c_str(), "%d,%d %d,%d %d,%d",
                        &vals[0], &vals[1], &vals[2],
                        &vals[3], &vals[4], &vals[5]);
            s.x1 = vals[0]; s.y1 = h - vals[1];
            s.x2 = vals[2]; s.y2 = h - vals[3];
            s.x3 = vals[4]; s.y3 = h - vals[5];
            shapes.push_back(s);
        } else if (dtype == "Polyline") {
            s.type = ShapeType::Polyline;
            std::string pts = get_attr(line, "points");
            std::istringstream iss(pts);
            std::string pair;
            while (iss >> pair) {
                int px = 0, py = 0;
                std::sscanf(pair.c_str(), "%d,%d", &px, &py);
                s.pts.push_back({px, h - py});
            }
            shapes.push_back(s);
        }
    }

    in.close();
    return true;
}
