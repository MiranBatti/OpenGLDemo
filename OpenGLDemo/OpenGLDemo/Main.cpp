#include <GL\glew.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_opengl.h>
#include <Windows.h>
#include "stb_image.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <chrono>

#define GLSL(src) "#version 450 core\n" #src

int main(int argc, char *argv[])
{
	auto startingTimer = std::chrono::high_resolution_clock::now();
	//Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	
	//Must be set to create a window
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	SDL_Window *window = SDL_CreateWindow("Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL); //Create the window
	SDL_GLContext context = SDL_GL_CreateContext(window);
	
	//Initialize GLEW. Important that window context is created before init.
	glewExperimental = GL_TRUE;
	glewInit();
	if (glewInit() != GLEW_OK) {
		OutputDebugStringA("Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize OpenGL
	glEnable(GL_DEPTH_TEST);  //"If enabled, do depth comparisons and update the depth buffer."
//	glDepthRangef(1.0f, 0.0f);//Depth range is actually set automatically. 

	//Create Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vbo;
	glGenBuffers(1, &vbo); //Generate 1 buffer
	
	GLfloat vertices[] = {
		//  Position(X,Y,Z)      Color(R,G,B)           Texcoords(U,V)
		-0.5f, -0.5f, -0.5f,     1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,		 1.0f, 1.0f, 1.0f,		1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,	 1.0f, 1.0f, 1.0f,		0.0f, 1.0f, //fucking forgot the comma and it ruined my fucking life fuck evrthng
		
		//Planar reflection
		-1.0f, -1.0f, -0.5f,	 0.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		1.0f, -1.0f, -0.5f,		 0.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		1.0f,  1.0f, -0.5f,		 0.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		1.0f,  1.0f, -0.5f,		 0.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		-1.0f,  1.0f, -0.5f,	 0.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-1.0f, -1.0f, -0.5f,	 0.0f, 0.0f, 0.0f,		0.0f, 0.0f
	};
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
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
	*/

	//Create vertex shader
    const char* vertexSource = GLSL(
        in vec3 position;
        in vec3 color;
        in vec2 texCoord;

        out vec3 Color;
        out vec2 TexCoord;
        
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 project;
		uniform vec3 overrideColor;

        void main() {
            Color = overrideColor * color;
            TexCoord = texCoord;
            gl_Position = project * view * model * vec4(position, 1.0);
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

		void main()
		{
			vec4 texColor = mix(texture(texKitten, TexCoord), texture(texPuppy, TexCoord), 0.5);
			outColor = vec4(Color, 1.0) * texColor;
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
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	//Transform matrices
	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
	
	//Set up projection
	glm::mat4 view = glm::lookAt(
		glm::vec3(2.5f, 2.5f, 2.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f)
	);
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 project = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
	GLint uniProject = glGetUniformLocation(shaderProgram, "project");
	glUniformMatrix4fv(uniProject, 1, GL_FALSE, glm::value_ptr(project)); 

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//Set wrap parameter for coordinate s to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//Set wrap parameter for coordinate s to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//Filter texture using Linear filtering for a smoother texture/image
	//TODO: Try using mipmaps for higher quality textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Specify layout of vertex data
	GLint positionAttribute = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	GLint colorAttribute = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	GLint textureAttribute = glGetAttribLocation(shaderProgram, "texCoord");
	glEnableVertexAttribArray(textureAttribute);
	glVertexAttribPointer(textureAttribute, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

	GLint uniColor = glGetUniformLocation(shaderProgram, "overrideColor");

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

		//2D transformation
		auto currentTimer = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(currentTimer - startingTimer).count();
		glm::mat4 model;
		model = glm::rotate(model, time * glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		//Clear screen to white
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //DEPTH_BUFFER_BIT is a must when depth test capability is enabled

		// Draw a rectangle from the 2 triangles using 6 indices
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Draw cube
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glEnable(GL_STENCIL_TEST);
			//Draw floor
			glStencilFunc(GL_ALWAYS, 1, 0xFF); //Set any stencil to 1
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilMask(0xFF); //Write to stencil buffer
			glDepthMask(GL_FALSE); //Don't write to depth buffer
			glClear(GL_STENCIL_BUFFER_BIT); //Clear stencil buffer (0 by default)

			glDrawArrays(GL_TRIANGLES, 36, 6);

			//Draw cube reflection
			glStencilFunc(GL_EQUAL, 1, 0xFF); //Pass test if stencil value is 1
			glStencilMask(0x00); //Don't write anything to stencil buffer
			glDepthMask(GL_TRUE); //Write to depth buffer
			model = glm::scale(
				glm::translate(model, glm::vec3(0, 0, -1)),
				glm::vec3(1, 1, -1)
				);
			glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
			
			glUniform3f(uniColor, 0.3f, 0.3f, 0.3f);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			glUniform3f(uniColor, 1.0f, 1.0f, 1.0f);
		glDisable(GL_STENCIL_TEST);

		//Swap buffers
		SDL_GL_SwapWindow(window);
	}

	SDL_Delay(1000);
	//Destruct
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteTextures(2, textures);
//	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	SDL_GL_DeleteContext(context);
	SDL_Quit;

	return 0;
}