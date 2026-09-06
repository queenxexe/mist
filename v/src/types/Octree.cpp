#include "types/Octree.hpp"
#include <bit>

#define INITIAL_EDIT_SIZE 64u
#define MASK_SHIFT 24
#define POINTER_MASK 0x00FFFFFF

namespace v {
	// NODES

	Node::Node() {
		SetValue(VoxelID::NONE);
	}

	Node::Node(const VoxelID value) {
		SetValue(value);
	}

	uint32_t Node::GetFirstChild() {
		return (uint32_t)(data & POINTER_MASK);
	}

	uint8_t Node::GetChildMask() {
		return (uint32_t)(data >> MASK_SHIFT);
	}

	bool Node::IsLeaf() {
		return GetChildMask() == 0;
	}

	bool Node::IsEqual(const VoxelID value) {
		if (!IsLeaf())
			return false;
		return GetValue() == value;
	}

	void Node::SetBranch(const uint8_t mask, const uint32_t firstChild) {
		data = ((uint32_t)mask << MASK_SHIFT) | (firstChild & POINTER_MASK);
	}

	VoxelID Node::GetValue() {
		return (VoxelID)data;
	}

	void Node::SetValue(const VoxelID value) {
		data = (uint32_t)value;
	}

	// OCTREE

	Octree::Octree(const uint32_t size) : size(size), nodeCount(1), nodes({ Node(VoxelID::NONE) }) {}

	uint32_t Octree::GetNodeCount() {
		return nodeCount;
	}

	uint32_t Octree::GetMaxDepth() {
		return std::countr_zero(size);
	}

	bool Octree::Insert(const glm::ivec3 voxel, const VoxelID value) {
		if (mist::Math::AnyLess(voxel, 0) || mist::Math::AnyGreaterOrEqual(voxel, size))
			return false;

		uint32_t nodeIndex = 0;
		for (uint32_t bit = GetMaxDepth() - 1; bit >= 0; bit--) {
			Node& node = nodes[nodeIndex];

			if (node.IsLeaf()) {
				uint32_t firstChild = AllocateChildren();
				VoxelID existing = node.GetValue();
				for (uint32_t i = 0; i < 8; i++)
					nodes[firstChild + i] = Node(existing);

				node.SetBranch(0xFF, firstChild);
			}
		}

		nodes[nodeIndex] = Node(value);
		return true;
	}

	bool Octree::InsertBox(const glm::ivec3 min, const glm::ivec3 max, const VoxelID value) {
		glm::ivec3 trueMin = glm::max(min, glm::ivec3(0));
		glm::ivec3 trueMax = glm::min(max, glm::ivec3(size));
		return InsertBoxRecursive(0, glm::ivec3(0), size, trueMin, trueMax, value);
	}
	
	bool Octree::InsertBoxRecursive(const uint32_t nodeIndex, const glm::ivec3 nodeMin, const uint32_t nodeSize, const glm::ivec3 min, const glm::ivec3 max, const VoxelID value) {
		glm::ivec3 nodeMax = nodeMin + glm::ivec3(nodeSize);
		if (!BoxIntersectsBox(min, max, nodeMin, nodeMax))
			return false;

		Node& node = nodes[nodeIndex];
		if (node.IsLeaf()) {
			if (node.IsEqual(value))
				return false;

			if (nodeSize == 1 || BoxContainsBox(min, max, nodeMin, nodeMax)) {
				node = Node(value);
				return true;
			}

			VoxelID old = node.GetValue();
			uint32_t firstChild = AllocateChildren();
			for (uint32_t i = 0; i < 8; i++)
				nodes[firstChild + i] = Node(old);

			node.SetBranch(0xFF, firstChild);
		}

		bool changed = false;
		uint32_t childSize = nodeSize >> 1;
		for (uint32_t i = 0; i < 8; i++) {
			glm::ivec3 childMin = nodeMin + glm::ivec3(childSize) * glm::ivec3(
				(i >> 0) & 1,
				(i >> 1) & 1,
				(i >> 2) & 1
			);

			changed |= InsertBoxRecursive(node.GetFirstChild() + i, childMin, childSize, min, max, value);
		}

		if (changed && CanCollapse(nodeIndex))
			CompressNode(nodeIndex);

		return changed;
	}

	bool Octree::InsertSphere(const glm::ivec3 centerVoxel, const uint32_t radius, const VoxelID value) {
		uint32_t radiusSq = radius * radius;
		return InsertSphereRecursive(0, centerVoxel, glm::ivec3(0), size, radiusSq, value);
	}

	bool Octree::InsertSphereRecursive(const uint32_t nodeIndex, const glm::ivec3 centerVoxel, const glm::ivec3 nodeMin, const uint32_t nodeSize, const uint32_t radiusSquared, const VoxelID value) {
		glm::ivec3 nodeMax = nodeMin + glm::ivec3(nodeSize);
		if (!SphereIntersectsBox(centerVoxel, radiusSquared, nodeMin, nodeMax))
			return false;

		Node& node = nodes[nodeIndex];
		if (node.IsLeaf()) {
			if (node.IsEqual(value))
				return false;

			if (SphereContainsBox(centerVoxel, radiusSquared, nodeMin, nodeMax)) {
				node = Node(value);
				return true;
			}

			if (nodeSize == 1) {
				if (mist::Math::DistanceSq(nodeMin, centerVoxel) <= radiusSquared) {
					node = Node(value);
					return true;
				}
				return false;
			}

			VoxelID old = node.GetValue();
			uint32_t firstChild = AllocateChildren();
			for (uint32_t i = 0; i < 8; i++)
				nodes[firstChild + i] = Node(old);

			node.SetBranch(0xFF, firstChild);
		}

		bool changed = false;
		uint32_t childSize = nodeSize >> 1;
		for (uint32_t i = 0; i < 8; i++) {
			glm::ivec3 childMin = nodeMin + glm::ivec3(childSize) * glm::ivec3(
				(i >> 0) & 1,
				(i >> 1) & 1,
				(i >> 2) & 1
			);

			changed |= InsertSphereRecursive(node.GetFirstChild() + i, centerVoxel, childMin, childSize, radiusSquared, value);
		}

		if (changed && CanCollapse(nodeIndex))
			CompressNode(nodeIndex);

		return changed;
	}

	VoxelID Octree::Get(const glm::ivec3 voxel) {
		if (mist::Math::AnyLess(voxel, 0) || mist::Math::AnyGreaterOrEqual(voxel, size))
			return VoxelID::NONE;

		uint32_t nodeIndex = 0;
		for (uint32_t bit = GetMaxDepth() - 1; bit >= 0; bit--) {
			Node& node = nodes[nodeIndex];

			if (node.IsLeaf())
				return node.GetValue();

			int child = GetChildIndex(voxel, bit);
			nodeIndex = node.GetFirstChild() + child;
		}

		return nodes[nodeIndex].GetValue();
	}

	bool Octree::SphereIntersectsBox(const glm::ivec3 center, const uint32_t radiusSq, const glm::ivec3 boxMin, const glm::ivec3 boxMax) {
		glm::ivec3 closest = glm::clamp(center, boxMin, boxMax);
		return mist::Math::DistanceSq(closest, center) <= radiusSq;
	}

	bool Octree::SphereContainsBox(const glm::ivec3 center, const uint32_t radiusSq, const glm::ivec3 boxMin, const glm::ivec3 boxMax) {
		for (uint32_t x = 0; x <= 1; x++) {
			for (uint32_t y = 0; y <= 1; y++) {
				for (uint32_t z = 0; z <= 1; z++) {
					glm::ivec3 corner = glm::ivec3(
						x == 0 ? boxMin.x : boxMax.x,
						y == 0 ? boxMin.y : boxMax.y,
						z == 0 ? boxMin.z : boxMax.z
					);
					if (mist::Math::DistanceSq(corner, center) > radiusSq)
						return false;
				}
			}
		}
		return true;
	}
	
	bool Octree::BoxIntersectsBox(const glm::ivec3 aMin, const glm::ivec3 aMax, const glm::ivec3 bMin, const glm::ivec3 bMax) {
		return mist::Math::AllLessOrEqual(aMin, bMax) && mist::Math::AllGreaterOrEqual(aMax, bMin); 
	}
	
	bool Octree::BoxContainsBox(const glm::ivec3 aMin, const glm::ivec3 aMax, const glm::ivec3 bMin, const glm::ivec3 bMax) {
		return mist::Math::AllLessOrEqual(aMin, bMin) && mist::Math::AllGreaterOrEqual(aMax, bMax);
	}

	bool Octree::CanCollapse(const uint32_t nodeIndex) {
		Node& node = nodes[nodeIndex];
		if (node.IsLeaf())
			return true;

		VoxelID firstValue = nodes[node.GetFirstChild()].GetValue();
		for (uint32_t i = 1; i < 8; i++) {
			Node& child = nodes[node.GetFirstChild() + i];
			if (!child.IsLeaf() || !child.IsEqual(firstValue))
				return false;
		}
		return true;
	}
	
	void Octree::Compress() {
		CompressNode(0);
	}
	
	bool Octree::CompressNode(const uint32_t nodeIndex) {
		Node& node = nodes[nodeIndex];

		if (node.IsLeaf())
			return true;

		bool canCollapse = true;
		VoxelID firstValue = VoxelID::NONE;
		for (int i = 0; i < 8; i++) {
			int childIndex = node.GetFirstChild() + i;

			if (!CompressNode(childIndex))
				canCollapse = false;

			Node& child = nodes[childIndex];

			if (!child.IsLeaf())
				canCollapse = false;

			if (i == 0) {
				firstValue = child.GetValue();
			} else if (!child.IsEqual(firstValue)) {
				canCollapse = false;
			}
		}

		if (canCollapse) {
			nodes[nodeIndex] = Node(firstValue);
			return true;
		}

		return false;
	}

	void Octree::Expand() {
		uint32_t newSize = glm::max(INITIAL_EDIT_SIZE * 2, glm::min((uint32_t)nodes.size() * 2, GetMaxNodeCount(GetMaxDepth())));
		nodes.resize(newSize);
	}

	uint32_t Octree::AllocateChildren() {
		if (nodeCount + 8 > nodes.size())
			Expand();

		uint32_t firstChild = nodeCount;
		nodeCount += 8;
		return firstChild;
	}

	uint32_t Octree::GetChildIndex(const glm::ivec3 voxel, const uint32_t bit) {
		return 
			(((voxel.x >> bit) & 1) << 0) |
			(((voxel.y >> bit) & 1) << 1) |
			(((voxel.z >> bit) & 1) << 2);
	}

	uint32_t Octree::GetMaxNodeCount(const uint32_t depth) {
		uint32_t total = 0;
		uint32_t levelNodes = 1;

		for (uint32_t i = 0; i <= depth; i++) {
			total += levelNodes;
			levelNodes *= 8;
		}
		return total;
	}
}