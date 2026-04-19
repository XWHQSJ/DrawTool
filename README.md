# DrawTool

A feature-rich interactive drawing tool built with OpenGL and GLUT.

![C++](https://img.shields.io/badge/C%2B%2B17-00599C?logo=cplusplus&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?logo=opengl&logoColor=white)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

## About

DrawTool is an OpenGL-based drawing application that lets you create, edit, and export shapes interactively. It features a persistent shape model with undo/redo, layer support, SVG and PNG export, keyboard shortcuts, and a HUD overlay.

## Features

### Drawing Tools

- **Line** -- click and drag
- **Circle** -- outline or filled, click and drag
- **Rectangle** -- outline or filled, click and drag
- **Triangle** -- outline or filled, 3-click vertex placement
- **Freehand Pen** -- continuous polyline from mouse motion
- **Arrow** -- line with arrowhead
- **Bezier Curve** -- 4-click control point placement
- **Text** -- click to place, type on keyboard, Enter to commit

### Editing

- **Selection tool** -- click a shape to select it (highlighted in white dashes)
- **Move** -- drag a selected shape, or use arrow keys (5px steps)
- **Delete** -- press Delete/Backspace to remove the selected shape
- **Undo / Redo** -- unlimited history via Ctrl+Z / Ctrl+Y
- **Clear All** -- right-click menu or programmatic reset

### Rendering

- **Line styles** -- solid, dashed, dotted stipple patterns
- **Line width** -- configurable 1-8 px from menu
- **Polygon modes** -- front/back face rendering as fill, line, or point
- **Shading** -- flat or smooth
- **Colors** -- seven preset colors via right-click menu
- **Layers** -- 4 layers (0-3); shapes render back-to-front by layer
- **Double buffering** -- flicker-free rendering

### Export / Import

- **Save PNG** -- Ctrl+S exports a timestamped PNG screenshot
- **Save SVG** -- Ctrl+E exports `drawing.svg` with shape metadata
- **Load SVG** -- Ctrl+O reloads `drawing.svg` (round-trip preserving types and colors)

### HUD

A translucent top bar shows the current tool, shape count, active layer, and a color swatch.

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `1` | Line tool |
| `2` | Circle outline |
| `3` | Circle fill |
| `4` | Rectangle outline |
| `5` | Rectangle fill |
| `6` | Triangle outline |
| `7` | Triangle fill |
| `8` | Freehand pen |
| `9` | Arrow |
| `0` | Select / Move tool |
| `Ctrl+Z` | Undo |
| `Ctrl+Y` | Redo |
| `Ctrl+S` | Save PNG |
| `Ctrl+E` | Save SVG |
| `Ctrl+O` | Load SVG |
| `Delete` / `Backspace` | Delete selected shape |
| Arrow keys | Move selected shape (5px) |
| `Escape` | Cancel text input |
| `Enter` | Commit text |
| Right-click | Context menu |

## File Layout

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Cross-platform build (macOS, Linux, Windows) |
| `openGL5/main.cpp` | Entry point, display, keyboard, menus |
| `openGL5/app.h` | `DrawingApp` singleton encapsulating all state |
| `openGL5/shape.h` | `Shape` struct, undo/redo stack |
| `openGL5/mouse.h` | Mouse button and motion callbacks |
| `openGL5/process.h` | Shape renderer (layer-sorted) |
| `openGL5/selection.h` | Menu callback handlers |
| `openGL5/hittest.h` | Geometry hit-testing (point-in-rect, point-on-line, etc.) |
| `openGL5/save.h` | SVG save and load |
| `openGL5/png_export.h` | PNG export via stb_image_write |
| `openGL5/stb_image_write.h` | Single-header PNG writer (vendored) |
| `tests/geometry_test.cpp` | GoogleTest unit tests (30 tests) |

## Build

### Prerequisites

- C++17 compiler
- CMake 3.14+
- OpenGL + GLU + GLUT (or FreeGLUT)

### Build and Run

```bash
cmake -B build
cmake --build build
./build/drawtool
```

The program prompts for line width on startup, then opens the drawing window.

### Run Tests

```bash
ctest --test-dir build --output-on-failure
```

### Platform Notes

- **macOS**: Uses `-framework OpenGL -framework GLUT`. Deprecated GLUT warnings are suppressed.
- **Linux**: Install FreeGLUT (`sudo apt install freeglut3-dev`).
- **Windows**: Link against `opengl32.lib`, `glu32.lib`, `freeglut.lib`.

## Architecture

The application uses a shape-based data model (`std::vector<Shape> g_shapes`). All tools push shapes into this vector on mouse-up. The display callback clears the screen, re-renders all shapes sorted by layer, draws a rubber-band preview for the active tool, and overlays the HUD. This replaced the original direct-to-framebuffer approach and enables undo/redo, selection, export, and layer sorting.

State is encapsulated in the `DrawingApp` singleton, accessed via `DrawingApp::instance()`. GLUT callbacks delegate to this instance.

### Dear ImGui

ImGui integration was evaluated for Tier 3. Given the legacy OpenGL + GLUT architecture, a full ImGui rewrite would require replacing the GLUT event loop or adding a compatibility shim. Instead, the HUD overlay provides the essential status display (tool, color, shape count, layer) natively. ImGui integration is a candidate for a future version using GLFW + OpenGL 3.3.

<!-- TODO: Dear ImGui integration via GLFW backend for a full toolbar replacement -->

## License

[MIT](LICENSE)
