#include <gtest/gtest.h>

#include "shape.h"
#include "save.h"
#include "script.h"

#include <cstdio>
#include <fstream>
#include <string>

// Helper: reset global state before each test
class ScriptTest : public ::testing::Test {
protected:
    void SetUp() override {
        g_shapes.clear();
        g_undoStack.clear();
        g_redoStack.clear();
        g_selectedIndex = -1;
    }
};

// ═══════════════════════════════════════════════════════════════════
// Individual command tests
// ═══════════════════════════════════════════════════════════════════

TEST_F(ScriptTest, LineCommand) {
    ScriptState st{};
    run_script_line("line 10 20 100 200", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::Line);
    EXPECT_EQ(g_shapes[0].x1, 10);
    EXPECT_EQ(g_shapes[0].y1, 20);
    EXPECT_EQ(g_shapes[0].x2, 100);
    EXPECT_EQ(g_shapes[0].y2, 200);
}

TEST_F(ScriptTest, CircleOutlineCommand) {
    ScriptState st{};
    run_script_line("circle 150 150 50", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::LineCircle);
    EXPECT_EQ(g_shapes[0].x1, 100);
    EXPECT_EQ(g_shapes[0].y1, 100);
    EXPECT_EQ(g_shapes[0].x2, 200);
    EXPECT_EQ(g_shapes[0].y2, 200);
}

TEST_F(ScriptTest, CircleFillCommand) {
    ScriptState st{};
    run_script_line("circle 150 150 50 fill", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::FillCircle);
}

TEST_F(ScriptTest, RectOutlineCommand) {
    ScriptState st{};
    run_script_line("rect 10 20 300 400", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::LineRect);
    EXPECT_EQ(g_shapes[0].x1, 10);
    EXPECT_EQ(g_shapes[0].y1, 20);
    EXPECT_EQ(g_shapes[0].x2, 300);
    EXPECT_EQ(g_shapes[0].y2, 400);
}

TEST_F(ScriptTest, RectFillCommand) {
    ScriptState st{};
    run_script_line("rect 200 200 300 300 fill", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::FillRect);
}

TEST_F(ScriptTest, TriangleOutlineCommand) {
    ScriptState st{};
    run_script_line("triangle 0 0 100 0 50 80", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::LineTri);
    EXPECT_EQ(g_shapes[0].x1, 0);
    EXPECT_EQ(g_shapes[0].y1, 0);
    EXPECT_EQ(g_shapes[0].x3, 50);
    EXPECT_EQ(g_shapes[0].y3, 80);
}

TEST_F(ScriptTest, TriangleFillCommand) {
    ScriptState st{};
    run_script_line("triangle 0 0 100 0 50 80 fill", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::FillTri);
}

TEST_F(ScriptTest, ArrowCommand) {
    ScriptState st{};
    run_script_line("arrow 0 0 200 200", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::Arrow);
}

TEST_F(ScriptTest, TextCommand) {
    ScriptState st{};
    run_script_line("text 50 100 Hello World", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].type, ShapeType::Text);
    EXPECT_EQ(g_shapes[0].x1, 50);
    EXPECT_EQ(g_shapes[0].y1, 100);
    EXPECT_EQ(g_shapes[0].text, "Hello World");
}

TEST_F(ScriptTest, TextCommandQuoted) {
    ScriptState st{};
    run_script_line("text 50 100 \"Quoted Text\"", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].text, "Quoted Text");
}

TEST_F(ScriptTest, ColorCommand) {
    ScriptState st{};
    run_script_line("color 1.0 0.0 0.0", st);
    run_script_line("line 0 0 100 100", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_NEAR(g_shapes[0].r, 1.0f, 0.01f);
    EXPECT_NEAR(g_shapes[0].g, 0.0f, 0.01f);
    EXPECT_NEAR(g_shapes[0].b, 0.0f, 0.01f);
}

TEST_F(ScriptTest, LinewidthCommand) {
    ScriptState st{};
    run_script_line("linewidth 5", st);
    run_script_line("line 0 0 100 100", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].lineWidth, 5);
}

TEST_F(ScriptTest, LayerCommand) {
    ScriptState st{};
    run_script_line("layer 2", st);
    run_script_line("line 0 0 100 100", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    EXPECT_EQ(g_shapes[0].layer, 2);
}

TEST_F(ScriptTest, UndoCommand) {
    ScriptState st{};
    run_script_line("line 0 0 100 100", st);
    ASSERT_EQ(g_shapes.size(), 1u);
    run_script_line("undo", st);
    EXPECT_EQ(g_shapes.size(), 0u);
}

TEST_F(ScriptTest, RedoCommand) {
    ScriptState st{};
    run_script_line("line 0 0 100 100", st);
    run_script_line("undo", st);
    EXPECT_EQ(g_shapes.size(), 0u);
    run_script_line("redo", st);
    EXPECT_EQ(g_shapes.size(), 1u);
}

TEST_F(ScriptTest, ClearCommand) {
    ScriptState st{};
    run_script_line("line 0 0 100 100", st);
    run_script_line("rect 10 10 50 50", st);
    ASSERT_EQ(g_shapes.size(), 2u);
    run_script_line("clear", st);
    EXPECT_EQ(g_shapes.size(), 0u);
}

TEST_F(ScriptTest, SaveSVG) {
    ScriptState st{};
    run_script_line("line 10 20 100 200", st);
    run_script_line("save /tmp/drawtool_script_test.svg", st);

    // Verify file was written and round-trips
    std::vector<Shape> loaded;
    ASSERT_TRUE(load_svg("/tmp/drawtool_script_test.svg", loaded, st.canvasH));
    ASSERT_EQ(loaded.size(), 1u);
    EXPECT_EQ(loaded[0].type, ShapeType::Line);

    std::remove("/tmp/drawtool_script_test.svg");
}

TEST_F(ScriptTest, CanvasDimensions) {
    ScriptState st{};
    run_script_line("width 1024", st);
    run_script_line("height 768", st);
    EXPECT_EQ(st.canvasW, 1024);
    EXPECT_EQ(st.canvasH, 768);
}

TEST_F(ScriptTest, CommentsAndBlanks) {
    ScriptState st{};
    EXPECT_TRUE(run_script_line("", st));
    EXPECT_TRUE(run_script_line("   ", st));
    EXPECT_TRUE(run_script_line("# this is a comment", st));
    EXPECT_EQ(g_shapes.size(), 0u);
}

TEST_F(ScriptTest, UnknownCommand) {
    ScriptState st{};
    run_script_line("foobar 1 2 3", st);
    EXPECT_EQ(st.errors, 1);
}

TEST_F(ScriptTest, BadArguments) {
    ScriptState st{};
    run_script_line("line 10 20", st);  // missing two args
    EXPECT_EQ(st.errors, 1);
    EXPECT_EQ(g_shapes.size(), 0u);
}

// ═══════════════════════════════════════════════════════════════════
// Full script file tests
// ═══════════════════════════════════════════════════════════════════

TEST_F(ScriptTest, FullScript_DrawAndSave) {
    const char* script_path = "/tmp/drawtool_test_script.dtxt";
    const char* svg_path = "/tmp/drawtool_test_script_out.svg";

    // Write a test script
    {
        std::ofstream out(script_path);
        out << "# Demo script\n"
            << "color 1.0 0.0 0.0\n"
            << "line 10 20 100 200\n"
            << "line 50 50 150 150\n"
            << "line 80 80 200 200\n"
            << "color 0.0 1.0 0.0\n"
            << "rect 200 200 300 300 fill\n"
            << "save " << svg_path << "\n"
            << "quit\n";
    }

    int result = run_script(script_path);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(g_shapes.size(), 4u);

    // Verify SVG output
    std::vector<Shape> loaded;
    ASSERT_TRUE(load_svg(svg_path, loaded, 600));
    EXPECT_EQ(loaded.size(), 4u);
    EXPECT_EQ(loaded[0].type, ShapeType::Line);
    EXPECT_EQ(loaded[3].type, ShapeType::FillRect);

    std::remove(script_path);
    std::remove(svg_path);
}

TEST_F(ScriptTest, FullScript_UndoThenSave) {
    // "after drawing 3 lines + undo, SVG should contain 2 lines"
    const char* script_path = "/tmp/drawtool_test_undo.dtxt";
    const char* svg_path = "/tmp/drawtool_test_undo_out.svg";

    {
        std::ofstream out(script_path);
        out << "line 10 20 100 200\n"
            << "line 50 50 150 150\n"
            << "line 80 80 200 200\n"
            << "undo\n"
            << "save " << svg_path << "\n"
            << "quit\n";
    }

    int result = run_script(script_path);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(g_shapes.size(), 2u);

    std::vector<Shape> loaded;
    ASSERT_TRUE(load_svg(svg_path, loaded, 600));
    EXPECT_EQ(loaded.size(), 2u);

    std::remove(script_path);
    std::remove(svg_path);
}
