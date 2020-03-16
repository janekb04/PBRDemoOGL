#pragma once

#include "../Vendor.h"
#include "Vertex.h"
#include "Index.h"

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<Index> indices;

	static Mesh from_file(const char* const path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			throw std::runtime_error(std::string("failed to load mesh: ") + importer.GetErrorString());
		}

		aiMesh* mesh = scene->mMeshes[0];

		Mesh result;
		result.vertices.reserve(mesh->mNumVertices);
		result.indices.reserve(mesh->mNumFaces / 3);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 temp; // we declare a placeholder temp since assimp uses its own temp class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            temp.x = mesh->mVertices[i].x;
            temp.y = mesh->mVertices[i].y;
            temp.z = mesh->mVertices[i].z;
            vertex.position = temp;
            // normals
            temp.x = mesh->mNormals[i].x;
            temp.y = mesh->mNormals[i].y;
            temp.z = mesh->mNormals[i].z;
            vertex.normal = temp;
            // texture coordinates
            if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.uv = vec;
            }
            else
                vertex.uv = glm::vec2(0.0f, 0.0f);
            // tangent
            temp.x = mesh->mTangents[i].x;
            temp.y = mesh->mTangents[i].y;
            temp.z = mesh->mTangents[i].z;
            vertex.tangent = temp;

            result.vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices temp
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                result.indices.push_back(face.mIndices[j]);
        }

        return result;
	}
};