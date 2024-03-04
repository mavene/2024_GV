////////////////////////////////////////////////////////////////////////
//
//
//  Assignment 2 of SUTD Course 50.017
//
//    Hierarchical Skeleton
//
//
//
////////////////////////////////////////////////////////////////////////

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "math.h"
//#include "shaderSource.h"
//#include "shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;



#define _ROTATE_FACTOR              0.005f
#define _SCALE_FACTOR               0.005f
#define _TRANS_FACTOR               0.003f

#define _Z_NEAR                     0.001f
#define _Z_FAR                      100.0f

#define NUMBER_OF_VERTICES          1000
#define M_PI                        3.1415926535


/***********************************************************************/
/**************************   global variables   ***********************/
/***********************************************************************/

// declaration
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

// Window size
unsigned int winWidth  = 800;
unsigned int winHeight = 800;

// Camera
glm::vec3 camera_position = glm::vec3 (0.0f, 0.0f, 2.5f);
glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
float camera_fovy = 45.0f;    
glm::mat4 projection;

// Mouse interaction
bool leftMouseButtonHold = false;
bool isFirstMouse = true;
float prevMouseX;
float prevMouseY;
glm::mat4 modelMatrix = glm::mat4(1.0f);

// vertex shader source
// ------------------------------
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

// fragment shader source
// ------------------------------
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";



///=========================================================================================///
///                             Functions for Rendering 3D Model 
///=========================================================================================///






///=========================================================================================///
///                            Functions for Manipulating 3D Model  
///=========================================================================================///

void RotateModel(float angle, glm::vec3 axis)
{
    glm::vec3 rotateCenter = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

    glm::mat4 rotateMatrix = glm::mat4(1.0f);
    rotateMatrix = glm::translate(rotateMatrix, rotateCenter);
    rotateMatrix = glm::rotate(rotateMatrix, angle, axis);
    rotateMatrix = glm::translate(rotateMatrix, -rotateCenter);

    modelMatrix = rotateMatrix * modelMatrix;
}

void TranslateModel(glm::vec3 transVec)
{
    glm::mat4 translateMatrix = glm::mat4(1.0f);
    translateMatrix = glm::translate(translateMatrix, transVec);

    modelMatrix = translateMatrix * modelMatrix;
}

void ScaleModel(float scale)
{
    glm::vec3 scaleCenter = glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);

    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::translate(scaleMatrix, scaleCenter);
    scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scale, scale, scale));
    scaleMatrix = glm::translate(scaleMatrix, -scaleCenter);

    modelMatrix = scaleMatrix * modelMatrix;
}




///=========================================================================================///
///                                    Callback Functions
///=========================================================================================///

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.

    glViewport(0, 0, width, height);

    winWidth  = width;
    winHeight = height;
}


// This function is called whenever a "Normal" key press is received.
// ---------------------------------------------------------------------------------------------
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    if (key == GLFW_KEY_C && action == GLFW_PRESS)
//    {
//        cout << "input the joint index from the list:" << endl;
//        for (size_t i = 0; i < myModel.jointMatList.size(); i++)
//        {
//            cout << i << " ";
//        }
//        cout << endl;
//        int chosenIndex;
//        cin >> chosenIndex;
//
//        printf("selected joint index: %d \n", chosenIndex);
//
//        if (chosenIndex >= 0 && chosenIndex < myModel.jointMatList.size())
//        {
//            float jointRotateAngle[3];
//            cout << "choose rotation angle for x, y, z axis, range: [-180, 180] (example: 90, 90, 120)" << endl;
//            cout << "input the rotation angle for X axis:" << endl;
//            cin >> jointRotateAngle[0];
//            cout << "input the rotation angle for Y axis:" << endl;
//            cin >> jointRotateAngle[1];
//            cout << "input the rotation angle for Z axis:" << endl;
//            cin >> jointRotateAngle[2];
//
//            if (jointRotateAngle[0] >= -180 && jointRotateAngle[0] <= 180 &&
//                jointRotateAngle[1] >= -180 && jointRotateAngle[1] <= 180 &&
//                jointRotateAngle[2] >= -180 && jointRotateAngle[2] <= 180)
//            {
//                myModel.setJointTransform(chosenIndex, jointRotateAngle[0], jointRotateAngle[1], jointRotateAngle[2]);
//                myModel.computeTransforms();
//            }
//            else
//            {
//                cout << "wrong input angles" << endl;
//            }
//        }
//        else
//        {
//            cout << "wrong joint index" << endl;
//        }
//    }
//}


// glfw: whenever the mouse button is clicked, this callback is called
// ---------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        leftMouseButtonHold = true;
    }
    else
    {
        leftMouseButtonHold = false;
    }
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
    float scale = 1.0f + _SCALE_FACTOR * yOffset;

    ScaleModel( scale );
}


// glfw: whenever the cursor moves, this callback is called
// ---------------------------------------------------------
void cursor_pos_callback(GLFWwindow* window, double mouseX, double mouseY)
{
    float  dx, dy;
    float  nx, ny, scale, angle;


    if ( leftMouseButtonHold )
    {
        if (isFirstMouse)
        {
            prevMouseX = mouseX;
            prevMouseY = mouseY;
            isFirstMouse = false;
        }

        else
        {
            if( glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS )
            {
                float dx =         _TRANS_FACTOR * (mouseX - prevMouseX);
                float dy = -1.0f * _TRANS_FACTOR * (mouseY - prevMouseY); // reversed since y-coordinates go from bottom to top

                prevMouseX = mouseX;
                prevMouseY = mouseY;

                TranslateModel( glm::vec3(dx, dy, 0) );
            }

            else
            {
                float dx =   mouseX - prevMouseX;
                float dy = -(mouseY - prevMouseY); // reversed since y-coordinates go from bottom to top

                prevMouseX = mouseX;
                prevMouseY = mouseY;

                // Rotation
                nx    = -dy;
                ny    =  dx;
                scale = sqrt(nx*nx + ny*ny);

                // We use "ArcBall Rotation" to compute the rotation axis and angle based on the mouse motion
                nx    = nx / scale;
                ny    = ny / scale;
                angle = scale * _ROTATE_FACTOR;

                RotateModel( angle, glm::vec3(nx, ny, 0.0f) );
            }
        }
    }

    else
    {
        isFirstMouse = true;
    }

}

//void harmonograph() {
//
//    GLdouble x, y, angle;
//
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    glBegin(GL_POINTS);
//    for (angle = 0.0f; angle <= (2.0f * glm::pi<float>()); angle += 0.01f)
//    {
//        x = 50.0f * sin(angle);
//        y = 50.0f * cos(angle);
//        glVertex3f(x, y, 0.0f);
//    }
//    glEnd();
//
//}




///=========================================================================================///
///                                      Main Function
///=========================================================================================///

int main()
{
    //LoadInput();
    //myModel.computeTransforms();

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "Project - Parametric Harmonograph", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    //glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // correct resize
    glfwSetScrollCallback(window, scroll_callback); // scale
    glfwSetCursorPosCallback(window, cursor_pos_callback); // translate OR rotate
    glfwSetMouseButtonCallback(window, mouse_button_callback); 

    // tell GLFW to capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    //// build and compile our shader program
    //// ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
   /* vector<float> vertices;
    float x, y, angle;
    const float radius = 0.75f;
    const float k = 4;*/

    // circle eqn
    //for (angle = 0.0f; angle < (2.0f * M_PI); angle += ((2.0f * M_PI) / NUMBER_OF_VERTICES))
    //{
    //    vertices.push_back(radius * sin(angle)); //radius is 0.75
    //    vertices.push_back(radius * cos(angle));
    //    vertices.push_back(0.0f);

    //    // colours
    //    vertices.push_back(0.2f);
    //    vertices.push_back(0.1f);
    //    vertices.push_back(1.0f);

    //}

    // polar rose eqn
    //for (angle = 0.0f; angle < (2.0f * M_PI); angle += ((2.0f * M_PI) / NUMBER_OF_VERTICES))
    //{
    //    vertices.push_back(radius * cos(k*angle) * cos(angle));
    //    vertices.push_back(radius * cos(k*angle) * sin(angle));
    //    vertices.push_back(0.0f);

    //    // colours
    //    vertices.push_back(0.2f);
    //    vertices.push_back(0.1f);
    //    vertices.push_back(1.0f);

    //}

    // todo: change to equation for harmonograph
    // variables : amplitude (x,y), phase (x,y), damping, frequency
    vector<float> vertices;
    float x, y, time;
    const float phase = M_PI / 2;
    const float amplitude = 1.0f;
    const float damping = 0.02f;
    const float freq1 = 3.001f,
        freq2 = 2.0f,
        freq3 = 3.0f,
        freq4 = 2.0f,
        damping1 = 0.004f,
        damping2 = 0.0065f,
        damping3 = 0.008f,
        damping4 = 0.019f,
        phase1 = 0,
        phase2 = 0,
        phase3 = M_PI / 2,
        phase4 = 3 * M_PI / 2;

    //amplitude_1, amplitude_2, amplitude_3, amplitude_4

    for (time = 0; time < NUMBER_OF_VERTICES; time += 0.01)
    {
        vertices.push_back(amplitude * sin(time*freq1+phase1)*exp(-damping1*time) + amplitude * sin(time*freq2+phase2)*exp(-damping2*time));
        vertices.push_back(amplitude * sin(time*freq3+phase3)*exp(-damping3*time) + amplitude * sin(time*freq4+phase4)*exp(-damping4*time));
        vertices.push_back(0.0f);

        // colours
        vertices.push_back(0.2f);
        vertices.push_back(0.1f);
        vertices.push_back(1.0f);

    }


   for (int i = 0; i < vertices.size(); i++) {
        std::cout << vertices[i] << std::endl;
   }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    // as we only have a single shader, we could also just activate our shader once beforehand if we want to 
    glUseProgram(shaderProgram);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // exit program when ESC is pressed
        // -----
        processInput(window);

        // clear screen with colour
        // ------
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw circle
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP, 0, NUMBER_OF_VERTICES); // try GL_LINES and other primitives...

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteProgram(myShader.ID);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




