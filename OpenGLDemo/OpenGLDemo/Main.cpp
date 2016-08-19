#include <GL\glew.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_opengl.h>
#include <Windows.h>


#define GLSL(src) "#version 450 core\n" #src

int main(int argc, char *argv[])
{
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

	GLuint vbo;
	glGenBuffers(1, &vbo); //Generate 1 buffer

						   //Create a Vertex Buffer Object and copy the vertex data to it
	GLfloat vertices[] =
	{
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
		0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f  // Bottom-left
	};

	/*
	GLfloat vertices[] =
	{
	0.0f, 0.5f, 1.0f, 0.0f, 0.0f,	//Vertex 1, Red, Top-left
	0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	//Vertex 2, Blue, Top-right
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, //Vertex 3, Green, Bottom-righ
	};
	*/

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
	const char* vertexSource = GLSL
	(
		in vec2 position;
	in vec3 color;

	out vec3 Color;

	void main()
	{
		Color = color;
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

	out vec4 outColor;

	void main()
	{
		outColor = vec4(Color, 1.0);
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

	//Specify layout of vertices
	GLint positionAttribute = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	GLint colorAttribute = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colorAttribute);
	glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	//Main loop
	bool running = true;
	SDL_Event windowEvent;
	while (running)
	{
		/*
		GLint uniColor = glGetUniformLocation(shaderProgram, "triangleColor");
		glUniform3f(uniColor, 1.0f, 0.0f, 0.0f); //glUniformXY, X is the number of components, Y is the type (e.g f = float, i = int, etc.), parameters sets color of triangle.
		*/

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

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);

		//Swap buffers
		SDL_GL_SwapWindow(window);
	}

	SDL_Delay(1000);
	//De-struct
	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	SDL_GL_DeleteContext(context);
	SDL_Quit;

	return 0;
}