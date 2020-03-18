#pragma once

#include "Vendor.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "MultiDrawBuilder.h"

class DemoScene
{
	VertexArray VAO;
	Buffer VBO, EBO;
	Buffer draw_indirect;
	Buffer instanced_vbo;
	int command_count{ 0 };
public:
	DemoScene()
	{
		{
			MultiDrawElementsBuilder<glm::vec3, GLuint> builder;
			builder.add_mesh(
				{
					{ 0.5f,  0.5f,  0.0f}, // top right
					{ 0.5f, -0.5f,  0.0f}, // bottom right
					{-0.5f, -0.5f,  0.0f}, // bottom left
					{-0.5f,  0.5f,  0.0f}  // top left 
				},
			{
				0, 1, 3, // first Triangle
				1, 2, 3  // second Triangle
			},
				50
			);
			builder.add_mesh(
				{
					{-1, -1, -1},
					{1, -1, -1},
					{1, 1, -1},
					{-1, 1, -1},
					{-1, -1, 1},
					{1, -1, 1},
					{1, 1, 1},
					{-1, 1, 1}
				},
			{
				0, 1, 3, 3, 1, 2,
				1, 5, 2, 2, 5, 6,
				5, 4, 6, 6, 4, 7,
				4, 0, 7, 7, 0, 3,
				3, 2, 7, 7, 2, 6,
				4, 5, 0, 0, 5, 1
			},
				50
			);

			VBO.data(sizeof(glm::vec3) * builder.vertices().size(), builder.vertices().data(), GL_STATIC_DRAW);
			EBO.data(sizeof(GLuint) * builder.indices().size(), builder.indices().data(), GL_STATIC_DRAW);
			draw_indirect.data(sizeof(glDrawElementsIndirectCommand) * builder.commands().size(), builder.commands().data(), GL_STATIC_DRAW);
			command_count = builder.commands().size();

			std::vector<glm::mat4> transforms(builder.instances());
			unsigned side_length = ceil(sqrt(transforms.size()));
			for (int i = 0; i < transforms.size(); ++i)
			{
				transforms[i] = glm::translate(glm::mat4(1), glm::vec3(i / side_length, i % side_length, 0));
			}

			instanced_vbo.data(transforms.size() * sizeof(glm::mat4), glm::value_ptr(*transforms.data()), GL_STATIC_DRAW);
		}

		const unsigned int VBO_IDX = 0;
		VAO.vertex_buffer(VBO_IDX, VBO, 0, sizeof(glm::vec3));
		const VertexAttribute POS_IDX{ 0 };

		VAO.enable_attrib(POS_IDX);
		VAO.attrib_binding(POS_IDX, VBO_IDX);
		VAO.attrib_format(POS_IDX, 3, GL_FLOAT, GL_FALSE, 0);

		const unsigned int INSTANCED_VBO_IDX = 1;
		VAO.vertex_buffer(INSTANCED_VBO_IDX, instanced_vbo, 0, sizeof(glm::mat4));
		VAO.binding_divisor(INSTANCED_VBO_IDX, 1);
		const VertexAttribute MODEL_IDX{ 4 };
		for (int i = 0; i < 4; ++i)
		{
			VertexAttribute column{ MODEL_IDX.index() + i };
			VAO.enable_attrib(column);
			VAO.attrib_binding(column, INSTANCED_VBO_IDX);
			VAO.attrib_format(column, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4::col_type) * i);
		}

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