#include "InputHandler.hpp"
#include "TypeBuffer.hpp"

#define KEY(KEY_NAME) GLFW_KEY_##KEY_NAME


void character_processing(GLFWwindow* window, unsigned int character)
{
	AppInfo* info = static_cast<AppInfo*>(glfwGetWindowUserPointer(window));
	insert_char(info->type_buffer, info->table, character);
}
