#pragma once

#include "Vendor.h"
#include "Window.h"
#include "Viewport.h"
#include "Event.h"

//window with OpenGL context
class OpenGLWindow : public Window
{
	static void APIENTRY opengl_error_callback(
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
			//__debugbreak();
		std::clog << message << '\n';
	}
	static CreationHints create_hints()
	{
		Window::CreationHints hints;
		hints.context_version_major = 4;
		hints.context_version_minor = 6;
		hints.opengl_profile = Window::OpenGLProfile::CORE;
		hints.decorated = false;
#ifndef NDEBUG
		hints.opengl_debug_context = true;
#endif // !NDEBUG
		return hints;
	}
	static Options create_options()
	{
		Window::Options options;
		options.width = 1920;
		options.height = 1080;
		options.monitor = Monitor::get_primary_monitor();
        return options;
	}
private:
	double scroll;
	double cursor_x, cursor_y;
	Event<int, int> m_on_framebuffer_size;
protected:
	virtual void framebuffer_size_callback(int width, int height) override 
	{
		m_on_framebuffer_size(width, height);
	}
	virtual void scroll_callback(double xoffset, double yoffset) override
	{
		scroll = yoffset;
	}
public:
	OpenGLWindow() : 
		Window(create_options(), create_hints())
	{
		get_context().make_current();

		if (glewInit() != GLEW_OK)
		{
			throw std::runtime_error("failed to initialize GLEW");
		}

#ifndef NDEBUG
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(opengl_error_callback, NULL);
#endif // DEBUG
	}

	Viewport viewport() const
	{
		auto [width, height] = get_framebuffer_size();
		return Viewport{ {0,0}, {width, height} };
	}

	void end_frame()
	{
		scroll = 0;
		auto [cx, cy] = get_cursor_position();
		cursor_x = cx;
		cursor_y = cy;
		swap_buffers();
	}

	double get_scroll() const
	{
		return scroll;
	}

	std::pair<double, double> cursor_delta() const
	{
		auto [new_x, new_y] = get_cursor_position();
		return { new_x - cursor_x, new_y - cursor_y };
	}

	const Event<int, int>& on_framebuffer_size() const 
	{
		return m_on_framebuffer_size;
	}
};