#pragma once

#include "Scene.h"
#include <memory>

std::unique_ptr<Scene> create_test_scene(int obj_count)
{
	std::vector<Mesh> meshes{
			Mesh::from_file("res/container.obj"),
			Mesh::from_file("res/Table.obj")
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

	scene->materials.add({
		scene->add_texture(Image2d::from_file("res/metal_rusty_albedo.jpg", true)),
		scene->add_texture(Image2d::from_file("res/metal_rusty_normal.jpg", false)),
		scene->add_texture(Image2d::from_file("res/black.png", true)),
		scene->add_texture(Image2d::from_file("res/metal_rusty_roughness.jpg", true)),
		scene->add_texture(Image2d::from_file("res/metal_rusty_ao.jpg", true))
	});

	scene->point_lights.add({
		{10, 10, 10, 0},
		{0, 20, 0, 0}
	});

	scene->directional_lights.add({
		glm::normalize(glm::vec4{0, .9, 0.1, 0}),
		{10, 10, 10, 0}
	});

	unsigned side_length = ceil(cbrt(obj_count));
	unsigned face_size = side_length * side_length;
	for (int i = 0; i < obj_count; ++i)
	{
		Scene::Model model;
		model.model_transform = glm::translate(glm::mat4(1), glm::vec3(i / face_size * 2, (i % face_size) / side_length * 2, (i % face_size) % side_length * 2));
		model.material_idx = rand() % scene->materials.size();
		model.normal_mat = glm::mat3(glm::transpose(glm::inverse(model.model_transform)));

		scene->add_model(i % meshes.size(), model);
	}

	return scene;
}