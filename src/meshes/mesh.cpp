#include "mesh.h"
#include <cmath>
#include <vector>

// Helper to create VAO from vertex buffer and index buffer
static Mesh buildMesh(const std::vector<float>& verts, const std::vector<unsigned int>& idx, int strideFloats=8) {
    Mesh m;
    glGenVertexArrays(1, &m.vao);
    glGenBuffers(1, &m.vbo);
    glGenBuffers(1, &m.ebo);
    glBindVertexArray(m.vao);
    glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(float), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size()*sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);
    // pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,strideFloats*sizeof(float),(void*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,strideFloats*sizeof(float),(void*)(3*sizeof(float)));
    // uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,strideFloats*sizeof(float),(void*)(6*sizeof(float)));
    glBindVertexArray(0);
    m.elemCount = (GLsizei)idx.size();
    return m;
}

Mesh makeCube() {
    // 24 vertices (6 faces * 4), each vertex: pos(3) normal(3) uv(2) => 8 floats
    float V[] = {
        // front
        -0.5f,-0.5f,0.5f,   0,0,1,  0,0,
         0.5f,-0.5f,0.5f,   0,0,1,  1,0,
         0.5f, 0.5f,0.5f,   0,0,1,  1,1,
        -0.5f, 0.5f,0.5f,   0,0,1,  0,1,
        // back
        -0.5f,-0.5f,-0.5f,  0,0,-1, 0,0,
         0.5f,-0.5f,-0.5f,  0,0,-1, 1,0,
         0.5f, 0.5f,-0.5f,  0,0,-1, 1,1,
        -0.5f, 0.5f,-0.5f,  0,0,-1, 0,1,
        // left
        -0.5f,-0.5f,-0.5f, -1,0,0,  0,0,
        -0.5f,-0.5f, 0.5f, -1,0,0,  1,0,
        -0.5f, 0.5f, 0.5f, -1,0,0,  1,1,
        -0.5f, 0.5f,-0.5f, -1,0,0,  0,1,
        // right
         0.5f,-0.5f,-0.5f,  1,0,0,  0,0,
         0.5f,-0.5f, 0.5f,  1,0,0,  1,0,
         0.5f, 0.5f, 0.5f,  1,0,0,  1,1,
         0.5f, 0.5f,-0.5f,  1,0,0,  0,1,
         // top
        -0.5f,0.5f, 0.5f,   0,1,0,  0,0,
         0.5f,0.5f, 0.5f,   0,1,0,  1,0,
         0.5f,0.5f,-0.5f,   0,1,0,  1,1,
        -0.5f,0.5f,-0.5f,   0,1,0,  0,1,
        // bottom
        -0.5f,-0.5f,0.5f,   0,-1,0, 0,0,
         0.5f,-0.5f,0.5f,   0,-1,0, 1,0,
         0.5f,-0.5f,-0.5f,  0,-1,0, 1,1,
        -0.5f,-0.5f,-0.5f,  0,-1,0, 0,1
    };
    unsigned int idx[] = {
        0,1,2, 0,2,3,
        4,5,6, 4,6,7,
        8,9,10, 8,10,11,
        12,13,14, 12,14,15,
        16,17,18, 16,18,19,
        20,21,22, 20,22,23
    };
    std::vector<float> verts(std::begin(V), std::end(V));
    std::vector<unsigned int> indices(std::begin(idx), std::end(idx));
    return buildMesh(verts, indices, 8);
}

Mesh makeQuadXZ() {
    float V[] = {
        -0.5f,0.0f, 0.5f,  0,1,0,  0,1,
         0.5f,0.0f, 0.5f,  0,1,0,  1,1,
         0.5f,0.0f,-0.5f,  0,1,0,  1,0,
        -0.5f,0.0f,-0.5f,  0,1,0,  0,0
    };
    unsigned int idx[] = {0,1,2, 0,2,3};
    std::vector<float> verts(std::begin(V), std::end(V));
    std::vector<unsigned int> indices(std::begin(idx), std::end(idx));
    return buildMesh(verts, indices, 8);
}

Mesh makeCircleFan(float cx, float cz, float radius, int segments) {
    // center + ring points
    std::vector<float> verts;
    std::vector<unsigned int> idx;
    verts.reserve((segments+2)*8);
    // center
    verts.push_back(cx); verts.push_back(0.01f); verts.push_back(cz);
    verts.push_back(0); verts.push_back(1); verts.push_back(0);
    verts.push_back(0.5f); verts.push_back(0.5f);
    for (int i=0;i<=segments;i++) {
        float a = (float)i * 2.0f * 3.14159265f / segments;
        float cos_a = cosf(a);
        float sin_a = sinf(a);
        float x = cx + radius * cos_a;
        float z = cz + radius * sin_a;
        verts.push_back(x); verts.push_back(0.01f); verts.push_back(z);
        verts.push_back(0); verts.push_back(1); verts.push_back(0);
        verts.push_back((cos_a+1.0f)*0.5f); verts.push_back((sin_a+1.0f)*0.5f);
    }
    for (int i=1;i<=segments;i++){
        idx.push_back(0);
        idx.push_back(i);
        idx.push_back(i+1);
    }
    return buildMesh(verts, idx, 8);
}
