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

	WindowManager::set_swap_interval(0);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    ShaderProgram lit{
		Shader{read_file("res/lit.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/lit.frag").c_str(), GL_FRAGMENT_SHADER}
	};
    
	//DemoScene scene;
	auto scene = create_test_scene(1'000'000);

	PerspectiveCamera camera;
	Uniform camera_mat = lit.get_uniform_location("a_camera");
	OrbitCamera controller(camera.transform);

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
			lit.uniform(camera_mat, false, camera.get_projection_matrix(wnd.viewport()) * camera.get_view_matrix());

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