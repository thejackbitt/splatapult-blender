#pragma once
#include <string>
#include <GL/glew.h>

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    void use() const;

    void setMat4(const char* name, const GLfloat* mat) const;
    void setInt (const char* name, int   value) const;
    void setFloat(const char* name, float value) const;

private:
    GLuint ID;

    std::string readFile(const char* path);
    GLuint compile(GLenum type, const char* src);
    void   checkErrors(GLuint obj, const char* what) const;
};
