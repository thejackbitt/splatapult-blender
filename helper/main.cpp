#include <windows.h>
#include <iostream>
#include "renderer.h"

int main() {
    //inits the buffer info for the reader 
    const char* readBufferName = "Local\\C3DINPUT";
    const size_t readBufferSize = 1024;
    //inits the buffer info for the writer 
    const char* writeBufferName = "Local\\C3DOUTPUT";
    const size_t writeBufferSize = 1024;

    int rc = runRender(readBufferName, readBufferSize, writeBufferName, writeBufferSize);
    if(rc!=0) std::cerr<<"Render failed :("<<'\n'<<std::endl;
    return rc;
}