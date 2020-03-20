#include "Vendor.h"
#include "Window.h"
#include "Shader.h"
#include "Utility.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "Defines.h"
#include "Camera.h"
#include "OpenGLWindow.h"
#include "DemoScene.h"

int main()
{
    WindowManager::init();
	OpenGLWindow wnd;

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    ShaderProgram lit{
		Shader{read_file("res/lit.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/lit.frag").c_str(), GL_FRAGMENT_SHADER}
	};
    
	DemoScene scene;
    
	PerspectiveCamera camera;
	camera.transform.set_position({ 0, 0, -1 });
	Uniform camera_mat = lit.get_uniform_location("a_camera");

	double old_time = WindowManager::get_time();
	double delta_time = 0;
    while (!wnd.should_close())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
			if (wnd.is_key_pressed(GLFW_KEY_R))
				move += glm::vec3{ 0, -1, 0 };
			if (wnd.is_key_pressed(GLFW_KEY_F))
				move += glm::vec3{ 0, 1, 0 };
			
			if (glm::length(move) != 0)
				move = glm::normalize(move);
			const float speed = 50;
			move *= speed * delta_time;
			camera.transform.translate(move);

			scene.new_frame(delta_time);
		}

		//draw
		{
			lit.use();
			lit.uniform(camera_mat, false, camera.get_projection_matrix(wnd.viewport()) * camera.get_view_matrix());

			scene.draw(wnd.is_key_pressed(GLFW_KEY_M));
		}

        wnd.swap_buffers();
        WindowManager::poll_events();
		
		double new_time = WindowManager::get_time();
		delta_time = new_time - old_time;
		old_time = new_time;
	}
}