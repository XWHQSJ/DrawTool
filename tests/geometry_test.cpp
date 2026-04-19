#include <gtest/gtest.h>

// Include the header-only modules that don't need OpenGL at compile time.
// We test pure-geometry and data-model logic here.
#include "shape.h"
#include "hittest.h"
#include "save.h"

#include <cstdio>
#include <fstream>

// ═══════════════════════════════════════════════════════════════════
// Point-in-rect tests
// ═══════════════════════════════════════════════════════════════════

TEST(Geometry, PointInRect_Inside) {
    EXPECT_TRUE(pointInRect(50, 50, 0, 0, 100, 100));
}

TEST(Geometry, PointInRect_OnEdge) {
    EXPECT_TRUE(pointInRect(0, 50, 0, 0, 100, 100));
    EXPECT_TRUE(pointInRect(100, 100, 0, 0, 100, 100));
}

TEST(Geometry, PointInRect_Outside) {
    EXPECT_FALSE(pointInRect(150, 50, 0, 0, 100, 100));
    EXPECT_FALSE(pointInRect(-1, 50, 0, 0, 100, 100));
}

TEST(Geometry, PointInRect_ReversedCoords) {
    // x2 < x1, y2 < y1 should still work
    EXPECT_TRUE(pointInRect(50, 50, 100, 100, 0, 0));
}

// ═══════════════════════════════════════════════════════════════════
// Point-in-triangle tests
// ═══════════════════════════════════════════════════════════════════

TEST(Geometry, PointInTriangle_Inside) {
    EXPECT_TRUE(pointInTriangle(50, 30, 0, 0, 100, 0, 50, 80));
}

TEST(Geometry, PointInTriangle_Outside) {
    EXPECT_FALSE(pointInTriangle(200, 200, 0, 0, 100, 0, 50, 80));
}

TEST(Geometry, PointInTriangle_OnVertex) {
    EXPECT_TRUE(pointInTriangle(0, 0, 0, 0, 100, 0, 50, 80));
}

// ═══════════════════════════════════════════════════════════════════
// Line intersection tests
// ═══════════════════════════════════════════════════════════════════

TEST(Geometry, LinesIntersect_Crossing) {
    EXPECT_TRUE(linesIntersect(0, 0, 100, 100, 100, 0, 0, 100));
}

TEST(Geometry, LinesIntersect_Parallel) {
    EXPECT_FALSE(linesIntersect(0, 0, 100, 0, 0, 10, 100, 10));
}

TEST(Geometry, LinesIntersect_NonOverlapping) {
    EXPECT_FALSE(linesIntersect(0, 0, 50, 0, 60, 10, 110, 10));
}

TEST(Geometry, LinesIntersect_TShaped) {
    EXPECT_TRUE(linesIntersect(0, 50, 100, 50, 50, 0, 50, 100));
}

// ═══════════════════════════════════════════════════════════════════
// Point-to-segment distance
// ═══════════════════════════════════════════════════════════════════

TEST(Geometry, PointToSegDist_OnSegment) {
    float d = pointToSegDistSq(50, 0, 0, 0, 100, 0);
    EXPECT_FLOAT_EQ(d, 0.0f);
}

TEST(Geometry, PointToSegDist_Perpendicular) {
    float d = pointToSegDistSq(50, 10, 0, 0, 100, 0);
    EXPECT_FLOAT_EQ(d, 100.0f); // 10^2
}

TEST(Geometry, PointToSegDist_BeyondEndpoint) {
    float d = pointToSegDistSq(110, 0, 0, 0, 100, 0);
    EXPECT_FLOAT_EQ(d, 100.0f); // 10^2
}

// ═══════════════════════════════════════════════════════════════════
// Hit testing shapes
// ═══════════════════════════════════════════════════════════════════

TEST(HitTest, Line_OnLine) {
    Shape s{};
    s.type = ShapeType::Line;
    s.x1 = 0; s.y1 = 0;
    s.x2 = 100; s.y2 = 0;
    EXPECT_TRUE(hitTestShape(s, 50, 0));
}

TEST(HitTest, Line_FarAway) {
    Shape s{};
    s.type = ShapeType::Line;
    s.x1 = 0; s.y1 = 0;
    s.x2 = 100; s.y2 = 0;
    EXPECT_FALSE(hitTestShape(s, 50, 100));
}

TEST(HitTest, FillRect_Inside) {
    Shape s{};
    s.type = ShapeType::FillRect;
    s.x1 = 10; s.y1 = 10;
    s.x2 = 90; s.y2 = 90;
    EXPECT_TRUE(hitTestShape(s, 50, 50));
}

TEST(HitTest, FillRect_Outside) {
    Shape s{};
    s.type = ShapeType::FillRect;
    s.x1 = 10; s.y1 = 10;
    s.x2 = 90; s.y2 = 90;
    EXPECT_FALSE(hitTestShape(s, 200, 200));
}

TEST(HitTest, FillCircle_Inside) {
    Shape s{};
    s.type = ShapeType::FillCircle;
    s.x1 = 0; s.y1 = 0;
    s.x2 = 100; s.y2 = 100;
    // Center is (50,50), radius ~70.7/2 = 35.35
    EXPECT_TRUE(hitTestShape(s, 50, 50));
}

TEST(HitTest, FillCircle_Outside) {
    Shape s{};
    s.type = ShapeType::FillCircle;
    s.x1 = 0; s.y1 = 0;
    s.x2 = 100; s.y2 = 100;
    EXPECT_FALSE(hitTestShape(s, 200, 200));
}

// ═══════════════════════════════════════════════════════════════════
// Undo / Redo stack invariants
// ═══════════════════════════════════════════════════════════════════

TEST(UndoRedo, PushAndUndo) {
    g_shapes.clear();
    g_undoStack.clear();
    g_redoStack.clear();

    // Add first shape
    pushUndo();
    Shape s1{}; s1.type = ShapeType::Line; s1.x1 = 0;
    g_shapes.push_back(s1);
    EXPECT_EQ(g_shapes.size(), 1u);

    // Add second shape
    pushUndo();
    Shape s2{}; s2.type = ShapeType::FillRect; s2.x1 = 10;
    g_shapes.push_back(s2);
    EXPECT_EQ(g_shapes.size(), 2u);

    // Undo -> back to 1 shape
    performUndo();
    EXPECT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::Line);

    // Undo -> back to 0 shapes
    performUndo();
    EXPECT_EQ(g_shapes.size(), 0u);

    // Undo on empty -> noop
    performUndo();
    EXPECT_EQ(g_shapes.size(), 0u);
}

TEST(UndoRedo, Redo) {
    g_shapes.clear();
    g_undoStack.clear();
    g_redoStack.clear();

    pushUndo();
    Shape s1{}; s1.type = ShapeType::Line;
    g_shapes.push_back(s1);

    performUndo();
    EXPECT_EQ(g_shapes.size(), 0u);

    performRedo();
    EXPECT_EQ(g_shapes.size(), 1u);
}

TEST(UndoRedo, NewActionClearsRedo) {
    g_shapes.clear();
    g_undoStack.clear();
    g_redoStack.clear();

    pushUndo();
    Shape s1{}; s1.type = ShapeType::Line;
    g_shapes.push_back(s1);

    performUndo();
    EXPECT_EQ(g_redoStack.size(), 1u);

    // New action should clear redo
    pushUndo();
    Shape s2{}; s2.type = ShapeType::FillRect;
    g_shapes.push_back(s2);
    EXPECT_EQ(g_redoStack.size(), 0u);
}

// ═══════════════════════════════════════════════════════════════════
// SVG round-trip
// ═══════════════════════════════════════════════════════════════════

TEST(SVG, RoundTrip_Line) {
    const char* path = "/tmp/drawtool_test_line.svg";
    int w = 500, h = 500;

    std::vector<Shape> original;
    Shape s{};
    s.type = ShapeType::Line;
    s.x1 = 10; s.y1 = 20; s.x2 = 300; s.y2 = 400;
    s.r = 1.0f; s.g = 0.0f; s.b = 0.0f;
    s.lineWidth = 3;
    original.push_back(s);

    ASSERT_TRUE(save_svg(path, original, w, h));

    std::vector<Shape> loaded;
    ASSERT_TRUE(load_svg(path, loaded, h));
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded[0].type, ShapeType::Line);
    EXPECT_EQ(loaded[0].x1, 10);
    EXPECT_EQ(loaded[0].y1, 20);
    EXPECT_EQ(loaded[0].x2, 300);
    EXPECT_EQ(loaded[0].y2, 400);
    EXPECT_NEAR(loaded[0].r, 1.0f, 0.02f);
    EXPECT_EQ(loaded[0].lineWidth, 3);

    std::remove(path);
}

TEST(SVG, RoundTrip_Rect) {
    const char* path = "/tmp/drawtool_test_rect.svg";
    int w = 500, h = 500;

    std::vector<Shape> original;
    Shape s{};
    s.type = ShapeType::FillRect;
    s.x1 = 50; s.y1 = 50; s.x2 = 200; s.y2 = 200;
    s.r = 0.0f; s.g = 1.0f; s.b = 0.0f;
    s.lineWidth = 1;
    original.push_back(s);

    ASSERT_TRUE(save_svg(path, original, w, h));

    std::vector<Shape> loaded;
    ASSERT_TRUE(load_svg(path, loaded, h));
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded[0].type, ShapeType::FillRect);
    EXPECT_NEAR(loaded[0].g, 1.0f, 0.02f);

    std::remove(path);
}

TEST(SVG, RoundTrip_Triangle) {
    const char* path = "/tmp/drawtool_test_tri.svg";
    int w = 500, h = 500;

    std::vector<Shape> original;
    Shape s{};
    s.type = ShapeType::LineTri;
    s.x1 = 100; s.y1 = 100;
    s.x2 = 200; s.y2 = 100;
    s.x3 = 150; s.y3 = 250;
    s.r = 0.0f; s.g = 0.0f; s.b = 1.0f;
    s.lineWidth = 2;
    original.push_back(s);

    ASSERT_TRUE(save_svg(path, original, w, h));

    std::vector<Shape> loaded;
    ASSERT_TRUE(load_svg(path, loaded, h));
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded[0].type, ShapeType::LineTri);
    EXPECT_EQ(loaded[0].x1, 100);
    EXPECT_EQ(loaded[0].y1, 100);
    EXPECT_EQ(loaded[0].x3, 150);
    EXPECT_EQ(loaded[0].y3, 250);

    std::remove(path);
}

TEST(SVG, RoundTrip_MultipleShapes) {
    const char* path = "/tmp/drawtool_test_multi.svg";
    int w = 800, h = 600;

    std::vector<Shape> original;

    Shape line{}; line.type = ShapeType::Line;
    line.x1 = 0; line.y1 = 0; line.x2 = 100; line.y2 = 100;
    line.r = 1; line.g = 1; line.b = 1; line.lineWidth = 1;
    original.push_back(line);

    Shape arrow{}; arrow.type = ShapeType::Arrow;
    arrow.x1 = 200; arrow.y1 = 200; arrow.x2 = 400; arrow.y2 = 300;
    arrow.r = 1; arrow.g = 0; arrow.b = 0; arrow.lineWidth = 2;
    original.push_back(arrow);

    ASSERT_TRUE(save_svg(path, original, w, h));

    std::vector<Shape> loaded;
    ASSERT_TRUE(load_svg(path, loaded, h));
    ASSERT_EQ(loaded.size(), 2u);
    EXPECT_EQ(loaded[0].type, ShapeType::Line);
    EXPECT_EQ(loaded[1].type, ShapeType::Arrow);

    std::remove(path);
}

// ═══════════════════════════════════════════════════════════════════
// Shape data model
// ═══════════════════════════════════════════════════════════════════

TEST(ShapeModel, DefaultValues) {
    Shape s{};
    EXPECT_EQ(s.x1, 0);
    EXPECT_EQ(s.y1, 0);
    EXPECT_EQ(s.lineWidth, 1);
    EXPECT_EQ(s.layer, 0);
    EXPECT_FALSE(s.selected);
    EXPECT_TRUE(s.pts.empty());
}

TEST(ShapeModel, LayerSorting) {
    g_shapes.clear();

    Shape s1{}; s1.type = ShapeType::Line; s1.layer = 2;
    Shape s2{}; s2.type = ShapeType::FillRect; s2.layer = 0;
    Shape s3{}; s3.type = ShapeType::FillCircle; s3.layer = 1;

    g_shapes = {s1, s2, s3};

    // Sort by layer
    std::vector<int> indices = {0, 1, 2};
    std::sort(indices.begin(), indices.end(), [](int a, int b) {
        return g_shapes[a].layer < g_shapes[b].layer;
    });

    EXPECT_EQ(indices[0], 1); // layer 0
    EXPECT_EQ(indices[1], 2); // layer 1
    EXPECT_EQ(indices[2], 0); // layer 2

    g_shapes.clear();
}

TEST(ShapeModel, FindShapeAt) {
    g_shapes.clear();

    Shape s{};
    s.type = ShapeType::FillRect;
    s.x1 = 10; s.y1 = 10;
    s.x2 = 90; s.y2 = 90;
    g_shapes.push_back(s);

    EXPECT_EQ(findShapeAt(50, 50), 0);
    EXPECT_EQ(findShapeAt(200, 200), -1);

    g_shapes.clear();
}
