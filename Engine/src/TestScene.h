#pragma once

#include "Scene.h"

std::unique_ptr<Scene> create_test_scene(int obj_count)
{
	std::vector<const char*> texture_paths
	{
		"res/container.png",
		"res/wood.png",
		"res/grass.jpg",
		"res/uv.png",
		"res/plastic.jpg",
		"res/cobble.jpg",
		"res/stone.png",
		"res/brick.jpg",
		"res/concrete.jpg"
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

	std::vector<Scene::MaterialHandle> materials;
	materials.reserve(scene->texture_count());
	for (int i = 0; i < scene->texture_count(); ++i)
		materials.push_back(scene->add_material({ i }));

	unsigned side_length = ceil(cbrt(obj_count));
	unsigned face_size = side_length * side_length;
	for (int i = 0; i < obj_count; ++i)
	{
		Scene::Model model;
		model.model_transform = glm::translate(glm::mat4(1), glm::vec3(i / face_size * 2, (i % face_size) / side_length * 2, (i % face_size) % side_length * 2));
		//model.model_transform = glm::rotate(model.model_transform, glm::radians<float>(rand() % 360), glm::vec3(0, 1, 0));
		model.material_idx = rand() % texture_paths.size();
		model.normal_mat = glm::mat3(glm::transpose(glm::inverse(model.model_transform)));

		scene->add_model(i & 1, model);
	}

	return scene;
}