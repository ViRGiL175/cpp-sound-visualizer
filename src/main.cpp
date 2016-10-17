#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>
#include <map>
#include "FFTAudioStream.h"

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
//#define SHADER_FILE "D:/Code/Projects/sound-visualizer/Shader.frag"
//#define SHADER_FILE "D:/Code/Projects/sound-visualizer/Shader_equalizer.frag"
#define SONG_FILE "D:/Code/Projects/sound-visualizer/BTO.ogg"

enum VISUALIZATION_MODE {
    WAVE, SPECTRUM, EQUALIZER
};

static const int WAVE_DATA_SIZE = 256;
static const int EQUALIZER_COLUMNS = 32;
static const int EQUALIZER_INNERTION = 500;
static const int COLUMNS_MARGIN = 4;

float waveData[WAVE_DATA_SIZE];
float spectrumData[WAVE_DATA_SIZE];
float previousSpectrumData[WAVE_DATA_SIZE];
float visualSpectrumData[WAVE_DATA_SIZE];
int spectrumIterator = 0;
int columnsInertia[EQUALIZER_COLUMNS];
std::map<int, VISUALIZATION_MODE> visualizerModesMap;
int currentVisualizerMode = 0;

std::string audioFilePath = "D:/Code/Projects/sound-visualizer/BTO.ogg";
std::string shaderFilePath = "D:/Code/Projects/sound-visualizer/Shader_equalizer.frag";
sf::SoundBuffer soundBuffer;
FFTAudioStream fftAudioStream;
GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;
GLint posAttrib;
GLint timeLoc;
GLint sampleLoc;
GLint waveLoc;
GLint resLoc;
GLuint vao;
GLuint vbo;

float shaderVertices[] = {
        -1.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f
};


// Vertex shader
const GLchar *vertexSource =
        "#version 150 core\n"
                "in vec2 position;"
                "void main() {"
                "   gl_Position = vec4(position, 0.0, 1.0);"
                "}";

struct ToGLStr {
    const char *p;

    ToGLStr(const std::string &s) : p(s.c_str()) {}

    operator const char **() { return &p; }
};

void equalizerModesInitialization();

void loadAudioFile(std::string filePath);

void deleteShader();

void loadShader(std::string shaderFilePath);

void openGLInitialization();

int main() {

    equalizerModesInitialization();
    loadAudioFile(audioFilePath);

    // Create window
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Visualizer", sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    openGLInitialization();
    loadShader(shaderFilePath);

    while (window.isOpen()) {
        sf::Event windowEvent;
        while (window.pollEvent(windowEvent)) {
            switch (windowEvent.type) {
                case sf::Event::Closed:
                    fftAudioStream.stop();
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (windowEvent.key.code) {
                        case sf::Keyboard::P:
                            // Play Sound
                            fftAudioStream.getStatus() != sf::SoundSource::Status::Playing
                            ? fftAudioStream.play()
                            : fftAudioStream.pause();
                            break;
                        case sf::Keyboard::M:
//                          Mode switching
                            currentVisualizerMode == visualizerModesMap.size() - 1
                            ? (currentVisualizerMode = 0)
                            : (currentVisualizerMode++);
                            switch (visualizerModesMap[currentVisualizerMode]) {
                                case WAVE:
                                    break;
                                case SPECTRUM:
                                    break;
                                case EQUALIZER:
                                    break;
                            }
                            break;
                        case sf::Keyboard::R: //Reload the shader
                            //hopefully this is safe
                            deleteShader();
                            loadShader(shaderFilePath);
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

        const auto filteredSpectrumDataVector = fftAudioStream.getCurrentSampleSpectrumVector();

//        if (filteredSpectrumDataVector.data() != NULL) {
//            int picker = FFTAudioStream::SAMPLES_TO_STREAM / 4 / WAVE_DATA_SIZE;
//            for (int i = 0; i < WAVE_DATA_SIZE; i++) {
//                spectrumData[i] = (float) filteredSpectrumDataVector[i * picker].re();
//                spectrumData[i] *= 0.000001;
////                spectrumData[i] = 0;
//            }
//        }

        if (filteredSpectrumDataVector.data() != NULL) {
            int picker = FFTAudioStream::SAMPLES_TO_STREAM / 4 / WAVE_DATA_SIZE;
            float sum = 0;
            int columnWidth = WAVE_DATA_SIZE / EQUALIZER_COLUMNS;
            for (int columnNumber = 0; columnNumber < EQUALIZER_COLUMNS; columnNumber++) {
                for (int i = 0; i < columnWidth - COLUMNS_MARGIN; i++) {
                    int currentIndex = columnWidth * columnNumber + i;
                    sum += filteredSpectrumDataVector[currentIndex * picker].re();
                }
                sum *= 0.00000015;
                for (int i = 0; i < columnWidth - COLUMNS_MARGIN; i++) {
                    int currentIndex = columnWidth * columnNumber + i;
                    if (sum > previousSpectrumData[currentIndex]) {
                        columnsInertia[columnNumber] = EQUALIZER_INNERTION;
                        spectrumData[currentIndex] = sum;
                    } else {
                        spectrumData[currentIndex] = previousSpectrumData[currentIndex] -
                                                     previousSpectrumData[currentIndex] *
                                                     ((float) EQUALIZER_INNERTION + 1 -
                                                      columnsInertia[columnNumber]) / ((float) EQUALIZER_INNERTION);
                    }
                }
                sum = 0;
            }
        }

        for (int i = 0; i < EQUALIZER_COLUMNS; ++i) {
            if (columnsInertia[i] != 0) {
                columnsInertia[i]--;
            }
        }

        for (int i = 0; i < WAVE_DATA_SIZE; ++i) {
            previousSpectrumData[i] = spectrumData[i];
        }

        glUniform1fv(sampleLoc, WAVE_DATA_SIZE, spectrumData);

        const auto filteredWaveDataVector = fftAudioStream.getCurrentSampleWaveVector();

//        if (filteredWaveDataVector.data() != NULL) {
//            int picker = FFTAudioStream::SAMPLES_TO_STREAM / WAVE_DATA_SIZE;
//            for (int i = 0; i < WAVE_DATA_SIZE; i++) {
//                waveData[i] = (float) filteredWaveDataVector[i * picker].re();
//                waveData[i] *= 0.0001;
////                waveData[i] *= 0.0;
//            }
//        }

        glUniform1fv(waveLoc, WAVE_DATA_SIZE, spectrumData);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        window.display();
    }

    // Clean up
    deleteShader();
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void openGLInitialization() {
    glewExperimental = GL_TRUE;
    glewInit();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shaderVertices), shaderVertices, GL_STATIC_DRAW);
}

void loadShader(std::string shaderFilePath) {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the fragment shader
    std::string shaderData = get_file_contents(shaderFilePath.data());

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, ToGLStr(shaderData), NULL);
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
}

void loadAudioFile(std::string filePath) {
    // load an audio soundBuffer from a sound file
    soundBuffer.loadFromFile(audioFilePath);
    // initialize and play our custom stream
    fftAudioStream.load(soundBuffer);
}

void equalizerModesInitialization() {
    visualizerModesMap[0] = WAVE;
    visualizerModesMap[1] = SPECTRUM;
    visualizerModesMap[2] = EQUALIZER;
}

void deleteShader() {
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}