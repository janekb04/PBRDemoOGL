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
#include "OpenGLWindow.h"

int main()
{
    WindowManager::init();
	OpenGLWindow wnd;

    ShaderProgram lit{
		Shader{read_file("res/lit.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/lit.frag").c_str(), GL_FRAGMENT_SHADER}
	};
    
    Buffer VBO, EBO;
	Buffer draw_indirect;
	Buffer instanced_vbo;
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

		glm::mat4 tranforms[] = {
			glm::mat4(1),
			glm::mat4(1)
		};

		instanced_vbo.data(sizeof(tranforms), glm::value_ptr(tranforms[0]), GL_STATIC_DRAW);
	}
    
	VertexArray VAO;
	
	const unsigned int VBO_IDX = 0;
    VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(glm::vec3)); //VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(Vertex));
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

	const unsigned int INSTANCED_VBO_IDX = 1;
	VAO.vertex_buffer(INSTANCED_VBO_IDX, instanced_vbo, 0, sizeof(glm::mat4));
	VAO.binding_divisor(INSTANCED_VBO_IDX, 1);
	const VertexAttribute MODEL_IDX = lit.get_attrib_location("a_model");
	for (int i = 0; i < 4; ++i)
	{
		VertexAttribute column{ MODEL_IDX.index() + i };
		VAO.enable_attrib(column);
		VAO.attrib_binding(column, INSTANCED_VBO_IDX);
		VAO.attrib_format(column, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4::col_type) * i);
	}

	VAO.element_buffer(EBO);
    
	PerspectiveCamera camera;
	camera.transform.set_position({ 0, 0, -1 });
	Uniform camera_mat = lit.get_uniform_location("a_camera");
    
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	double old_time = WindowManager::get_time();
	double delta_time = 0;
    while (!wnd.should_close())
    {
        glClear(GL_COLOR_BUFFER_BIT);

		//update
		{
			glm::vec3 move{ 0, 0, 0 };
			if (wnd.is_key_pressed(GLFW_KEY_W))
				move += glm::vec3{ 0, 0, 1 };
			if (wnd.is_key_pressed(GLFW_KEY_S))
				move += glm::vec3{ 0, 0, -1 };
			if (wnd.is_key_pressed(GLFW_KEY_A))
				move += glm::vec3{ 1, 0, 0 };
			if (wnd.is_key_pressed(GLFW_KEY_D))
				move += glm::vec3{ -1, 0, 0 };
			
			if (glm::length(move) != 0)
				move = glm::normalize(move);
			const float speed = 5;
			move *= speed * delta_time;
			camera.transform.translate(move);
		}

		//draw
		{
			lit.use();
			lit.uniform(camera_mat, false, camera.get_projection_matrix(wnd.viewport()) * camera.get_view_matrix());

			VAO.bind();
			draw_indirect.bind(GL_DRAW_INDIRECT_BUFFER);
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, command_count, 0);
		}

        wnd.swap_buffers();
        WindowManager::poll_events();
		
		double new_time = WindowManager::get_time();
		delta_time = new_time - old_time;
		old_time = new_time;
	}
}