#include "data/Importer.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Debug.hpp"
#include <glm/gtx/string_cast.hpp>
#include <stb_image.h>
#include "Utils.hpp"

namespace mist {
	glm::vec3 AssimpVec3ToGLM(const aiVector3D& v) {
		return { v.x, v.y, v.z };
	}

	glm::mat4 AssimpMat4ToGLM(const aiMatrix4x4& m) {
	    return glm::transpose(glm::mat4(
	        m.a1, m.a2, m.a3, m.a4,
	        m.b1, m.b2, m.b3, m.b4,
	        m.c1, m.c2, m.c3, m.c4,
	        m.d1, m.d2, m.d3, m.d4
	    ));
	}

	std::shared_ptr<Mesh> ProcessMesh(aiMesh* meshToProcess, const glm::mat4& transform) {
	    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform)));
	    std::vector<Vertex> vertices(meshToProcess->mNumVertices);

	    for (size_t j = 0; j < meshToProcess->mNumVertices; ++j) {
	        glm::vec3 pos = AssimpVec3ToGLM(meshToProcess->mVertices[j]);
	        vertices[j].position = transform * glm::vec4(pos, 1.0f);

	        if (meshToProcess->HasNormals()) {
	            glm::vec3 normal = AssimpVec3ToGLM(meshToProcess->mNormals[j]);
	            vertices[j].normal = normalMatrix * normal;
	        }
	    }

	    std::vector<uint32_t> indices;
	    for (size_t j = 0; j < meshToProcess->mNumFaces; ++j) {
	        aiFace& face = meshToProcess->mFaces[j];

	        indices.push_back(face.mIndices[0]);
	        indices.push_back(face.mIndices[1]);
	        indices.push_back(face.mIndices[2]);
	    }

	    return std::make_shared<Mesh>(vertices, indices);
	}

	void ProcessNode(aiNode* node, const aiScene* scene, const glm::mat4& parentTransform, std::vector<std::shared_ptr<Mesh>>& meshes) {
	    glm::mat4 nodeTransform = parentTransform * AssimpMat4ToGLM(node->mTransformation);

	    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
	        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
	        meshes.push_back(ProcessMesh(mesh, nodeTransform));
	    }

	    for (uint32_t i = 0; i < node->mNumChildren; i++) {
	        ProcessNode(node->mChildren[i], scene, nodeTransform, meshes);
	    }
	}

	std::vector<std::shared_ptr<Mesh>> Importer::ImportMeshes(const std::string& path, bool flipWinding) {
		unsigned int flags = 
		aiProcess_MakeLeftHanded				|
		aiProcess_Triangulate                   |
		aiProcess_JoinIdenticalVertices         |
		aiProcess_ImproveCacheLocality          |
		aiProcess_RemoveRedundantMaterials      |
		aiProcess_SortByPType                   |
		aiProcess_FindDegenerates               |
		aiProcess_FindInvalidData				|
		aiProcess_OptimizeMeshes				|
		aiProcess_OptimizeGraph;
		
		if (flipWinding)
			flags |= aiProcess_FlipWindingOrder;
		
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path.c_str(), flags);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			MIST_WARN(std::string("Assimp import error: ") + importer.GetErrorString());
			return std::vector<std::shared_ptr<Mesh>>();
		}

		std::vector<std::shared_ptr<Mesh>> meshes;
		ProcessNode(scene->mRootNode, scene, glm::mat4(1.0f), meshes);
		return meshes;
	}

	ImageData Importer::ImportImage(const std::string& path) {
		ImageData data;
		std::string absPath = Utils::GetAbsolutePath(path);

		if (!Utils::Exists(absPath)) {
			MIST_WARN("File doesnt exist: " + absPath);
			return data;
		}

		data.pixels = stbi_loadf(absPath.c_str(), &data.width, &data.height, &data.channels, STBI_rgb_alpha);		
		if (!data.pixels)
			MIST_WARN("Failed to load image: " + absPath + " Reason: " + stbi_failure_reason());
		return data;
	}
}