#pragma once

#include "Vendor.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "MultiDrawBuilder.h"
#include "Vertex.h"
#include "Texture2dArray.h"
#include "Mesh.h"
#include "Material.h"
#include "UnorderedArraySet.h"
#include "GPUVector.h"
#include "Utility.h"

class Scene
{
public:
	struct Model
	{
		glm::mat4 model_transform;
		int material_idx;
	};
private:
	template <typename T>
	using gpu_unordered_array_set = unordered_array_set<T, GPUVector<T>>;
private:
	VertexArray VAO;
	MultiDrawElementsBuilder<Vertex, GLuint, GPUVector, GPUVector, gpu_unordered_array_set> builder;
	Texture2dArray textures;

	gpu_unordered_array_set<Model> instance_data;
	GPUVector<Material> materials;
public:
	using MaterialHandle = typename GPUVector<Material>::iterator;
	using ModelHandle = std::pair<typename unordered_array_set<Model>::iterator, gpu_unordered_array_set<glDrawElementsIndirectCommand>::iterator>;
	using MeshHandle = typename decltype(builder)::MeshID;
	using TextureHandle = unsigned;
public:
	Scene(const std::vector<const char*>& texture_paths, const std::vector<Mesh>& meshes) :
		textures(Texture2dArray::from_files(texture_paths.data(), texture_paths.size()))
	{
		for (const Mesh& mesh : meshes)
			builder.add_mesh(mesh.vertices, mesh.indices);

		Buffer& VBO = builder.vertices().get_buffer();
		Buffer& EBO = builder.indices().get_buffer();
		Buffer& instanced_vbo = get_container(instance_data).get_buffer();

		const unsigned int VBO_IDX = 0;
		VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(Vertex));

		const VertexAttribute POS_IDX{ 0 };
		VAO.enable_attrib(POS_IDX);
		VAO.attrib_binding(POS_IDX, VBO_IDX);
		VAO.attrib_format(POS_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

		const VertexAttribute NORMAL_IDX{ 1 };
		VAO.enable_attrib(NORMAL_IDX);
		VAO.attrib_binding(NORMAL_IDX, VBO_IDX);
		VAO.attrib_format(NORMAL_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));

		const VertexAttribute UV_IDX{ 2 };
		VAO.enable_attrib(UV_IDX);
		VAO.attrib_binding(UV_IDX, VBO_IDX);
		VAO.attrib_format(UV_IDX, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));

		const VertexAttribute TANGENT_IDX{ 3 };
		VAO.enable_attrib(TANGENT_IDX);
		VAO.attrib_binding(TANGENT_IDX, VBO_IDX);
		VAO.attrib_format(TANGENT_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));

		const unsigned int INSTANCED_VBO_IDX = 1;
		VAO.vertex_buffer(INSTANCED_VBO_IDX, instanced_vbo, 0, sizeof(Model));
		VAO.binding_divisor(INSTANCED_VBO_IDX, 1);
		const VertexAttribute MODEL_IDX{ 4 };
		for (int i = 0; i < 4; ++i)
		{
			VertexAttribute column{ MODEL_IDX.index() + i };
			VAO.enable_attrib(column);
			VAO.attrib_binding(column, INSTANCED_VBO_IDX);
			VAO.attrib_format(column, 4, GL_FLOAT, GL_FALSE, offsetof(Model, model_transform) + sizeof(glm::mat4::col_type) * i);
		}
		const VertexAttribute MATERIAL_IDX_IDX{ 8 };
		VAO.enable_attrib(MATERIAL_IDX_IDX);
		VAO.attrib_binding(MATERIAL_IDX_IDX, INSTANCED_VBO_IDX);
		VAO.attrib_format_i(MATERIAL_IDX_IDX, 1, GL_INT, offsetof(Model, material_idx));

		VAO.element_buffer(EBO);
	}

public:
	MaterialHandle add_material(const Material& material)
	{
		return materials.push_back(material);
	}
	Material& get_material(MaterialHandle material)
	{
		return *material;
	}

	ModelHandle add_model(MeshHandle mesh_id, MaterialHandle material)
	{
		unsigned material_idx = &*material - materials.data();
		auto data_it = instance_data.insert({ glm::mat4(1.0f),  material_idx});
		builder.add_batch(mesh_id, 1);
		auto command_it = std::prev(builder.commands().end());

		return { data_it, command_it };
	}
	Model& get_model(ModelHandle model_id)
	{
		return *model_id.first;
	}
	void set_model_mesh(ModelHandle model_id, MeshHandle mesh)
	{
		builder.change_batch_mesh(model_id.second, mesh);
	}
	void remove_model(ModelHandle model_id)
	{
		instance_data.erase(model_id.first);
		const_cast<gpu_unordered_array_set<glDrawElementsIndirectCommand>&>(builder.commands()).erase(model_id.second);
	}
public:
	void draw() const
	{
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, builder.commands().size(), 0);
	}

	void setup_state()
	{
		VAO.bind();
		get_container(builder.commands()).get_buffer().bind(GL_DRAW_INDIRECT_BUFFER);

		glActiveTexture(GL_TEXTURE0);
		textures.bind();
	}
};