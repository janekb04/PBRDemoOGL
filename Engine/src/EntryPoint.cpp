#include "Vendor.h"
#include "Window.h"
#include "Shader.h"
#include "Utility.h"
#include "VertexArray.h"
#include "Buffer.h"

const char* vertexShaderSource = \
"#version 330 core\n"
"layout (location = 4) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = \
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

int main()
{
    WindowManager::init();
    Window::CreationHints hints;
    hints.context_version_major = 4;
    hints.context_version_minor = 6;
    hints.opengl_profile = Window::OpenGLProfile::CORE;
    Window wnd{ {}, hints };
    
    wnd.get_context().make_current();

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    ShaderProgram program{ Shader{vertexShaderSource, GL_VERTEX_SHADER}, Shader{fragmentShaderSource, GL_FRAGMENT_SHADER} };

    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    VertexArray VAO;
    Buffer VBO, EBO;

    VBO.data(sizeof(vertices), vertices, GL_STATIC_DRAW);
    EBO.data(sizeof(indices), indices, GL_STATIC_DRAW);
    
    const unsigned int VBO_IDX = 0;
    const VertexAttribute POS_IDX = program.get_attrib_location("aPos");
    VAO.vertex_buffer(VBO_IDX, VBO, 0, 3 * sizeof(float));
    VAO.enable_attrib(POS_IDX);
    VAO.attrib_binding(POS_IDX, VBO_IDX);
    VAO.attrib_format(POS_IDX, 3, GL_FLOAT, GL_FALSE, 0);

    VAO.element_buffer(EBO);

    while (!wnd.should_close())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        program.use();
        VAO.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        wnd.swap_buffers();
        WindowManager::poll_events();
    }
}