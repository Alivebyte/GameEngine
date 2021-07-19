
#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

class Shader
{
public:
    

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
private:
    // the program ID
    unsigned int ID;
};

#endif // !SHADER_H