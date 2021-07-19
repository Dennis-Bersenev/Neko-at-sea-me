#include "program.hpp"

program::program(unsigned int s1, unsigned int s2)
{
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, s1);
    glAttachShader(shaderProgram, s2);
    glLinkProgram(shaderProgram);

    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(s1);
    glDeleteShader(s2);
    this->ID = shaderProgram;
}

int program::getID()
{
    return this->ID;
}