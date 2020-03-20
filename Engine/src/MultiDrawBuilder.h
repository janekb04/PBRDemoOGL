#pragma once

#include "Vendor.h"
#include "Defines.h"

template <typename Vertex, typename Index>
class MultiDrawElementsBuilder
{
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
	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;
	std::vector<glDrawElementsIndirectCommand> m_commands;
	unsigned m_instances;
public:
	MultiDrawElementsBuilder() :
		m_instances(0)
	{
	}

	unsigned add_mesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices)
	{
		MeshData mesh;
		mesh.first_index = m_indices.size();
		mesh.indices_count = indices.size();
		mesh.first_vertex = m_vertices.size();
		mesh.first_vertex = m_vertices.size();

		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
		m_indices.insert(m_indices.end(), indices.begin(), indices.end());
		m_meshes.push_back(mesh);

		return m_meshes.size() - 1;
	}

	unsigned add_instance(unsigned mesh_idx, unsigned instances)
	{
		const MeshData& mesh = m_meshes[mesh_idx];
		
		glDrawElementsIndirectCommand cmd;
		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;
		cmd.baseInstance = m_instances;
		cmd.primCount = instances;

		m_instances += instances;
		m_commands.push_back(cmd);
		
		return m_commands.size() - 1;
	}

	const std::vector<Vertex>& vertices() const
	{
		return m_vertices;
	}

	const std::vector<Index>& indices() const
	{
		return m_indices;
	}

	const std::vector<glDrawElementsIndirectCommand>& commands() const
	{
		return m_commands;
	}

	unsigned instances() const
	{
		return m_instances;
	}
};