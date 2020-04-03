#pragma once

#include "Scene.h"

std::unique_ptr<Scene> create_test_scene(int obj_count)
{
	std::unique_ptr<Scene> scene;
	int mat_count = 0;
	{
		std::vector<Image2d> images;
		images.push_back(Image2d::from_file("res/concrete_albedo.jpg", true));
		images.push_back(Image2d::from_file("res/concrete_gloss.jpg", true));
		images.push_back(Image2d::from_file("res/concrete_normal.jpg", false));
		images.push_back(Image2d::from_file("res/metal_albedo.jpg", true));
		images.push_back(Image2d::from_file("res/metal_gloss.jpg", true));
		images.push_back(Image2d::from_file("res/metal_normal.jpg", false));

		mat_count = images.size() / 3;

		Image2dArray image_array = Image2dArray::from_images(images.begin(), images.end());
		scene = std::unique_ptr<Scene>{new Scene(
			image_array,
			{
				Mesh::from_file("res/container.obj"),
				Mesh::from_file("res/Table.obj")
			},
			obj_count,
			mat_count
		)};
	}

	for (int i = 0; i < mat_count; ++i)
	{
		int base = 3 * i;
		scene->add_material({ base, base + 1, base + 2 });
	}

	unsigned side_length = ceil(cbrt(obj_count));
	unsigned face_size = side_length * side_length;
	for (int i = 0; i < obj_count; ++i)
	{
		Scene::Model model;
		model.model_transform = glm::translate(glm::mat4(1), glm::vec3(i / face_size * 2, (i % face_size) / side_length * 2, (i % face_size) % side_length * 2));
		//model.model_transform = glm::rotate(model.model_transform, glm::radians<float>(rand() % 360), glm::vec3(0, 1, 0));
		model.material_idx = rand() % mat_count;
		model.normal_mat = glm::mat3(glm::transpose(glm::inverse(model.model_transform)));

		scene->add_model(i & 1, model);
	}

	return scene;
}