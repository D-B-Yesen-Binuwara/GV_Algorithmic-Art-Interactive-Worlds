#include "shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vsrc, const char* fsrc) {
    GLuint vs = compile(GL_VERTEX_SHADER, vsrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsrc);
    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);
    GLint ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[1024]; glGetProgramInfoLog(id, 1024, NULL, buf);
        std::cerr << "Shader link error: " << buf << "\n";
    }
    glDeleteShader(vs); glDeleteShader(fs);
}

Shader::~Shader() {
    if (id) glDeleteProgram(id);
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[1024]; glGetShaderInfoLog(s, 1024, NULL, buf);
        std::cerr << "Shader compile error: " << buf << "\n";
    }
    return s;
}
void Shader::use() const { glUseProgram(id); }
void Shader::setMat4(const std::string& name, const glm::mat4& m) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(m));
}
void Shader::setVec3(const std::string& name, const glm::vec3& v) const {
    glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &v[0]);
}
void Shader::setFloat(const std::string& name, float v) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), v);
}
void Shader::setInt(const std::string& name, int v) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), v);
}
