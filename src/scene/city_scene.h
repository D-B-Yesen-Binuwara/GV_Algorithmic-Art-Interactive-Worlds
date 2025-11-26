#ifndef CITY_SCENE_H
#define CITY_SCENE_H

#include "../meshes/mesh.h"
#include "../render/shader.h"
#include "../render/texture.h"
#include "../render/camera.h"
#include <vector>
#include <utility>

struct CityConfig {
    int citySize;      // 1=Small, 2=Medium, 3=Large
    int numBuildings;  // 5-20
    int numSkyscrapers, numTowers, numOfficeBuildings;
    int buildingStyle; // 1=Skyscrapers, 2=Towers, 3=Mixed
    float towerWidth, towerHeight;
    float buildingWidth, buildingHeight;
    float skyscraperWidth, skyscraperHeight;
    float pondRadius;
};

class CityScene {
public:
    CityScene() {}
    ~CityScene();
    bool init(int citySize, int numBuildings, int buildingStyle, float towerW, float towerH, float buildingW, float buildingH, float skyW, float skyH, float pondR, int numSky = 0, int numTow = 0, int numOff = 0);
    void update(float dt);
    void render(const Shader& shader, const Camera& cam);
private:
    Mesh cubeMesh;
    Mesh quadMesh;
    Mesh pondMesh;
    GLuint buildingTex = 0;
    GLuint towerTex = 0;
    GLuint skyscraperTex = 0;
    GLuint roadTex = 0;
    GLuint pondTex = 0;

    // procedural placement
    std::vector<std::pair<int,int>> roadCells;
    std::vector<std::pair<int,int>> buildingCells;
    std::vector<float> buildingHeights;
    std::vector<int> buildingTypes; // 0=skyscraper, 1=tower, 2=office
    std::vector<std::pair<int,int>> treeCells;
    std::vector<std::pair<int,int>> streetLamps;
    std::pair<int,int> carPosition;
    int pond_cx, pond_cy, pond_r;
    CityConfig config;
    int gridSize;
    float waterTime;
    float ripplePhase;
    std::vector<std::vector<bool>> occupiedGrid;
    // helper
    std::pair<float,float> cellToWorld(int i, int j) const;
    bool isGridCellFree(int x, int z, int size = 1) const;
    void markGridCells(int x, int z, int size = 1);
    void generateFuturisticLayout();
    void placeBuildingsRandomly();
    void placeTrees();
    void placeStreetLamps();
    void placeRandomCar();
    float getFuturisticHeight(int type, int index);
    glm::vec3 getFuturisticColor(int type);
};

#endif // CITY_SCENE_H
