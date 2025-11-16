#pragma once

#include <atomic>

struct SharedData {
    std::atomic<bool> data_ready;
    std::atomic<bool> processing_complete;
    std::atomic<bool> division_by_zero;
    std::atomic<bool> terminate_process;
    int numbers_count;
    float numbers[100];
    char filename[256];
};