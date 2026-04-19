# DrawTool

A simple interactive drawing tool built with OpenGL and GLUT.

![C++](https://img.shields.io/badge/C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![OpenGL](https://img.shields.io/badge/OpenGL-5586A4?logo=opengl&logoColor=white)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

## About

DrawTool is a small OpenGL-based drawing application that lets you create shapes interactively with the mouse. All drawing options, colors, and rendering modes are accessible through a right-click context menu.

## Features

- **Shapes** -- draw lines, circles, rectangles, and triangles with click-and-drag (triangles use three-click placement)
- **Outline and filled variants** -- each shape supports wireframe (line) and solid (polygon) modes
- **Line styles** -- six stipple patterns with configurable repeat factors
- **Polygon modes** -- front/back face rendering as fill, line, or point
- **Shading** -- flat or smooth shading
- **Colors** -- seven preset colors: red, green, blue, yellow, purple, cyan, white
- **Canvas controls** -- clear the canvas or exit from the menu
- **Object picking** -- Ctrl+click selection using OpenGL selection buffer

> 鼠标绘制多种图形（直线、圆形、正方形、三角形等），支持多种线型、填充模式和光影模式，颜色可通过右键菜单选择，基本实现画板功能。

## File Layout

| File | Purpose |
|------|---------|
| `openGL5/main.cpp` | Entry point, GLUT setup, menu construction, window callbacks |
| `openGL5/mouse.h` | Mouse button and motion callbacks for drawing each shape |
| `openGL5/process.h` | Object rendering and OpenGL selection/picking logic |
| `openGL5/selection.h` | Shape, color, polygon mode, and line style menu handlers |

## Requirements

- C/C++ compiler (GCC, Clang, or MSVC)
- OpenGL + GLU + GLUT (or FreeGLUT)

## Build and Run

The program prompts for line width on startup via the terminal, then opens the drawing window.

### macOS

```bash
g++ openGL5/main.cpp -framework OpenGL -framework GLUT -o drawtool
./drawtool
```

> Note: macOS marks the GLUT framework as deprecated. The deprecation warnings are harmless and can be ignored.

### Linux

Install FreeGLUT first (`sudo apt install freeglut3-dev` on Debian/Ubuntu), then:

```bash
g++ openGL5/main.cpp -lGL -lGLU -lglut -o drawtool
./drawtool
```

### Windows (Visual Studio)

1. Open the project or create a new C++ console project and add the source files.
2. Link against `opengl32.lib`, `glu32.lib`, and `freeglut.lib`.
3. Build and run.

## Controls

| Input | Action |
|-------|--------|
| Right-click | Open context menu (shapes, colors, polygon modes, line styles, clear, exit) |
| Left-click + drag | Draw the selected shape |
| Left-click (triangle mode) | Place vertices one at a time; shape draws after the third click |
| Ctrl + left-click | Pick/select an object on the canvas |
| Terminal prompt at startup | Set the line width for subsequent drawing |

## License

[MIT](LICENSE)
