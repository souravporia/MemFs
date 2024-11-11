#pragma once
#include "VirtualDisk.h"
#include "Schema.h"
#include <bitset>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>

#define MAX_NUM_FILES 16000


class FileSystem {
private:
	std::unordered_map<std::string, Inode> fileTable;
	std::bitset<MAX_NUM_FILES> bitmap;
	std::mutex mtx;
	size_t totalBlocks; // Number of blocks
	size_t diskSize; // In Bytes
	size_t blockSize;
	VirtualDisk &vdisk;

public:

	FileSystem(VirtualDisk &vdisk)
		: vdisk(vdisk), blockSize(vdisk.blockSize), totalBlocks(vdisk.totalBlocks), diskSize(vdisk.diskSize) {}

	void createFile(const std::string& fileName) {
	
		if (fileTable.find(fileName) != fileTable.end()) {
			std::cerr << "Error: " << fileName << " already exists\n";
			return;
		}
		std::lock_guard<std::mutex> lock(mtx);
		fileTable[fileName] = Inode(fileName);
		std::cout << "File " << fileName << " created successfully\n";
	}

	bool writeFile(const std::string& fileName, const std::vector<char>& data) {
		std::lock_guard<std::mutex> lock(mtx);
		auto it = fileTable.find(fileName);
		if (it == fileTable.end()) {
			std::cout << "File not found!" << std::endl;
			return false;
		}

		Inode& inode = it->second;
		size_t dataSize = data.size(); // In Bytes
		size_t numBlocksNeeded = (dataSize + blockSize - 1) / blockSize;

		// Early return if not enough free blocks
		if (bitmap.size() - bitmap.count() < numBlocksNeeded) {
			std::cout << "Not enough free blocks to store the file content!" << std::endl;
			return false;
		}

		size_t dataIndex = 0;
		size_t remainingDataSize = dataSize;
		const size_t size_tWidth = sizeof(size_t); // Number of bytes in a size_t
		for (size_t i = 0; i < totalBlocks && remainingDataSize > 0; ++i) {
			if (!bitmap[i]) {
				bitmap[i] = 1;

				size_t chunkSize = std::min(blockSize, remainingDataSize);
				std::vector<size_t> blockData(blockSize / size_tWidth, 0); // Prepare a block with `size_t` elements

				size_t byteIndex = 0;
				for (size_t j = 0; j < blockData.size() && byteIndex < chunkSize; ++j) {
					size_t value = 0;
					for (size_t k = 0; k < size_tWidth && byteIndex < chunkSize; ++k, ++dataIndex, ++byteIndex) {
						value |= static_cast<size_t>(static_cast<unsigned char>(data[dataIndex])) << (k * 8);
					}
					blockData[j] = value;
				}

				vdisk.writeBlock(i, blockData);
				inode.dataPtr.push_back(i);

				remainingDataSize -= chunkSize;
			}
		}

		inode.size = data.size();
		inode.updateModifiedTime();
		std::cout << "Successfully written to " << fileName << "\n";
		return true;
	}

	bool deleteFile(const std::string& fileName) {
		std::lock_guard<std::mutex> lock(mtx);
		if (fileTable.erase(fileName)) {
			std::cout << "File " << fileName << " deleted successfully\n";
			return true;
		}
		else {
			std::cout << "Error: " << fileName << " does not exist\n";
		}
		return false;
	}

	void read(const std::string& fileName, std::vector<char>& data) {
		std::lock_guard<std::mutex> lock(mtx);
		auto it = fileTable.find(fileName);

		if (it == fileTable.end()) {
			std::cout << "Error: " << fileName << " does not exist\n";
			return;
		}

		const Inode& inode = it->second;
		const size_t size_tWidth = sizeof(size_t); // Number of bytes in a size_t

		for (size_t blockNumber : inode.dataPtr) {
			std::vector<size_t> buffer;
			vdisk.readBlock(blockNumber, buffer);

			for (size_t value : buffer) {
				// Unpack each `size_t` value back into bytes (chars) and add them to `data`
				for (size_t i = 0; i < size_tWidth; ++i) {
					char byte = static_cast<char>((value >> (i * 8)) & 0xFF);
					data.push_back(byte);
				}
			}
		}
	}

	void listFiles(bool detailed) {
		std::lock_guard<std::mutex> lock(mtx);
		for (const auto&[name, inode] : fileTable) {
			std::cout << name << "\n";
			if (detailed) {
				std::cout << inode.size << "\t" << inode.fileName << std::endl;
			}
		}
	}
};