#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include "render/shader.h"
#include "render/camera.h"
#include "scene/city_scene.h"

// Shaders (simple)
const char* vertexSrc = R"glsl(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aUV;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
out vec3 vNormal;
out vec3 vFragPos;
out vec2 vUV;
void main(){
    vFragPos = vec3(model * vec4(aPos,1.0));
    vNormal = mat3(transpose(inverse(model))) * aNormal;
    vUV = aUV;
    gl_Position = proj * view * vec4(vFragPos, 1.0);
}
)glsl";

const char* fragmentSrc = R"glsl(
#version 330 core
in vec3 vNormal;
in vec3 vFragPos;
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D tex;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 baseColor;
uniform float useTexture;
uniform int texIndex;
void main(){
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vFragPos);
    float diff = max(dot(N,L), 0.0);
    vec3 color = baseColor;
    if (useTexture > 0.5) {
        color = texture(tex, vUV).rgb;
    }
    
    // Night lighting with lower ambient
    vec3 ambient = 0.15 * color;
    vec3 diffuse = diff * color * 0.6;
    
    // Subtle specular for natural materials
    vec3 viewDir = normalize(viewPos - vFragPos);
    vec3 H = normalize(L + viewDir);
    float spec = pow(max(dot(N,H), 0.0), 16.0);
    vec3 specular = vec3(0.2) * spec;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)glsl";

static Camera camera;
static bool rightMouseDown = false;
static double lastX=0, lastY=0;

void cursor_cb(GLFWwindow* w, double xpos, double ypos) {
    if (!rightMouseDown) { lastX = xpos; lastY = ypos; return; }
    float dx = (float)(xpos - lastX);
    float dy = (float)(lastY - ypos);
    lastX = xpos; lastY = ypos;
    camera.processMouseDelta(dx, dy);
}
void mouse_button_cb(GLFWwindow* w, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        rightMouseDown = (action == GLFW_PRESS);
    }
}
void scroll_cb(GLFWwindow* w, double x, double y) {
    camera.processScroll((float)y);
}

int main() {
    std::cout << "Initializing GLFW...\n";
    if (!glfwInit()) { std::cerr<<"GLFW init failed\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    std::cout << "Creating window...\n";
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* win = glfwCreateWindow(mode->width, mode->height, "Night Cityscape Generator", NULL, NULL);
    if (!win) { std::cerr<<"Window create failed\n"; glfwTerminate(); return -1; }
    std::cout << "Window created successfully\n";
    glfwMakeContextCurrent(win);
    glfwSetCursorPosCallback(win, cursor_cb);
    glfwSetMouseButtonCallback(win, mouse_button_cb);
    glfwSetScrollCallback(win, scroll_cb);

    std::cout << "Initializing GLEW...\n";
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { std::cerr<<"GLEW init failed\n"; return -1; }
    std::cout << "GLEW initialized successfully\n";

    glEnable(GL_DEPTH_TEST);

    // Build shader
    std::cout << "Building shaders...\n";
    Shader shader(vertexSrc, fragmentSrc);
    std::cout << "Shaders built successfully\n";

    // Get user input for city generation
    std::cout << "\n=== NIGHT CITYSCAPE GENERATOR ===\n";
    std::cout << "Design your custom night city!\n\n";
    
    int citySize, buildingStyle;
    float towerWidth, towerHeight, buildingWidth, buildingHeight, skyscraperWidth, skyscraperHeight, pondRadius;
    std::string cityName;
    
    std::cout << "Enter city name: ";
    std::getline(std::cin, cityName);
    
    do {
        std::cout << "City size [1=Small (50x50), 2=Medium (70x70), 3=Large (100x100)]: ";
        std::cin >> citySize;
    } while (citySize < 1 || citySize > 3);
    
    do {
        std::cout << "Building style (1=Skyscrapers, 2=Towers, 3=Mixed): ";
        std::cin >> buildingStyle;
    } while (buildingStyle < 1 || buildingStyle > 3);
    
    int numSkyscrapers = 0, numTowers = 0, numBuildings = 0, numOfficeBuildings = 0;
    
    if (buildingStyle == 1) {
        do {
            std::cout << "Number of skyscrapers (5-20): ";
            std::cin >> numSkyscrapers;
        } while (numSkyscrapers < 5 || numSkyscrapers > 20);
        numBuildings = numSkyscrapers;
    } else if (buildingStyle == 2) {
        do {
            std::cout << "Number of towers (5-20): ";
            std::cin >> numTowers;
        } while (numTowers < 5 || numTowers > 20);
        numBuildings = numTowers;
    } else {
        do {
            std::cout << "Number of skyscrapers (1-10): ";
            std::cin >> numSkyscrapers;
        } while (numSkyscrapers < 1 || numSkyscrapers > 10);
        do {
            std::cout << "Number of towers (1-10): ";
            std::cin >> numTowers;
        } while (numTowers < 1 || numTowers > 10);
        do {
            std::cout << "Number of office buildings (1-10): ";
            std::cin >> numOfficeBuildings;
        } while (numOfficeBuildings < 1 || numOfficeBuildings > 10);
        numBuildings = numSkyscrapers + numTowers + numOfficeBuildings;
    }
    
    std::cout << "\n=== CUSTOM DIMENSIONS ===\n";
    
    if (buildingStyle == 1 || buildingStyle == 3) {
        do {
            std::cout << "Skyscraper width (1.0-10.0): ";
            std::cin >> skyscraperWidth;
        } while (skyscraperWidth < 1.0f || skyscraperWidth > 10.0f);
        do {
            std::cout << "Skyscraper height (10.0-100.0): ";
            std::cin >> skyscraperHeight;
        } while (skyscraperHeight < 10.0f || skyscraperHeight > 100.0f);
    }
    
    if (buildingStyle == 2 || buildingStyle == 3) {
        do {
            std::cout << "Tower width (1.0-5.0): ";
            std::cin >> towerWidth;
        } while (towerWidth < 1.0f || towerWidth > 5.0f);
        do {
            std::cout << "Tower height (5.0-50.0): ";
            std::cin >> towerHeight;
        } while (towerHeight < 5.0f || towerHeight > 50.0f);
    }
    
    if (buildingStyle == 3) {
        do {
            std::cout << "Building width (1.0-8.0): ";
            std::cin >> buildingWidth;
        } while (buildingWidth < 1.0f || buildingWidth > 8.0f);
        do {
            std::cout << "Building height (3.0-80.0): ";
            std::cin >> buildingHeight;
        } while (buildingHeight < 3.0f || buildingHeight > 80.0f);
    }
    
    do {
        std::cout << "Pond radius (2.0-15.0): ";
        std::cin >> pondRadius;
    } while (pondRadius < 2.0f || pondRadius > 15.0f);
    
    std::cout << "\nGenerating night city: " << cityName << "...\n";
    
    CityScene scene;
    std::cout << "Initializing scene...\n";
    if (!scene.init(citySize, numBuildings, buildingStyle, towerWidth, towerHeight, buildingWidth, buildingHeight, skyscraperWidth, skyscraperHeight, pondRadius, numSkyscrapers, numTowers, numOfficeBuildings)) { 
        std::cerr<<"Scene init failed\n"; return -1; 
    }
    std::cout << "Scene initialized successfully\n";

    // camera target at origin
    camera.target = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.distance = 80.0f; // Start further back for better city view
    camera.pitch = -15.0f;   // Slight downward angle

    std::cout << "\n=== CAMERA CONTROLS ===\n";
    std::cout << "Right-click + drag: Rotate camera\n";
    std::cout << "Mouse scroll: Zoom in/out\n";
    std::cout << "ESC: Exit\n\n";

    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();
        
        int width, height;
        glfwGetFramebufferSize(win, &width, &height);
        glViewport(0, 0, width, height);
        
        glClearColor(0.05f,0.05f,0.15f,1.0f); // Dark night sky
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.update(0.016f); // Keep for water animation
        scene.update(0.016f);
        scene.render(shader, camera);

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
