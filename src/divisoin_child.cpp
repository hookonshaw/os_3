#include "division_os.h"
#include "shared_data.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

int main() {
    const char* shm_name = "/division_shm";
    SharedData* shared_data = static_cast<SharedData*>(open_shared_memory(shm_name, sizeof(SharedData)));
    
    std::ofstream out(shared_data->filename, std::ios::app);
    if (!out.is_open()) {
        std::cerr << "Fault of opening file: " << shared_data->filename << std::endl;
        close_shared_memory(shm_name, shared_data, sizeof(SharedData));
        return 1;
    }

    while (!shared_data->terminate_process) {
        if (shared_data->data_ready) {
            if (shared_data->numbers_count == 0) {
                break;
            }
            
            if (shared_data->numbers_count < 2) {
                out << "Fault: program needs minimum 2 numbers" << std::endl;
            } else {
                float result = shared_data->numbers[0];
                bool error_occurred = false;
                
                for (int i = 1; i < shared_data->numbers_count; i++) {
                    if (shared_data->numbers[i] == 0.0f) {
                        out << "Fault: division by zero" << std::endl;
                        error_occurred = true;
                        shared_data->division_by_zero = true;
                        break;
                    }
                    result /= shared_data->numbers[i];
                }
                
                if (!error_occurred) {
                    out << shared_data->numbers[0];
                    for (int i = 1; i < shared_data->numbers_count; i++) {
                        out << " / " << shared_data->numbers[i];
                    }
                    out << " = " << result << std::endl;
                }
            }
            
            out.flush();
            shared_data->processing_complete = true;
            shared_data->data_ready = false;
        }
        
        usleep(1000);
    }
    
    out.close();
    close_shared_memory(shm_name, shared_data, sizeof(SharedData));
    return 0;
}