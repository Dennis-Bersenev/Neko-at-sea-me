#include "shader.hpp"

shader::shader(std::string path, unsigned int type)
{
    this->data = parseShader(path);

    compileShader(type);
}


const std::string shader::parseShader(const std::string& path)
{
    std::stringstream result;
    std::ifstream file(path);

    if (file.is_open()) {
        std::string line;
        while (getline(file, line))
        {
            result << line << '\n';
        }
        file.close();
    }
    else
        std::cout << "Unable to open file\n";

    return result.str();
}


unsigned int shader::compileShader(unsigned int type)
{
    //Create the shader
    unsigned int id = glCreateShader(type);

    //Attach the source file
    const char* src = &data[0];
    glShaderSource(id, 1, &src, NULL);

    //Compile it
    glCompileShader(id);

    //Check to make sure it was compiled
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* mssg = new char[length];
        glGetShaderInfoLog(id, length, &length, mssg);
        std::cout << "Shader failed to compile: " << mssg << std::endl;
        delete[] mssg;
        mssg = NULL;
        glDeleteShader(id);
        return 0;
    }

    this->ID = id;
    return id;
}

unsigned int shader::getID()
{
    return this->ID;
}