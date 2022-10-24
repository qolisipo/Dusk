#include "pch.h"
#include "Texture.h"

#include "GLCommon.h"
#include "Utils/Profiling/Timer.h"

#include "stb/stb_image.h"

#include <filesystem>

namespace DuskEngine
{
    Texture::Texture(const std::string& filepath, const std::string& name)
        :m_ID(0), m_Width(0), m_Height(0), m_Size(0)
    {
        if (name.empty()) 
        {
            std::string::size_type begin = filepath.rfind("/") + 1;
            std::string::size_type end = filepath.rfind(".");
            std::string filename = filepath.substr(begin, end - begin);
            m_Name = filename;
        }
        else
            m_Name = name;

        stbi_set_flip_vertically_on_load(true); // move this somewhere else

        int width, height, channels;
        unsigned char* data;

        data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

        if (data)
        {
            m_Width = width;
            m_Height = height;
            
            GLenum type = 0;

            if (channels == 4)
            {
                type = GL_RGBA;
            }
            else if (channels == 3)
            {
                type = GL_RGB;
            }

            glGenTextures(1, &m_ID);
            glBindTexture(GL_TEXTURE_2D, m_ID);

            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, type, m_Width, m_Height, 0, type, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::string message = "Error loading texture '" + m_Name + "'";
            WARN(message.c_str());
            return;
        }

        stbi_image_free(data);

        std::string message = "Created Texture " + m_Name;
        LOG(message.c_str());
    }

    Texture::Texture(const std::filesystem::path& path, const uuids::uuid& uuid)
        :m_ID(0), m_Width(0), m_Height(0), m_Size(0)
    {
        m_UUID = uuid;
        m_Path = path;
        m_Name = path.filename().string();

        std::string importFile = "res/.import/" + path.filename().string() + "-" + uuids::to_string(uuid) + ".import";

        if (std::filesystem::exists(importFile))
        //if (false)
        {
            //TRACE("File has been imported");
            std::ifstream stream(importFile, std::ios::binary);
            TextureData headerData;

            // expect 0
            /*std::streampos pos = stream.tellg();
            TRACE(std::to_string(pos).c_str());*/
            
            stream.read((char*)&headerData, sizeof(TextureData));
            
            // expect 24
            /*pos = stream.tellg();
            TRACE(std::to_string(pos).c_str());*/

            m_Width = headerData.Width;
            m_Height = headerData.Height;

            GLenum type = 0;

            if (headerData.Channels == 4)
            {
                type = GL_RGBA;
            }
            else if (headerData.Channels == 3)
            {
                type = GL_RGB;
            }

            uint8_t* textureData = new uint8_t[headerData.DataSize];
            stream.read((char*)textureData, headerData.DataSize);
            
            // expect 1200024
            /*pos = stream.tellg();
            TRACE(std::to_string(pos).c_str());*/


            //memcpy(textureData, &stream + sizeof(TextureData), (size_t)headerData.DataSize);
            
            glGenTextures(1, &m_ID);
            glBindTexture(GL_TEXTURE_2D, m_ID);

            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            {
                //Timer upload("upload");
                glTexImage2D(GL_TEXTURE_2D, 0, type, m_Width, m_Height, 0, type, GL_UNSIGNED_BYTE, textureData);
            }

            glGenerateMipmap(GL_TEXTURE_2D);

            delete[] textureData;
        }
        else
        {
            TRACE("Using raw version");

            stbi_set_flip_vertically_on_load(true);
            int width, height, channels;
            unsigned char* data;
            {
                //Timer timer("Decode");
                data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
            }

            if (data)
            {
                m_Width = width;
                //TRACE(std::to_string(channels).c_str());
                m_Height = height;

                //int size = width * height * channels;

                GLenum type = 0;

                if (channels == 4)
                {
                    type = GL_RGBA;
                }
                else if (channels == 3)
                {
                    type = GL_RGB;
                }

                glGenTextures(1, &m_ID);
                glBindTexture(GL_TEXTURE_2D, m_ID);

                // set the texture wrapping/filtering options (on the currently bound texture object)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                {
                    //Timer upload("upload");
                    glTexImage2D(GL_TEXTURE_2D, 0, type, m_Width, m_Height, 0, type, GL_UNSIGNED_BYTE, data);
                }

                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::string message = "Error loading texture " + m_Name;
                WARN(message.c_str());
                return;
            }

            stbi_image_free(data);
        }


        

        std::string message = "Created Texture " + m_Name;
        LOG(message.c_str());
    }

    Texture::~Texture()
    {
        //glDeleteTextures(1, &m_ID);
        std::string message = "Destroyed Texture " + m_Name;
        LOG(message.c_str());
    }

    void Texture::Bind(unsigned int slot) const
    {
        glBindTextureUnit(slot, m_ID);
    }

    void Texture::ImportTest(const std::filesystem::path& path, std::ofstream& fout)
    {
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels;
        unsigned char* data;
        data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

        if (data)
        {
            TextureData headerData;
            headerData.Width = width;
            TRACE(std::to_string(headerData.Width).c_str());
            headerData.Height = height;
            headerData.Channels = channels;
            headerData.DataSize = width * height * channels;

            size_t dataSize = headerData.DataSize + sizeof(TextureData);
            uint8_t* textureData = new uint8_t[dataSize];

            memcpy(textureData, &headerData, sizeof(TextureData));
            memcpy(textureData + sizeof(TextureData), data, headerData.DataSize);
            fout.write((char*)textureData, dataSize);
            delete[] textureData;
        }
        else
        {
            std::string message = "Error importing texture " + path.string();
            ERR(message.c_str());
            return;
        }

        stbi_image_free(data);
    }
}