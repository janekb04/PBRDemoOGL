#pragma once

#include "Scene.h"

std::unique_ptr<Scene> create_test_scene(int obj_count)
{
	std::vector<const char*> texture_paths
	{
		"res/concrete_albedo.jpg",
		"res/concrete_gloss.jpg",
		"res/metal_diffuse.jpg",
		"res/metal_gloss.jpg"
	};
	std::unique_ptr<Scene> scene{ new Scene(
		texture_paths,
		{
			Mesh::from_file("res/container.obj"),
			Mesh::from_file("res/Table.obj")
		},
		obj_count,
		texture_paths.size()
	)};

	scene->add_material({ 0, 1 });
	scene->add_material({ 2, 3 });

	unsigned side_length = ceil(cbrt(obj_count));
	unsigned face_size = side_length * side_length;
	for (int i = 0; i < obj_count; ++i)
	{
		Scene::Model model;
		model.model_transform = glm::translate(glm::mat4(1), glm::vec3(i / face_size * 2, (i % face_size) / side_length * 2, (i % face_size) % side_length * 2));
		//model.model_transform = glm::rotate(model.model_transform, glm::radians<float>(rand() % 360), glm::vec3(0, 1, 0));
		model.material_idx = rand() % 2;
		model.normal_mat = glm::mat3(glm::transpose(glm::inverse(model.model_transform)));

		scene->add_model(i & 1, model);
	}

	return scene;
}