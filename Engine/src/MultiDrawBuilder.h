#pragma once

#include "Vendor.h"
#include "Defines.h"

template <typename Vertex, typename Index>
class MultiDrawElementsBuilder
{
private:
	std::vector<Vertex> m_vertices;
	std::vector<Index> m_indices;
	std::vector<glDrawElementsIndirectCommand> m_commands;
	unsigned m_instances;
public:
	MultiDrawElementsBuilder() :
		m_instances(0)
	{
	}

	void add_mesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, unsigned instances)
	{
		glDrawElementsIndirectCommand cmd;
		cmd.count = indices.size();
		cmd.primCount = instances;
		cmd.firstIndex = m_indices.size();
		cmd.baseVertex = m_vertices.size();
		cmd.baseInstance = m_instances;

		m_commands.push_back(cmd);
		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
		m_indices.insert(m_indices.end(), indices.begin(), indices.end());
		m_instances += instances;
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