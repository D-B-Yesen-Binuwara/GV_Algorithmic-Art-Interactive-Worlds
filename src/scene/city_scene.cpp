#include "city_scene.h"
#include "../algorithms/algorithms.h"
#include "../meshes/mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <GL/glew.h>

CityScene::~CityScene() {
    // cleanup VAOs/buffers in Mesh destructor not implemented; free OpenGL objects
    if (cubeMesh.vao) { glDeleteVertexArrays(1, &cubeMesh.vao); glDeleteBuffers(1, &cubeMesh.vbo); glDeleteBuffers(1, &cubeMesh.ebo); }
    if (quadMesh.vao) { glDeleteVertexArrays(1, &quadMesh.vao); glDeleteBuffers(1, &quadMesh.vbo); glDeleteBuffers(1, &quadMesh.ebo); }
    if (pondMesh.vao) { glDeleteVertexArrays(1, &pondMesh.vao); glDeleteBuffers(1, &pondMesh.vbo); glDeleteBuffers(1, &pondMesh.ebo); }
    if (buildingTex) glDeleteTextures(1, &buildingTex);
    if (towerTex) glDeleteTextures(1, &towerTex);
    if (skyscraperTex) glDeleteTextures(1, &skyscraperTex);
    if (roadTex) glDeleteTextures(1, &roadTex);
    if (pondTex) glDeleteTextures(1, &pondTex);

}

bool CityScene::init(int citySize, int numBuildings, int buildingStyle, float towerW, float towerH, float buildingW, float buildingH, float skyW, float skyH, float pondR, int numSky, int numTow, int numOff) {
    config.citySize = citySize;
    config.numBuildings = numBuildings;
    config.numSkyscrapers = numSky;
    config.numTowers = numTow;
    config.numOfficeBuildings = numOff;
    config.buildingStyle = buildingStyle;
    config.towerWidth = towerW;
    config.towerHeight = towerH;
    config.buildingWidth = buildingW;
    config.buildingHeight = buildingH;
    config.skyscraperWidth = skyW;
    config.skyscraperHeight = skyH;
    config.pondRadius = pondR;
    
    // Set grid size based on city size
    gridSize = (citySize == 1) ? 50 : (citySize == 2) ? 70 : 100;
    
    std::cout << "Grid size: " << gridSize << std::endl;
    
    // Initialize occupancy grid
    occupiedGrid = std::vector<std::vector<bool>>(gridSize, std::vector<bool>(gridSize, false));
    std::cout << "Occupancy grid initialized\n";
    
    // meshes
    std::cout << "Creating meshes...\n";
    cubeMesh = makeCube();
    quadMesh = makeQuadXZ();
    std::cout << "Meshes created\n";
    
    std::cout << "Generating layout...\n";
    generateFuturisticLayout();
    std::cout << "Layout generated\n";
    
    // pond mesh (world coords)
    std::cout << "Creating pond mesh...\n";
    auto pondWorld = cellToWorld(pond_cx, pond_cy);
    pondMesh = makeCircleFan(pondWorld.first, pondWorld.second, (float)pond_r, 64);
    std::cout << "Pond mesh created\n";
    
    // Initialize water animation
    waterTime = 0.0f;
    ripplePhase = 0.0f;
    
    // Load realistic textures
    std::cout << "Loading textures...\n";
    buildingTex = loadTexture("assets/building.jpg");
    std::cout << "Building texture loaded\n";
    towerTex = loadTexture("assets/tower.png");
    std::cout << "Tower texture loaded\n";
    std::cout << "Loading skyscraper texture...\n";
    skyscraperTex = loadTexture("assets/skyscraper.jpg");
    if (skyscraperTex == 0) {
        std::cout << "Skyscraper texture failed, using building texture instead\n";
        skyscraperTex = buildingTex;
    }
    std::cout << "Skyscraper texture loaded\n";
    roadTex = loadTexture("assets/road.jpg");
    std::cout << "Road texture loaded\n";
    pondTex = loadTexture("assets/pond.jpg");
    std::cout << "Pond texture loaded\n";

    if (!buildingTex) std::cout << "Building texture not found - using realistic colors\n";
    if (!towerTex) std::cout << "Tower texture not found - using realistic colors\n";
    if (!skyscraperTex) std::cout << "Skyscraper texture not found - using realistic colors\n";
    if (!roadTex) std::cout << "Road texture not found - using realistic colors\n";
    if (!pondTex) std::cout << "Pond texture not found - using realistic colors\n";

    std::cout << "Realistic city generated with " << numBuildings << " buildings\n";
    return true;
}

std::pair<float,float> CityScene::cellToWorld(int i, int j) const {
    float startX = -(float)gridSize/2.0f;
    float startZ = -(float)gridSize/2.0f;
    float x = startX + (float)i;
    float z = startZ + (float)j;
    return {x,z};
}

bool CityScene::isGridCellFree(int x, int z, int size) const {
    for (int i = x - size/2; i <= x + size/2; i++) {
        for (int j = z - size/2; j <= z + size/2; j++) {
            if (i < 0 || i >= gridSize || j < 0 || j >= gridSize || occupiedGrid[i][j]) {
                return false;
            }
        }
    }
    return true;
}

void CityScene::markGridCells(int x, int z, int size) {
    for (int i = x - size/2; i <= x + size/2; i++) {
        for (int j = z - size/2; j <= z + size/2; j++) {
            if (i >= 0 && i < gridSize && j >= 0 && j < gridSize) {
                occupiedGrid[i][j] = true;
            }
        }
    }
}

void CityScene::generateFuturisticLayout() {
    // Generate main roads in cross pattern
    int center = gridSize / 2;
    
    // Horizontal road
    auto road1 = bresenhamLine(5, center, gridSize-5, center);
    // Vertical road  
    auto road2 = bresenhamLine(center, 5, center, gridSize-5);
    
    roadCells.insert(roadCells.end(), road1.begin(), road1.end());
    roadCells.insert(roadCells.end(), road2.begin(), road2.end());
    
    // Mark road cells as occupied
    for (auto& cell : roadCells) {
        markGridCells(cell.first, cell.second);
    }
    
    // Place buildings randomly in quadrants
    placeBuildingsRandomly();
    
    // Add trees for realism
    placeTrees();
    
    // Place street lamps along roads
    placeStreetLamps();
    
    // Place a car on the road
    placeRandomCar();
    
    // City park pond with custom radius
    pond_cx = center + 8; pond_cy = center - 8; pond_r = (int)config.pondRadius;
    
    // Mark pond area as occupied (simplified)
    std::cout << "Marking pond area at (" << pond_cx << ", " << pond_cy << ") radius " << pond_r << std::endl;
    for (int i = std::max(0, pond_cx - pond_r); i <= std::min(gridSize-1, pond_cx + pond_r); i++) {
        for (int j = std::max(0, pond_cy - pond_r); j <= std::min(gridSize-1, pond_cy + pond_r); j++) {
            int dx = i - pond_cx;
            int dy = j - pond_cy;
            if (dx*dx + dy*dy <= pond_r*pond_r) {
                if (i >= 0 && i < gridSize && j >= 0 && j < gridSize) {
                    occupiedGrid[i][j] = true;
                }
            }
        }
    }
    std::cout << "Pond area marked\n";
}

void CityScene::placeBuildingsRandomly() {
    buildingCells.clear();
    buildingHeights.clear();
    buildingTypes.clear();
    
    srand(12345); // Fixed seed to prevent shivering
    int center = gridSize / 2;
    
    for (int i = 0; i < config.numBuildings; i++) {
        int x, z;
        bool validPos = false;
        
        // Find valid position using grid system
        for (int attempts = 0; attempts < 100 && !validPos; attempts++) {
            x = 5 + rand() % (gridSize - 10);
            z = 5 + rand() % (gridSize - 10);
            
            // Check 5x5 area for building (larger buffer)
            if (isGridCellFree(x, z, 5)) {
                validPos = true;
            }
        }
        
        if (validPos) {
            buildingCells.push_back({x, z});
            markGridCells(x, z, 5); // Mark 5x5 area
            
            // Determine building type based on style and counts
            int type;
            if (config.buildingStyle == 1) {
                type = 0; // All skyscrapers
            } else if (config.buildingStyle == 2) {
                type = 1; // All towers
            } else {
                // Mixed - use specific counts
                int skyscrapersPlaced = 0, towersPlaced = 0, buildingsPlaced = 0;
                for (int j = 0; j < (int)buildingTypes.size(); j++) {
                    if (buildingTypes[j] == 0) skyscrapersPlaced++;
                    else if (buildingTypes[j] == 1) towersPlaced++;
                    else buildingsPlaced++;
                }
                
                if (skyscrapersPlaced < config.numSkyscrapers) type = 0;
                else if (towersPlaced < config.numTowers) type = 1;
                else type = 2;
            }
            
            buildingTypes.push_back(type);
            buildingHeights.push_back(getFuturisticHeight(type, i));
        }
    }
}

float CityScene::getFuturisticHeight(int type, int index) {
    switch (type) {
        case 0: return config.skyscraperHeight; // Custom skyscraper height
        case 1: return config.towerHeight;   // Custom tower height
        case 2: return config.buildingHeight * 0.7f; // Smaller buildings
        default: return 15.0f;
    }
}

void CityScene::placeTrees() {
    treeCells.clear();
    srand(54321); // Fixed seed for trees to prevent shivering
    
    int numTrees = config.citySize * 8; // More trees for larger cities
    int center = gridSize / 2;
    
    for (int i = 0; i < numTrees; i++) {
        int x, z;
        bool validPos = false;
        
        for (int attempts = 0; attempts < 50 && !validPos; attempts++) {
            x = 3 + rand() % (gridSize - 6);
            z = 3 + rand() % (gridSize - 6);
            
            // Check 3x3 area for trees
            if (isGridCellFree(x, z, 3)) {
                validPos = true;
            }
        }
        
        if (validPos) {
            treeCells.push_back({x, z});
            markGridCells(x, z, 3);
        }
    }
}

void CityScene::placeStreetLamps() {
    streetLamps.clear();
    int center = gridSize / 2;
    
    // Place lamps along horizontal road (avoid intersections)
    for (int i = 8; i < gridSize - 8; i += 6) {
        if (abs(i - center) > 3) {
            if (center + 2 < gridSize && isGridCellFree(i, center + 2)) {
                streetLamps.push_back({i, center + 2});
                markGridCells(i, center + 2);
            }
            if (center - 2 >= 0 && isGridCellFree(i, center - 2)) {
                streetLamps.push_back({i, center - 2});
                markGridCells(i, center - 2);
            }
        }
    }
    
    // Place lamps along vertical road (avoid intersections)
    for (int j = 8; j < gridSize - 8; j += 6) {
        if (abs(j - center) > 3) {
            if (center + 2 < gridSize && isGridCellFree(center + 2, j)) {
                streetLamps.push_back({center + 2, j});
                markGridCells(center + 2, j);
            }
            if (center - 2 >= 0 && isGridCellFree(center - 2, j)) {
                streetLamps.push_back({center - 2, j});
                markGridCells(center - 2, j);
            }
        }
    }
}

void CityScene::placeRandomCar() {
    int center = gridSize / 2;
    // Place car on horizontal road
    carPosition = {center - 5, center};
}

glm::vec3 CityScene::getFuturisticColor(int type) {
    switch (type) {
        case 0: return glm::vec3(0.7f, 0.7f, 0.8f); // Light gray concrete
        case 1: return glm::vec3(0.6f, 0.5f, 0.4f); // Brown brick
        case 2: return glm::vec3(0.8f, 0.8f, 0.7f); // Beige stone
        default: return glm::vec3(0.6f, 0.6f, 0.6f);
    }
}

void CityScene::update(float dt) {
    // Animate water effects
    waterTime += dt * 2.0f; // Water animation speed
    ripplePhase += dt * 3.0f; // Ripple animation speed
}

void CityScene::render(const Shader& shader, const Camera& cam) {
    shader.use();
    shader.setMat4("view", cam.viewMatrix());
    
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    float aspect = (float)viewport[2] / (float)viewport[3];
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 200.0f);
    shader.setMat4("proj", proj);
    shader.setVec3("lightPos", glm::vec3(0.0f, 8.0f, 0.0f)); // Lower night lighting
    shader.setVec3("viewPos", cam.position());


    // dark night ground
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.501f, 0.0f));
    model = glm::scale(model, glm::vec3((float)gridSize, 1.0f, (float)gridSize));
    shader.setMat4("model", model);
    shader.setVec3("baseColor", glm::vec3(0.1f, 0.15f, 0.1f)); // Dark grass at night
    shader.setFloat("useTexture", 0.0f);
    glBindVertexArray(cubeMesh.vao);
    glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // realistic asphalt roads
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, roadTex);
    int center = gridSize / 2;
    for (auto &c : roadCells) {
        auto w = cellToWorld(c.first, c.second);
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 0.02f, w.second));
        
        // Rotate texture for vertical roads to align dotted lines
        if (c.first == center) { // Vertical road
            m = glm::rotate(m, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        
        m = glm::scale(m, glm::vec3(1.0f,1.0f,1.0f));
        shader.setMat4("model", m);
        shader.setVec3("baseColor", glm::vec3(0.2f, 0.2f, 0.2f)); // Dark asphalt
        shader.setFloat("useTexture", roadTex ? 1.0f : 0.0f);
        shader.setInt("tex", 0);
        glBindVertexArray(quadMesh.vao);
        glDrawElements(GL_TRIANGLES, quadMesh.elemCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Water pond with texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pondTex);
    glm::mat4 pondModel = glm::mat4(1.0f);
    shader.setMat4("model", pondModel);
    shader.setVec3("baseColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setFloat("useTexture", pondTex ? 1.0f : 0.0f);
    shader.setInt("tex", 0);
    glBindVertexArray(pondMesh.vao);
    glDrawElements(GL_TRIANGLES, pondMesh.elemCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    


    // realistic buildings with different textures per type
    glActiveTexture(GL_TEXTURE0);
    for (size_t i = 0; i < buildingCells.size(); i++) {
        auto &bcell = buildingCells[i];
        auto w = cellToWorld(bcell.first, bcell.second);
        float height = buildingHeights[i];
        int type = buildingTypes[i];
        
        // Bind appropriate texture based on building type
        GLuint currentTex = 0;
        if (type == 0) currentTex = skyscraperTex; // Skyscrapers
        else if (type == 1) currentTex = towerTex; // Towers
        else currentTex = buildingTex; // Office buildings
        
        glBindTexture(GL_TEXTURE_2D, currentTex);
        
        // Custom building proportions from user input
        float width = (type == 0) ? config.skyscraperWidth : (type == 1) ? config.towerWidth : config.buildingWidth;
        float depth = width * 0.9f; // Fixed ratio
        
        glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, height/2.0f, w.second));
        m = glm::scale(m, glm::vec3(width, height, depth));
        shader.setMat4("model", m);
        shader.setVec3("baseColor", glm::vec3(1.0f, 1.0f, 1.0f)); // White to show texture colors
        shader.setFloat("useTexture", currentTex ? 1.0f : 0.0f);
        shader.setInt("tex", 0);
        glBindVertexArray(cubeMesh.vao);
        glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }



    // Realistic trees
    shader.setFloat("useTexture", 0.0f);
    for (auto &tree : treeCells) {
        auto w = cellToWorld(tree.first, tree.second);
        
        // Tree trunk
        glm::mat4 trunk = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 1.0f, w.second));
        trunk = glm::scale(trunk, glm::vec3(0.3f, 2.0f, 0.3f));
        shader.setMat4("model", trunk);
        shader.setVec3("baseColor", glm::vec3(0.4f, 0.2f, 0.1f)); // Brown trunk
        glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
        
        // Tree foliage
        glm::mat4 leaves = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 2.5f, w.second));
        leaves = glm::scale(leaves, glm::vec3(1.5f, 1.5f, 1.5f));
        shader.setMat4("model", leaves);
        shader.setVec3("baseColor", glm::vec3(0.1f, 0.6f, 0.1f)); // Green leaves
        glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
    }

    // Street lamps with glowing lights
    shader.setFloat("useTexture", 0.0f);
    for (auto &lamp : streetLamps) {
        auto w = cellToWorld(lamp.first, lamp.second);
        
        // Lamp pole
        glm::mat4 pole = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 2.5f, w.second));
        pole = glm::scale(pole, glm::vec3(0.1f, 5.0f, 0.1f));
        shader.setMat4("model", pole);
        shader.setVec3("baseColor", glm::vec3(0.2f, 0.2f, 0.2f)); // Dark metal
        glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
        
        // Glowing lamp head
        glm::mat4 light = glm::translate(glm::mat4(1.0f), glm::vec3(w.first, 5.2f, w.second));
        light = glm::scale(light, glm::vec3(0.3f, 0.2f, 0.3f));
        shader.setMat4("model", light);
        shader.setVec3("baseColor", glm::vec3(1.0f, 0.9f, 0.6f)); // Warm street light
        glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
    }

    // Car on the road
    auto carW = cellToWorld(carPosition.first, carPosition.second);
    
    // Car body
    glm::mat4 carBody = glm::translate(glm::mat4(1.0f), glm::vec3(carW.first, 0.4f, carW.second));
    carBody = glm::scale(carBody, glm::vec3(2.0f, 0.8f, 1.0f));
    shader.setMat4("model", carBody);
    shader.setVec3("baseColor", glm::vec3(0.8f, 0.1f, 0.1f)); // Red car
    glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
    
    // Car roof
    glm::mat4 carRoof = glm::translate(glm::mat4(1.0f), glm::vec3(carW.first, 1.0f, carW.second));
    carRoof = glm::scale(carRoof, glm::vec3(1.6f, 0.4f, 0.8f));
    shader.setMat4("model", carRoof);
    shader.setVec3("baseColor", glm::vec3(0.7f, 0.1f, 0.1f)); // Darker red roof
    glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
    
    // Car headlights
    glm::mat4 headlight1 = glm::translate(glm::mat4(1.0f), glm::vec3(carW.first + 1.1f, 0.5f, carW.second + 0.3f));
    headlight1 = glm::scale(headlight1, glm::vec3(0.1f, 0.2f, 0.2f));
    shader.setMat4("model", headlight1);
    shader.setVec3("baseColor", glm::vec3(1.0f, 1.0f, 0.9f)); // Bright headlight
    glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
    
    glm::mat4 headlight2 = glm::translate(glm::mat4(1.0f), glm::vec3(carW.first + 1.1f, 0.5f, carW.second - 0.3f));
    headlight2 = glm::scale(headlight2, glm::vec3(0.1f, 0.2f, 0.2f));
    shader.setMat4("model", headlight2);
    shader.setVec3("baseColor", glm::vec3(1.0f, 1.0f, 0.9f)); // Bright headlight
    glBindVertexArray(cubeMesh.vao); glDrawElements(GL_TRIANGLES, cubeMesh.elemCount, GL_UNSIGNED_INT, 0); glBindVertexArray(0);
}
