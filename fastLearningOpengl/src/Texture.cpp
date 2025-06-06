#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb_image/stb_image.h"
#include <iostream>


Texture::Texture(const std::string& path) 
: textureID(0), m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0) 
{
    stbi_set_flip_vertically_on_load(1);

    m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);
    glCall(glGenTextures(1, &textureID));
    glCall(glBindTexture(GL_TEXTURE_2D, textureID));

    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));


        
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    

    // maybe not needed (if we want to sample this data again.. ?)
    if (m_LocalBuffer) {
        stbi_image_free(m_LocalBuffer);
    } else {
        std::cout << "Error (TEXTURE): " << stbi_failure_reason() << std::endl;
    }

}


Texture::~Texture() {

    glCall(glDeleteTextures(1, &textureID));
}

void Texture::Bind(unsigned int slot) const {
    glCall(glActiveTexture(GL_TEXTURE0 + slot));
    glCall(glBindTexture(GL_TEXTURE_2D, textureID));
    // glCall(glBindTextureUni(textureID, m_img)); // for opengl 4.5>
}

void Texture::Unbind() const {
    glCall(glBindTexture(GL_TEXTURE_2D, 0));
}