#pragma once

#include <d3d11.h>




struct Image
{
    Image() : Width(0), Height(0), my_texture(0)
    {

    }

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    ~Image() {};

    bool LoadFromFile(const char* filepath);
    
    int Width{};
    int Height{};

    ID3D11ShaderResourceView* my_texture = NULL;

    //GLuint ID;
};



