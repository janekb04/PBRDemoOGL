#pragma once

#include "Vendor.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "MultiDrawBuilder.h"
#include "Vertex.h"

class DemoScene
{
	VertexArray VAO;
	Buffer VBO, EBO;
	Buffer draw_indirect;
	Buffer instanced_vbo;
	Buffer material_ssbo;
	int command_count{ 0 };

	struct obj_data
	{
		glm::mat4 model;
		int material_idx;
	};
	struct material
	{
		//int base_idx;
		glm::vec4 color;
	};
public:
	DemoScene()
	{
		const int mat_count = 7;
		{
			std::vector<material> mats(mat_count);
			for (int i = 0; i < mats.size(); ++i)
			{
				//mats[i].base_idx = 1;
				mats[i].color = glm::vec4(rand01(), rand01(), rand01(), 1);
			}
			material_ssbo.data(sizeof(material) * mats.size(), mats.data(), GL_STATIC_DRAW);
			const unsigned MATERIAL_SSBO_IDX = 0;
			material_ssbo.bind_base(GL_SHADER_STORAGE_BUFFER, MATERIAL_SSBO_IDX);
		}
		{
			MultiDrawElementsBuilder<Vertex, GLuint> builder;
			builder.add_mesh(
				{
					{
						{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}},
						{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 1.0f}, {0,0,0}},
						{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, { 1.0f, 0.0f}, {0,0,0}},
						{{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 0.0f}, {0,0,0}},
						{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, { 0.0f, 1.0f}, {0,0,0}}
					}
				},
				{
					2,
					1,
					0,
					5,
					4,
					3,
					6,
					7,
					8,
					9,
					10,
					11,
					12,
					13,
					14,
					15,
					16,
					17,
					20,
					19,
					18,
					23,
					22,
					21,
					24,
					25,
					26,
					27,
					28,
					29,
					32,
					31,
					30,
					35,
					34,
					33
				},
				100
			);

			VBO.data(sizeof(Vertex) * builder.vertices().size(), builder.vertices().data(), GL_STATIC_DRAW);
			EBO.data(sizeof(GLuint) * builder.indices().size(), builder.indices().data(), GL_STATIC_DRAW);
			draw_indirect.data(sizeof(glDrawElementsIndirectCommand) * builder.commands().size(), builder.commands().data(), GL_STATIC_DRAW);
			command_count = builder.commands().size();

			std::vector<obj_data> transforms(builder.instances());
			unsigned side_length = ceil(sqrt(transforms.size()));
			for (int i = 0; i < transforms.size(); ++i)
			{
				transforms[i].model = glm::translate(glm::mat4(1), glm::vec3(i / side_length * 2, i % side_length * 2, 0));
				transforms[i].material_idx = i % mat_count;
			}

			instanced_vbo.data(transforms.size() * sizeof(obj_data), transforms.data(), GL_STATIC_DRAW);
		}

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
		VAO.vertex_buffer(INSTANCED_VBO_IDX, instanced_vbo, 0, sizeof(obj_data));
		VAO.binding_divisor(INSTANCED_VBO_IDX, 1);
		const VertexAttribute MODEL_IDX{ 4 };
		for (int i = 0; i < 4; ++i)
		{
			VertexAttribute column{ MODEL_IDX.index() + i };
			VAO.enable_attrib(column);
			VAO.attrib_binding(column, INSTANCED_VBO_IDX);
			VAO.attrib_format(column, 4, GL_FLOAT, GL_FALSE, offsetof(obj_data, model) + sizeof(glm::mat4::col_type) * i);
		}
		const VertexAttribute MATERIAL_IDX_IDX{ 8 };
		VAO.enable_attrib(MATERIAL_IDX_IDX);
		VAO.attrib_binding(MATERIAL_IDX_IDX, INSTANCED_VBO_IDX);
		VAO.attrib_format_i(MATERIAL_IDX_IDX, 1, GL_INT, offsetof(obj_data, material_idx));

		VAO.element_buffer(EBO);
	}

	void draw() const
	{
		VAO.bind();
		draw_indirect.bind(GL_DRAW_INDIRECT_BUFFER);
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, 0, command_count, 0);
	}

	void new_frame(double delta_time)
	{

	}
};