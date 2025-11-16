#include "division_os.h"

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

ProcessRole ProcessCreate() {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Fault of creating process" << std::endl;
        exit(-1);
    }

    if (pid > 0) {
        return IS_PARENT;
    } else {
        return IS_CHILD;
    }
}

void* create_shared_memory(const char* name, size_t size) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error creating shared memory: " << strerror(errno) << std::endl;
        exit(-1);
    }
    
    if (ftruncate(shm_fd, size) == -1) {
        std::cerr << "Error setting shared memory size: " << strerror(errno) << std::endl;
        exit(-1);
    }
    
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Error mapping shared memory: " << strerror(errno) << std::endl;
        exit(-1);
    }
    
    close(shm_fd);
    return ptr;
}

void* open_shared_memory(const char* name, size_t size) {
    int shm_fd = shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1) {
        std::cerr << "Error opening shared memory: " << strerror(errno) << std::endl;
        exit(-1);
    }
    
    void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "Error mapping shared memory: " << strerror(errno) << std::endl;
        exit(-1);
    }
    
    close(shm_fd);
    return ptr;
}

void close_shared_memory(const char* name, void* ptr, size_t size) {
    if (munmap(ptr, size) == -1) {
        std::cerr << "Error unmapping shared memory: " << strerror(errno) << std::endl;
    }
    
    if (shm_unlink(name) == -1 && errno != ENOENT) {
        std::cerr << "Error unlinking shared memory: " << strerror(errno) << std::endl;
    }
}