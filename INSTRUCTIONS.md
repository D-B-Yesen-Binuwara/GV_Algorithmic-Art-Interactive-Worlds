# Realistic Cityscape Generator - Build & Run Instructions

## Prerequisites
- MSYS2 installed with MinGW-w64 toolchain
- GLEW, GLFW, GLM libraries installed via MSYS2
- `C:\msys64\mingw64\bin` added to Windows PATH

## Build Instructions

### Option 1: Using Command Prompt/Terminal

1. **Open Command Prompt** in project directory:
   ```cmd
   cd D:\Yesen\Learning\Projects\C_programming\GV_Algorithmic-Art-Interactive-Worlds
   ```

2. **Compile using MSYS2 g++**:
   ```cmd
   set PATH=C:\msys64\mingw64\bin;%PATH%
   
   g++ -std=c++17 -IC:\msys64\mingw64\include src\main.cpp src\render\shader.cpp src\render\camera.cpp src\render\texture.cpp src\meshes\mesh.cpp src\algorithms\algorithms.cpp src\scene\city_scene.cpp src\stb_impl.cpp -o bin\city_designer.exe -LC:\msys64\mingw64\lib -lglfw3 -lglew32 -lopengl32 -lgdi32
   ```

3. **Run the program**:
   ```cmd
   bin\city_designer.exe
   ```

### Option 2: Using Makefile

1. **Open Command Prompt** in project directory
2. **Build**:
   ```cmd
   set PATH=C:\msys64\mingw64\bin;%PATH%
   make
   ```
3. **Run**:
   ```cmd
   bin\city_designer.exe
   ```

### Option 3: Using VS Code

1. **Open project in VS Code**
2. **Press `Ctrl+Shift+P`**
3. **Type**: "Tasks: Run Build Task"
4. **Select the build task**
5. **Run from terminal**: `bin\city_designer.exe`

## Texture Setup (Required)

The program needs texture files in the `assets` folder:

1. **Create/Download textures**:
   - `assets/building.jpg` - Brick/concrete texture (256x256 or 512x512 px)
   - `assets/road.jpg` - Asphalt/road texture (256x256 or 512x512 px)

2. **Alternative - Run without textures**:
   - Program will use solid colors if textures are missing
   - Check console output for texture loading status

## Controls

- **Right-click + drag**: Rotate camera around the city
- **Mouse scroll**: Zoom in/out
- **ESC**: Exit program

## User Input Process

When you run the program, you'll be prompted to:

1. **Enter city name** (e.g., "Neo Tokyo", "Cyber City")
2. **Choose city size**:
   - 1 = Small city (30x30 grid)
   - 2 = Medium city (50x50 grid) 
   - 3 = Large city (70x70 grid)
3. **Set number of buildings** (5-20 buildings)
4. **Select building style**:
   - 1 = All Skyscrapers (tall concrete buildings)
   - 2 = All Towers (medium brick buildings)
   - 3 = Mixed (random combination of all types)

The program will then procedurally generate your custom realistic cityscape!

### Camera Features:
- **Manual Control**: Full manual camera control with mouse
- **Smooth Rotation**: Right-click and drag to orbit around the city
- **Zoom Control**: Mouse scroll for precise distance adjustment

## Troubleshooting

### Program closes immediately:
```cmd
# Run with error output visible:
bin\city_designer.exe
pause
```

### Compilation errors:
- Ensure MSYS2 g++ is first in PATH
- Check all library installations:
  ```cmd
  pacman -S mingw-w64-x86_64-glew
  pacman -S mingw-w64-x86_64-glfw
  pacman -S mingw-w64-x86_64-glm
  ```

### Missing libraries:
```cmd
# Check if libraries are installed:
ls C:\msys64\mingw64\lib\libglfw*
ls C:\msys64\mingw64\lib\libglew*
ls C:\msys64\mingw64\include\glm\
```

## Project Structure

```
GV_Algorithmic-Art-Interactive-Worlds/
├── assets/                 # Texture files
│   ├── building.jpg       # Building texture
│   └── road.jpg          # Road texture
├── bin/                   # Compiled executable
│   └── city_designer.exe
├── src/                   # Source code
│   ├── algorithms/        # Bresenham & Midpoint Circle
│   ├── meshes/           # 3D mesh generation
│   ├── render/           # OpenGL rendering
│   ├── scene/            # City scene composition
│   ├── main.cpp          # Application entry point
│   └── stb_impl.cpp      # Image loading implementation
└── include/              # Header files
    └── stb_image.h       # Image loading library
```

## What the Program Does

**Realistic Cityscape Generator** allows users to design and visualize custom realistic cities with:

### User Input Features:
- **City Name**: Custom naming for your realistic city
- **City Size**: Small (30x30), Medium (50x50), or Large (70x70) grid
- **Building Count**: 5-20 procedurally placed buildings
- **Building Style**: Skyscrapers, Towers, or Mixed architecture

### Generated Elements:
- **Realistic Buildings**: Different architectural types with natural colors
  - Gray Concrete Skyscrapers (25-65 units tall)
  - Brown Brick Towers (12-30 units tall) 
  - Beige Stone Office Buildings (6-18 units tall)
- **Cross-Pattern Roads**: Asphalt roads using **Bresenham's line algorithm**
- **City Park**: Natural water pond using **Midpoint circle algorithm**
- **Street Lighting**: Realistic lamp posts with warm lighting
- **Natural Ground**: Green grass surface
- **Trees**: Procedurally placed trees with trunks and foliage
- **Interactive Camera**: Manual orbit controls
- **Natural Lighting**: Realistic daylight with blue sky

## Alternative Build Methods

### Using MSYS2 Terminal Directly:
1. Open MSYS2 MinGW 64-bit terminal
2. Navigate to project: `cd /d/Yesen/Learning/Projects/C_programming/GV_Algorithmic-Art-Interactive-Worlds`
3. Compile: `g++ -std=c++17 src/*.cpp src/*/*.cpp -o bin/city_designer.exe -lglfw3 -lglew32 -lopengl32 -lgdi32`
4. Run: `$ g++ -std=c++17 src/*.cpp src/*/*.cpp -o bin/city_designer.exe -lglfw3 -lglew32 -lopengl32 -lgdi32
src/scene/city_scene.cpp: In member function 'void CityScene::render(const Shader&, const Camera&)':
src/scene/city_scene.cpp:266:30: error: 'lampCell' was not declared in this scope
  266 |     auto lampW = cellToWorld(lampCell.first, lampCell.second);
      |                              ^~~~~~~~

mahima@MSI MINGW64 /d/GV_Algorithmic-Art-Interactive-Worlds_1/GV_Algorithmic-Art-Interactive-Worlds
$`

### Using CodeBlocks/Dev-C++:
1. Create new project
2. Add all .cpp files from src/ and subdirectories
3. Add include directories: `C:\msys64\mingw64\include`
4. Add library directories: `C:\msys64\mingw64\lib`
5. Link libraries: `glfw3`, `glew32`, `opengl32`, `gdi32`
6. Build and run