#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

struct vec2 {
	float x = 0.0f;
	float y = 0.0f;
};
const float WORLD_H = 150.0f;
const float GROUND_H = floorf(WORLD_H * 0.25);

//Define an entity struct, that tells us its color (for now) its size and center
struct entity {
	glm::vec2 pos; //The center of the entity
	glm::vec2 size;
	glm::vec4 color;
	float vy = 0.0f;
};
vec2 offset;



void updatePlayer(GLFWwindow* window, entity& player, float dt, float worldW) {
	const float SPEED = 50.0f;   // horizontal, units/sec
	const float JUMP = 100.0f;   // initial upward speed, units/sec
	const float GRAVITY = 200.0f;  // units/sec^2
	float halfW = player.size.x / 2.0f;
	float halfH = player.size.y / 2.0f;

	// horizontal: opposite keys cancel, then clamp to the screen
	float dir = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) dir += 1.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) dir -= 1.0f;
	player.pos.x += dir * SPEED * dt;
	player.pos.x = std::clamp(player.pos.x, halfW, worldW - halfW);

	// vertical: jump only from the ground, gravity every frame
	bool onGround = (player.pos.y - halfH) <= GROUND_H + 0.01f;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && onGround) {
		player.vy = JUMP;
	}
	player.vy -= GRAVITY * dt;
	player.pos.y += player.vy * dt;

	// landing: put the feet exactly on the ground and stop falling
	if (player.pos.y - halfH < GROUND_H) {
		player.pos.y = GROUND_H + halfH;
		player.vy = 0.0f;
	}
}



		
int main()
{
	
	if (!glfwInit()) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Game Engine", NULL, NULL);
	//If the window fails to be created, print an error message and return -1
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		return -1;
	}



	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();	
		return -1;
	}
	//Define the vertex shader source code as a string
	std::string vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 position;
    uniform mat4 uModel;
    uniform mat4 uProj;
    void main(){
     gl_Position = uProj * uModel * vec4(position, 1.0);
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
	//lets define a uniform
	std::string fragmentShaderSource = R"(
    #version 330 core
    out vec4 fragColor;
    uniform vec4 wColor; 
    void main(){
	 fragColor = wColor;
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
		//first vertex pos and color
		0.5f, 0.5f, 0.0f, /*color*/
		//2nd
		-0.5f, 0.5f, 0.0f, /*color*/
		//3rd
		-0.5f, -0.5f, 0.0f, /*color*/
		//4th
		0.5f,-0.5f, 0.0f,
	};

	std::vector<unsigned int> indices = {
		0,1,2,
		0,2,3
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//Now, Vertex data is in GPU memeory, now we need to provide the interperation of said array buffer
	//We use a VAO, to store the vertex attribute layout and how it maps to hsader inputs
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glVertexAttribPointer(0, 3, GL_FLOAT, false, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Background: one fullscreen triangle (no vertex buffer, positions come from gl_VertexID),
	//the fragment shader paints pixel-art sky, clouds and ground procedurally
	const char* bgVs = R"(
    #version 330 core
    void main(){
     vec2 p = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
     gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
    }
    )";
	const char* bgFs = R"(
    #version 330 core
    out vec4 fragColor;
    uniform vec2 uRes;
    float box(vec2 c, vec2 o, vec2 s){
     vec2 d = c - o;
     return step(0.0, d.x) * step(d.x, s.x - 1.0) * step(0.0, d.y) * step(d.y, s.y - 1.0);
    }
    float cloud(vec2 c, vec2 o){
     return max(box(c, o, vec2(12,2)), max(box(c, o + vec2(2,2), vec2(8,2)), box(c, o + vec2(4,4), vec2(4,2))));
    }
    void main(){
     float PX = uRes.y / 150.0;
     vec2 c = floor(gl_FragCoord.xy / PX);
     vec2 grid = floor(uRes / PX);
     vec3 col = vec3(0.35, 0.65, 0.95); // sky
     float clouds = max(cloud(c, floor(grid * vec2(0.12, 0.70))),
                    max(cloud(c, floor(grid * vec2(0.50, 0.82))),
                        cloud(c, floor(grid * vec2(0.75, 0.62)))));
     col = mix(col, vec3(1.0), clouds);
     if (c.y < floor(grid.y * 0.25)) { // ground, with a few darker speckles
      float h = fract(sin(dot(c, vec2(12.9898, 78.233))) * 43758.5453);
      col = h > 0.85 ? vec3(0.35, 0.22, 0.11) : vec3(0.47, 0.30, 0.15);
     }
     fragColor = vec4(col, 1.0);
    }
    )";
	GLuint bgProgram = glCreateProgram();
	for (auto [type, src] : { std::pair<GLenum, const char*>{GL_VERTEX_SHADER, bgVs}, {GL_FRAGMENT_SHADER, bgFs} }) {
		GLuint s = glCreateShader(type);
		glShaderSource(s, 1, &src, nullptr);
		glCompileShader(s);
		glGetShaderiv(s, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(s, sizeof(infoLog), nullptr, infoLog);
			std::cerr << "Background shader compilation failed:\n" << infoLog << std::endl;
		}
		glAttachShader(bgProgram, s);
		glDeleteShader(s);
	}
	glLinkProgram(bgProgram);
	GLint bgResLoc = glGetUniformLocation(bgProgram, "uRes");
	GLuint bgVao; // core profile needs a VAO bound even with no attributes
	glGenVertexArrays(1, &bgVao);
	//Assign IDs to all uniforms
	GLuint wColorLoc = glGetUniformLocation(shaderProgram, "wColor");
	GLuint uModelLoc = glGetUniformLocation(shaderProgram, "uModel");
	GLuint uProjLoc = glGetUniformLocation(shaderProgram, "uProj");

	

	entity player;
	player.size = { 12.0f, 10.f };
	player.pos = { 25.0f, GROUND_H };
	player.color = { 0.85f, 0.65f, 0.47f, 1.0f };
	double lastTime = glfwGetTime();
	//Event loop to keep the window open until the user closes it
	while (!glfwWindowShouldClose(window)) {
		
		int fbW, fbH;
		glfwGetFramebufferSize(window, &fbW, &fbH);
		float worldW = WORLD_H * (float)fbW / (float)fbH;
		double now = glfwGetTime();
		float time_between_frames = (float)(now - lastTime);
		lastTime = now;
		updatePlayer(window, player, time_between_frames, worldW);
		glUseProgram(bgProgram);
		glUniform2f(bgResLoc, (float)fbW, (float)fbH);
		glBindVertexArray(bgVao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		
		glm::mat4 proj = glm::ortho(0.0f, worldW, 0.0f, WORLD_H, -1.0f, 1.0f);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(player.pos, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(player.size, 1.0f));

		glUseProgram(shaderProgram);
		glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform4fv(wColorLoc, 1, glm::value_ptr(player.color));
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Swap the front and back buffers to display the rendered image
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}