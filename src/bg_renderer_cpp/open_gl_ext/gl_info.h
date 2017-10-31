#pragma once

#include <string>
#include <vector>
#include <iostream>

class GLInfo
{
    
private:
    
    std::string vendor;
    std::string renderer;
    std::string version;
    std::string glslVersion;
    std::vector <std::string> extensions;
    int redBits;
    int greenBits;
    int blueBits;
    int alphaBits;
    int depthBits;
    int stencilBits;
    int maxTextureSize;
    int maxLights;
    int maxAttribStacks;
    int maxModelViewStacks;
    int maxProjectionStacks;
    int maxClipPlanes;
    int maxTextureStacks;
    
    //
    int gl_window;

public:
    
    GLInfo() : redBits(0), greenBits(0), blueBits(0), alphaBits(0), depthBits(0),
               stencilBits(0), maxTextureSize(0), maxLights(0), maxAttribStacks(0),
               maxModelViewStacks(0), maxClipPlanes(0), maxTextureStacks(0)
    {
        init();
    };

    ~GLInfo();
    
    void init();
    
    void get_info();                                     // extract info

    bool is_extension_supported(const std::string& ext); // check if a extension is supported
};
