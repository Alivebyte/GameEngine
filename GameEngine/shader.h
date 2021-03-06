
#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Shader
{
public:
    

    // constructor reads and builds the shader
    Shader(std::string vertexPath, std::string fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, glm::mat4 mat) const;
    void setVec3(const std::string& name, glm::vec3 vec3) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, glm::vec4 vec4) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
private:
    // the program ID
    unsigned int ID;
};

#endif // !SHADER_H