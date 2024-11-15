
#ifndef MAPPINGFILE_H
#define MAPPINGFILE_H

#include <iostream>
#include <vector>
#include <string>

#ifdef _WIN32 
#  ifdef MAPFILELIB_EXPORTS
#    define MAPFILELIB_API __declspec(dllexport)
#  else
#    define MAPFILELIB_API __declspec(dllimport)
#  endif
#else
#  define MAPFILELIB_API
#endif

class MAPFILELIB_API MappingFile {
public:
    MappingFile(const std::string& filename, size_t size);
    ~MappingFile();

    int* data();
    size_t size() const;

private:
    int* mapped_data_;
    size_t size_;
#ifdef _WIN32
    void* file_handle_;
    void* mapping_handle_;
#else
    int fd_;
#endif
};

#endif // MAPPINGFILE_H
