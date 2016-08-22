#include <GL\glew.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_opengl.h>
#include <Windows.h>
#include "stb_image.h"
#include <chrono>

#define GLSL(src) "#version 450 core\n" #src

int main(int argc, char *argv[])
{
	auto startingTimer = std::chrono::high_resolution_clock::now();
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Init(SDL_INIT_VIDEO);
	
	//Must be set to create a window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	SDL_Window* window = SDL_CreateWindow("Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL); //Create the window
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//Initialize GLEW. Important that window context is created before init.
	glewExperimental = GL_TRUE;
	glewInit();
	if (glewInit() != GLEW_OK) {
		OutputDebugStringA("Failed to initialize GLEW\n");
		return -1;
	}

	//Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	glGenBuffers(1, &vbo); //Generate 1 buffer
	
	GLfloat vertices[] = {
		//  Position      Color             Texcoords
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
		0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-left
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Create an element array
	GLuint ebo;
	glGenBuffers(1, &ebo);
	
	GLuint elements[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	//Create vertex shader
    const char* vertexSource = GLSL(
        in vec2 position;
        in vec3 color;
        in vec2 texCoord;

        out vec3 Color;
        out vec2 TexCoord;
        
        void main() {
            Color = color;
            TexCoord = texCoord;
            gl_Position = vec4(position, 0.0, 1.0);
        }
    );

	//Compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	//Create fragment shader
	const char* fragmentSource = GLSL
	(
		//uniform vec3 triangleColor;
		in vec3 Color;
		in vec2 TexCoord;

		out vec4 outColor;

		uniform sampler2D texKitten;
		uniform sampler2D texPuppy;
		uniform float time;

		void main()
		{
			float factor = (sin(time * 3.0) + 1.0) / 2.0;
			outColor = mix(texture(texKitten, TexCoord), texture(texPuppy, TexCoord), factor);
		}
	);

	//Compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);

	//Link both shaders to a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor"); //Might not be necessary
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	//Create a texture
	GLuint textures[2];
	glGenTextures(2, textures);

	int width, height, bpp = 0;
	unsigned char* image;

	//Load first image
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	image = stbi_load("sample.png", &width, &height, &bpp, 3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);
	stbi_image_free(image);

	//Here we wrap textures and sample them by repeating them
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//Set wrap parameter for coordinate s to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Filter texture using Linear filtering for a smoother texture/image
	//TODO: Try using mipmaps for higher quality textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Load second image
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	image = stbi_load("sample2.png", &width, &height, &bpp, 3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);
	stbi_image_free(image);

	//Here we wrap textures and sample them by repeating them
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	//Set wrap parameter for coordinate s to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Filter texture using Linear filtering for a smoother texture/image
	//TODO: Try using mipmaps for higher quality textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint uniformTimer = glGetUniformLocation(shaderProgram, "time");

	//Specify layout of vertices
	GLint positionAttribute = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

	GLint colorAttribute = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(GLfloat)));

	GLint textureAttribute = glGetAttribLocation(shaderProgram, "texCoord");
	glEnableVertexAttribArray(textureAttribute);
	glVertexAttribPointer(textureAttribute, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(GLfloat)));

	//Main loop
	bool running = true;
	SDL_Event windowEvent;
	while (running)
	{

		if (SDL_PollEvent(&windowEvent))		//Check if there are any events to handle
		{
			if (windowEvent.type == SDL_QUIT)
			{
				running = false;				//Exit main loop if the user has pressed the exit button
				break;
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		auto currentTime = std::chrono::high_resolution_clock::now();
		auto totalTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - startingTimer).count();

		glUniform1f(uniformTimer, totalTime);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//Swap buffers
		SDL_GL_SwapWindow(window);
	}

	SDL_Delay(1000);
	//Destruct
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteTextures(2, textures);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	SDL_GL_DeleteContext(context);
	SDL_Quit;

	return 0;
}