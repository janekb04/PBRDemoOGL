#pragma once

#include "Vendor.h"
#include "Window.h"
#include "Event.h"

namespace GUI
{
	Event<> on_gui_render;

	void render_imgui()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		on_gui_render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			Context backup_current_context = Context::get_current();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			backup_current_context.make_current();
		}
	}

	void init_imgui(const Context& ctx)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		ImGui_ImplGlfw_InitForOpenGL(ctx.get_handle(), true);
		ImGui_ImplOpenGL3_Init();
	}
}
