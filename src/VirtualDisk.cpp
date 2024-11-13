#include "VirtualDisk.h"

VirtualDisk::VirtualDisk()
	: numBlocks(diskSize/blockSize)
{
	vdisk = new uint8_t[diskSize]();

	for (size_t i = 0; i < numBlocks; ++i) {
		block_versions[i].store(0);
	}
}

VirtualDisk::~VirtualDisk() 
{
	delete[] vdisk;
}

void VirtualDisk::readBlock(size_t blockIndex, uint8_t* buffer)
{
	if (blockIndex >= numBlocks) {
		throw std::out_of_range("Block index out of range");
	}

	std::memcpy(buffer, vdisk + blockIndex * blockSize, blockSize);
}

void VirtualDisk::writeBlock(size_t blockIndex, const  uint8_t* buffer)
{
	if (blockIndex >= numBlocks) {
		throw std::out_of_range("Block index out of range");
	}

	uint8_t* blockPtr = vdisk + blockIndex * blockSize;

	while (true) {
		// Capture the current version of the block
		uint64_t expected_version = block_versions[blockIndex].load( std::memory_order_acquire);

		// Perform the write
		std::memcpy(blockPtr, buffer, blockSize);

		// Attempt to increment the version if it hasn't been changed
		// by another writer. If this fails, another write happened.
		if (block_versions[blockIndex].compare_exchange_strong(expected_version, expected_version + 1,
			std::memory_order_acq_rel)) {
			break;
		}
	}
}