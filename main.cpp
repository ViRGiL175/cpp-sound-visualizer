#include "fmod.hpp"
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <cstring>

std::string get_file_contents(const char *filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize((unsigned int) in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	return "";
}

#define WIDTH 1280
#define HEIGHT 720
#define SHADER_FILE "D:/Code/Projects/sound-visualizer/Shader.frag"
#define SONG_FILE "D:/Code/Projects/sound-visualizer/BTO.mp3"

//Vertex shader
const GLchar *vertexSource =
		"#version 150 core\n"
				"in vec2 position;"
				"void main() {"
				"   gl_Position = vec4(position, 0.0, 1.0);"
				"}";

FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length,
                                     int inchannels, int *outchannels);

void checkError(FMOD_RESULT result);

struct ToGLStr {
	const char *p;

	ToGLStr(const std::string &s) : p(s.c_str()) { }

	operator const char **() { return &p; }
};

int main() {
	//initiate FMOD
	FMOD_SYSTEM *fmodSystem;
	FMOD_SOUND *fmodSound;
	FMOD_CHANNEL *fmodChannel = 0;
	FMOD_CHANNELGROUP *fmodMasterChannelGroup;
	FMOD_DSP *fmodDsp;

	FMOD_RESULT result;

	FMOD_System_Create(&fmodSystem);
	result = FMOD_System_Init(fmodSystem, 32, FMOD_INIT_NORMAL, 0);
	checkError(result);

	//Create window
	sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Visualizer", sf::Style::Close);
	window.setVerticalSyncEnabled(true);

	glewExperimental = GL_TRUE;
	glewInit();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);

	float vertices[] = {
			-1.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, -1.0f,
			-1.0f, 1.0f,
			-1.0f, -1.0f,
			1.0f, -1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	std::string shader = get_file_contents(SHADER_FILE);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, ToGLStr(shader), NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "gl_FragColor");
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the layout of the vertex data
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray((GLuint) posAttrib);
	glVertexAttribPointer((GLuint) posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLint timeLoc = glGetUniformLocation(shaderProgram, "iGlobalTime");

	GLint sampleLoc = glGetUniformLocation(shaderProgram, "Spectrum");
	GLint waveLoc = glGetUniformLocation(shaderProgram, "Wavedata");

	GLint resLoc = glGetUniformLocation(shaderProgram, "iResolution");

	glUniform3f(resLoc, WIDTH, HEIGHT, WIDTH * HEIGHT);

	while (window.isOpen()) {
		sf::Event windowEvent;
		while (window.pollEvent(windowEvent)) {
			switch (windowEvent.type) {
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
					switch (windowEvent.key.code) {
						case sf::Keyboard::P:
							result = FMOD_System_CreateSound(fmodSystem, SONG_FILE, FMOD_DEFAULT, 0, &fmodSound);
							checkError(result);
							result = FMOD_Sound_SetMode(fmodSound, FMOD_LOOP_OFF);
							checkError(result);
							result = FMOD_System_PlaySound(fmodSystem, fmodSound, 0, false, &fmodChannel);
							checkError(result);
							result = FMOD_System_Update(fmodSystem);
							checkError(result);
							break;
						case sf::Keyboard::R: //Reload the shader
							//hopefully this is safe
							glDeleteProgram(shaderProgram);
							glDeleteShader(fragmentShader);
							glDeleteShader(vertexShader);

							vertexShader = glCreateShader(GL_VERTEX_SHADER);
							glShaderSource(vertexShader, 1, &vertexSource, NULL);
							glCompileShader(vertexShader);

							// Create and compile the fragment shader
							shader = get_file_contents(SHADER_FILE);

							fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
							glShaderSource(fragmentShader, 1, ToGLStr(shader), NULL);
							glCompileShader(fragmentShader);

							// Link the vertex and fragment shader into a shader program
							shaderProgram = glCreateProgram();
							glAttachShader(shaderProgram, vertexShader);
							glAttachShader(shaderProgram, fragmentShader);
							glBindFragDataLocation(shaderProgram, 0, "gl_FragColor");
							glLinkProgram(shaderProgram);
							glUseProgram(shaderProgram);

							// Specify the layout of the vertex data
							posAttrib = glGetAttribLocation(shaderProgram, "position");
							glEnableVertexAttribArray(posAttrib);
							glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

							timeLoc = glGetUniformLocation(shaderProgram, "iGlobalTime");

							sampleLoc = glGetUniformLocation(shaderProgram, "Spectrum");
							waveLoc = glGetUniformLocation(shaderProgram, "Wavedata");

							resLoc = glGetUniformLocation(shaderProgram, "iResolution");

							glUniform3f(resLoc, WIDTH, HEIGHT, WIDTH * HEIGHT);
							break;
						case sf::Keyboard::Escape:
							window.close();
							break;
					}
					break;
			}
		}


		float spectrum[256];
		float waveData[512];
		float X = 1;

//		Create the DSP effect.
		{
			FMOD_DSP_DESCRIPTION fmodDspDescription;
			memset(&fmodDspDescription, 0, sizeof(fmodDspDescription));

			strncpy(fmodDspDescription.name, "Spectrum DSP unit", sizeof(fmodDspDescription.name));
			fmodDspDescription.version = 0x00010000;
			fmodDspDescription.numinputbuffers = 1;
			fmodDspDescription.numoutputbuffers = 1;
			fmodDspDescription.read = myDSPCallback;
			fmodDspDescription.userdata = (void *) 0x12345678;

			result = FMOD_System_CreateDSP(fmodSystem, &fmodDspDescription, &fmodDsp);
			checkError(result);
		}

//		Attach the DSP, inactive by default.

		result = FMOD_DSP_SetBypass(fmodDsp, true);
		checkError(result);
		result = FMOD_System_GetMasterChannelGroup(fmodSystem, &fmodMasterChannelGroup);
		checkError(result);
		result = FMOD_ChannelGroup_AddDSP(fmodMasterChannelGroup, 0, fmodDsp);
		checkError(result);

		FMOD_DSP_GetParameterFloat(fmodDsp, FMOD_DSP_FFT_SPECTRUMDATA, spectrum, NULL, 8);


		for (int i = 0; i < 512; i++) {
			waveData[i] = static_cast <float> (rand()) / (RAND_MAX / X);
		}

		std::cout << spectrum[4] << std::endl;

		GLfloat time = (GLfloat) clock() / (GLfloat) CLOCKS_PER_SEC;
		glUniform1f(timeLoc, time);
		glUniform1fv(sampleLoc, 256, spectrum);
		glUniform1fv(waveLoc, 256, waveData);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		window.display();
	}

	//Clean up
	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);
}

FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length,
                                     int inchannels, int *outchannels) {
	FMOD_RESULT result;
	char name[256];
	void *userData;
	FMOD_DSP *fmodDsp = dsp_state->instance;

	/*
		This redundant call just shows using the instance parameter of FMOD_DSP_STATE to
		call a DSP information function.
	*/
	result = FMOD_DSP_GetInfo(fmodDsp, name, 0, 0, 0, 0);
	checkError(result);

	result = FMOD_DSP_GetUserData(fmodDsp, &userData);
	checkError(result);

	/*
		This loop assumes inchannels = outchannels, which it will be if the DSP is created with '0'
		as the number of channels in FMOD_DSP_DESCRIPTION.
		Specifying an actual channel count will mean you have to take care of any number of channels coming in,
		but outputting the number of channels specified. Generally it is best to keep the channel
		count at 0 for maximum compatibility.
	*/
	for (unsigned int samp = 0; samp < length; samp++) {
		/*
			Feel free to unroll this.
		*/
		for (int chan = 0; chan < *outchannels; chan++) {
			/*
				This DSP filter just halves the volume!
				Input is modified, and sent to output.
			*/
			outbuffer[(samp * *outchannels) + chan] = inbuffer[(samp * inchannels) + chan] * 0.2f;
		}
	}

	return FMOD_OK;
}

void checkError(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		std::cout << result << std::endl;
	}
}