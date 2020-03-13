#pragma once

#include "Vendor.h"

class Monitor
{
private:
	GLFWmonitor* handle;
private:
	Monitor(GLFWmonitor* handle) :
		handle(handle)
	{
	}
public:
	Monitor() :
		handle(nullptr)
	{
	}
	Monitor(Monitor&& other) :
		handle(other.handle)
	{
		other.handle = nullptr;
	}
public: //static
	static Monitor get_primary_monitor()
	{
		return Monitor(glfwGetPrimaryMonitor());
	}
	static std::vector<Monitor> enumerate_monitors()
	{
		int count = 0;
		GLFWmonitor** raw = glfwGetMonitors(&count);

		std::vector<Monitor> result;
		result.reserve(count);

		for (int i = 0; i < count; ++i)
		{
			result.push_back(Monitor{ raw[i] });
		}

		return result;
	}
public:
	GLFWmonitor* get_handle() const
	{
		return handle;
	}
	bool is_null() const
	{
		return handle == nullptr;
	}
};

class Context
{
private:
	GLFWwindow* handle;
private:
	Context(GLFWwindow* handle) :
		handle(handle)
	{
	}
	friend class Window;
public:
	Context() :
		handle(nullptr)
	{
	}
	Context(const Context& other) :
		handle(other.handle)
	{
	}
	Context(Context&& other) :
		handle(other.handle)
	{
		other.handle = nullptr;
	}
	Context& operator=(const Context& other)
	{
		handle = other.handle;
		return *this;
	}
	Context& operator=(Context&& other)
	{
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}
public:
	static Context get_current()
	{
		return Context(glfwGetCurrentContext());
	}
public:
	GLFWwindow* get_handle() const
	{
		return handle;
	}
	bool is_null() const
	{
		return handle == nullptr;
	}
	void make_current() const
	{
		glfwMakeContextCurrent(handle);
	}
};

static class WindowManager
{
private:
	inline static std::thread::id main_thread_id = std::this_thread::get_id();;
private:
	static void error_callback(int error_code, const char* description)
	{
		throw std::runtime_error("GLFW error(" + std::to_string(error_code) + "): " + description);
	}
public:
	static inline void force_main_thread(const std::string& operation_name)
	{
		if (std::this_thread::get_id() != main_thread_id)
		{
			throw std::logic_error("Operation " + operation_name + " can only be performed on the main thread");
		}
	}
	static void init()
	{
		force_main_thread("WindowManager::init");
		if (glfwInit() != GLFW_TRUE)
			throw std::runtime_error("failed to initialize glfw");
		glfwSetErrorCallback(error_callback);
	}
	static void poll_events()
	{
		force_main_thread("WindowManager::poll_events");
		glfwPollEvents();
	}
	static void wait_events()
	{
		force_main_thread("WindowManager::wait_events");
		glfwWaitEvents();
	}
	static void terminate()
	{
		force_main_thread("WindowManager::terminate");
		glfwTerminate();
	}
public:
	static constexpr int DONT_CARE = GLFW_DONT_CARE;
};

class Window
{
private: //private members
	GLFWwindow* handle;
	Context context;
public:
	enum class ClientApi
	{
		OPENGL = GLFW_OPENGL_API,
		OPENGL_ES = GLFW_OPENGL_ES_API,
		NO = GLFW_NO_API
	};
	enum class ContextCreationApi
	{
		NATIVE = GLFW_NATIVE_CONTEXT_API,
		EGL = GLFW_EGL_CONTEXT_API,
		OSMESA = GLFW_OSMESA_CONTEXT_API
	};
	enum class ContextRobustness
	{
		NO_ROBUSTNESS = GLFW_NO_ROBUSTNESS,
		NO_RESET_NOTIFICATION = GLFW_NO_RESET_NOTIFICATION,
		LOSE_CONTEXT_ON_RESET = GLFW_LOSE_CONTEXT_ON_RESET
	};
	enum class ContextReleaseBehaviour
	{
		ANY = GLFW_ANY_RELEASE_BEHAVIOR,
		FLUSH = GLFW_RELEASE_BEHAVIOR_FLUSH,
		NONE = GLFW_RELEASE_BEHAVIOR_NONE
	};
	enum class OpenGLProfile
	{
		ANY = GLFW_OPENGL_ANY_PROFILE,
		COMPAT = GLFW_OPENGL_COMPAT_PROFILE,
		CORE = GLFW_OPENGL_CORE_PROFILE
	};
	struct CreationHints
	{
		bool resizable = true;
		bool visible = true;
		bool decorated = true;
		bool focused = true;
		bool auto_iconify = true;
		bool floating = false;
		bool maximized = false;
		bool center_cursor = true;
		bool transparent_framebuffer = false;
		bool focus_on_show = true;
		bool scale_to_monitor = false;
		int red_bits = 8;
		int green_bits = 8;
		int blue_bits = 8;
		int alpha_bits = 8;
		int depth_bits = 24;
		int stencil_bits = 8;
		int accum_red_bits = 0;
		int accum_green_bits = 0;
		int accum_blue_bits = 0;
		int accum_alpha_bits = 0;
		int aux_buffers = 0;
		int samples = 0;
		int refresh_rate = WindowManager::DONT_CARE;
		bool stereo = false;
		bool srgb_capable = false;
		bool doublebuffer = true;

		ClientApi client_api = ClientApi::OPENGL;
		ContextCreationApi context_creation_api = ContextCreationApi::NATIVE;
		int context_version_major = 1;
		int context_version_minor = 0;
		ContextRobustness context_robustness = ContextRobustness::NO_ROBUSTNESS;
		ContextReleaseBehaviour context_release_behaviour = ContextReleaseBehaviour::ANY;
		bool opengl_forward_compat = false;
		bool opengl_debug_context = false;
		OpenGLProfile opengl_profile = OpenGLProfile::ANY;
		bool cocoa_retina_framebuffer = true;
		const char* cocoa_frame_name = "";
		bool cocoa_graphics_switching = false;
		const char* x11_class_name = "";
		const char* x11_instance_name = "";
	};
	struct Options
	{
		int width;
		int height;
		std::string title;
		Monitor monitor;
		Context share;
	};
private: //private static methods
	static GLFWwindow* create_window(const Window& self, const Options& options, const CreationHints& hints)
	{
		WindowManager::force_main_thread("Window::Window");

		glfwWindowHint(GLFW_RESIZABLE, hints.resizable);
		glfwWindowHint(GLFW_VISIBLE, hints.visible);
		glfwWindowHint(GLFW_DECORATED, hints.decorated);
		glfwWindowHint(GLFW_FOCUSED, hints.focused);
		glfwWindowHint(GLFW_AUTO_ICONIFY, hints.auto_iconify);
		glfwWindowHint(GLFW_FLOATING, hints.floating);
		glfwWindowHint(GLFW_MAXIMIZED, hints.maximized);
		glfwWindowHint(GLFW_CENTER_CURSOR, hints.center_cursor);
		glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, hints.transparent_framebuffer);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, hints.focus_on_show);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, hints.scale_to_monitor);
		glfwWindowHint(GLFW_RED_BITS, hints.red_bits);
		glfwWindowHint(GLFW_GREEN_BITS, hints.green_bits);
		glfwWindowHint(GLFW_BLUE_BITS, hints.blue_bits);
		glfwWindowHint(GLFW_ALPHA_BITS, hints.alpha_bits);
		glfwWindowHint(GLFW_DEPTH_BITS, hints.depth_bits);
		glfwWindowHint(GLFW_STENCIL_BITS, hints.stencil_bits);
		glfwWindowHint(GLFW_ACCUM_RED_BITS, hints.accum_red_bits);
		glfwWindowHint(GLFW_ACCUM_GREEN_BITS, hints.accum_green_bits);
		glfwWindowHint(GLFW_ACCUM_BLUE_BITS, hints.accum_blue_bits);
		glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, hints.accum_alpha_bits);
		glfwWindowHint(GLFW_AUX_BUFFERS, hints.aux_buffers);
		glfwWindowHint(GLFW_SAMPLES, hints.samples);
		glfwWindowHint(GLFW_REFRESH_RATE, hints.refresh_rate);
		glfwWindowHint(GLFW_STEREO, hints.stereo);
		glfwWindowHint(GLFW_SRGB_CAPABLE, hints.srgb_capable);
		glfwWindowHint(GLFW_DOUBLEBUFFER, hints.doublebuffer);
		glfwWindowHint(GLFW_DOUBLEBUFFER, hints.doublebuffer);
		glfwWindowHint(GLFW_CLIENT_API, (int)hints.client_api);
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, (int)hints.context_creation_api);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, hints.context_version_major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, hints.context_version_minor);
		glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, (int)hints.context_robustness);
		glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, (int)hints.context_release_behaviour);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, hints.opengl_forward_compat);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, hints.opengl_debug_context);
		glfwWindowHint(GLFW_OPENGL_PROFILE, (int)hints.opengl_profile);
		glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, hints.cocoa_retina_framebuffer);
		glfwWindowHintString(GLFW_COCOA_FRAME_NAME, hints.cocoa_frame_name);
		glfwWindowHint(GLFW_COCOA_GRAPHICS_SWITCHING, hints.cocoa_graphics_switching);
		glfwWindowHintString(GLFW_X11_CLASS_NAME, hints.x11_class_name);
		glfwWindowHintString(GLFW_X11_INSTANCE_NAME, hints.x11_instance_name);

		GLFWwindow* handle = glfwCreateWindow(
			options.width,
			options.height,
			options.title.c_str(),
			options.monitor.get_handle(),
			options.share.get_handle()
		);
		if (handle == nullptr)
		{
			throw std::runtime_error("Failed to create Window");
		}

		glfwSetWindowUserPointer(handle, const_cast<Window*>(&self));
		
		//Window
		glfwSetFramebufferSizeCallback(handle, framebuffer_size_handler);
		glfwSetWindowRefreshCallback(handle, refresh_handler);
		glfwSetWindowFocusCallback(handle, focus_handler);
		glfwSetWindowIconifyCallback(handle, minimize_handler);
		glfwSetWindowMaximizeCallback(handle, maximize_handler);
		glfwSetWindowCloseCallback(handle, close_handler);
		glfwSetWindowPosCallback(handle, position_handler);
		glfwSetWindowSizeCallback(handle, size_handler);
		glfwSetWindowContentScaleCallback(handle, content_scale_handler);
		//Input
		glfwSetDropCallback(handle, drop_handler);

		return handle;
	}
	static Context create_context(const Window& self, const Options& options, const CreationHints& hints)
	{
		if (options.share.is_null())
		{
			return Context(self.handle);
		}
		else
		{
			return options.share;
		}
	}

	static inline Window* get_instance(GLFWwindow* HWND) noexcept
	{
		Window* instance = (Window*)(glfwGetWindowUserPointer(HWND));
		assert(instance != nullptr);
		return instance;
	}

	//Window callback handlers
	static void framebuffer_size_handler(GLFWwindow* HWND, int x, int y)
	{
		auto instance = get_instance(HWND);
		instance->framebuffer_size_callback(x, y);
	}
	static void refresh_handler(GLFWwindow* HWND)
	{
		auto instance = get_instance(HWND);
		instance->refresh_callback();
	}
	static void focus_handler(GLFWwindow* HWND, int state)
	{
		auto instance = get_instance(HWND);
		instance->focus_callback(state);
	}
	static void minimize_handler(GLFWwindow* HWND, int state)
	{
		auto instance = get_instance(HWND);
		instance->minimize_callback(state);
	}
	static void maximize_handler(GLFWwindow* HWND, int state)
	{
		auto instance = get_instance(HWND);
		instance->maximize_callback(state);
	}
	static void close_handler(GLFWwindow* HWND)
	{
		auto instance = get_instance(HWND);
		instance->close_callback();
	}
	static void position_handler(GLFWwindow* HWND, int x, int y)
	{
		auto instance = get_instance(HWND);
		instance->position_callback(x, y);
	}
	static void size_handler(GLFWwindow* HWND, int x, int y)
	{
		auto instance = get_instance(HWND);
		instance->size_callback(x, y);
	}
	static void content_scale_handler(GLFWwindow* HWND, float x, float y)
	{
		auto instance = get_instance(HWND);
		instance->content_scale_callback(x, y);
	}
	//Input callback handlers
	static void drop_handler(GLFWwindow* HWND, int amount, const char** paths)
	{
		auto instance = get_instance(HWND);
		instance->drop_callback(amount, paths);
	}
protected: // callbacks
	//Window callbacks
	virtual void framebuffer_size_callback(int width, int height) {}
	virtual void refresh_callback() {}
	virtual void focus_callback(bool focused) {}
	virtual void minimize_callback(bool minimized) {}
	virtual void maximize_callback(bool maximized) {}
	virtual void close_callback() {}
	virtual void position_callback(int x, int y) {}
	virtual void size_callback(int width, int height) {}
	virtual void content_scale_callback(float x, float y) {}
	//Input callbacks
	virtual void drop_callback(int count, const char** paths) {}

public:	//public essential methods
	Window(const Options& options, const CreationHints& hints = {}) :
		handle(create_window(*this, options, hints)),
		context(create_context(*this, options, hints))
	{
	}

	Window(Window&& other) :
		handle(other.handle),
		context(std::move(other.context))
	{
		other.handle = nullptr;
	}

	Window& operator=(const Window&) = delete;
	Window& operator=(Window&& other) = delete;

	virtual ~Window() 
	{
		WindowManager::force_main_thread("Window::~Window");
		glfwDestroyWindow(handle);
	}
public: // window options
	bool should_close() const noexcept
	{
		return glfwWindowShouldClose(handle);
	}

	void swap_buffers() const noexcept
	{
		glfwSwapBuffers(handle);
	}

	void set_should_close(bool value) const noexcept
	{
		glfwSetWindowShouldClose(handle, value);
	}

	std::pair<int, int> get_framebuffer_size() const noexcept
	{
		int width, height;
		glfwGetFramebufferSize(handle, &width, &height);
		return { width, height };
	}

public: //Input
	bool is_key_pressed(int key_code)
	{
		return glfwGetKey(handle, key_code) == GLFW_PRESS;
	}
public:
	GLFWwindow* get_handle() const
	{
		return handle;
	}
	const Context get_context() const
	{
		return context;
	}
	Context get_context()
	{
		return context;
	}
};