#pragma once
#include "types/Octree.hpp"

#define CHUNK_SIZE 32
#define CHUNK_VOLUME CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

namespace v {
    struct Chunk {
    public:
		Chunk(const uint32_t octreeSize) : edits(octreeSize) {}

        glm::ivec3 coord;
        std::array<VoxelID, CHUNK_VOLUME> voxels;
		Octree edits;
	};
}