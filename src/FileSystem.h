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
	std::unordered_map<std::string, bool> openFiles;
	std::bitset<MAX_NUM_FILES> bitmap;
	std::mutex mtx;
	size_t totalBlocks; // Number of blocks
	size_t diskSize; // In Bytes
	size_t blockSize;
	VirtualDisk &vdisk;

public:

	FileSystem(VirtualDisk &vdisk);
	
	void createFile(const std::string& fileName);

	bool writeFile(const std::string& fileName, const std::vector<char>& data);

	bool deleteFile(const std::string& fileName);

	void readFile(const std::string& fileName, std::vector<char>& data);

	void listFiles(bool detailed);
};