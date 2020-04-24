#pragma once

#include "Scene.h"
#include "GUI.h"
#include <memory>

std::unique_ptr<Scene> create_test_scene(int obj_count)
{
	std::vector<Mesh> meshes{
			Mesh::from_file("res/meshes/Cerberus_LP.FBX"),
	};

	std::vector<Scene::PointLight> point_lights;
	std::vector<Scene::Spotlight> spotlights;
	
	auto scene = std::make_unique<Scene>( 
		meshes,
		obj_count,
		1,
		4,
		4,
		4
	);

	//scene->materials.add({
	//	scene->add_texture(Image2d::from_file("res/metal_rusty_albedo.jpg", true)),
	//	scene->add_texture(Image2d::from_file("res/metal_rusty_normal.jpg", false)),
	//	scene->add_texture(Image2d::from_file("res/black.png", true)),
	//	scene->add_texture(Image2d::from_file("res/metal_rusty_roughness.jpg", true)),
	//	scene->add_texture(Image2d::from_file("res/metal_rusty_ao.jpg", true))
	//});
	scene->materials.add({
		scene->add_texture(Image2d::from_file("res/textures/Cerberus_A.tga", true)),
		scene->add_texture(Image2d::from_file("res/textures/Cerberus_N.tga", false)),
		scene->add_texture(Image2d::from_file("res/textures/Cerberus_M.tga", true)),
		scene->add_texture(Image2d::from_file("res/textures/Cerberus_R.tga", true)),
		scene->add_texture(Image2d::from_file("res/textures/white.png", true))
	});

	scene->directional_lights.add({
		glm::normalize(glm::vec4{-0.4, .9, 0.3, 0}),
		50.0f * glm::vec4{1, 0.2, 0.2, 0}
	});
	scene->directional_lights.add({
	glm::normalize(glm::vec4{0.4, .9, 0.3, 0}),
		50.0f * glm::vec4{0.2, 0.2, 1, 0}
	});

	unsigned side_length = ceil(cbrt(obj_count));
	unsigned face_size = side_length * side_length;
	for (int i = 0; i < obj_count; ++i)
	{
		Scene::Model model;
		model.model_transform = glm::translate(glm::mat4(1), glm::vec3(0, 0, i * 50));
		model.model_transform = glm::rotate(model.model_transform, glm::radians(90.0f), glm::vec3(0, 0, 1));
		model.model_transform = glm::rotate(model.model_transform, glm::radians(90.0f), glm::vec3(0, 1, 0));
		model.material_idx = rand() % scene->materials.size();
		model.normal_mat = glm::mat3(glm::transpose(glm::inverse(model.model_transform)));

		scene->add_model(i % meshes.size(), model);
	}

	GUI::on_gui_render += std::bind([](Scene* scene) {

		if (ImGui::Begin("Settings"))
		{
			if (ImGui::TreeNode("Directional lights"))
			{
				for (int i = 0; i < scene->directional_lights.size(); ++i)
				{
					ImGui::PushID(&scene->directional_lights + i);
					if (ImGui::TreeNode("%i", std::to_string(i).c_str()))
					{
						Scene::DirectionalLight light = scene->directional_lights.get(i);
					
						ImGui::DragFloat3("Direction", &light.direction[0], 0.1f, -1, 1);
						light.direction = glm::normalize(light.direction);

						ColorEdit3HDR(&light.color[0]);

						scene->directional_lights.set(i, light);

						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				if (scene->directional_lights.size() < scene->directional_lights.capacity())
				{
					if (ImGui::Button("Add light"))
					{
						scene->directional_lights.add(Scene::DirectionalLight{
								glm::vec4(0, 1, 0, 0),
								glm::vec4(0.001, 0.001, 0.001, 0)
						});
					}
				}
				else
				{
					ImGui::Text("Maximum amount of lights reached");
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Point lights"))
			{
				for (int i = 0; i < scene->point_lights.size(); ++i)
				{
					ImGui::PushID(&scene->point_lights + i);
					if (ImGui::TreeNode("%i", std::to_string(i).c_str()))
					{
						Scene::PointLight light = scene->point_lights.get(i);

						ImGui::DragFloat3("Position", &light.pos[0], 0.1f, 0, 0);

						ColorEdit3HDR(&light.color[0]);

						scene->point_lights.set(i, light);

						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				if (scene->point_lights.size() < scene->point_lights.capacity())
				{
					if (ImGui::Button("Add light"))
					{
						scene->point_lights.add(Scene::PointLight{
							glm::vec4(0, 0, 0, 0),
							glm::vec4(0.001, 0.001, 0.001, 0)
						});
					}
				}
				else
				{
					ImGui::Text("Maximum amount of lights reached");
				}
				ImGui::TreePop();
			}
		}
		ImGui::End();
	}, scene.get());

	return scene;
}