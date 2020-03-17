#include "Vendor.h"
#include "Window.h"
#include "Shader.h"
#include "Utility.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "Defines.h"
#include "MultiDrawBuilder.h"
#include "Vertex.h"
#include "Camera.h"

void APIENTRY opengl_error_callback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* msg,
	const void* data
) {
	char message[BUFSIZ];

	const char* _source;
	const char* _type;
	const char* _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW_SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER_COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD_PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "OTHER";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED_BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED_BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "NOTIFICATION";
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

	std::snprintf(message, BUFSIZ, "%s (%d) of %s severity, raised from %s: %s\n", _type, id, _severity, _source, msg);

	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
		throw std::runtime_error(message);
}

int main()
{
    WindowManager::init();
    Window::CreationHints hints;
    hints.context_version_major = 4;
    hints.context_version_minor = 6;
    hints.opengl_profile = Window::OpenGLProfile::CORE;
#ifndef NDEBUG
	hints.opengl_debug_context = true;
#endif // !NDEBUG

	Window wnd{ {}, hints };
    
    wnd.get_context().make_current();

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

#ifndef NDEBUG
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(opengl_error_callback, NULL);
#endif // DEBUG

    ShaderProgram lit{
		Shader{read_file("res/lit.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/lit.frag").c_str(), GL_FRAGMENT_SHADER}
	};
    
    Buffer VBO, EBO;
	Buffer draw_indirect;
	int command_count{ 0 };
	{
		MultiDrawElementsBuilder<glm::vec3, GLuint> builder;
		builder.add_mesh(
			{
				{ 0.5f,  0.5f,  0.0f}, // top right
				{ 0.5f, -0.5f,  0.0f}, // bottom right
				{-0.5f, -0.5f,  0.0f}, // bottom left
				{-0.5f,  0.5f,  0.0f}  // top left 
			},
			{
				0, 1, 3, // first Triangle
				1, 2, 3  // second Triangle
			},
			1
		);
		builder.add_mesh(
			{
				{ 0.0f,  0.5f,  0.0f}, // top middle
				{ 0.5f, -0.5f,  0.0f}, // bottom right
				{-0.5f, -0.5f,  0.0f}, // bottom left
			},
			{
				0, 1, 2
			},
			1
		);

		VBO.data(sizeof(glm::vec3) * builder.vertices().size(), builder.vertices().data(), GL_STATIC_DRAW);
		EBO.data(sizeof(GLuint) * builder.indices().size(), builder.indices().data(), GL_STATIC_DRAW);
		draw_indirect.data(sizeof(glDrawElementsIndirectCommand) * builder.commands().size(), builder.commands().data(), GL_STATIC_DRAW);
		command_count = builder.commands().size();
	}
    
	VertexArray VAO;
	const unsigned int VBO_IDX = 0;
    //VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(Vertex));
    VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(glm::vec3));
	VAO.element_buffer(EBO);
    
	const VertexAttribute POS_IDX = lit.get_attrib_location("a_pos");
    //const VertexAttribute NORMAL_IDX = lit.get_attrib_location("a_normal");
    //const VertexAttribute UV_IDX = lit.get_attrib_location("a_uv");
    //const VertexAttribute TANGENT_IDX = lit.get_attrib_location("a_tangent");

    VAO.enable_attrib(POS_IDX);
    VAO.attrib_binding(POS_IDX, VBO_IDX);
    VAO.attrib_format(POS_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

	//VAO.enable_attrib(NORMAL_IDX);
	//VAO.attrib_binding(NORMAL_IDX, VBO_IDX);
	//VAO.attrib_format(NORMAL_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

	//VAO.enable_attrib(UV_IDX);
	//VAO.attrib_binding(UV_IDX, VBO_IDX);
	//VAO.attrib_format(UV_IDX, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

	//VAO.enable_attrib(TANGENT_IDX);
	//VAO.attrib_binding(TANGENT_IDX, VBO_IDX);
	//VAO.attrib_format(TANGENT_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));

	PerspectiveCamera camera;
	camera.transform.set_position({ 0, 0, -1 });
	Viewport viewport{ {0,0}, {wnd.get_framebuffer_size().first, wnd.get_framebuffer_size().second} };
	Uniform camera_mat = lit.get_uniform_location("a_camera");

    while (!wnd.should_close())
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        lit.use();
		lit.uniform(camera_mat, false, camera.get_projection_matrix(viewport) * camera.get_view_matrix());

        VAO.bind();
		draw_indirect.bind(GL_DRAW_INDIRECT_BUFFER);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, command_count, 0);

        wnd.swap_buffers();
        WindowManager::poll_events();
    }
}