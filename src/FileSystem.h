#pragma once
#include "VirtualDisk.h"
#include "Schema.h"
#include <bitset>
#include <unordered_map>
#include "../lib/parallel_hashmap/phmap.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <atomic>

#define MAX_NUM_FILES 16000

using phmap::flat_hash_map;

class FileSystem {
private:
	VirtualDisk &vdisk;
	size_t blockSize;
	size_t totalBlocks; // Number of blocks
	size_t diskSize; // In Bytes
	flat_hash_map<std::string, Inode> fileTable;
	std::bitset<MAX_NUM_FILES> bitmap;
	std::mutex mtx;
	

public:

	FileSystem(VirtualDisk &vdisk);

	void mkfs();
	
	void createFile(const std::string& fileName);

	bool writeFile(const std::string& fileName, const std::vector<char>& data);

	bool deleteFile(const std::string& fileName);

	void readFile(const std::string& fileName, std::vector<char>& data);

	void listFiles(bool detailed);
};