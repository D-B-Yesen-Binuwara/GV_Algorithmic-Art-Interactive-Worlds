#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    GLuint id = 0;
    Shader() {}
    Shader(const char* vsrc, const char* fsrc);
    ~Shader();
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& m) const;
    void setVec3(const std::string& name, const glm::vec3& v) const;
    void setFloat(const std::string& name, float v) const;
    void setInt(const std::string& name, int v) const;
private:
    GLuint compile(GLenum type, const char* src);
};

#endif // SHADER_H
