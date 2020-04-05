#pragma once

#include "Scene.h"

std::unique_ptr<Scene> create_test_scene(int obj_count)
{
	const int MAX_MATS = 2;
	auto scene = std::unique_ptr<Scene>{ new Scene(
		{
			Mesh::from_file("res/container.obj"),
			Mesh::from_file("res/Table.obj")
		},
		obj_count,
		MAX_MATS
	)};

	scene->add_material({
		scene->add_texture(Image2d::from_file("res/concrete_albedo.jpg", true)),
		scene->add_texture(Image2d::from_file("res/concrete_gloss.jpg", true)),
		scene->add_texture(Image2d::from_file("res/concrete_normal.jpg", false)),
	});
	scene->add_material({
		scene->add_texture(Image2d::from_file("res/metal_albedo.jpg", true)),
		scene->add_texture(Image2d::from_file("res/metal_gloss.jpg", true)),
		scene->add_texture(Image2d::from_file("res/metal_normal.jpg", false)),
	});

	unsigned side_length = ceil(cbrt(obj_count));
	unsigned face_size = side_length * side_length;
	for (int i = 0; i < obj_count; ++i)
	{
		Scene::Model model;
		model.model_transform = glm::translate(glm::mat4(1), glm::vec3(i / face_size * 2, (i % face_size) / side_length * 2, (i % face_size) % side_length * 2));
		//model.model_transform = glm::rotate(model.model_transform, glm::radians<float>(rand() % 360), glm::vec3(0, 1, 0));
		model.material_idx = rand() % scene->material_count();
		model.normal_mat = glm::mat3(glm::transpose(glm::inverse(model.model_transform)));

		scene->add_model(i & 1, model);
	}

	return scene;
}