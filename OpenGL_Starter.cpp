/*  

    * This is kinda "rework" of the projects I made in the university 

    * I'll be using the window manager GLFW instead of freeglut

    * I'll try to play more with my imagination for these projects
      taking advantage of the fact that now no one forces me to do
      them and there are no time limits

    * I'll be uploading my achievements to GitHub
    
    * This is a starter project for OpenGL, kinda "Hello World" but for OpenGL
    * You should be able to load a Cube rotating
    * Of Course, you can change it and load some other geometric shapes

*/

#define GLM_ENABLE_EXPERIMENTAL
#define PI 3.1415926535

#include "gl.h" // GLAD

#include "glfw3.h" // GLFW

#include "glm.hpp" // GLM 
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/string_cast.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// ************************* FUNCTIONS FOR LOADING SHADERS *************************
std::string extractShaderSourceFile(const char* filepath);
GLuint compileShader(const char* filepath, GLenum shaderType);
GLuint renderVerticesToGPU(float vertices[], size_t numVertices);

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW";
        return -1;
    }

    // Configure GLFW for OpenGL 4.6 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  // I have 4.6 version 'cause i have a GTX 1650, look up for what opengl version your GPU supports
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // I'm not sure whether compatibility profile makes problem or not, but for the moment, i prefer to use core

    /* This won't be able for APPLE */

    // Creating a Window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello World - OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create the window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    /*
        Initialize GLAD for OpenGL functions
        If you haven't declared the current context in 63 line, GLAD won't initialize
    */
    if (!gladLoaderLoadGL()) {
        std::cerr << "Failed to initialize GLAD";
        return -1;
    }

    // Configuring Viewport and size of the window
    glViewport(0, 0, 1280, 720);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* ********** HERE I'M CREATING THE PIPELINE PROGRAM ********** */
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;

    vertexShader = compileShader("Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
    fragmentShader = compileShader("Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
    program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // In case there is any error
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[128];
        glGetProgramInfoLog(program, 128, NULL, infoLog);
        std::cerr << "Error at linking program: " << infoLog;
    }

    // Now that shaders are compiled, I have to delete them
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Now I tell to OpenGL what program to use
    glUseProgram(program);

    /* ********** HERE I'M DEFINING MY ARRAYS OF VERTICES ********** */
    // I'm still not using an EBO, so the arrays will have repeated vertices
    
    float cube[] = {
        // front face
        -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f,
        // back face
        -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f,  0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
        // left face
        -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
        // right face
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
        // top face
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        // bottom face
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f
    };

    GLuint cubeVAO = renderVerticesToGPU(cube, sizeof(cube));

    /* ********** NOW IT'S TIME TO DECLARE THE REST OF THE DATA ********** */

    // Matrices
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 cubeModel = glm::mat4(1.0f);
    // Color & Transparency
    glm::vec3 cubeRGB = glm::vec3(0.2f, 1.0f, 0.2f); // I wanna green color
    float transparency = 1.0f;

    projection = glm::perspective(glm::radians(135.0f), float(16.0f / 9), 0.05f, 100.0f);
    view = glm::lookAt(glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Locations of matrices in shaders
    GLuint viewLoc = glGetUniformLocation(program, "View");
    GLuint modelLoc = glGetUniformLocation(program, "Model");
    GLuint projectionLoc = glGetUniformLocation(program, "Projection");
    // Locations of color & transparency in fragment shader
    GLuint RGBLoc = glGetUniformLocation(program, "RGB");
    GLuint transparencyLoc = glGetUniformLocation(program, "transparency");

    // Send View, Projection, Model, and color to shaders
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeModel));

    glUniform3fv(RGBLoc, 1, glm::value_ptr(cubeRGB));
    glUniform1fv(transparencyLoc, 1, &transparency);

    //The rest will be used inside the while loop

    while (!glfwWindowShouldClose(window)) {
    
        // Handling input
        processInput(window);

        // Render 
        glClearColor(0.5f, 0.5f, 0.8f, 1.0f); //A light blue color
        glClear(GL_COLOR_BUFFER_BIT);

        //cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.0f, -0.0002f));
        cubeModel = glm::rotate(cubeModel, glm::radians(0.05f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(cubeModel));

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Finishin the window
    glfwDestroyWindow(window);
    glfwTerminate();

    // This sh*t will stay here for the posterity >:D
    // std::cout << "Hello World!\n";
}

// Window Resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Handling input: For now, if you press escape, the window will close
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
// If I'm not wrong, it's a good practice handle all input only in a function like this

std::string extractShaderSourceFile(const char* filepath) {
    std::ifstream shaderfile(filepath);
    std::stringstream shaderstream;
    shaderstream << shaderfile.rdbuf();
    return shaderstream.str();
}

GLuint compileShader(const char* filepath, GLenum shadertype) {
    std::string shaderCode = extractShaderSourceFile(filepath);
    const char* shaderSource = shaderCode.c_str();

    GLuint shader = glCreateShader(shadertype); // OpenGL stores its shaders as GLuints as "references"
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    // In case there is any error
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[128];
        glGetShaderInfoLog(shader, 128, NULL, infoLog);
        std::cerr << "Error compiling shader: " << infoLog;
    }

    return shader;
}

GLuint renderVerticesToGPU(float vertices[], size_t numVertices) {
    GLuint VAO, VBO; // Vertex Array Object and Vertex Buffer Object

    // This generates respective "spaces" in the GPU, the GLuints are just "references"
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Link the VBO and VAO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Now this loads the vertices to the VBO
    glBufferData(GL_ARRAY_BUFFER, numVertices, vertices, GL_STATIC_DRAW); 
    /* 
        The GL_STATIC_DRAW macro means that data stored won't be modified once uploaded
        There exists also the GL_DYNAMIC_DRAW, whose purpose is to store data and modified it repeatedly
    */  

    // This is the part where I configure VAO's context
    // This part'll be based on how have written your shaders
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * (sizeof(float)), (void*)0);
    glEnableVertexAttribArray(0); // Enable the context
    
    // Now I'm disabling the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

