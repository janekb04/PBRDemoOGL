#pragma once

#include "Scene.h"

std::unique_ptr<Scene> create_test_scene(int obj_count)
{
	std::unique_ptr<Scene> scene { new Scene(
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
		},
		{
			Mesh::from_file("res/container.obj"),
			Mesh::from_file("res/Table.obj")
		}
	)};

	std::vector<Scene::MaterialHandle> materials;
	materials.reserve(scene->texture_count());
	for (int i = 0; i < scene->texture_count(); ++i)
		materials.push_back(scene->add_material({ i }));

	for (int i = 0; i < obj_count; ++i)
		scene->add_model(i & 2, materials[i % materials.size()]);

	return scene;
}