#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include <iostream>

// Thanks to https://youtu.be/7-dL6a5_B3I for the 
// setup for openGL, glfw, and glad and template code.


void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

// settings
unsigned int WIDTH = 1080;
unsigned int HEIGHT = 720;

bool resized = true;

int main()
{
    
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif




    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(WIDTH/2, HEIGHT/2, "Raytracing", NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    Shader shaders("./vert.glsl","./frag.glsl");



    float vertices[] = {
        -1.0, -1.0, 0.0, //BL
         1.0, -1.0, 0.0, //BR
        -1.0,  1.0, 0.0, //TL
         1.0,  1.0, 0.0  //TR
    }; 

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    float frameCount = 0;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        frameCount++;
        // input
        processInput(window);

        // render
        

        if (resized) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);   
            shaders.use();
            
            shaders.setVec2("iResolution", glm::vec2(WIDTH,HEIGHT));
            shaders.setFloat("frameCount", frameCount);
            glBindVertexArray(VAO); 
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            resized = false;
            glfwSwapBuffers(window);
            
        }
        glfwPollEvents();
    }

    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    
    glfwTerminate();
    return 0;
}


// credits to https://github.com/contessanicola/Raymarching-in-OpenGL
// for hints on how to add shaders to GLSL and the (unused) code below:
void processInput(GLFWwindow *window)
{	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //     camera.ProcessKeyboard(FORWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //     camera.ProcessKeyboard(BACKWARD, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //     camera.ProcessKeyboard(LEFT, deltaTime);
    // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //     camera.ProcessKeyboard(RIGHT, deltaTime);
	// if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    //     camera.ProcessKeyboard(UP, deltaTime);
	// if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    //     camera.ProcessKeyboard(DOWN, deltaTime);
	// if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	// 	camera.ProcessKeyboard(Q, deltaTime);
	// if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	// 	camera.ProcessKeyboard(E, deltaTime);

}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    WIDTH = width;
    HEIGHT = height;
    std::cout << "Width: " << width << "Height: " << height << std::endl;
	glViewport(0, 0, width, height);
    resized = true;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{	
	// if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
	// 	if (firstMouse)
	// 	{
	// 		lastX = xpos;
	// 		lastY = ypos;
	// 		firstMouse = false;
	// 	}

	// 	float xoffset = xpos - lastX;
	// 	float yoffset = lastY - ypos; 

	// 	lastX = xpos;
	// 	lastY = ypos;

		
	// 	camera.ProcessMouseMovement(xoffset, yoffset);
	// }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // camera.ProcessMouseScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // if (key == GLFW_KEY_LEFT && action == GLFW_PRESS){
    //     scene--;
    // }
	// if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS){
    // 	scene++;
    // }		
}