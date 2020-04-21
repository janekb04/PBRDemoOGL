#include "Vendor.h"
#include "Window.h"
#include "gl/Shader.h"
#include "Utility.h"
#include "gl/VertexArray.h"
#include "gl/Buffer.h"
#include "Defines.h"
#include "Viewer.h"
#include "OpenGLWindow.h"
#include "OrbitCamera.h"
#include "TestScene.h"
#include "GUI.h"
#include "GBuffer.h"
#include "Camera.h"

int main()
{
    WindowManager::init();
	OpenGLWindow wnd;
	GUI::init_imgui(wnd.get_context());

	WindowManager::set_swap_interval(0);
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
		Shader{read_file("res/shaders/deffered.comp").c_str(), GL_COMPUTE_SHADER}
	};
	ShaderProgram draw_to_screen{
		Shader{read_file("res/shaders/fullscreen_triangle.vert").c_str(), GL_VERTEX_SHADER},
		Shader{read_file("res/shaders/draw_to_framebuffer.frag").c_str(), GL_FRAGMENT_SHADER}
	};

	Uniform gbuffer_camera = gbuffer_prog.get_uniform_location("a_camera");
	Uniform deffered_gbuffer_idx = deffered.get_uniform_location("a_gbuffer_idx");
	Uniform deffered_output_idx = deffered.get_uniform_location("a_output_idx");
	Uniform deffered_camera_pos = deffered.get_uniform_location("a_camera_pos");
	Uniform deffered_ambient = deffered.get_uniform_location("a_ambient");
	Uniform draw_to_screen_input_idx = draw_to_screen.get_uniform_location("a_input_idx");

	//DemoScene scene;
	auto scene = create_test_scene(1);

	Camera camera = Camera{ {}, std::make_unique<PerspectiveViewer>() };
	OrbitCamera controller(camera.transform);

	std::unique_ptr<GBuffer> gbuffer;
	std::unique_ptr<Texture2d> output;

	int width = 0, height = 0;
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

				output = std::make_unique<Texture2d>();
				output->storage(1, GL_RGBA16F, width, height);
				auto output_image_handle = output->get_image_handle(0, false, 0, GL_RGBA16F);
				output_image_handle.make_resident(GL_WRITE_ONLY);
				deffered.uniform(deffered_output_idx, output_image_handle);
				auto output_texture_handle = output->get_texture_handle();
				output_texture_handle.make_resident();
				draw_to_screen.uniform(draw_to_screen_input_idx, output_texture_handle);

				glViewport(0, 0, width, height);
			}
		}

		if (frame % 60 == 0)
		{
			std::cout << 1 / delta_time << '\n';
		}

		controller.update(wnd, delta_time);

		//draw
		{	
			scene->setup_state();

			gbuffer_prog.use();
			gbuffer_prog.uniform(gbuffer_camera, false, camera.viewer->get_projection_matrix(wnd.viewport()) * camera.viewer->get_view_matrix(camera.transform));
			
			gbuffer->bind(GL_FRAMEBUFFER);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			scene->draw();

			deffered.use();
			deffered.uniform(deffered_camera_pos, camera.transform.get_position());
			deffered.uniform(deffered_ambient, ambient);

			glDispatchCompute(width / 8, height / 8, 1);

			//Data in texture 'output'. lets draw it to screen

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			draw_to_screen.use();

			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
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