#pragma once

#include "common.hpp"


class shader {
private:
	std::string data;
	unsigned int ID;
	/*
	Takes a .shader file and parses it.
	@param path - the filepath containing the shader file.
	@return the const string which openGL uses to compile shader programs.
	*/
	const std::string parseShader(const std::string& path);

	/*
	Compiles the shader given the data field already contains the string representation of the source file.
	@param type - the type of shader to be compiled.
	@return the id used to reference the compiled shader.
	*/
	unsigned int compileShader(unsigned int type);

public:
	/*
	Creates a new shader object.
	@param - path the filepath of the .shader file.
	@return a new shader object.
	*/
	shader(std::string path, unsigned int type);

	/*
	Public getter for this shader's ID.
	*/
	unsigned int getID();

};