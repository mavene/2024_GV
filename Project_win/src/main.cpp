////////////////////////////////////////////////////////////////////////
//
//
//  
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

#define NUMBER_OF_VERTICES          10000 // MSAA looks good with 6000
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
unsigned int winWidth  = 1200;
unsigned int winHeight = 1200;

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

// Colour
glm::vec3 meshColor;


///=========================================================================================///
///                             Functions for Rendering 3D Model 
///=========================================================================================///




// vertex shader sources
// ------------------------------
const char* vertexShaderSource = "#version 330 core\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform vec3 meshColor;\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec3 ocolor;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   ocolor = meshColor;\n"
"}\n";

const char* vertexShaderSource2 = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"out VS_OUT{\n"
"    vec3 normal;\n"
"} vs_out;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"void main() {\n"
"   mat3 normalMatrix = mat3(transpose(inverse(view * model)));\n"
"   vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));\n"
"   gl_Position = view * model * vec4(aPos, 1.0);\n"
"}\n";

// fragment shader sources
// ------------------------------
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 oColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(oColor, 1.0f);\n"
"}\n\0";

// geometry shader source
// ------------------------------
const char* geometryShaderSource = "#version 330 core\n"
"layout (points) in;\n"
"layout (line_strip, max_vertices = 2) out;\n"
"in VS_OUT {\n"
"   vec3 normal;\n"
"} gs_in[];\n"
"const float MAGNITUDE = 0.15;\n"
"uniform mat4 projection;\n"
"void GenerateLine(int index) {\n"
"   gl_Position = projection * gl_in[index].gl_Position;\n"
"   EmitVertex();\n"
"   gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE);\n"
"   EmitVertex();\n"
"   EndPrimitive();\n"
"}\n"
"void main() {\n"
"    GenerateLine(0); // one vertex normal\n"
"}\n";

// Mesh color table
glm::vec3 colorTable[4] =
{
   glm::vec3(0.6, 1.0, 0.6),
   glm::vec3(1.0, 0.6, 0.6),
   glm::vec3(0.6, 0.6, 1.0),
   glm::vec3(1.0, 1.0, 0.6)
};



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



///=========================================================================================///
///                                      Main Function
///=========================================================================================///

int main()
{

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // activate multi-sampling (smoothen lines)
    glfwWindowHint(GLFW_SAMPLES, 8);
    
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

    // configure multi-sampling
    glEnable(GL_MULTISAMPLE);

    //// build and compile our shader program
    //// ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource2, NULL);
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

    // geometry shader
    unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
    glCompileShader(geometryShader);

    // check for shader compile errors
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geometryShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
   /* vector<float> vertices;
    float x, y, angle;
    const float radius = 0.75f;
    const float k = 4;*/

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

    vector<float> positions, vertices;
    float x, y, z, time;
    const float phase = M_PI / 2;
    const float amplitude = 0.5f;
    const float damping = 0.02f;
    const float freq1 = 3.001f,
        freq2 = 2.0f,
        freq3 = 3.0f,
        freq4 = 2.0f,
        freq5 = 3.0f,
        freq6 = 2.0f,
        damping1 = 0.004f,
        damping2 = 0.0065f,
        damping3 = 0.008f,
        damping4 = 0.019f,
        damping5 = 0.012f,
        damping6 = 0.005f,
        phase1 = 0,
        phase2 = 0,
        phase3 = M_PI / 2,
        phase4 = 3 * M_PI / 2,
        phase5 = M_PI / 4,
        phase6 = 2 * M_PI;

    for (time = 0; time < NUMBER_OF_VERTICES; time += 0.01)
    {
        positions.push_back(amplitude * sin(time*freq1+phase1)*exp(-damping1*time) + amplitude * sin(time*freq2+phase2)*exp(-damping2*time));
        positions.push_back(amplitude * sin(time*freq3+phase3)*exp(-damping3*time) + amplitude * sin(time*freq4+phase4)*exp(-damping4*time));
        positions.push_back(amplitude * sin(time * freq5 + phase5) * exp(-damping5 * time) + amplitude * sin(time * freq6 + phase6) * exp(-damping6 * time)); //push_back(0.0f);
    }

    float x1, x2, x3, y1, y2, y3, z1, z2, z3, nx, ny, nz;
    float min_x = 0,
        min_y = 0,
        min_z = 0,
        max_x = 0,
        max_y = 0,
        max_z = 0;

    for (int i = 0; i < positions.size(); i = i + 9) {
            // Retrieve 3 point coords
            x1 = positions.at(i);
            y1 = positions.at(i + 1);
            z1 = positions.at(i + 2);
            x2 = positions.at(i + 3);
            y2 = positions.at(i + 4);
            z2 = positions.at(i + 5);
            x3 = positions.at(i + 6);
            y3 = positions.at(i + 7);
            z3 = positions.at(i + 8);

            // Assemble points into vecs
            glm::vec3 pt1 = glm::vec3(x1, y1, z1);
            glm::vec3 pt2 = glm::vec3(x2, y2, z2);
            glm::vec3 pt3 = glm::vec3(x3, y3, z3);
            glm::vec3 norm = glm::vec3(1.0f);

            // Calculate normals
            norm = glm::normalize(glm::cross(pt3-pt2, pt1-pt2)); // cross product
            nx = norm[0];
            ny = norm[1];
            nz = norm[2];

            // Update vertices with normals
            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(z1);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
            vertices.push_back(x2);
            vertices.push_back(y2);
            vertices.push_back(z2);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
            vertices.push_back(x3);
            vertices.push_back(y3);
            vertices.push_back(z3);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);

        }



    // For debugging purposes
   /*for (int i = 0; i < vertices.size(); i++) {
        std::cout << vertices[i] << std::endl;
   }*/

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normals attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), ((void*)(3 * sizeof(float))));
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

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera_fovy), (float)winWidth / (float)winHeight, _Z_NEAR, _Z_FAR);
        glm::mat4 view = glm::lookAt(camera_position, camera_target, camera_up);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &modelMatrix[0][0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "meshColor"), 1, &colorTable[0][0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &camera_position[0]);

        // render harmonograph
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, NUMBER_OF_VERTICES); // original primitive is GL_LINE_LOOP
  
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}




