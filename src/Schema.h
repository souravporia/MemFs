#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <vector>

class Inode {
public:
	std::string fileName;
	size_t size;
	std::chrono::system_clock::time_point createdAt;
	std::chrono::system_clock::time_point lastModified;
	std::vector<size_t> dataPtr;

	Inode(const std::string& name = "") : fileName(name), size(0), createdAt(std::chrono::system_clock::now()), lastModified(createdAt) {}

	void updateModifiedTime();
};