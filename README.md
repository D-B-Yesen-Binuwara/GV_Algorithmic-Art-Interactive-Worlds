## 🏙️ Overview

**Interactive 3D City Designer** is a modular OpenGL project built using **C++**, **GLFW**, **GLEW**, and **GLM**.
The system allows users to visualize a small 3D city layout that includes:

* **Two buildings**
* **A road generated using Bresenham’s algorithm**
* **A pond generated using the Midpoint Circle algorithm**
* **A lamppost**
* **Ground plane**
* **Camera orbit controls**

The entire codebase is split into modular components so that **three team members** can work independently without conflicts.

---

## ✨ Features

* Fully modular multi-file C++ project
* Uses **Bresenham Line algorithm** for generating road coordinates
* Uses **Midpoint Circle algorithm** for generating the pond boundary
* GPU-ready mesh generation (cube, quad, circle fan)
* Basic lighting (diffuse, ambient, specular)
* Camera orbit with mouse drag and scroll zoom
* Texture support for buildings and road (via stb_image)
* Clean rendering pipeline with shader abstraction

---

## 🧩 System Requirements

* C++11 or newer
* OpenGL 3.3+
* GLEW
* GLFW
* GLM
* stb_image.h

---

## 🚀 Build Instructions (Linux / MinGW / WSL / CodeBlocks / VS)

### **Using Makefile (Linux / WSL)**

```
make
./bin/city_designer
```

### **On Windows (CodeBlocks / Visual Studio)**

* Include:

  * `GLEW`
  * `GLFW`
  * `GLM`
  * `stb_image.h`
* Link:

  * `opengl32`
  * `glew32`
  * `glfw3`

---

## 🎮 Controls

| Action       | Control                 |
| ------------ | ----------------------- |
| Orbit camera | **Right-click + drag**  |
| Zoom         | **Mouse scroll**        |
| Quit         | **ESC or close window** |

---

## 📁 Project Folder Structure

```
GV_FINAL/
├─ include/
│  └─ stb_image.h
├─ assets/
│  ├─ building.jpg
│  ├─ tower.png
│  ├─ skyscraper.jpg
│  ├─ road.jpg
│  └─ pond.jpg
├─ src/
│  ├─ algorithms/
│  │  ├─ algorithms.h
│  │  └─ algorithms.cpp
│  ├─ meshes/
│  │  ├─ mesh.h
│  │  └─ mesh.cpp
│  ├─ render/
│  │  ├─ shader.h
│  │  ├─ shader.cpp
│  │  ├─ texture.h
│  │  ├─ texture.cpp
│  │  ├─ camera.h
│  │  └─ camera.cpp
│  ├─ scene/
│  │  ├─ city_scene.h
│  │  └─ city_scene.cpp
│  ├─ main.cpp
│  └─ stb_impl.cpp
├─ bin/
│  └─ city_designer.exe
├─ guideline.txt
└─ README.md
```

---

# 👥 **Team Work Distribution (3 Members)**

## 🔹 **Member 1 — Algorithms & Mesh Generation**

Responsibilities:

* Implement procedural algorithms for geometry placement
* Create reusable GPU-ready mesh builders

### Files Owned:

#### **Algorithms**

* `src/algorithms/algorithms.h`
* `src/algorithms/algorithms.cpp`

**Duties:**

* Implement **Bresenham Line Algorithm** (for the road path)
* Implement **Midpoint Circle Algorithm** (for pond boundary)
* Provide clean function interfaces returning coordinate lists

#### **Meshes**

* `src/meshes/mesh.h`
* `src/meshes/mesh.cpp`

**Duties:**

* Build general-purpose meshes:

  * **Cube**
  * **Ground quad**
  * **Circle fan**
* Generate VAO/VBO/EBO for OpenGL
* Provide simple, reusable `Mesh` struct

---

## 🔹 **Member 2 — Rendering Engine (Shaders, Camera, Textures)**

Responsibilities:

* All rendering-related utilities
* Shader abstraction
* Texture loader
* Camera system

### Files Owned:

#### **Shaders**

* `src/render/shader.h`
* `src/render/shader.cpp`

Duties:

* Build shader loader & compiler
* Provide uniform setter functions (mat4, vec3, float, int)

#### **Textures**

* `src/render/texture.h`
* `src/render/texture.cpp`

Duties:

* Implement `loadTexture()` using stb_image
* Handle mipmaps & filtering

#### **Camera**

* `src/render/camera.h`
* `src/render/camera.cpp`

Duties:

* Orbit camera behavior (yaw, pitch, zoom)
* Provide view matrix

---

## 🔹 **Member 3 — Scene Builder & Application Entry**

Responsibilities:

* Compose the final city layout
* Use algorithms + meshes + renderer
* Integrate all modules
* Implement main application loop

### Files Owned:

#### **Scene**

* `src/scene/city_scene.h`
* `src/scene/city_scene.cpp`

Duties:

* Place city objects:

  * 2 buildings
  * Road (from Bresenham output)
  * Pond (from circle output)
  * Lamppost
* Call mesh builders and shader functions
* Perform object transformations (scale, translate)

#### **Main application**

* `src/main.cpp`

Duties:

* Window creation
* OpenGL context setup
* Input callbacks (mouse & scroll)
* Instantiate `CityScene`, `Camera`, and `Shader`
* Game/render loop

---

# 🧪 How the Algorithms Power the Scene

### ✔ Bresenham Line → Road cells

Bresenham provides integer grid coordinates → each converted to a textured quad.

### ✔ Midpoint Circle → Pond boundary

Circle points → create a circle-fan mesh centered at world coordinates.

---

# 🖼️ Screenshots (Add After Running)

```
assets/screenshots/
    city_view.png
    buildings.png
    pond.png
```

*(Include these in submission.)*

---

# 🛠️ Possible Extensions

* Add GUI (ImGui) for real-time editing
* Add more building types
* Add vehicle or pedestrian animations
* Add day/night lighting
* Add reflections to the pond

---

# ✔ Summary

This README provides a fully documented overview of the Interactive 3D City Designer project, the complete module breakdown, team roles, and compilation steps.
It is suitable for **academic submission, GitHub repositories, or group assignment documentation**.
