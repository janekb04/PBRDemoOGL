#include "Vendor.h"
#include "Window.h"
#include "Shader.h"
#include "Utility.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "Defines.h"
#include "Camera.h"
#include "OpenGLWindow.h"
#include "OrbitCamera.h"
#include "TestScene.h"

int main()
{
    WindowManager::init();
	OpenGLWindow wnd;

	WindowManager::set_swap_interval(1);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    ShaderProgram lit{
		Shader{read_file("res/lit.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/lit.frag").c_str(), GL_FRAGMENT_SHADER}
	};
    
	//DemoScene scene;
	auto scene = create_test_scene(8);

	PerspectiveCamera camera;
	Uniform a_camera = lit.get_uniform_location("a_camera");
	Uniform a_view = lit.get_uniform_location("a_view");
	OrbitCamera controller(camera.transform);

	Uniform a_light_dir = lit.get_uniform_location("a_light_dir");
	Uniform a_light_color = lit.get_uniform_location("a_light_color");
	Uniform a_ambient = lit.get_uniform_location("a_ambient");

	double old_time = WindowManager::get_time();
	double delta_time = 0;
	unsigned long long frame = 0;
    while (!wnd.should_close())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (frame % 60 == 0)
		{
			std::cout << 1/delta_time << '\n';
		}

		controller.update(wnd, delta_time);

		//draw
		{
			lit.use();
			auto view = camera.get_view_matrix();
			lit.uniform(a_camera, false, camera.get_projection_matrix(wnd.viewport()) * view);
			lit.uniform(a_view, false, view);

			
			glm::vec3 dir = glm::normalize(view * glm::vec4(-1, 0, 0, 0));
			lit.uniform(a_light_dir, dir);
			lit.uniform(a_light_color, glm::vec3(1, 1, 1));
			lit.uniform(a_ambient, glm::vec3(0.2f, 0.3f, 0.3f));



			scene->setup_state();
			scene->draw();
		}

		wnd.end_frame();
        WindowManager::poll_events();

		double new_time = WindowManager::get_time();
		delta_time = new_time - old_time;
		old_time = new_time;
		++frame;
	}
}