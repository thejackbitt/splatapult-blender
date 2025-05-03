#include "renderer.h"
#include <nlohmann/json.hpp>
#include "shader.h"               

#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <cstring>

using json = nlohmann::json;

int runRender(const char* readBufName,
              size_t      readBufSize,
              const char* writeBufName,
              size_t      writeBufSize)
{
    HANDLE hIn = OpenFileMappingA(FILE_MAP_READ, FALSE, readBufName);
    if (!hIn) { std::cerr<<"OpenFileMappingA failed\n"; return -1; }
    void* pIn = MapViewOfFile(hIn, FILE_MAP_READ, 0, 0, readBufSize);
    if (!pIn) { std::cerr<<"MapViewOfFile failed\n"; CloseHandle(hIn); return -1; }

    std::string txt((char*)pIn, strnlen_s((char*)pIn, readBufSize));
    UnmapViewOfFile(pIn);
    CloseHandle(hIn);
    json j = json::parse(txt);
    int width      = j["view_width"].get<int>();
    int height     = j["view_height"].get<int>();
    auto viewM     = j["view_matrix"].get<std::vector<float>>();
    auto projM     = j["projection_matrix"].get<std::vector<float>>();
    // auto camPos = j["camera_position"].get<std::vector<float>>();

    if (!glfwInit()) { std::cerr<<"glfwInit failed\n"; return -1; }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* win = glfwCreateWindow(width, height, "offscreen", nullptr, nullptr);
    if (!win) { std::cerr<<"glfwCreateWindow failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(win);
    if (glewInit() != GLEW_OK) { std::cerr<<"glewInit failed\n"; return -1; }

    Shader shader("vs.glsl","fs.glsl");
    shader.use();
    shader.setMat4("uView", glm::value_ptr(glm::make_mat4(viewM.data())));
    shader.setMat4("uProj", glm::value_ptr(glm::make_mat4(projM.data())));

    TinyPlyLoader loader;
    if (!loader.load("./helper/test.ply")) {
        std::cerr<<"Failed to load test.ply\n"; 
        return -1;
    }
    GLuint vao    = loader.uploadToGPU();
    GLsizei count = loader.vertexCount();

    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    shader.use();
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, count);

    std::vector<uint8_t> pixels(width*height*4);
    glReadPixels(0,0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    HANDLE hOut = CreateFileMappingA(
        INVALID_HANDLE_VALUE, nullptr,
        PAGE_READWRITE,
        0, (DWORD)std::min(writeBufSize, pixels.size()),
        writeBufName
    );
    if (!hOut) { std::cerr<<"CreateFileMappingA failed\n"; return -1; }
    void* pOut = MapViewOfFile(hOut, FILE_MAP_WRITE, 0,0, writeBufSize);
    if (!pOut) { std::cerr<<"MapViewOfFile(w) failed\n"; CloseHandle(hOut); return -1; }

    std::memcpy(pOut, pixels.data(), std::min(writeBufSize, pixels.size()));
    UnmapViewOfFile(pOut);
    CloseHandle(hOut);

    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}