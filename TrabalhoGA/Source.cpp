#include <iostream>
#include <string>
#include <assert.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Common/include/Shader.h"

using namespace std;

// Variáveis globais para a câmera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Dimensões da janela
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Configuração do controle do mouse
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = WIDTH / 2.0;
float lastY = HEIGHT / 2.0;
float sensitivity = 0.1f;
float zoomSpeed = 1.0f;

// Estrutura para os objetos 3D
struct Object {
    GLuint VAO;
    int nVertices;
    glm::mat4 model;
    float rotationX;
    float rotationY;
    float rotationZ;
    glm::vec3 translation;
    float scale;
};

// Declara os objetos 3D
Object obj1, obj2, obj3;
int selectedObject = 1;

// Declara as funções
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
int loadSimpleOBJ(string filePATH, int& nVertices);
void applyTransformations(Object& obj);

int main()
{
    // Inicialização do GLFW
    glfwInit();

    // Criação da janela
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, " Visualizador de cenas 3D", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Configuração das funções de callback e mouse
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializa o GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // Informações do renderizador e versão do OpenGL
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Configuração da viewport
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Carrega os shaders
    Shader shader("C:\\Users\\I555002\\OneDrive - SAP SE\\Desktop\\CG2024-2\\TrabalhoGA\\phong.vs", "C:\\Users\\I555002\\OneDrive - SAP SE\\Desktop\\CG2024-2\\TrabalhoGA\\phong.fs");
    glUseProgram(shader.ID);

    // Carrega os objetos 3D
    obj1.VAO = loadSimpleOBJ("C:\\Users\\I555002\\OneDrive - SAP SE\\Desktop\\CG2024-2\\TrabalhoGA\\Nave.obj", obj1.nVertices);
    obj2.VAO = loadSimpleOBJ("C:\\Users\\I555002\\OneDrive - SAP SE\\Desktop\\CG2024-2\\TrabalhoGA\\Suzanne.obj", obj2.nVertices); 
    obj3.VAO = loadSimpleOBJ("C:\\Users\\I555002\\OneDrive - SAP SE\\Desktop\\CG2024-2\\TrabalhoGA\\cube.obj", obj3.nVertices); 

    // Localização da matriz de modelo
    GLint modelLoc = glGetUniformLocation(shader.ID, "model");

    // Configuração da matriz de visualização
    glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

    // Configuração da matriz de projeção
    glm::mat4 projection = glm::perspective(glm::radians(39.6f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Habilita o teste de profundidade
    glEnable(GL_DEPTH_TEST);

    // Configurações de iluminação
    shader.setFloat("ka", 0.2);
    shader.setFloat("ks", 0.5);
    shader.setFloat("kd", 0.5);
    shader.setFloat("q", 10.0);
    shader.setVec3("lightPos", -2.0, 10.0, 3.0);
    shader.setVec3("lightColor", 1.0, 1.0, 1.0);

    // Escala inicial dos objetos
    obj1.scale = 1.0f;
    obj2.scale = 1.0f;
    obj3.scale = 1.0f;

    // Loop principal da aplicação
    while (!glfwWindowShouldClose(window))
    {
        // Verifica eventos (teclado, mouse, etc.)
        glfwPollEvents();

        // Limpa o buffer de cor e profundidade
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glLineWidth(10);
        glPointSize(20);

        // Aplica transformações a cada objeto
        applyTransformations(obj1);
        applyTransformations(obj2);
        applyTransformations(obj3);

        // Desenha o objeto 1
        glBindVertexArray(obj1.VAO);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj1.model));
        glDrawArrays(GL_TRIANGLES, 0, obj1.nVertices);

        // Desenha o objeto 2
        glBindVertexArray(obj2.VAO);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj2.model));
        glDrawArrays(GL_TRIANGLES, 0, obj2.nVertices);

        // Desenha o objeto 3
        glBindVertexArray(obj3.VAO);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(obj3.model));
        glDrawArrays(GL_TRIANGLES, 0, obj3.nVertices);

        // Atualiza a matriz de visualização com a posição da câmera
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Atualiza a posição da câmera no shader
        shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

        // Troca os buffers e exibe o próximo frame
        glfwSwapBuffers(window);
    }
    // Libera os recursos
    glDeleteVertexArrays(1, &obj1.VAO);
    glDeleteVertexArrays(1, &obj2.VAO);
    glDeleteVertexArrays(1, &obj3.VAO);
    glfwTerminate(); // Finaliza o GLFW
    return 0; // Finaliza o programa
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Verifica se é a primeira vez que o mouse é movido
    if (firstMouse)
    {
        lastX = xpos; // Armazena a posição inicial do mouse em X
        lastY = ypos; // Armazena a posição inicial do mouse em Y
        firstMouse = false; // Atualiza a variável para não entrar mais aqui
    }

    // Calcula o deslocamento do mouse e atualiza posição
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // Aplica a sensibilidade do mouse ao deslocamento
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Atualiza os ângulos de visão (yaw e pitch)
    yaw += xoffset;
    pitch += yoffset;

    // Limita o pitch para evitar uma rotação excessiva
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Calcula a direção para onde a câmera está olhando
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Normaliza o vetor de direção
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Ajusta a posição da câmera com base na rolagem do mouse
    cameraPos += cameraFront * (float)yoffset * zoomSpeed;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Fecha a janela se a tecla ESC for pressionada
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Seleciona o objeto baseado na tecla pressionada
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
        selectedObject = 1;
    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
        selectedObject = 2;
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
        selectedObject = 3;

    // Ponto de referência do objeto selecionado
    Object* currentObj = (selectedObject == 1) ? &obj1 : (selectedObject == 2) ? &obj2 : &obj3;

    // Controle de rotação do objeto em X, Y e Z
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
    {
        currentObj->rotationX += glm::radians(15.0f);
    }

    if (key == GLFW_KEY_Y && action == GLFW_PRESS)
    {
        currentObj->rotationY += glm::radians(15.0f);
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
    {
        currentObj->rotationZ += glm::radians(15.0f);
    }

    // Controle de translação do objeto em X, Y e Z
    float translationSpeed = 0.1f;
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para cima
    {
        currentObj->translation.y += translationSpeed;
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para baixo
    {
        currentObj->translation.y -= translationSpeed;
    }
    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para a esquerda
    {
        currentObj->translation.x -= translationSpeed;
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para a direita
    {
        currentObj->translation.x += translationSpeed;
    }
    if (key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para frente
    {
        currentObj->translation.z -= translationSpeed;
    }
    if (key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Move para trás
    {
        currentObj->translation.z += translationSpeed;
    }

    // Controle de escala
    float scaleSpeed = 0.1f;
    if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Aumenta escala
    {
        currentObj->scale += scaleSpeed;
    }
    if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT)) // Diminui escala
    {
        currentObj->scale -= scaleSpeed;
    }
}

void applyTransformations(Object& obj)
{
    // Inicializa a matriz do modelo como a matriz identidade
    obj.model = glm::mat4(1.0f);

    // Aplica rotação nos eixos X, Y e Z
    obj.model = glm::rotate(obj.model, obj.rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
    obj.model = glm::rotate(obj.model, obj.rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
    obj.model = glm::rotate(obj.model, obj.rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

    // Aplica translação 
    obj.model = glm::translate(obj.model, obj.translation);

    // Aplica escala
    obj.model = glm::scale(obj.model, glm::vec3(obj.scale, obj.scale, obj.scale));
}

int loadSimpleOBJ(string filePath, int& nVertices)
{
    // Vetores para armazenar os dados do objeto
    vector <glm::vec3> vertices;
    vector <glm::vec2> texCoords;
    vector <glm::vec3> normals;
    vector <GLfloat> vBuffer;

    // Define uma cor padrão
    glm::vec3 color = glm::vec3(1.0, 0.0, 0.0);

    // Abre o arquivo OBJ
    ifstream arqEntrada;

    arqEntrada.open(filePath.c_str());
    if (arqEntrada.is_open())
    {
        string line;
        // Lê o arquivo linha por linha
        while (!arqEntrada.eof())
        {
            getline(arqEntrada, line);
            istringstream ssline(line);
            string word;
            ssline >> word;

            // Processa os vértices
            if (word == "v")
            {
                glm::vec3 vertice;
                ssline >> vertice.x >> vertice.y >> vertice.z;
                vertices.push_back(vertice);
            }

            // Processa as coordenadas de textura
            if (word == "vt")
            {
                glm::vec2 vt;
                ssline >> vt.s >> vt.t;
                texCoords.push_back(vt);
            }

            // Processa as normais
            if (word == "vn")
            {
                glm::vec3 normal;
                ssline >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            }

            // Processa as faces
            else if (word == "f")
            {
                while (ssline >> word)
                {
                    int vi, ti, ni;
                    istringstream ss(word);
                    std::string index;

                    std::getline(ss, index, '/');
                    vi = std::stoi(index) - 1;

                    std::getline(ss, index, '/');
                    ti = std::stoi(index) - 1;

                    std::getline(ss, index);
                    ni = std::stoi(index) - 1;

                    vBuffer.push_back(vertices[vi].x);
                    vBuffer.push_back(vertices[vi].y);
                    vBuffer.push_back(vertices[vi].z);

                    vBuffer.push_back(color.r);
                    vBuffer.push_back(color.g);
                    vBuffer.push_back(color.b);

                    vBuffer.push_back(texCoords[ti].s);
                    vBuffer.push_back(texCoords[ti].t);

                    vBuffer.push_back(normals[ni].x);
                    vBuffer.push_back(normals[ni].y);
                    vBuffer.push_back(normals[ni].z);
                }
            }
        }
        // Fecha o arquivo após leitura
        arqEntrada.close();

        // Gera o buffer de geometria
        cout << "Gerando o buffer de geometria..." << endl;
        GLuint VBO, VAO;

        // Gera o buffer de vértices
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);

        // Gera o Vertex Array Object (VAO)
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // Configura os atributos de vértices
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);

        // Desativa o buffer e o VAO após configuração
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        nVertices = vBuffer.size() / 2;
        return VAO;
    }
    else
    {
        // Erro ao abrir o arquivo
        cout << "Erro ao tentar ler o arquivo " << filePath << endl;
        return -1;
    }
}