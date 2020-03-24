#pragma once

#include "Vendor.h"
#include "Defines.h"

template <
	typename Vertex,
	typename Index,
	template<typename> typename VerticesContainer = std::vector,
	template<typename> typename IndicesContainer = std::vector,
	template<typename> typename CommandsContainer = std::vector
>
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
	VerticesContainer<Vertex> m_vertices;
	IndicesContainer<Index> m_indices;
	CommandsContainer<glDrawElementsIndirectCommand> m_commands;
	unsigned m_instances;
public:
	using MeshID = unsigned;
	using BatchID = typename decltype(m_commands)::iterator;
public:
	MultiDrawElementsBuilder() :
		m_instances(0)
	{
	}

	MeshID add_mesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices)
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

	BatchID add_batch(MeshID mesh_id, unsigned instances)
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

	void change_batch_mesh(BatchID batch_id, MeshID mesh_id)
	{
		const MeshData& mesh = m_meshes[mesh_id];

		glDrawElementsIndirectCommand& cmd = *batch_id;
		cmd.firstIndex = mesh.first_index;
		cmd.count = mesh.indices_count;
		cmd.baseVertex = mesh.first_vertex;
	}

	const VerticesContainer<Vertex>& vertices() const
	{
		return m_vertices;
	}

	const IndicesContainer<Index>& indices() const
	{
		return m_indices;
	}

	const CommandsContainer<glDrawElementsIndirectCommand>& commands() const
	{
		return m_commands;
	}

	unsigned total_instances() const
	{
		return m_instances;
	}
};