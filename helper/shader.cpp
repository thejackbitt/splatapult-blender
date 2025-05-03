#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertPath, const char* fragPath) {
    // 1) Read sources
    std::string vsrc = readFile(vertPath);
    std::string fsrc = readFile(fragPath);

    // 2) Compile
    GLuint vID = compile(GL_VERTEX_SHADER,   vsrc.c_str());
    GLuint fID = compile(GL_FRAGMENT_SHADER, fsrc.c_str());

    // 3) Link
    ID = glCreateProgram();
    glAttachShader(ID, vID);
    glAttachShader(ID, fID);
    glLinkProgram(ID);
    checkErrors(ID, "PROGRAM");

    // 4) Cleanup intermediate shaders
    glDeleteShader(vID);
    glDeleteShader(fID);
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setMat4(const char* name, const GLfloat* mat) const {
    GLint loc = glGetUniformLocation(ID, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void Shader::setInt(const char* name, int v) const {
    GLint loc = glGetUniformLocation(ID, name);
    glUniform1i(loc, v);
}

void Shader::setFloat(const char* name, float v) const {
    GLint loc = glGetUniformLocation(ID, name);
    glUniform1f(loc, v);
}

std::string Shader::readFile(const char* path) {
    std::ifstream in(path, std::ios::in|std::ios::binary);
    if(!in) {
        std::cerr<<"Failed to open shader: "<<path<<"\n";
        return {};
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

GLuint Shader::compile(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    checkErrors(sh, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
    return sh;
}

void Shader::checkErrors(GLuint obj, const char* what) const {
    GLint success;
    if(std::string(what)=="PROGRAM") {
        glGetProgramiv(obj, GL_LINK_STATUS, &success);
        if(!success) {
            char log[1024];
            glGetProgramInfoLog(obj, 1024, nullptr, log);
            std::cerr<<"Program link error:\n"<<log<<"\n";
        }
    } else {
        glGetShaderiv(obj, GL_COMPILE_STATUS, &success);
        if(!success) {
            char log[1024];
            glGetShaderInfoLog(obj, 1024, nullptr, log);
            std::cerr<<"Shader compile ("<<what<<"):\n"<<log<<"\n";
        }
    }
}
