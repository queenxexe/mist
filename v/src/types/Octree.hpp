#pragma once
#include <cstdint>
#include "voxel/VoxelData.hpp"

// This attempts to place a value at the maximum depth
// unlike a more typical octree that will place in first free slot

namespace v {
	struct Node {
	public:
		Node();
        Node(const VoxelID value);

		uint32_t GetFirstChild();
		uint8_t GetChildMask();
		bool IsLeaf();
		bool IsEqual(const VoxelID value);
		void SetBranch(const uint8_t mask, const uint32_t firstChild);
		VoxelID GetValue();
		void SetValue(const VoxelID value);
	private:
		uint32_t data;
	};

	struct Octree {
	public:
		Octree(const uint32_t size);

		bool Insert(const glm::ivec3 voxel, const VoxelID value);
		bool InsertBox(const glm::ivec3 min, const glm::ivec3 max, const VoxelID value);
		bool InsertSphere(const glm::ivec3 centerVoxel, const uint32_t radius, const VoxelID value);

		VoxelID Get(const glm::ivec3 voxel);

		uint32_t GetNodeCount();
		uint32_t GetMaxDepth();

		std::vector<Node> nodes;
	private:
		bool InsertBoxRecursive(const uint32_t nodeIndex, const glm::ivec3 nodeMin, const uint32_t nodeSize, const glm::ivec3 min, const glm::ivec3 max, const VoxelID value);
		bool InsertSphereRecursive(const uint32_t nodeIndex, const glm::ivec3 centerVoxel, const glm::ivec3 nodeMin, const uint32_t nodeSize, const uint32_t radiusSquared, const VoxelID value);

		bool SphereIntersectsBox(const glm::ivec3 center, const uint32_t radiusSq, const glm::ivec3 boxMin, const glm::ivec3 boxMax);
		bool SphereContainsBox(const glm::ivec3 center, const uint32_t radiusSq, const glm::ivec3 boxMin, const glm::ivec3 boxMax);
		bool BoxIntersectsBox(const glm::ivec3 aMin, const glm::ivec3 aMax, const glm::ivec3 bMin, const glm::ivec3 bMax);
		bool BoxContainsBox(const glm::ivec3 aMin, const glm::ivec3 aMax, const glm::ivec3 bMin, const glm::ivec3 bMax);

		bool CanCollapse(const uint32_t nodeIndex);
		void Compress();
		bool CompressNode(const uint32_t nodeIndex);

		void Expand();
		uint32_t AllocateChildren();
		
		uint32_t GetChildIndex(const glm::ivec3 voxel, const uint32_t bit);
		uint32_t GetMaxNodeCount(const uint32_t depth);

		uint32_t nodeCount;
		uint32_t size;
	};
}