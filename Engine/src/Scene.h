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
#include "MappedGPUArray.h"
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
	using MeshData = MeshBuilder<Vertex, GLuint>::MeshData;
	using BatchHandle = unsigned;
private:
	std::vector<MeshData> m_meshes;
	VertexArray VAO;
	Buffer VBO;
	Buffer EBO;
	Buffer m_commands;
	Buffer instance_data;
	Buffer materials;
	unsigned m_material_count;
	unsigned m_instances = 0;
	unsigned m_model_count = 0;

	Texture2dArray textures;
	size_t texture_num;

public:
	using MaterialHandle = unsigned;
	using ModelHandle = unsigned;
	using MeshHandle = unsigned;
	using TextureHandle = unsigned;
private:
private:
	BatchHandle add_batch(MeshHandle mesh_id, unsigned instances)
	{
		const MeshData& mesh = m_meshes[mesh_id];

		glDrawElementsIndirectCommand cmd;
		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;
		cmd.baseInstance = m_instances;
		cmd.primCount = instances;

		m_commands.sub_data(sizeof(glDrawElementsIndirectCommand) * m_model_count, sizeof(glDrawElementsIndirectCommand), &cmd);
		m_instances += instances;
		++m_model_count;

		return m_model_count - 1;
	}

	void change_batch_mesh(BatchHandle batch_id, MeshHandle mesh_id)
	{
		const MeshData& mesh = m_meshes[mesh_id];

		glDrawElementsIndirectCommand cmd;
		m_commands.get_sub_data(sizeof(glDrawElementsIndirectCommand) * batch_id, sizeof(glDrawElementsIndirectCommand), &cmd);

		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;

		m_commands.sub_data(sizeof(glDrawElementsIndirectCommand) * batch_id, sizeof(glDrawElementsIndirectCommand), &cmd);
	}
public:
	Scene(const std::vector<const char*>& texture_paths, const std::vector<Mesh>& meshes, size_t max_models, size_t max_materials) :
		textures(Texture2dArray::from_files(texture_paths.data(), texture_paths.size())),
		texture_num(texture_paths.size())
	{
		{
			MeshBuilder<Vertex, unsigned> builder;
		
			for (const auto& mesh : meshes)
				builder.add_mesh(mesh.vertices, mesh.indices);

			VBO.storage(sizeof(Vertex) * builder.vertices().size(), builder.vertices().data(), 0);
			EBO.storage(sizeof(GLuint) * builder.indices().size(), builder.indices().data(), 0);
			m_meshes = builder.mesh_data();
		}
		m_commands.storage(sizeof(glDrawElementsIndirectCommand) * max_models, nullptr, GL_DYNAMIC_STORAGE_BIT);
		instance_data.storage(sizeof(Model) * max_models, nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_model_count = 0;

		materials.storage(sizeof(Material) * max_materials, nullptr, GL_DYNAMIC_STORAGE_BIT);
		m_material_count = 0;

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
		VAO.vertex_buffer(INSTANCED_VBO_IDX, instance_data, 0, sizeof(Model));
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
		materials.sub_data(sizeof(Material) * m_material_count, sizeof(Material), &material);
		++m_material_count;
		return m_material_count - 1;
	}
	Material get_material(MaterialHandle material_id)
	{
		Material material;
		instance_data.get_sub_data(sizeof(Material) * material_id, sizeof(Material), &material);
		return material;
	}

	ModelHandle add_model(MeshHandle mesh_id, const Model& model)
	{
		instance_data.sub_data(sizeof(Model) * m_model_count, sizeof(Model), &model);
		add_batch(mesh_id, 1);
		return m_model_count - 1;
	}
	Model get_model_data(ModelHandle model_id)
	{
		Model model;
		instance_data.get_sub_data(sizeof(Model) * model_id, sizeof(Model), &model);
		return model;
	}
	void set_model_mesh(ModelHandle model_id, MeshHandle mesh)
	{
		change_batch_mesh(model_id, mesh);
	}
	void set_model_data(ModelHandle model_id, const Model& data)
	{
		instance_data.sub_data(sizeof(Model) * model_id, sizeof(Model), &data);
	}
public:
	size_t texture_count() const
	{
		return texture_num;
	}
public:
	void draw() const
	{
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, m_model_count, 0);
	}

	void setup_state()
	{
		VAO.bind();
		m_commands.bind(GL_DRAW_INDIRECT_BUFFER);
		const unsigned MATERIAL_SSBO_IDX = 0;
		materials.bind_base(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO_IDX);
		glActiveTexture(GL_TEXTURE0);
		textures.bind();
	}
};