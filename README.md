🐧 **randomOpenGLstuff**

Welcome to **randomOpenGLstuff**! 🎨✨
My playground for learning, experimenting, and having fun with OpenGL, GLFW, and all things graphics. 🚀

---

## What’s in here?

* **Modern OpenGL (3.3+)** with [GLFW](https://www.glfw.org/) and [glad](https://glad.dav1d.de/)
* Batch rendering, shaders, and more
* CMake-based build system for easy setup

> **Note:** I haven’t tested this on  Windows—macOS and linux only for now.

---

## Prerequisites

I’m assuming that you already have CMake, GCC/Clang, and the usual build tools installed

### Fedora (Linux)

```bash
sudo dnf install glfw-devel
```

### Ubuntu (Linux)

```bash
sudo apt update
sudo apt install build-essential cmake \
  libgl1-mesa-dev libglu1-mesa-dev \
  libx11-dev libxrandr-dev libxi-dev libxcursor-dev libxinerama-dev \
  libgtk-3-dev glfw3 glfw3-dev
```

---

## Tested Platforms

* **macOS** — Apple-Clang on VS Code
* **Fedora 41** — GCC 15.1.1 (Red Hat) on VS Code v1.100
* **Ubuntu 25.04** — GCC 14.2.0 on VS Code v1.100

---

## Getting Started

1. **Clone the repo**

   ```bash
   git clone https://github.com/danielqvu/randomOpenGLstuff.git
   cd randomOpenGLstuff/fastLearningOpengl
   ```

2. **Build with CMake**

   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. **Run the app**

   ```bash
   ./app
   ```

---

## Folder Structure

```
fastLearningOpengl/
├── src/           # C++ source code
├── res/           # Shaders, textures, and other resources
├── dependencies/  # GLFW, glad, etc.
├── CMakeLists.txt
└── README.md
```

---

## License

This project is MIT-licensed. See the [GLFW](https://www.glfw.org/license.html) and [glad](https://glad.dav1d.de/) sites for their licenses as well.
