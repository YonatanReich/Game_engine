#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

int main()
{

	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		
	GLFWwindow* window = glfwCreateWindow(1500, 1000, "Game Engine", NULL, NULL);
	//If the window fails to be created, print an error message and return -1
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}
	//Set the window position to (1000, 150) and make the context current
	glfwSetWindowPos(window, 1000, 150);
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();	
		return -1;
	}

	//Event loop to keep the window open until the user closes it
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}