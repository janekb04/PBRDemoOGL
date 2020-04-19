#pragma once

#include "Scene.h"
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
		1,
		1
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
		glm::normalize(glm::vec4{0.1, .9, 0.3, 0}),
		20.0f * glm::vec4{0.9294, 0.8196, 0.5216, 0}
	});

	unsigned side_length = ceil(cbrt(obj_count));
	unsigned face_size = side_length * side_length;
	for (int i = 0; i < obj_count; ++i)
	{
		Scene::Model model;
		model.model_transform = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 0, 1));
		model.model_transform = glm::rotate(model.model_transform, glm::radians(90.0f), glm::vec3(0, 1, 0));
		model.material_idx = rand() % scene->materials.size();
		model.normal_mat = glm::mat3(glm::transpose(glm::inverse(model.model_transform)));

		scene->add_model(i % meshes.size(), model);
	}

	return scene;
}