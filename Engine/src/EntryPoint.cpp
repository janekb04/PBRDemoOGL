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
#include "GUI.h"

int main()
{
    WindowManager::init();
	OpenGLWindow wnd;
	GUI::init_imgui(wnd.get_context());

	WindowManager::set_swap_interval(1);
	stbi_set_flip_vertically_on_load(true);

	const glm::vec3 ambient = 0.05f * glm::vec4{ 0.7490f, 0.9765f, 1.0f, 0.0f };
	const float depth_clear{ 1.0f };
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

    ShaderProgram lit{
		Shader{read_file("res/shaders/lit.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/shaders/lit.frag").c_str(), GL_FRAGMENT_SHADER}
	};
    
	//DemoScene scene;
	auto scene = create_test_scene(1);

	PerspectiveCamera camera;
	Uniform a_camera = lit.get_uniform_location("a_camera");
	Uniform a_camera_pos = lit.get_uniform_location("a_camera_pos");
	OrbitCamera controller(camera.transform);

	lit.uniform(lit.get_uniform_location("a_ambient"), ambient);

	double old_time = WindowManager::get_time();
	double delta_time = 1;
	unsigned long long frame = 0;
	while (!wnd.should_close())
	{
		glClearBufferfv(GL_COLOR, 0, &(RGBtoSRGB(ambient)[0]));
		glClearBufferfv(GL_DEPTH, 0, &depth_clear);

		if (frame % 60 == 0)
		{
			std::cout << 1 / delta_time << '\n';
		}

		controller.update(wnd, delta_time);

		//draw
		{
			lit.use();
			auto view = camera.get_view_matrix();
			lit.uniform(a_camera, false, camera.get_projection_matrix(wnd.viewport()) * view);
			lit.uniform(a_camera_pos, camera.transform.get_position());

			scene->setup_state();
			scene->draw();
		}

		GUI::render_imgui();

		wnd.end_frame();
		WindowManager::poll_events();

		double new_time = WindowManager::get_time();
		delta_time = new_time - old_time;
		old_time = new_time;
		++frame;
	}
}