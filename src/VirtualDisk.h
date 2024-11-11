#include <vector>


class VirtualDisk {
public:
	std::vector<size_t> vdisk; // Disk blocks are 64B/32B
	size_t totalBlocks; // Number of blocks
	size_t diskSize; // In Bytes
	size_t blockSize;

	VirtualDisk()
	{
		diskSize = 50 * 1024 * 1024; // 50MB
		blockSize = sizeof(size_t); // Block size in bytes
		totalBlocks = diskSize / blockSize;
		vdisk.resize(totalBlocks, 0); // Initialize the disk with zeroes
	}

	void readBlock(size_t blockIndex, std::vector<size_t>& buffer)
	{
		if (buffer.size() > 0) {
			buffer[0] = vdisk[blockIndex];
		}
		else
		{
			buffer.push_back(vdisk[blockIndex]);
		}
	}

	void writeBlock(size_t blockIndex, const std::vector<size_t>& buffer)
	{
		if (buffer.size() > 0) {
			vdisk[blockIndex] = buffer.at(0);
		}
	}
};