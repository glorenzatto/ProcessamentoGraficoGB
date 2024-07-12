#define _CRT_SECURE_NO_WARNINGS
#include "gl_utils.cpp"
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <vector>
#include "TileMap.cpp"
#include "TilemapView.h"
#include "DiamondView.h"
#include <fstream>
#include "Tank.h"
#include "CharacterController.h"
#include "InputController.h"

using namespace std;

// Declaração de variáveis globais
float frameWidth = 0.25f;
float frameHeight = 0.25f;
int directionSign = 1;
float previousTime = glfwGetTime();

int g_gl_width = 900;
int g_gl_height = 900;

float xInitial = -1.0f;
float xFinal = 1.0f;

float yInitial = -1.0f;
float yFinal = -1.0f;

float mapWidth = xFinal - xInitial;
float mapHeight = yFinal - yInitial;

float tileWidth;
float tileHeight;
float halfTileWidth;
float halfTileHeight;

int tileSetColumns = 9;
int tileSetRows = 9;

float tileTextureWidth;
float halfTileTextureWidth;

float tileTextureHeight;
float halfTileTextureHeight;

int characterX;
int characterY;

float textureX;
float textureY;

int tanksCollected;
bool isGameOver = false;

TilemapView* tilemapView = new DiamondView();
TileMap* terrainMap = NULL;
TileMap* waterMap = NULL;

std::vector<Tank> tanks;

GLFWwindow* g_window = NULL;

// Função para ler um mapa de tiles de um arquivo
TileMap* readMap(const char* filename) {
    ifstream arq(filename);
    int width, height;
    arq >> width >> height;
    TileMap* tileMap = new TileMap(width, height, 0);
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            int tileId;
            arq >> tileId;
            tileMap->setTile(c, height - r - 1, tileId);
        }
    }
    arq.close();
    return tileMap;
}

// Função para carregar uma textura
int loadTexture(unsigned int& texture, const char* filename)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data)
    {
        if (nrChannels == 4)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            cout << "Without Alpha channel" << endl;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    return 1;
}

// Função para inicializar as posições dos tanques de grito
void initializeTanks() {
    tanks.emplace_back(5, 13, true); // x, y ambos começam em 0
    tanks.emplace_back(12, 12, true);
    tanks.emplace_back(9, 8, true);
    tanks.emplace_back(0, 7, true);
    tanks.emplace_back(6, 0, true);
    tanks.emplace_back(11, 0, true);
}

// Função para criar um tanque de grito na tela
void createTank(GLuint tankTexture, GLuint shader_programme, int x, int y) {
    glUniform1f(glGetUniformLocation(shader_programme, "isObject"), true);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tankTexture);
    glUniform1i(glGetUniformLocation(shader_programme, "sprite"), 0);

    tilemapView->computeDrawPosition(x, y, tileWidth, tileHeight, textureX, textureY);
    glUniform1f(glGetUniformLocation(shader_programme, "tx"), 1.23f + textureX);
    glUniform1f(glGetUniformLocation(shader_programme, "ty"), 0.78f + textureY);
    glUniform1f(glGetUniformLocation(shader_programme, "offsetx"), 1);
    glUniform1f(glGetUniformLocation(shader_programme, "offsety"), 1);
    glUniform1f(glGetUniformLocation(shader_programme, "applyOffset"), false);
    glUniform1f(glGetUniformLocation(shader_programme, "layer_z"), 0.10f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

// Função para renderizar todos os tanques de grito na tela
void renderTanks(GLuint tankTexture, GLuint shader_programme) {
    for (const auto& tank : tanks) {
        if (tank.create) {
            createTank(tankTexture, shader_programme, tank.x, tank.y);
        }
    }
}

CharacterController* characterController = nullptr;
InputController* inputController = nullptr;

void initializeControllers() {
    characterController = new CharacterController(tilemapView, waterMap, tanks, tanksCollected, isGameOver);
    inputController = new InputController(g_window);
}

int main()
{
    restart_gl_log();
    start_gl();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Carrega os mapas de tiles
    terrainMap = readMap("tilemaps/terrain.tmap");
    waterMap = readMap("tilemaps/watter.tmap");
    tileWidth = mapWidth / (float)terrainMap->getWidth();
    tileHeight = tileWidth / 2.0f;
    halfTileWidth = tileHeight;
    halfTileHeight = tileHeight / 2.0f;
    tileTextureWidth = 1.0f / (float)tileSetColumns;
    halfTileTextureWidth = tileTextureWidth / 2.0f;
    tileTextureHeight = 1.0f / (float)tileSetRows;
    halfTileTextureHeight = tileTextureHeight / 2.0f;

    // Carrega as texturas
    GLuint terrainTexture;
    loadTexture(terrainTexture, "images/terrain.png");
    terrainMap->setTid(terrainTexture);

    GLuint characterTexture;
    loadTexture(characterTexture, "images/sully.png");

    GLuint tankTexture;
    loadTexture(tankTexture, "images/shout_tank.png");

    // Define os vértices do mapa, personagem e tanque de grito
    float mapVertices[] = {
        // positions   // texture coords
        xInitial, yInitial + halfTileHeight, 0.0f, halfTileTextureHeight,   // left
        xInitial + halfTileWidth, yInitial, halfTileTextureWidth, 0.0f,   // bottom
        xInitial + tileWidth, yInitial + halfTileHeight, tileTextureWidth, halfTileTextureHeight,  // right
        xInitial + halfTileWidth, yInitial + tileHeight, halfTileTextureWidth, tileTextureHeight,  // top
    };
    unsigned int indexes[] = {
        0, 1, 3, // first triangle
        3, 1, 2  // second triangle
    };

    float characterVertices[] = {
         -2.6f * 0.8f, -0.8f * 0.8f, 0.25f, 0.25f, // top right
         -2.6f * 0.8f, -1.0f * 0.8f, 0.25f, 0.0f, // bottom right
         -2.8f * 0.8f, -1.0f * 0.8f, 0.0f, 0.0f, // bottom left
         -2.8f * 0.8f, -0.8f * 0.8f, 0.0f, 0.25f, // top left
    };

    float tankVertices[] = {
         -2.6f * 0.2f, -0.8f * 0.2f, 0.25f, 0.25f, // top right
         -2.6f * 0.2f, -1.0f * 0.2f, 0.25f, 0.0f, // bottom right
         -2.8f * 0.2f, -1.0f * 0.2f, 0.0f, 0.0f, // bottom left
         -2.8f * 0.2f, -0.8f * 0.2f, 0.0f, 0.25f, // top left
    };

    // Configuração dos buffers e arrays
    unsigned int VBOMap, VBOCharacter, VBOTank, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBOMap);
    glGenBuffers(1, &VBOCharacter);
    glGenBuffers(1, &VBOTank);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBOMap);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mapVertices), mapVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBOCharacter);
    glBufferData(GL_ARRAY_BUFFER, sizeof(characterVertices), characterVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, VBOTank);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tankVertices), tankVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(5);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Carrega e compila os shaders
    char vertex_shader[1024 * 256];
    char fragment_shader[1024 * 256];
    parse_file_into_str("shaders/vertex_shader.glsl", vertex_shader, 1024 * 256);
    parse_file_into_str("shaders/fragment_shader.glsl", fragment_shader, 1024 * 256);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* p = (const GLchar*)vertex_shader;
    glShaderSource(vs, 1, &p, NULL);
    glCompileShader(vs);

    // Verifica erros de compilação do shader de vértice
    int params = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile.\n", vs);
        print_shader_info_log(vs);
        return 1; // or exit or something
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    p = (const GLchar*)fragment_shader;
    glShaderSource(fs, 1, &p, NULL);
    glCompileShader(fs);

    // Verifica erros de compilação do shader de fragmento
    glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: GL shader index %i did not compile.\n", fs);
        print_shader_info_log(fs);
        return 1; // or exit or something
    }

    GLuint shader_programme = glCreateProgram();
    glAttachShader(shader_programme, fs);
    glAttachShader(shader_programme, vs);
    glLinkProgram(shader_programme);

    glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
    if (GL_TRUE != params)
    {
        fprintf(stderr, "ERROR: could not link shader programme GL index %i.\n",
            shader_programme);
        return false;
    }

    // Inicializa o OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initializeTanks();
    initializeControllers();

    glfwSetWindowTitle(g_window, "Gustavo Lorenzatto Cauduro - GB P.G.");

    cout << "\n------- SEJA BEM-VINDO ---------\n\nSeu objetivo eh fazer Sully coletar todos os tanques de grito e chegar na posicao final para se tornar o monstro mais assustador!\nUsando os direcionais do teclado e as teclas ctrl e shift, percorra o mapa evitando pisar na agua ou sair dos limites.\nBoa sorte!\n\n--------------------------------\n" << endl;

    // Loop principal do jogo
    while (!glfwWindowShouldClose(g_window))
    {
        double current_seconds = glfwGetTime();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, g_gl_width, g_gl_height);

        glUseProgram(shader_programme);
        glBindVertexArray(VAO);

        // Renderiza o mapa
        glBindBuffer(GL_ARRAY_BUFFER, VBOMap);
        glUniform1f(glGetUniformLocation(shader_programme, "isObject"), false);

        float x, y;
        int r = 0, c = 0;
        for (int r = 0; r < terrainMap->getHeight(); r++) {
            for (int c = 0; c < terrainMap->getWidth(); c++) {
                int t_id = (int)terrainMap->getTile(c, r);
                int u = t_id % tileSetColumns;
                int v = t_id / tileSetColumns;

                tilemapView->computeDrawPosition(c, r, tileWidth, tileHeight, x, y);

                glUniform1f(glGetUniformLocation(shader_programme, "offsetx"), u * tileTextureWidth);
                glUniform1f(glGetUniformLocation(shader_programme, "offsety"), v * tileTextureHeight);
                glUniform1f(glGetUniformLocation(shader_programme, "applyOffset"), true);
                glUniform1f(glGetUniformLocation(shader_programme, "tx"), x);
                glUniform1f(glGetUniformLocation(shader_programme, "ty"), y + 1.0);
                glUniform1f(glGetUniformLocation(shader_programme, "layer_z"), 0.50);

                if (c == terrainMap->getWidth() - 1 && r == (terrainMap->getHeight() - 5)) {
                    glUniform1f(glGetUniformLocation(shader_programme, "weight"), 0.5);
                }
                else {
                    glUniform1f(glGetUniformLocation(shader_programme, "weight"), (c == characterX) && (r == characterY) ? 0.38 : 0.0);
                }

                glBindTexture(GL_TEXTURE_2D, terrainMap->getTileSet());
                glUniform1i(glGetUniformLocation(shader_programme, "sprite"), 0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }

        // Renderiza o personagem
        glBindBuffer(GL_ARRAY_BUFFER, VBOCharacter);
        glUniform1f(glGetUniformLocation(shader_programme, "isObject"), true);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, characterTexture);
        glUniform1i(glGetUniformLocation(shader_programme, "sprite"), 0);
        glUseProgram(shader_programme);

        tilemapView->computeDrawPosition(characterX, characterY, tileWidth, tileHeight, textureX, textureY);
        glUniform1f(glGetUniformLocation(shader_programme, "tx"), 1.22 + textureX);
        glUniform1f(glGetUniformLocation(shader_programme, "ty"), 0.82 + textureY);
        glUniform1f(glGetUniformLocation(shader_programme, "offsetx"), characterController->offsetX);
        glUniform1f(glGetUniformLocation(shader_programme, "offsety"), characterController->offsetY);
        glUniform1f(glGetUniformLocation(shader_programme, "applyOffset"), true);
        glUniform1f(glGetUniformLocation(shader_programme, "layer_z"), 0.10);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Renderiza os tanques de grito
        renderTanks(tankTexture, shader_programme);

        // Processa os eventos do GLFW
        glfwPollEvents();
        inputController->update();

        // Movimenta o personagem de acordo com as teclas pressionadas
        if (inputController->isCtrlPressed() && inputController->wasDownPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_SOUTHEAST);
            characterController->action = 2;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }
        else if (inputController->isCtrlPressed() && inputController->wasUpPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_NORTHEAST);
            characterController->action = 2;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }
        else if (inputController->isShiftPressed() && inputController->wasDownPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_SOUTHWEST);
            characterController->action = 1;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }
        else if (inputController->isShiftPressed() && inputController->wasUpPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_NORTHWEST);
            characterController->action = 1;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }
        else if (inputController->wasUpPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_NORTH);
            characterController->action = 0;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }
        else if (inputController->wasDownPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_SOUTH);
            characterController->action = 3;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }
        else if (inputController->wasLeftPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_EAST);
            characterController->action = 1;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }
        else if (inputController->wasRightPressed()) {
            characterController->moveCharacter(characterX, characterY, DIRECTION_WEST);
            characterController->action = 2;
            characterController->currentFrame = (characterController->currentFrame + 1) % 4;
            characterController->offsetX = frameWidth * (float)characterController->currentFrame;
            characterController->offsetY = frameHeight * (float)characterController->action;
        }

        if (inputController->wasSpacePressed()) {
            characterController->restart(characterX, characterY);
        }

        // Atualiza a tela
        glfwSwapBuffers(g_window);
    }

    // Finaliza o GLFW
    glfwTerminate();
    delete terrainMap;
    delete waterMap;
    delete characterController;
    delete inputController;
    return 0;
}
