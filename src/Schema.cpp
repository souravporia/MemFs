#include "Schema.h"

void Inode::updateModifiedTime() {
	lastModified = std::chrono::system_clock::now();
}