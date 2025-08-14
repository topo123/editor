#pragma once
#include <GLFW/glfw3.h>
#include <TypeBuffer.hpp>

struct AppInfo{
	TypeBuffer* type_buffer;
	PieceTable* table;
};

void character_processing(GLFWwindow* window, unsigned int character);
