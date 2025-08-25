#include <iostream>
#include "InputHandler.hpp"
#include "TypeBuffer.hpp"

#define KEY(KEY_NAME) GLFW_KEY_##KEY_NAME


void character_processing(GLFWwindow* window, unsigned int character)
{
	AppInfo* info = static_cast<AppInfo*>(glfwGetWindowUserPointer(window));
	insert_char(info->type_buffer, info->table, character);
}

void key_processing(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	AppInfo* info = static_cast<AppInfo*>(glfwGetWindowUserPointer(window));
	if(key == KEY(BACKSPACE) && action == GLFW_PRESS)
	{
		ldelete_char(info->type_buffer, info->table, 1);
	}
	else if(key == KEY(DELETE) && action == GLFW_PRESS)
	{
		rdelete_char(info->type_buffer, info->table, 1);
	}
	else if(key == KEY(LEFT) && action == GLFW_PRESS)
	{
		lseek(info->type_buffer, info->table, 1);
	}
	else if(key == KEY(RIGHT) && action == GLFW_PRESS)
	{
		rseek(info->type_buffer, info->table, 1);
	}
}
