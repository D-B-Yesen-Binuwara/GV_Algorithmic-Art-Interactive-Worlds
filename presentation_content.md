# Interactive 3D City Designer - Project Summary

## What We Created

A real-time 3D city visualization system that generates procedural cityscapes with:
- Buildings with different types (skyscrapers, towers, office buildings)
- Road networks using mathematical algorithms
- Water features (ponds) with circular geometry
- Environmental objects (trees, street lamps, cars)
- Interactive camera controls for exploration
- Realistic texturing and lighting

## Core Concepts Used

### 1. OpenGL Lines (Road Generation)

Concept: Instead of drawing actual lines, we create roads as sequences of 3D rectangular surfaces (quads) positioned at coordinates calculated by mathematical algorithms.

Process Flow:
Algorithm Coordinates → Grid Positions → World Coordinates → 3D Quads → Textured Roads

Why This Approach:
- OpenGL lines are thin and hard to see
- Quads can have textures (asphalt, road markings)
- Each road segment is a separate 3D object with width and texture
- Allows realistic road appearance with proper lighting

```cpp
// Step 1: Generate discrete road coordinates using Bresenham algorithm
auto road1 = bresenhamLine(5, center, gridSize-5, center);     // Horizontal road
auto road2 = bresenhamLine(center, 5, center, gridSize-5);     // Vertical road

// Step 2: Convert each coordinate to a 3D textured quad
for (auto &c : roadCells) {
    // Convert grid coordinate to world position
    auto w = cellToWorld(c.first, c.second);
    
    // Create transformation matrix for this road segment
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 0.02f, w.second));
    
    // Rotate texture for vertical roads to align road markings properly
    if (c.first == center) {
        m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    // Apply transformation and render the quad
    shader.setMat4("model", m);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, roadTex);  // Apply asphalt texture
    glBindVertexArray(quadMesh.vao);        // Bind quad geometry
    glDrawElements(GL_TRIANGLES, quadMesh.elemCount, GL_UNSIGNED_INT, 0);
}
```

Result: Realistic textured roads with proper width, surface details, and correct orientation.

### 2. OpenGL Transformations & Translations

Concept: Every 3D object starts as a unit cube (1x1x1) at origin (0,0,0). We use mathematical matrices to transform it to the correct position, size, and orientation in the 3D world.

Transformation Pipeline:
Unit Cube → Scale (resize) → Translate (move) → Rotate (orient) → World Position

Matrix Operations:
- Identity Matrix: No transformation (starting point)
- Translation Matrix: Moves object to world position
- Scale Matrix: Resizes object to desired dimensions  
- Rotation Matrix: Rotates object around axes

```cpp
// Building Transformation Example
float width = config.skyscraperWidth;   // User-defined width (e.g., 3.0)
float height = buildingHeights[i];      // Calculated height (e.g., 25.0)
float depth = width * 0.9f;             // Proportional depth (e.g., 2.7)

// Step 1: Start with identity matrix (no transformation)
glm::mat4 m = glm::mat4(1.0f);

// Step 2: Translate to world position (move building to grid location)
// height/2.0f centers the building vertically (bottom at ground level)
m = glm::translate(m, glm::vec3(w.first, height/2.0f, w.second));

// Step 3: Scale to desired size (transform unit cube to building dimensions)
m = glm::scale(m, glm::vec3(width, height, depth));

// Step 4: Send transformation matrix to GPU shader
shader.setMat4("model", m);

// Camera Orbit Transformation (spherical coordinates)
glm::vec3 dir;
dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));    // Horizontal rotation
dir.y = sin(glm::radians(pitch));                             // Vertical angle
dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));   // Depth rotation
glm::vec3 cameraPos = target - dir * distance;               // Calculate final camera position

// Tree Transformation Example (multiple objects)
glm::mat4 trunk = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 1.0f, w.second));
trunk = glm::scale(trunk, glm::vec3(0.3f, 2.0f, 0.3f));     // Thin, tall trunk

glm::mat4 leaves = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 2.5f, w.second));
leaves = glm::scale(leaves, glm::vec3(1.5f, 1.5f, 1.5f));   // Spherical foliage
```

Result: Unit cubes transformed into buildings of various sizes at correct world positions with proper camera viewing angles.

### 3. Bresenham Algorithm & Midpoint Circle

Concept: These are classic computer graphics algorithms that convert continuous mathematical shapes (lines and circles) into discrete pixel/grid coordinates suitable for digital displays and game worlds.

Why We Need These Algorithms:
- Computers work with discrete pixels, not continuous math
- Need to determine which grid cells to fill for smooth-looking shapes
- Must avoid floating-point calculations for speed and accuracy
- Essential for procedural generation in games and simulations

How Used in Our Project:

1. Bresenham Line Algorithm → Road Network Generation
   - Creates main city roads (horizontal and vertical)
   - Generates smooth, connected road segments
   - Ensures roads don't have gaps or overlaps

2. Midpoint Circle Algorithm → Pond/Water Feature Creation
   - Generates circular pond boundaries
   - Creates smooth circular shapes for water bodies
   - Used for any circular city features

When These Algorithms Execute:
- During city initialization (not every frame)
- Called once when user selects city parameters
- Results stored and reused for rendering

```cpp
// BRESENHAM LINE ALGORITHM - Road Generation
// Used in: src/scene/city_scene.cpp, Line 108-113
std::vector<std::pair<int,int>> bresenhamLine(int x0, int y0, int x1, int y1) {
    std::vector<std::pair<int,int>> pts;
    int dx = abs(x1 - x0);          // Total horizontal distance
    int sx = (x0 < x1) ? 1 : -1;    // Step direction: +1 right, -1 left
    int dy = -abs(y1 - y0);         // Total vertical distance (negative)
    int sy = (y0 < y1) ? 1 : -1;    // Step direction: +1 up, -1 down
    int err = dx + dy;              // Error accumulator
    
    while (true) {
        pts.emplace_back(x,y);      // Add current grid cell to road
        if (x==x1 && y==y1) break; // Reached destination
        
        int e2 = 2*err;             // Decision variable
        if (e2 >= dy) { err += dy; x += sx; }  // Move horizontally
        if (e2 <= dx) { err += dx; y += sy; }  // Move vertically
    }
    return pts;  // Return list of grid coordinates for road
}

// Project Usage Example:
int center = gridSize / 2;
auto road1 = bresenhamLine(5, center, gridSize-5, center);      // Horizontal main road
auto road2 = bresenhamLine(center, 5, center, gridSize-5);      // Vertical main road
// Result: Two intersecting roads forming city center cross-pattern

// MIDPOINT CIRCLE ALGORITHM - Pond Generation  
// Used in: src/scene/city_scene.cpp, Line 52-54
std::vector<std::pair<int,int>> midpointCircle(int cx, int cy, int r) {
    std::vector<std::pair<int,int>> pts;
    int x = r, y = 0;               // Start at rightmost point of circle
    int dx = 1 - (r << 1);          // Decision parameter for x-direction
    int dy = 0, err = 0;            // Decision parameter for y-direction
    
    while (x >= y) {                // Only calculate one octant (1/8 of circle)
        // Generate 8 symmetric points for complete circle
        pts.emplace_back(cx + x, cy + y);  // Octant 1 (0° to 45°)
        pts.emplace_back(cx + y, cy + x);  // Octant 2 (45° to 90°)
        pts.emplace_back(cx - y, cy + x);  // Octant 3 (90° to 135°)
        pts.emplace_back(cx - x, cy + y);  // Octant 4 (135° to 180°)
        pts.emplace_back(cx - x, cy - y);  // Octant 5 (180° to 225°)
        pts.emplace_back(cx - y, cy - x);  // Octant 6 (225° to 270°)
        pts.emplace_back(cx + y, cy - x);  // Octant 7 (270° to 315°)
        pts.emplace_back(cx + x, cy - y);  // Octant 8 (315° to 360°)
        
        // Decision logic for next point
        y++; err += dy; dy += 2;
        if ((err << 1) + dx > 0) {
            x--; err += dx; dx += 2;
        }
    }
    return pts;  // Return complete circle boundary coordinates
}

// Project Usage Example:
pond_cx = center + 8; pond_cy = center - 8; pond_r = (int)config.pondRadius;
auto pondPoints = midpointCircle(pond_cx, pond_cy, pond_r);
// Result: Circular pond positioned offset from city center

// 3D Mesh Creation from Algorithm Output:
auto pondWorld = cellToWorld(pond_cx, pond_cy);
pondMesh = makeCircleFan(pondWorld.first, pondWorld.second, (float)pond_r, 64);
// Result: 3D circular mesh for realistic water rendering
```

Algorithm Benefits:
- Bresenham: Perfect straight roads without gaps, integer-only math
- Midpoint Circle: Smooth circular ponds, 8x efficiency through symmetry
- Both: Fast execution, predictable results, no floating-point errors

Result: Mathematically precise city layouts with smooth roads and circular water features.

### 4. Collision Detection

Concept: A spatial management system that prevents objects from overlapping by tracking which areas of the city are already occupied.

Why We Need This:
- Prevents buildings from spawning inside each other
- Ensures realistic spacing between objects
- Maintains visual coherence in city layout
- Allows different object sizes (buildings need more space than trees)

How It Works in Our Project:
1. Create 2D boolean grid representing city space
2. Each grid cell can be true (occupied) or false (free)
3. Before placing object, check if area is free
4. After placing object, mark area as occupied
5. Different objects reserve different sized areas

When Used:
- During city generation (not during rendering)
- Called for every building, tree, lamppost placement
- Prevents overlapping during random object placement

Object Size Reservations:
- Buildings: 5x5 grid area (large footprint)
- Trees: 3x3 grid area (medium spacing)
- Lampposts: 1x1 grid area (small footprint)
- Roads: Linear path marking
- Pond: Circular area marking

```cpp
// GRID-BASED COLLISION SYSTEM
// Used in: src/scene/city_scene.cpp, Line 35-40

// Step 1: Initialize 2D boolean grid for entire city
occupiedGrid = std::vector<std::vector<bool>>(gridSize, std::vector<bool>(gridSize, false));
// Example: 50x50 grid = 2500 cells for small city

// Step 2: Check if area is free before placing objects
// Used in: src/scene/city_scene.cpp, Line 86-95
bool CityScene::isGridCellFree(int x, int z, int size) const {
    // Check rectangular area around position
    for (int i = x - size/2; i <= x + size/2; i++) {
        for (int j = z - size/2; j <= z + size/2; j++) {
            // Check bounds and occupancy
            if (i < 0 || i >= gridSize || j < 0 || j >= gridSize || occupiedGrid[i][j]) {
                return false;  // Area is occupied or out of bounds
            }
        }
    }
    return true;  // Area is completely free
}

// Step 3: Mark area as occupied after placing object
// Used in: src/scene/city_scene.cpp, Line 96-105
void CityScene::markGridCells(int x, int z, int size) {
    // Mark rectangular area as occupied
    for (int i = x - size/2; i <= x + size/2; i++) {
        for (int j = z - size/2; j <= z + size/2; j++) {
            if (i >= 0 && i < gridSize && j >= 0 && j < gridSize) {
                occupiedGrid[i][j] = true;  // Mark cell as occupied
            }
        }
    }
}

// Project Usage Example - Building Placement:
// Used in: src/scene/city_scene.cpp, Line 150-180
for (int i = 0; i < config.numBuildings; i++) {
    int x, z;
    bool validPos = false;
    
    // Try up to 100 times to find valid position
    for (int attempts = 0; attempts < 100 && !validPos; attempts++) {
        x = 5 + rand() % (gridSize - 10);  // Random position with border
        z = 5 + rand() % (gridSize - 10);
        
        if (isGridCellFree(x, z, 5)) {     // Check 5x5 area for building
            validPos = true;
        }
    }
    
    if (validPos) {
        buildingCells.push_back({x, z});   // Store building position
        markGridCells(x, z, 5);           // Reserve 5x5 area
    }
}

// Special Case - Circular Collision for Pond:
// Used in: src/scene/city_scene.cpp, Line 135-145
for (int i = pond_cx - pond_r; i <= pond_cx + pond_r; i++) {
    for (int j = pond_cy - pond_r; j <= pond_cy + pond_r; j++) {
        int dx = i - pond_cx;
        int dy = j - pond_cy;
        if (dx*dx + dy*dy <= pond_r*pond_r) {  // Inside circle
            occupiedGrid[i][j] = true;         // Mark as occupied
        }
    }
}
```

Collision Detection Benefits:
- Realistic city layouts without overlapping
- Efficient O(1) lookup for any grid position
- Supports different object sizes and shapes
- Prevents visual artifacts and unrealistic placement

Result: Well-spaced city objects with natural, realistic positioning.

### 5. 3D Modeling with Texture Mapping

Concept: The complete process of taking 2D images and applying them as surface details on 3D geometric shapes to create realistic-looking objects.

Why Texture Mapping:
- Makes simple 3D shapes look detailed and realistic
- More efficient than modeling complex geometry
- Allows different building types with same base mesh
- Provides visual variety without performance cost

Texture Pipeline in Our Project:
1. Load 2D images from files (building.jpg, road.jpg, etc.)
2. Convert images to GPU textures
3. Create 3D meshes with UV coordinates (texture mapping coordinates)
4. During rendering, apply textures to mesh surfaces
5. GPU interpolates texture across triangle surfaces

When Used:
- Texture loading: Once during initialization
- Texture application: Every frame during rendering
- Different textures for different building types
- Road textures with proper orientation

Texture Types in Project:
- skyscraperTex: Glass/concrete building texture
- towerTex: Brick building texture
- buildingTex: Office building texture
- roadTex: Asphalt with road markings
- pondTex: Water surface texture

```cpp
// TEXTURE LOADING PIPELINE
// Used in: src/render/texture.cpp, Line 5-24

// Step 1: Load image file and convert to GPU texture
GLuint loadTexture(const std::string& path) {
    int w, h, comp;                                    // Width, height, components
    stbi_set_flip_vertically_on_load(true);           // Flip for OpenGL coordinate system
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &comp, 3); // Load RGB data
    
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return 0;  // Return 0 for failed texture
    }
    
    // Step 2: Create GPU texture object
    GLuint tex;
    glGenTextures(1, &tex);                           // Generate texture ID
    glBindTexture(GL_TEXTURE_2D, tex);                // Bind for operations
    
    // Step 3: Upload image data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);                  // Generate smaller versions for distance
    
    // Step 4: Set texture parameters for quality
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // Repeat horizontally
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);    // Repeat vertically
    
    stbi_image_free(data);                            // Free CPU memory
    return tex;                                       // Return GPU texture ID
}

// 3D MESH WITH UV COORDINATES
// Used in: src/meshes/mesh.cpp, Line 30-50

// Step 1: Create mesh vertices with texture coordinates
Mesh makeCube() {
    float V[] = {
        // Position (x,y,z)    Normal (x,y,z)   UV (u,v) - Texture Coordinates
        -0.5f,-0.5f,0.5f,      0,0,1,           0,0,    // Bottom-left corner
         0.5f,-0.5f,0.5f,      0,0,1,           1,0,    // Bottom-right corner
         0.5f, 0.5f,0.5f,      0,0,1,           1,1,    // Top-right corner
        -0.5f, 0.5f,0.5f,      0,0,1,           0,1,    // Top-left corner
        // ... 20 more vertices for complete cube (6 faces)
    };
    
    // Step 2: Upload to GPU with proper vertex attributes
    // Position: attribute 0, Normal: attribute 1, UV: attribute 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);  // Enable UV coordinates
}

// TEXTURE APPLICATION DURING RENDERING
// Used in: src/scene/city_scene.cpp, Line 340-370

// Step 1: Select appropriate texture based on building type
GLuint currentTex = 0;
if (type == 0) currentTex = skyscraperTex;      // Glass/concrete for skyscrapers
else if (type == 1) currentTex = towerTex;     // Brick texture for towers
else currentTex = buildingTex;                 // Office building texture

// Step 2: Bind texture and configure shader
glActiveTexture(GL_TEXTURE0);                  // Use texture unit 0
glBindTexture(GL_TEXTURE_2D, currentTex);      // Bind selected texture
shader.setFloat("useTexture", currentTex ? 1.0f : 0.0f); // Tell shader to use texture
shader.setInt("tex", 0);                       // Set texture sampler to unit 0
shader.setVec3("baseColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White to show texture colors

// Step 3: Render textured building
glBindVertexArray(cubeMesh.vao);               // Bind mesh geometry
glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); // Draw with texture

// SHADER TEXTURE PROCESSING
// Used in: src/main.cpp, Line 35-45 (Fragment Shader)
// GPU automatically interpolates UV coordinates across triangle surfaces
uniform sampler2D tex;        // Texture sampler
uniform float useTexture;     // Whether to use texture or solid color
uniform vec3 baseColor;       // Base color if no texture

void main() {
    vec3 color = baseColor;
    if (useTexture > 0.5) {
        color = texture(tex, vUV).rgb;  // Sample texture at UV coordinate
    }
    // Apply lighting calculations to final color
    FragColor = vec4(result, 1.0);
}

// Project Usage Example - Road Texture Rotation:
// Used in: src/scene/city_scene.cpp, Line 295-310
for (auto &c : roadCells) {
    glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 0.02f, w.second));
    
    // Rotate texture for vertical roads to align road markings properly
    if (c.first == center) { // Vertical road
        m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    glBindTexture(GL_TEXTURE_2D, roadTex);     // Apply asphalt texture
    shader.setFloat("useTexture", roadTex ? 1.0f : 0.0f);
}
```

Texture Mapping Benefits:
- Realistic surface details without complex geometry
- Different building appearances using same base mesh
- Efficient GPU processing with hardware acceleration
- Visual variety and realism in city appearance

Result: Detailed, realistic-looking 3D city with varied building textures, proper road markings, and convincing surface materials.

## Development Process

1. Mathematical Foundation: Implemented Bresenham and Midpoint Circle algorithms for coordinate generation
2. 3D Graphics Pipeline: Created mesh generation system with proper vertex attributes
3. Spatial Management: Developed grid-based collision detection for realistic object placement
4. Rendering System: Built modular rendering pipeline with shader-based lighting and texturing
5. User Interaction: Added camera controls and customizable city parameters
6. Optimization: Used efficient algorithms and GPU-based rendering for real-time performance

## Technical Achievement

The project demonstrates integration of:
- Computer graphics algorithms (Bresenham, Midpoint Circle)
- 3D mathematics (matrix transformations, coordinate systems)
- OpenGL rendering pipeline (shaders, textures, meshes)
- Spatial data structures (grid-based collision detection)
- Real-time interactive systems (camera controls, dynamic rendering)

Result: A fully functional 3D city generator with procedural layout, realistic rendering, and interactive exploration capabilities.istic rendering, and interactive exploration capabilities.