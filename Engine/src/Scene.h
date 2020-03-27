#pragma once

#include "Vendor.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "MultiDrawBuilder.h"
#include "Vertex.h"
#include "Texture2dArray.h"
#include "Mesh.h"
#include "Material.h"
#include "ArraySet.h"
#include "GPUArray.h"
#include "Utility.h"

class Scene
{
private:
	template <typename T>
	using gpu_array_set = array_set<T, GPUArray<T>>;
public:
	struct Model
	{
		glm::mat4 model_transform;
		int material_idx;
	};
private:
	struct MeshData
	{
		Index first_index;
		Index indices_count;
		Index first_vertex;
		Index vertices_count; // currently not needed, kept for future (for example mesh removal)
	};
private:
	std::vector<MeshData> m_meshes;
	GPUArray<Vertex> m_vertices;
	GPUArray<Index> m_indices;
	gpu_array_set<glDrawElementsIndirectCommand> m_commands;
	unsigned m_instances;

	VertexArray VAO;
	Texture2dArray textures;
	size_t texture_num;

	gpu_array_set<Model> instance_data;
	GPUArray<Material> materials;
public:
	using MaterialHandle = typename GPUArray<Material>::iterator;
	using ModelHandle = std::pair<typename gpu_array_set<Model>::iterator, gpu_array_set<glDrawElementsIndirectCommand>::iterator>;
	using MeshHandle = unsigned;
	using TextureHandle = unsigned;
	using BatchHandle = typename decltype(m_commands)::iterator;
private:
	MeshHandle add_mesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices)
	{
		MeshData mesh;
		mesh.first_index = m_indices.size();
		mesh.indices_count = indices.size();
		mesh.first_vertex = m_vertices.size();
		mesh.first_vertex = m_vertices.size();

		for (const auto& v : vertices)
		{
			m_vertices.push_back(v);
		}
		for (const auto& i : indices)
		{
			m_indices.push_back(i);
		}
		m_meshes.push_back(mesh);

		return m_meshes.size() - 1;
	}

	BatchHandle add_batch(MeshHandle mesh_id, unsigned instances)
	{
		const MeshData& mesh = m_meshes[mesh_id];

		glDrawElementsIndirectCommand cmd;
		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;
		cmd.baseInstance = m_instances;
		cmd.primCount = instances;

		m_instances += instances;
		m_commands.push_back(cmd);

		return std::prev(m_commands.end());
	}

	void change_batch_mesh(BatchHandle batch_id, MeshHandle mesh_id)
	{
		const MeshData& mesh = m_meshes[mesh_id];

		glDrawElementsIndirectCommand& cmd = *batch_id;
		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;
	}
private:
	static size_t get_total_vertices(const std::vector<Mesh>& meshes)
	{
		size_t result = 0;
		for (const auto& mesh : meshes)
		{
			result += mesh.vertices.size();
		}
		return result;
	}
	static size_t get_total_indices(const std::vector<Mesh>& meshes)
	{
		size_t result = 0;
		for (const auto& mesh : meshes)
		{
			result += mesh.indices.size();
		}
		return result;
	}
	template <typename T>
	static GPUArray<T> create_preallocated_gpu_array(size_t max_batches)
	{
		return GPUArray<T>(max_batches);
	}
public:
	Scene(const std::vector<const char*>& texture_paths, const std::vector<Mesh>& meshes, size_t max_batches, size_t max_models, size_t max_materials) :
		textures(Texture2dArray::from_files(texture_paths.data(), texture_paths.size())),
		texture_num(texture_paths.size()),
		m_vertices(get_total_vertices(meshes)),
		m_indices(get_total_indices(meshes)),
		m_commands(create_preallocated_gpu_array<glDrawElementsIndirectCommand>(max_batches)),
		instance_data(create_preallocated_gpu_array<Model>(max_models)),
		materials(create_preallocated_gpu_array<Material>(max_materials))
	{
		for (const auto& mesh : meshes)
			add_mesh(mesh.vertices, mesh.indices);

		Buffer& VBO = m_vertices.get_buffer();
		Buffer& EBO = m_indices.get_buffer();
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
		auto data_it = instance_data.insert({ glm::mat4(1.0f),  (int)material_idx});
		add_batch(mesh_id, 1);
		auto command_it = std::prev(m_commands.end());

		return { data_it, command_it };
	}
	Model& get_model(ModelHandle model_id)
	{
		return *model_id.first;
	}
	void set_model_mesh(ModelHandle model_id, MeshHandle mesh)
	{
		change_batch_mesh(model_id.second, mesh);
	}
	void remove_model(ModelHandle model_id)
	{
		instance_data.erase(model_id.first);
		const_cast<gpu_array_set<glDrawElementsIndirectCommand>&>(m_commands).erase(model_id.second);
	}
public:
	size_t texture_count() const
	{
		return texture_num;
	}
public:
	void draw() const
	{
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, m_commands.size(), 0);
	}

	void setup_state()
	{
		VAO.bind();
		get_container(m_commands).get_buffer().bind(GL_DRAW_INDIRECT_BUFFER);
		const unsigned MATERIAL_SSBO_IDX = 0;
		materials.get_buffer().bind_base(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO_IDX);

		glActiveTexture(GL_TEXTURE0);
		textures.bind();
	}
};