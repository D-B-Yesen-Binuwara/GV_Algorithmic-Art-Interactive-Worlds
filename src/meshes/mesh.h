#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <vector>

// Simple mesh container
struct Mesh {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei elemCount = 0;
};

// Builders:
Mesh makeCube();        // unit cube centered at origin (with normals + uv)
Mesh makeQuadXZ();     // unit quad on XZ plane (y=0) (unit square)
Mesh makeCircleFan(float cx, float cz, float radius, int segments = 48);

#endif // MESH_H
