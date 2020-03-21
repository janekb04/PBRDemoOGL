//#pragma once
//
//#include "Vendor.h"
//#include "Buffer.h"
//#include "VertexArray.h"
//#include "MultiDrawBuilder.h"
//#include "Vertex.h"
//#include "Texture2dArray.h"
//#include "Mesh.h"
//
//class Scene
//{
//private:
//	VertexArray VAO;
//	Buffer VBO, EBO;
//	Buffer draw_indirect;
//	Buffer instanced_vbo;
//	Buffer material_ssbo;
//	Texture2dArray textures;
//	int texture_count;
//	int command_count;
//
//public:
//	struct obj_data
//	{
//		glm::mat4 model;
//		int material_idx;
//	};
//	struct material
//	{
//		int base_idx;
//	};
//public:
//	Scene(const std::vector<const char*>& texture_paths, const std::vector<Mesh>& meshes) :
//		textures(Texture2dArray::from_files(texture_paths.data(), texture_paths.size())),
//		texture_count(texture_paths.size()),
//		command_count(0)
//	{
//		const int mat_count = texture_paths.size();
//		{
//			std::vector<material> mats(mat_count);
//			for (int i = 0; i < mats.size(); ++i)
//			{
//				mats[i].base_idx = i;
//			}
//
//			material_ssbo.data(sizeof(material) * mats.size(), mats.data(), GL_STATIC_DRAW);
//			const unsigned MATERIAL_SSBO_IDX = 0;
//			material_ssbo.bind_base(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO_IDX);
//		}
//		{
//			MultiDrawElementsBuilder<Vertex, GLuint> builder;
//			for (const Mesh& mesh : meshes)
//				builder.add_mesh(mesh.vertices, mesh.indices);
//
//			VBO.data(sizeof(Vertex) * builder.vertices().size(), builder.vertices().data(), GL_STATIC_DRAW);
//			EBO.data(sizeof(GLuint) * builder.indices().size(), builder.indices().data(), GL_STATIC_DRAW);
//
//#error ... add instances via builder.add_instances() ...
//
//			draw_indirect.data(sizeof(glDrawElementsIndirectCommand) * builder.commands().size(), builder.commands().data(), GL_STATIC_DRAW);
//			command_count = builder.commands().size();
//
//			instanced_vbo.data(transforms.size() * sizeof(obj_data), transforms.data(), GL_STATIC_DRAW);
//		}
//
//		const unsigned int VBO_IDX = 0;
//		VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(Vertex));
//
//		const VertexAttribute POS_IDX{ 0 };
//		VAO.enable_attrib(POS_IDX);
//		VAO.attrib_binding(POS_IDX, VBO_IDX);
//		VAO.attrib_format(POS_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
//
//		const VertexAttribute NORMAL_IDX{ 1 };
//		VAO.enable_attrib(NORMAL_IDX);
//		VAO.attrib_binding(NORMAL_IDX, VBO_IDX);
//		VAO.attrib_format(NORMAL_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
//
//		const VertexAttribute UV_IDX{ 2 };
//		VAO.enable_attrib(UV_IDX);
//		VAO.attrib_binding(UV_IDX, VBO_IDX);
//		VAO.attrib_format(UV_IDX, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
//
//		const VertexAttribute TANGENT_IDX{ 3 };
//		VAO.enable_attrib(TANGENT_IDX);
//		VAO.attrib_binding(TANGENT_IDX, VBO_IDX);
//		VAO.attrib_format(TANGENT_IDX, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
//
//		const unsigned int INSTANCED_VBO_IDX = 1;
//		VAO.vertex_buffer(INSTANCED_VBO_IDX, instanced_vbo, 0, sizeof(obj_data));
//		VAO.binding_divisor(INSTANCED_VBO_IDX, 1);
//		const VertexAttribute MODEL_IDX{ 4 };
//		for (int i = 0; i < 4; ++i)
//		{
//			VertexAttribute column{ MODEL_IDX.index() + i };
//			VAO.enable_attrib(column);
//			VAO.attrib_binding(column, INSTANCED_VBO_IDX);
//			VAO.attrib_format(column, 4, GL_FLOAT, GL_FALSE, offsetof(obj_data, model) + sizeof(glm::mat4::col_type) * i);
//		}
//		const VertexAttribute MATERIAL_IDX_IDX{ 8 };
//		VAO.enable_attrib(MATERIAL_IDX_IDX);
//		VAO.attrib_binding(MATERIAL_IDX_IDX, INSTANCED_VBO_IDX);
//		VAO.attrib_format_i(MATERIAL_IDX_IDX, 1, GL_INT, offsetof(obj_data, material_idx));
//
//		VAO.element_buffer(EBO);
//	}
//
//	void draw() const
//	{
//		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, command_count, 0);
//	}
//
//	void setup_state()
//	{
//		VAO.bind();
//		draw_indirect.bind(GL_DRAW_INDIRECT_BUFFER);
//
//		glActiveTexture(GL_TEXTURE0);
//		textures.bind();
//	}
//};