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

float spectrum[256];
float waveData[512];

//Vertex shader
const GLchar *vertexSource =
		"#version 150 core\n"
				"in vec2 position;"
				"void main() {"
				"   gl_Position = vec4(position, 0.0, 1.0);"
				"}";

FMOD_RESULT F_CALLBACK myDSPCallback(
		FMOD_DSP_STATE *dsp_state,
		float *inBuffer,
		float *outBuffer,
		unsigned int length,
		int inChannels,
		int *outChannels
);

void printSpectrum();

struct ToGLStr {
	const char *p;

	ToGLStr(const std::string &s) : p(s.c_str()) { }

	operator const char **() { return &p; }
};

int main() {
	//initiate FMOD
	FMOD_SYSTEM *fModSystem;
	FMOD_SOUND *fModSound;
	FMOD_CHANNEL *fModChannel = 0;
	FMOD_CHANNELGROUP *fModMasterChannelGroup;
	FMOD_DSP *fModFFTDsp, *fModWaveDataDsp;

	FMOD_RESULT result;

	FMOD_System_Create(&fModSystem);
	result = FMOD_System_Init(fModSystem, 32, FMOD_INIT_NORMAL, 0);

//	Create the WaveData DSP effect.
	{
		FMOD_DSP_DESCRIPTION fModWaveDataDspDescription;
		memset(&fModWaveDataDspDescription, 0, sizeof(fModWaveDataDspDescription));

		strncpy(fModWaveDataDspDescription.name, "Spectrum DSP unit", sizeof(fModWaveDataDspDescription.name));
		fModWaveDataDspDescription.version = 0x00010000;
		fModWaveDataDspDescription.numinputbuffers = 1;
		fModWaveDataDspDescription.numoutputbuffers = 1;
		fModWaveDataDspDescription.read = myDSPCallback;
		fModWaveDataDspDescription.userdata = (void *) 0x12345678;

		result = FMOD_System_CreateDSP(fModSystem, &fModWaveDataDspDescription, &fModWaveDataDsp);
	}

//  Create the FFT DSP effect.
	FMOD_System_CreateDSPByType(fModSystem, FMOD_DSP_TYPE_FFT, &fModFFTDsp);

//	Attach the DSPs, inactive by default.
	result = FMOD_DSP_SetBypass(fModFFTDsp, true);
	result = FMOD_DSP_SetBypass(fModWaveDataDsp, false);
	result = FMOD_System_GetMasterChannelGroup(fModSystem, &fModMasterChannelGroup);
	result = FMOD_ChannelGroup_AddDSP(fModMasterChannelGroup, 0, fModFFTDsp);
	result = FMOD_ChannelGroup_AddDSP(fModMasterChannelGroup, 0, fModWaveDataDsp);

	FMOD_DSP_GetParameterFloat(fModFFTDsp, FMOD_DSP_FFT_SPECTRUMDATA, spectrum, NULL, 8);

	printSpectrum();

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
							result = FMOD_System_CreateSound(fModSystem, SONG_FILE, FMOD_DEFAULT, 0, &fModSound);
							result = FMOD_Sound_SetMode(fModSound, FMOD_LOOP_OFF);
							result = FMOD_System_PlaySound(fModSystem, fModSound, 0, false, &fModChannel);
							result = FMOD_System_Update(fModSystem);
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
							glEnableVertexAttribArray((GLuint) posAttrib);
							glVertexAttribPointer((GLuint) posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

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

void printSpectrum() {
	for (int i = 0; i < 256; ++i) {
		std::cout << spectrum[i] << ' ';
	}
	std::cout << std::endl;
}

FMOD_RESULT F_CALLBACK myDSPCallback(FMOD_DSP_STATE *dsp_state, float *inBuffer, float *outBuffer, unsigned int length,
                                     int inChannels, int *outChannels) {
	FMOD_RESULT result;
	char name[256];
	void *userData;
	FMOD_DSP *fModDsp = dsp_state->instance;

	/*
		This redundant call just shows using the instance parameter of FMOD_DSP_STATE to
		call a DSP information function.
	*/
	result = FMOD_DSP_GetInfo(fModDsp, name, 0, 0, 0, 0);
	result = FMOD_DSP_GetUserData(fModDsp, &userData);

	int arraySize = length / 4;
	for (int i = 0; i < (arraySize); ++i) {
		waveData[i] = inBuffer[i];
	}

	/*
		This loop assumes inChannels = outChannels, which it will be if the DSP is created with '0'
		as the number of channels in FMOD_DSP_DESCRIPTION.
		Specifying an actual channel count will mean you have to take care of any number of channels coming in,
		but outputting the number of channels specified. Generally it is best to keep the channel
		count at 0 for maximum compatibility.
	*/
	for (unsigned int samp = 0; samp < length; samp++) {
		/*
			Feel free to unroll this.
		*/
		for (int chan = 0; chan < *outChannels; chan++) {
			/*
				This DSP filter just halves the volume!
				Input is modified, and sent to output.
			*/
			outBuffer[(samp * *outChannels) + chan] = inBuffer[(samp * inChannels) + chan] * 0.2f;
		}
	}

	return FMOD_OK;
}