#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

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
	//Define the vertex shader source code as a string
	std::string vertexShaderSource = R"(d
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main(){
     gl_position = vec4(pos.x,pos.y,pos.z, 1.0);
    }
    )";
	//Create a vertex shader object and compile the shader source code
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//Convert the vertex shader source code string to a C-style string
	const char* vertexShaderSourceCStr = vertexShaderSource.c_str();
	//Set the source code for the vertex shader and compile it
	glShaderSource(vertexShader, 1, &vertexShaderSourceCStr, nullptr);
	glCompileShader(vertexShader);
	//Verify if the vertex shader compiled successfully and print an error message if it failed
	GLint success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
		std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
	}
	//
	std::string fragmentShaderSource = R"(
    #version 330 core
    out vec4 fragColor;
    
    void main(){
	 fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
	}
    )";

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, nullptr);
	glCompileShader(fragmentShader);

	//Check if the fragment shader compiled successfully and print an error message if it failed
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
		std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
	}
	//Create the shader program, bind both shaders to it and compile the entire thing
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
		std::cerr << " Fragment shader compilation failed:\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	
	
    std::vector<float> vertices = {
	0.0f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f
};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Now, Vertex data is in GPU memeory, now we need to provide the interperation of said array buffer
	//We use a VAO, to store the vertex attribute layout and how it maps to hsader inputs
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Event loop to keep the window open until the user closes it
	while (!glfwWindowShouldClose(window)) {
		//Set the clear color to a dark teal and clear the color buffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//Clear the color buffer to apply the clear color
		glClear(GL_COLOR_BUFFER_BIT);
		//Swap the front and back buffers to display the rendered image
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}