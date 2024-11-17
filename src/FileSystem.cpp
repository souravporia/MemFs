#include "FileSystem.h"

FileSystem::FileSystem(VirtualDisk &vdisk)
    : vdisk(vdisk), blockSize(vdisk.blockSize), totalBlocks(vdisk.numBlocks), diskSize(vdisk.diskSize) {
        fileTable.reserve(totalBlocks);
    }

void FileSystem::mkfs()
{
    fileTable.clear();
    bitmap.reset();
}

void FileSystem::createFile(const std::string& fileName) {
    if (fileTable.find(fileName) != fileTable.end()) {
        std::cerr << "Error: " << fileName << " already exists\n";
        return;
    }
    
	fileTable.insert_or_assign(fileName, Inode(fileName));
    std::cout << "File " << fileName << " created successfully\n";
}

bool FileSystem::writeFile(const std::string& fileName, const std::vector<char>& data) {


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

    uint8_t* buffer = new uint8_t[blockSize];
    size_t dataIndex = 0;
    size_t remainingDataSize = dataSize;
    inode.dataPtr.clear(); // Clear previous block pointers

    for (size_t i = 0; i < totalBlocks && remainingDataSize > 0; ++i) {
        if (!bitmap[i]) {  // If block is free
            bitmap[i] = 1;  // Mark block as occupied
            size_t chunkSize = std::min(blockSize, remainingDataSize);

            // Copy data to buffer and write to the block
            std::copy(data.begin() + dataIndex, data.begin() + dataIndex + chunkSize, buffer);
            vdisk.writeBlock(i, buffer);

            // Update inode with the block index and data progress
            inode.dataPtr.push_back(i);
            dataIndex += chunkSize;
            remainingDataSize -= chunkSize;
        }
    }

    delete[] buffer;

    // Check if the write was complete
    if (remainingDataSize > 0) {
        std::cout << "Error: Not enough blocks to complete write for " << fileName << "\n";
        return false;
    }

    inode.size = dataSize;
    inode.updateModifiedTime();
    std::cout << "Successfully written to " << fileName << "\n";
    return true;
}

bool FileSystem::deleteFile(const std::string& fileName) {

    

    if (fileTable.erase(fileName)) {
        std::cout << "File " << fileName << " deleted successfully\n";
        return true;
    } else {
        std::cout << "Error: " << fileName << " does not exist\n";
        return false;
    }
}

void FileSystem::readFile(const std::string& fileName, std::vector<char>& data) {
    auto it = fileTable.find(fileName);
    if (it == fileTable.end()) {
        std::cout << "File not found!" << std::endl;
        return;
    }


    const Inode& inode = it->second;
    data.clear();

    uint8_t* buffer = new uint8_t[blockSize];
    size_t remainingSize = inode.size;

    for (size_t blockIndex : inode.dataPtr) {
        vdisk.readBlock(blockIndex, buffer);
        size_t chunkSize = std::min(blockSize, remainingSize);
        data.insert(data.end(), buffer, buffer + chunkSize);

        // Update remaining size
        remainingSize -= chunkSize;
        if (remainingSize == 0) break;
    }

    delete[] buffer;
    std::cout << "Successfully read from " << fileName << "\n";
}

void FileSystem::listFiles(bool detailed) {
    if(detailed)
    {
        std::cout << "Size" << "\t" << "Created On" << "\t" 
                      << "Modifies" << "\t" << "File Name" << std::endl;
    }
    for (const auto& [name, inode] : fileTable) {

        if (detailed) {
            // Convert chrono::time_point to time_t
            auto createdTime = std::chrono::system_clock::to_time_t(inode.createdAt);
            auto modifiedTime = std::chrono::system_clock::to_time_t(inode.lastModified);
            
            // Convert time_t to readable format using std::put_time
            std::ostringstream createdStream, modifiedStream;
            createdStream << std::put_time(std::localtime(&createdTime), "%Y-%m-%d");
            modifiedStream << std::put_time(std::localtime(&modifiedTime), "%Y-%m-%d");

            std::cout << inode.size << "\t" << createdStream.str() << "\t" 
                      << modifiedStream.str() << "\t" << inode.fileName << std::endl;
        }else{
            std::cout << name << "\n";
        }
    }
}