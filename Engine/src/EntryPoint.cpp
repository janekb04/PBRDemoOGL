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
#include "GBuffer.h"

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
	glClearColor(0, 0, 0, 1);

    ShaderProgram gbuffer_prog{
		Shader{read_file("res/shaders/gbuffer.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/shaders/gbuffer.frag").c_str(), GL_FRAGMENT_SHADER}
	};
	ShaderProgram deffered{
		Shader{read_file("res/shaders/fullscreen_triangle.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/shaders/deffered.frag").c_str(), GL_FRAGMENT_SHADER}
	};

	Uniform gbuffer_camera = gbuffer_prog.get_uniform_location("a_camera");
	Uniform deffered_gbuffer_idx = deffered.get_uniform_location("a_gbuffer_idx");
	Uniform deffered_camera_pos = deffered.get_uniform_location("a_camera_pos");
	Uniform deffered_ambient = deffered.get_uniform_location("a_ambient");

	auto [width, height] = wnd.get_framebuffer_size();
	std::unique_ptr<GBuffer> gbuffer = std::make_unique<GBuffer>(width, height);
	auto gbuffer_texture_handle = gbuffer->get_texture().get_texture_handle();
	gbuffer_texture_handle.make_resident();
	deffered.uniform(deffered_gbuffer_idx, (glm::uvec2)gbuffer_texture_handle);

	//DemoScene scene;
	auto scene = create_test_scene(1);

	PerspectiveCamera camera;
	OrbitCamera controller(camera.transform);

	double old_time = WindowManager::get_time();
	double delta_time = 1;
	unsigned long long frame = 0;
	while (!wnd.should_close())
	{
		//Check if window got resized
		{
			auto [new_width, new_height] = wnd.get_framebuffer_size();
			if (width != new_width || height != new_height)
			{
				width = new_width;
				height = new_height;

				gbuffer = std::make_unique<GBuffer>(width, height);
				auto gbuffer_texture_handle = gbuffer->get_texture().get_texture_handle();
				gbuffer_texture_handle.make_resident();
				deffered.uniform(deffered_gbuffer_idx, (glm::uvec2)gbuffer_texture_handle);

				glViewport(0, 0, width, height);
			}
		}

		if (frame % 60 == 0)
		{
			std::cout << 1 / delta_time << '\n';
		}

		controller.update(wnd, delta_time);

		auto view = camera.get_view_matrix();
		//draw
		{
			gbuffer_prog.use();
			gbuffer_prog.uniform(gbuffer_camera, false, camera.get_projection_matrix(wnd.viewport()) * view);
			
			gbuffer->bind(GL_FRAMEBUFFER);
			glClear(GL_COLOR_BUFFER_BIT);

			scene->setup_state();
			scene->draw();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);

			deffered.use();
			deffered.uniform(deffered_camera_pos, camera.transform.get_position());
			deffered.uniform(deffered_ambient, ambient);

			scene->setup_state();
			glDrawArrays(GL_TRIANGLES, 0, 3);
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