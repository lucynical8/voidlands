# voidlands

voidlands is a work-in-progress voxel game built with c++ and opengl

## screenshots

| | |
|---|---|
| ![voidlands gameplay screenshot 1](screenshots/scr1.png) | ![voidlands gameplay screenshot 2](screenshots/scr2.png) |
| ![voidlands gameplay screenshot 3](screenshots/scr3.png) | ![voidlands gameplay screenshot 4](screenshots/scr4.png) |

## download

download a build from the [releases page](https://github.com/lucynical8/voidlands/releases), extract the archive and run the executable from inside its `build` directory so it can find the resources

on linux you may need to make the executable runnable first:

```bash
cd build
chmod +x voidlands
./voidlands
```

## building

you need a C++17 compiler, CMake, GLFW, GLM and OpenGL development libraries