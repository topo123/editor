#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PieceTable.hpp"
#include "Renderer.hpp"
#include "TypeBuffer.hpp"
#include "InputHandler.hpp"

void framebuffer_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Text editor", nullptr, nullptr);
	if(window == nullptr)
	{
		std::cout << "Window failed to create\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	Renderer render = {0};
	TypeBuffer type_buffer = {0};

	PieceTable* table = init_piece_table("txtfiles/hello.txt", &type_buffer, 128);
	init_render_data(&render);

	AppInfo info = {&type_buffer, table};

	glfwSetWindowUserPointer(window, &info);
	glfwSetFramebufferSizeCallback(window, framebuffer_callback);
	glfwSetCharCallback(window, character_processing);
	glfwSetKeyCallback(window, key_processing);

	while(!glfwWindowShouldClose(window))
	{
		glClearColor(0.350f, 0.35f, 0.35f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		render_piecetable(&render, table, &type_buffer, 24, 10);
		//render_character(&render, glm::vec2(300, 300), 'k');
		//render_quad(&render, glm::vec2(200.0f, 200.0f), glm::vec2(10.0f, 10.0f));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
