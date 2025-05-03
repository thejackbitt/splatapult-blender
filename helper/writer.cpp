#include <windows.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <cstring>
#include "writer.h"

// int write(char* data, char* view, const size_t bufferSize) {
//     // std::cout << "[Helper] JSON: " << '\n' << view << '\n' << std::endl;
//     std::memcpy(view, data, bufferSize);
//     return 0;
// };

int write(const char* bufferName, size_t bufferSize, int width, int height, const std::vector<uint8_t>& pixels) {
    HANDLE hMap = CreateFileMappingA(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        (DWORD)bufferSize,
        bufferName
    );
    if(!hMap){
        std::cerr<<"Failed to map handle: "<< GetLastError() << '\n' <<std::endl;
    }

    void* pBuf = MapViewOfFile(hMap, FILE_MAP_WRITE, 0,0, bufferSize);
    if(!pBuf) {
        std::cerr<<"Failed to write: "<<GetLastError()<<'\n'<<std::endl;
        CloseHandle(hMap);
        return -1;
    }

    std::memcpy(pBuf, pixels.data(), bufferSize);

    UnmapViewOfFile(pBuf);
    CloseHandle(hMap);
    return 0;
}