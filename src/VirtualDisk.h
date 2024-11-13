#include <bitset>
#include <atomic>
#include <cstdint>
#include <vector>
#include <cstring>
#include <stdexcept>

#define _NUM_BLOCKS 8096 // DONT USE OUTSIDE CLASS

class VirtualDisk {
public:
	uint8_t *vdisk = nullptr;
	uint32_t blockSize = 128; // 128B
	uint32_t diskSize = _NUM_BLOCKS*blockSize;
	uint32_t numBlocks = _NUM_BLOCKS;
	std::atomic<uint64_t> block_versions[_NUM_BLOCKS];

	VirtualDisk();

	~VirtualDisk();

	void readBlock(size_t blockIndex, uint8_t* buffer); // Reads one block and stores into the index

	void writeBlock(size_t blockIndex, const uint8_t* buffer); // Write one block adn store into buffer 
};