// @formatter:off

#include "Camera.hpp"
#include "Shader.hpp"
#include "Texture.hpp"


#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <vector>

// @formatter:on


void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);


void processInput(GLFWwindow *window);

void screenRay(GLFWwindow *window, glm::vec3 &origin, glm::vec3 &dir);


float quadrantColors[4][3] = {
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f}};

const float quadrantOffsets[4][2] = {
        {-1.0f, 0.0f},
        {0.0f, 0.0f},
        {-1.0f, -1.0f},
        {0.0f, -1.0f}};

int quadrantMix[4] = {0, 0, 0, 0};
int quadrantHasColor[4] = {0, 0, 0, 0};

struct CubeInfo {
    float x, z;
    float size;
    float angle;
};

CubeInfo cubes[4] = {
        {-1.2f, -1.0f, 0.8f, 15.0f},
        {1.0f, -1.3f, 0.5f, 45.0f},
        {-1.1f, 0.9f, 0.6f, 30.0f},
        {1.2f, 0.9f, 1.0f, 60.0f}};

const float cameraRadius = 0.2f;


void resolveCubeCollisions(glm::vec3 &pos);


std::vector<Texture> textures;
bool useLinear = true;

Camera camera;

float lastX = 320.0f, lastY = 240.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool cubeRed[4] = {false, false, false, false};

glm::mat4 cubeModel(const CubeInfo &c) {
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(c.x, c.size / 2.0f, c.z));
    model = glm::rotate(model, glm::radians(c.angle), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(c.size));
    return model;
}

bool rayIntersectsUnitCube(const glm::vec3 &o, const glm::vec3 &d, float &tHit);

int pickCube(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir);


bool lookActive = false;
glm::mat4 gView(1.0f), gProjection(1.0f);

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialize GLAD\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }
    //KVADRANTI
    Shader shader("resources/shaders/vertexShader.vs",
                  "resources/shaders/fragmentShader.fs");
    //OSE
    Shader axisShader("resources/shaders/axisShader.vs",
                      "resources/shaders/axisShader.fs");
    //RAVAN
    Shader planeShader("resources/shaders/axisShader.vs",
                       "resources/shaders/planeShader.fs");
    //POD
    Shader floorShader("resources/shaders/floorShader.vs",
                       "resources/shaders/floorShader.fs");
    Texture floorTexture("resources/textures/wood.png");

    float vertices[] = {

            0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f

    };

    //KVADRANTI

    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    textures.emplace_back("resources/textures/awesomeface.png");
    textures.emplace_back("resources/textures/bricks2.jpg");
    textures.emplace_back("resources/textures/marble.jpg");
    textures.emplace_back("resources/textures/matrix.jpg");

    //OSE

    float axisVertices[] = {

            0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

            0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    unsigned int axisVAO, axisVBO;
    glGenVertexArrays(1, &axisVAO);
    glGenBuffers(1, &axisVBO);

    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glm::mat4 view = camera.viewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);
    glm::mat4 axisMVP = projection * view;

    GLfloat widthRange[2];
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, widthRange);
    std::cout << "Line width range: " << widthRange[0] << " - " << widthRange[1] << "\n";
    glLineWidth(7.0f);

    //RAVAN

    float planeVertices[] = {
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f};

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    //POD

    float floorVertices[] = {

            -2.0f, 0.0f, -2.0f, 0.0f, 0.0f,
            2.0f, 0.0f, -2.0f, 4.0f, 0.0f,
            2.0f, 0.0f, 2.0f, 4.0f, 4.0f,

            -2.0f, 0.0f, -2.0f, 0.0f, 0.0f,
            2.0f, 0.0f, 2.0f, 4.0f, 4.0f,
            -2.0f, 0.0f, 2.0f, 0.0f, 4.0f};
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);

    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //KOCKE

    float cubeVertices[] = {

            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    double lastShaderCheck = 0.0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        glClearColor(0.3, 0.5, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        view = camera.viewMatrix();
        axisMVP = projection * view;
        gView = view;
        gProjection = projection;

        //glUseProgram(shaderProgram);
        double now = glfwGetTime();
        if (now - lastShaderCheck > 0.5) {
            shader.reloadIfChanged();
            lastShaderCheck = now;
        }
        shader.use();
        shader.setInt("uTexture", 0);
        glBindVertexArray(VAO);

        //KVADRANTI
        glDisable(GL_DEPTH_TEST);
        for (int i = 0; i < 4; i++) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                             glm::vec3(quadrantOffsets[i][0], quadrantOffsets[i][1], 0.0f));

            textures[i].bindTexture(0);
            shader.setInt("uMix", quadrantMix[i]);
            shader.setInt("uHasColor", quadrantHasColor[i]);
            shader.setMat4("uModel", model);
            shader.setVec3("uColor", quadrantColors[i][0], quadrantColors[i][1], quadrantColors[i][2]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        //POD
        glEnable(GL_DEPTH_TEST);
        floorShader.use();
        floorShader.setInt("uTexture", 0);
        floorShader.setInt("uRed", 0);
        floorShader.setMat4("uMVP", axisMVP);
        floorTexture.bindTexture(0);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //KOCKE
        glBindVertexArray(cubeVAO);
        for (int i = 0; i < 4; i++) {
            floorShader.setMat4("uMVP", axisMVP * cubeModel(cubes[i]));
            floorShader.setInt("uRed", cubeRed[i] ? 1 : 0);
            textures[i].bindTexture(0);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //RAVAN
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        planeShader.use();
        planeShader.setMat4("uMVP", axisMVP);
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        //OSE
        glDisable(GL_DEPTH_TEST);
        axisShader.use();
        axisShader.setMat4("uMVP", axisMVP);
        glBindVertexArray(axisVAO);
        glDrawArrays(GL_LINES, 0, 6);

        glfwSwapBuffers(window);
    }

    //KVADRANTI
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    //OSE
    glDeleteBuffers(1, &axisVBO);
    glDeleteVertexArrays(1, &axisVAO);
    axisShader.deleteProgram();

    //RAVAN
    glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &planeVAO);
    planeShader.deleteProgram();

    //POD
    glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &floorVAO);
    floorShader.deleteProgram();
    floorTexture.deleteTexture();

    //KOCKE
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);

    for (auto &t: textures) t.deleteTexture();
    shader.deleteProgram();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        camera.toggleWalkMode();
        return;
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        useLinear = !useLinear;
        for (auto &t: textures) {
            t.setFilter(useLinear ? GL_LINEAR : GL_NEAREST);
        }
        return;
    }

    if ((mods & GLFW_MOD_SHIFT) && action == GLFW_PRESS &&
        (key == GLFW_KEY_R || key == GLFW_KEY_G || key == GLFW_KEY_B || key == GLFW_KEY_T)) {

        for (int i = 0; i < 4; i++) {
            if (glfwGetKey(window, GLFW_KEY_1 + i) != GLFW_PRESS) continue;

            if (key == GLFW_KEY_T) {
                quadrantMix[i] = !quadrantMix[i];
            } else {
                float *c = quadrantColors[i];
                c[0] = (key == GLFW_KEY_R) ? 1.0f : 0.0f;
                c[1] = (key == GLFW_KEY_G) ? 1.0f : 0.0f;
                c[2] = (key == GLFW_KEY_B) ? 1.0f : 0.0f;
                quadrantHasColor[i] = 1;
            }
        }
        return;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        glClearColor(1.0, 0.0, 0.0, 1.0);
    }
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        glClearColor(0.0, 1.0, 0.0, 1.0);
    }
    if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        glClearColor(0.0, 0.0, 1.0, 1.0);
    }

    if (mods & GLFW_MOD_CONTROL && action == GLFW_PRESS) {

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        const int step = 50;

        switch (key) {
            case GLFW_KEY_UP:
                height += step;
                break;

            case GLFW_KEY_DOWN:
                height -= step;
                break;

            case GLFW_KEY_RIGHT:
                width += step;
                break;

            case GLFW_KEY_LEFT:
                width -= step;
                break;

            default:
                break;
        }

        width = std::max(width, 400);
        height = std::max(height, 300);

        glfwSetWindowSize(window, width, height);
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !lookActive) {
        glm::vec3 origin, dir;
        screenRay(window, origin, dir);
        int hit = pickCube(origin, dir);
        if (hit != -1) {
            cubeRed[hit] = !cubeRed[hit];
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            lookActive = true;
            firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (action == GLFW_RELEASE) {
            lookActive = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> distrib(0.0f, 1.0f);
        glClearColor(distrib(gen), distrib(gen), distrib(gen), 1.0);
    }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {

    if (lookActive) {
        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }

        camera.rotate(static_cast<float>(xpos) - lastX, lastY - static_cast<float>(ypos));
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
    }

    glfwGetCursorPos(window, &xpos, &ypos);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        auto x = static_cast<float>(xpos / width);
        float y = 1.0f - static_cast<float>(ypos / height);

        x = std::clamp(x, 0.0f, 1.0f);
        y = std::clamp(y, 0.0f, 1.0f);

        glClearColor(x, y, 0.0f, 1.0f);
    }
}

bool rayIntersectsUnitCube(const glm::vec3 &o, const glm::vec3 &d, float &tHit) {
    float tmin = 0.0f;
    float tmax = 1e9f;

    for (int a = 0; a < 3; a++) {
        if (std::abs(d[a]) < 1e-8f) {
            if (o[a] < -0.5f || o[a] > 0.5f) return false;
        } else {
            float t1 = (-0.5f - o[a]) / d[a];
            float t2 = (0.5f - o[a]) / d[a];
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }
    tHit = tmin;
    return true;
}

int pickCube(const glm::vec3 &rayOrigin, const glm::vec3 &rayDir) {
    int best = -1;
    float bestT = 1e9f;

    for (int i = 0; i < 4; i++) {
        glm::mat4 inv = glm::inverse(cubeModel(cubes[i]));
        glm::vec3 o = glm::vec3(inv * glm::vec4(rayOrigin, 1.0f));
        glm::vec3 d = glm::vec3(inv * glm::vec4(rayDir, 0.0f));

        float t;
        if (rayIntersectsUnitCube(o, d, t) && t < bestT) {
            bestT = t;
            best = i;
        }
    }
    return best;
}

void resolveCubeCollisions(glm::vec3 &pos) {
    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    for (const auto &c: cubes) {
        float half = c.size / 2.0f;
        glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(c.angle), up);
        glm::mat4 invRot = glm::rotate(glm::mat4(1.0f), glm::radians(-c.angle), up);


        glm::vec4 local = invRot * glm::vec4(pos.x - c.x, 0.0f, pos.z - c.z, 0.0f);
        float lx = local.x;
        float lz = local.z;


        float closestX = std::clamp(lx, -half, half);
        float closestZ = std::clamp(lz, -half, half);
        float dx = lx - closestX;
        float dz = lz - closestZ;
        float dist2 = dx * dx + dz * dz;

        if (dist2 >= cameraRadius * cameraRadius) continue;

        if (dist2 > 1e-8f) {

            float dist = std::sqrt(dist2);
            lx = closestX + dx / dist * cameraRadius;
            lz = closestZ + dz / dist * cameraRadius;
        } else {

            float px = half - std::abs(lx);
            float pz = half - std::abs(lz);
            if (px < pz) {
                lx = (lx >= 0.0f ? 1.0f : -1.0f) * (half + cameraRadius);
            } else {
                lz = (lz >= 0.0f ? 1.0f : -1.0f) * (half + cameraRadius);
            }
        }

        glm::vec4 world = rot * glm::vec4(lx, 0.0f, lz, 0.0f);
        pos.x = c.x + world.x;
        pos.z = c.z + world.z;
    }
}

void screenRay(GLFWwindow *window, glm::vec3 &origin, glm::vec3 &dir) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int w, h;
    glfwGetWindowSize(window, &w, &h);

    float x = 2.0f * static_cast<float>(xpos) / static_cast<float>(w) - 1.0f;
    float y = 1.0f - 2.0f * static_cast<float>(ypos) / static_cast<float>(h);

    glm::mat4 invVP = glm::inverse(gProjection * gView);
    glm::vec4 nearP = invVP * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 farP = invVP * glm::vec4(x, y, 1.0f, 1.0f);
    nearP /= nearP.w;
    farP /= farP.w;

    origin = glm::vec3(nearP);
    dir = glm::normalize(glm::vec3(farP - nearP));
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.move(CameraMove::Forward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.move(CameraMove::Backward, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.move(CameraMove::Right, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.move(CameraMove::Left, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) camera.move(CameraMove::Up, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) camera.move(CameraMove::Down, deltaTime);

    if (camera.isWalking()) {
        resolveCubeCollisions(camera.position);
    }
    camera.constrainToFloor();
}
