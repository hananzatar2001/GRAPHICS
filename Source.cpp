#define GLEW_STATIC
#include<glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
GLuint shader;
GLuint VBO, VAO;
GLfloat rotationAngle = 0.0f;
bool rotateCube = true;
bool isJumping = false;
float jumpHeight = 0.0f;
float jumpSpeed = 0.2f;
float zoom = 45.0f;

void framebuffer_size_callback(int width, int height) {
    glViewport(0, 0, width, height);
}
string loadshaderfile(const char* filePath) {
    ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        cout<< "can't open file: " << filePath << endl;
        return "";
    }
   stringstream buffer;
    buffer << fileStream.rdbuf();
    return buffer.str();
}
GLuint compileshader(GLenum shaderType, const char* shaderSource) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader); 
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cout<< "Shader compilation error: " << infoLog << endl;
        return 0;
    }
    return shader;
}
void initShaders() {
    string vertexShaderSource = loadshaderfile("vertex_shader.glsl");
    string fragmentShaderSource = loadshaderfile("fragment_shader.glsl");
    GLuint vertexShader = compileshader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    GLuint fragmentShader = compileshader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());
    shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    GLint success;
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        cout << "Shader link error: " << infoLog << endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
void initRectangularprism() {
    float vertices[] = {
        // Front face
        -0.5f, -0.9f,  0.5f,
         0.5f, -0.9f,  0.5f,
         0.5f,  0.9f,  0.5f,
        -0.5f,  0.9f,  0.5f,
        // Back face
        -0.5f, -0.9f, -0.5f,
        -0.5f,  0.9f, -0.5f,
         0.5f,  0.9f, -0.5f,
         0.5f, -0.9f, -0.5f,
         // Top face
         -0.5f,  0.9f, -0.5f,
         -0.5f,  0.9f,  0.5f,
          0.5f,  0.9f,  0.5f,
          0.5f,  0.9f, -0.5f,
          // Bottom face
          -0.5f, -0.9f, -0.5f,
           0.5f, -0.9f, -0.5f,
           0.5f, -0.9f,  0.5f,
          -0.5f, -0.9f,  0.5f,
          // Right face
           0.5f, -0.9f, -0.5f,
           0.5f,  0.9f, -0.5f,
           0.5f,  0.9f,  0.5f,
           0.5f, -0.9f,  0.5f,
           // Left face
           -0.5f, -0.9f, -0.5f,
           -0.5f, -0.9f,  0.5f,
           -0.5f,  0.9f,  0.5f,
           -0.5f,  0.f, -0.5f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void display() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader);
    glm::mat4 model = glm::rotate
    (glm::mat4(1.0f), glm::radians(rotationAngle),
        glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 view = glm::lookAt
    (glm::vec3(0.0f, 0.0f, 3.0f), 
     glm::vec3(0.0f, 0.0f, 0.0f), 
     glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective
    (glm::radians(zoom),
     800.0f / 600.0f, 0.1f, 100.0f);
    GLuint modelLoc = glGetUniformLocation(shader, "model");
    GLuint viewLoc = glGetUniformLocation(shader, "view");
    GLuint projectionLoc = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(VAO);
    glDrawArrays(GL_QUADS, 0, 24);
    glutSwapBuffers();
}
void update(int value) {
    static bool goingUp = true;
    const float jumpSpeed = 0.1f;
    if (goingUp) {
        jumpHeight += jumpSpeed;
        if (jumpHeight >= glm::pi<float>()) {
            goingUp = false;
        }
    }
    else {
        jumpHeight -= jumpSpeed;
        if (jumpHeight <= 0.0f) {
            goingUp = true;
        }
    }
    rotationAngle += 2.0f; 

    glutPostRedisplay();
    glutTimerFunc(16, update, 0); 
}
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        
        GLuint fragColorLoc = glGetUniformLocation(shader, "FragColor");
        glUniform4f(fragColorLoc, static_cast<GLfloat>(rand()) / RAND_MAX,
            static_cast<GLfloat>(rand()) / RAND_MAX,
            static_cast<GLfloat>(rand()) / RAND_MAX,
            1.0f);
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        
        glClearColor(static_cast<GLfloat>(rand()) / RAND_MAX,
            static_cast<GLfloat>(rand()) / RAND_MAX,
            static_cast<GLfloat>(rand()) / RAND_MAX,
            1.0f);
    }
    glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'R':
    case 'r':
        rotationAngle += 5.0f; 
        break;
    case 'L':
    case 'l':
        rotationAngle -= 5.0f; 
        break;
    case '+':
       
        zoom -= 5.0f; 
        break;
    case '-':
        
        zoom += 5.0f; 
        break;
    
    }
    glutPostRedisplay();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OpenGL  Jump Animation");

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
      cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    initShaders();
    initRectangularprism();
    glutDisplayFunc(display);
    glutReshapeFunc(framebuffer_size_callback);
    glutTimerFunc(0, update, 0);
    srand(time(NULL));
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glutMainLoop();

    return 0;
}
