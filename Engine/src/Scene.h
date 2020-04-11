#pragma once

#include "Vendor.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "MultiDrawBuilder.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Material.h"
#include "UnorderedArraySet.h"
#include "MappedGPUArray.h"
#include "Utility.h"
#include "Texture2d.h"
#include "BufferArray.h"

class Scene
{
public:
	//Structures
	struct Model
	{
		glm::mat4 model_transform;
		int material_idx;
		glm::mat3 normal_mat;
	};
	struct PointLight
	{
		glm::vec4 pos;
		glm::vec4 color;
	};
	struct DirectionalLight
	{
		glm::vec4 direction;
		glm::vec4 color;
	};
	struct Spotlight
	{
		glm::vec4 pos;
		glm::vec4 direction;
		glm::vec4 color;
		float cos_difference; // cos(inner_angle) - cos(outer_angle)
		float cos_outer_angle;
	};
	//Per-vertex attributes binding indices
	static inline const VertexAttribute POS_IDX{ 0 };
	static inline const VertexAttribute NORMAL_IDX{ 1 };
	static inline const VertexAttribute UV_IDX{ 2 };
	static inline const VertexAttribute TANGENT_IDX{ 3 };

	//Per-instance attributes binding indices
	static inline const VertexAttribute MODEL_IDX{ 4 };
	static inline const VertexAttribute MATERIAL_IDX_IDX{ 8 };
	static inline const VertexAttribute NORMAL_MAT_IDX{ 9 };

	//SSBO binding indices
	static inline const unsigned MATERIAL_SSBO_IDX = 0;
	static inline const unsigned POINT_LIGHT_SSBO_IDX = 1;
	static inline const unsigned DIRECTIONAL_LIGHT_SSBO_IDX = 2;
	static inline const unsigned SPOTLIGHT_SSBO_IDX = 3;
private:
	//VBO binding points
	static inline const unsigned int VBO_IDX = 0;
	static inline const unsigned int INSTANCED_VBO_IDX = 1;
private:
	using MeshData = MeshBuilder<Vertex, GLuint>::MeshData;
private:
	std::vector<MeshData> m_meshes;
	VertexArray VAO;
	Buffer VBO;
	Buffer EBO;
	BufferArray<glDrawElementsIndirectCommand> m_commands;
	std::vector<Texture2d> textures;
public:
	BufferArray<Model> instance_data;
	BufferArray<Material> materials;
	BufferArray<PointLight> point_lights;
	BufferArray<DirectionalLight> directional_lights;
	BufferArray<Spotlight> spotlights;
public:
	using MeshHandle = unsigned;
public:
	Scene(const std::vector<Mesh>& meshes, size_t max_models, size_t max_materials, size_t max_point_lights, size_t max_directional_lights, size_t max_spotlighits) :
		m_commands(max_models),
		instance_data(max_models),
		materials(max_materials),
		point_lights(max_point_lights),
		directional_lights(max_directional_lights),
		spotlights(max_spotlighits)
	{
		//Load mesh data
		{
			MeshBuilder<Vertex, unsigned> builder;

			for (const auto& mesh : meshes)
				builder.add_mesh(mesh.vertices, mesh.indices);

			VBO.storage(sizeof(Vertex) * builder.vertices().size(), builder.vertices().data(), 0);
			EBO.storage(sizeof(GLuint) * builder.indices().size(), builder.indices().data(), 0);
			m_meshes = builder.mesh_data();
		}
		//Setup vertex attributes
		{
			VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(Vertex));
			
			VAO.enable_attrib(POS_IDX);
			VAO.attrib_binding(POS_IDX, VBO_IDX);
			VAO.attrib_format(POS_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
			
			VAO.enable_attrib(NORMAL_IDX);
			VAO.attrib_binding(NORMAL_IDX, VBO_IDX);
			VAO.attrib_format(NORMAL_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
			
			VAO.enable_attrib(UV_IDX);
			VAO.attrib_binding(UV_IDX, VBO_IDX);
			VAO.attrib_format(UV_IDX, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
			
			VAO.enable_attrib(TANGENT_IDX);
			VAO.attrib_binding(TANGENT_IDX, VBO_IDX);
			VAO.attrib_format(TANGENT_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
		}
		//Setup instance attributes
		{
			VAO.vertex_buffer(INSTANCED_VBO_IDX, instance_data.buffer(), 0, sizeof(Model));
			VAO.binding_divisor(INSTANCED_VBO_IDX, 1);
			
			for (int i = 0; i < 4; ++i)
			{
				VertexAttribute column{ MODEL_IDX.index() + i };
				VAO.enable_attrib(column);
				VAO.attrib_binding(column, INSTANCED_VBO_IDX);
				VAO.attrib_format(column, 4, GL_FLOAT, GL_FALSE, offsetof(Model, model_transform) + sizeof(glm::mat4::col_type) * i);
			}
			
			VAO.enable_attrib(MATERIAL_IDX_IDX);
			VAO.attrib_binding(MATERIAL_IDX_IDX, INSTANCED_VBO_IDX);
			VAO.attrib_format_i(MATERIAL_IDX_IDX, 1, GL_INT, offsetof(Model, material_idx));

			for (int i = 0; i < 3; ++i)
			{
				VertexAttribute column{ NORMAL_MAT_IDX.index() + i };
				VAO.enable_attrib(column);
				VAO.attrib_binding(column, INSTANCED_VBO_IDX);
				VAO.attrib_format(column, 3, GL_FLOAT, GL_FALSE, offsetof(Model, normal_mat) + sizeof(glm::mat3::col_type) * i);
			}
		}

		VAO.element_buffer(EBO);
	}
public:
	const TextureHandle add_texture(const Image2d& data)
	{
		textures.emplace_back(data, 1 + log2(std::min(data.width(), data.height())));
		TextureHandle handle = textures.back().get_texture_handle();
		handle.make_resident();
		return handle;
	}
	size_t texture_count() const
	{
		return textures.size();
	}

	BufferArray<Model>::handle add_model(MeshHandle mesh_id, const Model& model)
	{
		const MeshData& mesh = m_meshes[mesh_id];

		glDrawElementsIndirectCommand cmd;
		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;
		cmd.baseInstance = m_commands.size();
		cmd.primCount = 1;

		m_commands.add(cmd);
		instance_data.add(model);

		return instance_data.size();
	}
	void set_model_mesh(BufferArray<Model>::handle model_id, MeshHandle mesh_id)
	{
		const MeshData& mesh = m_meshes[mesh_id];

		glDrawElementsIndirectCommand cmd = m_commands.get(model_id);

		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;

		m_commands.set(model_id, cmd);
	}
public:
	void draw() const
	{
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, m_commands.size(), 0);
	}

	void setup_state()
	{
		VAO.bind();
		m_commands.buffer().bind(GL_DRAW_INDIRECT_BUFFER);
		materials.buffer().bind_base(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO_IDX);
		point_lights.buffer().bind_base(GL_SHADER_STORAGE_BUFFER,POINT_LIGHT_SSBO_IDX);
		directional_lights.buffer().bind_base(GL_SHADER_STORAGE_BUFFER, DIRECTIONAL_LIGHT_SSBO_IDX);
		spotlights.buffer().bind_base(GL_SHADER_STORAGE_BUFFER, SPOTLIGHT_SSBO_IDX);
	}
};