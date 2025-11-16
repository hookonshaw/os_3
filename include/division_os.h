#pragma once

#include <cstddef>

enum ProcessRole {
    IS_PARENT,
    IS_CHILD
};

// Функции для работы с разделяемой памятью
void* create_shared_memory(const char* name, size_t size);
void* open_shared_memory(const char* name, size_t size);
void close_shared_memory(const char* name, void* ptr, size_t size);
ProcessRole ProcessCreate();