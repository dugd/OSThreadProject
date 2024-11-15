#include "MappingFile.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

MappingFile::MappingFile(const std::string& filename, size_t size) : mapped_data_(nullptr), size_(size) {
#ifdef _WIN32
    file_handle_ = CreateFileA(filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle_ == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Cannot open file");
    }

    mapping_handle_ = CreateFileMapping(file_handle_, NULL, PAGE_READWRITE, 0, size_ * sizeof(int), NULL);
    if (!mapping_handle_) {
        CloseHandle(file_handle_);
        throw std::runtime_error("Cannot create file mapping");
    }

    mapped_data_ = static_cast<int*>(MapViewOfFile(mapping_handle_, FILE_MAP_ALL_ACCESS, 0, 0, size_ * sizeof(int)));
    if (!mapped_data_) {
        CloseHandle(mapping_handle_);
        CloseHandle(file_handle_);
        throw std::runtime_error("Cannot map view of file");
    }
#else
    fd_ = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd_ == -1) {
        throw std::runtime_error("Cannot open file");
    }

    if (ftruncate(fd_, size_ * sizeof(int)) == -1) {
        close(fd_);
        throw std::runtime_error("Cannot set file size");
    }

    mapped_data_ = static_cast<int*>(mmap(nullptr, size_ * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0));
    if (mapped_data_ == MAP_FAILED) {
        close(fd_);
        throw std::runtime_error("Cannot map file to memory");
    }
#endif
}

MappingFile::~MappingFile() {
#ifdef _WIN32
    if (mapped_data_) {
        UnmapViewOfFile(mapped_data_);
    }
    if (mapping_handle_) {
        CloseHandle(mapping_handle_);
    }
    if (file_handle_) {
        CloseHandle(file_handle_);
    }
#else
    if (mapped_data_) {
        munmap(mapped_data_, size_ * sizeof(int));
    }
    if (fd_ != -1) {
        close(fd_);
    }
#endif
}

int* MappingFile::data() {
    return mapped_data_;
}

size_t MappingFile::size() const {
    return size_;
}
