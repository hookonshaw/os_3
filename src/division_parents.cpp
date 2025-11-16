#include "division_os.h"
#include "shared_data.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

int main() {
    const char* shm_name = "/division_shm";
    SharedData* shared_data = static_cast<SharedData*>(create_shared_memory(shm_name, sizeof(SharedData)));
    
    shared_data->data_ready = false;
    shared_data->processing_complete = false;
    shared_data->division_by_zero = false;
    shared_data->terminate_process = false;
    shared_data->numbers_count = 0;
    
    ProcessRole role = ProcessCreate();

    if (role == IS_CHILD) {
        close_shared_memory(shm_name, shared_data, sizeof(SharedData));
        execl("./division_child", "division_child", NULL);
        std::cerr << "Fault execl" << std::endl;
        exit(-1);
    }
    
    std::string file_name;
    std::cout << "Type file's name: ";
    std::cin >> file_name;
    
    strncpy(shared_data->filename, file_name.c_str(), sizeof(shared_data->filename) - 1);
    shared_data->filename[sizeof(shared_data->filename) - 1] = '\0';
    
    std::cout << "Write numbers separated by space (first divides by others):" << std::endl;
    std::cout << "For exit write 'q'" << std::endl;
    
    std::string input;
    std::cin.ignore();
    
    while (!shared_data->division_by_zero && !shared_data->terminate_process) {
        std::cout << "Write numbers: ";
        std::getline(std::cin, input);
        
        if (input == "q" || input == "quit") {
            shared_data->terminate_process = true;
            shared_data->data_ready = true;
            break;
        }
        
        if (input.empty()) {
            continue;
        }
        
        std::vector<float> numbers;
        std::stringstream ss(input);
        float num;
        
        while (ss >> num) {
            numbers.push_back(num);
        }
        
        if (numbers.size() > 0) {
            shared_data->numbers_count = numbers.size();
            for (size_t i = 0; i < numbers.size() && i < 100; i++) {
                shared_data->numbers[i] = numbers[i];
            }
            
            shared_data->data_ready = true;
            shared_data->processing_complete = false;
            
            while (!shared_data->processing_complete && !shared_data->division_by_zero) {
                usleep(1000);
            }
            
            if (shared_data->division_by_zero) {
                std::cout << "Divide by zero. Program is closing" << std::endl;
                break;
            }
        } else {
            std::cout << "Error: false format" << std::endl;
        }
    }
    
    sleep(1);
    
    close_shared_memory(shm_name, shared_data, sizeof(SharedData));
    wait(NULL);
    std::cout << "Program finished" << std::endl;
    
    return 0;
}