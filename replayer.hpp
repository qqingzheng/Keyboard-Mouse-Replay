#ifndef _REPLAYER_H
#define _REPLAYER_H

#include "monitor.hpp"
#include <string>

class Replayer{
    private:
        std::ifstream inputFile;
        Monitor* monitor;
    public:
        Replayer(const char* path, Monitor* _monitor){
            inputFile = std::ifstream(path);
            monitor = _monitor;
        }
        ~Replayer(){
            inputFile.close();
        }
        void StartReplay();
};

void Replayer::StartReplay(){
    char line[64];
    unsigned long long last_time = 0;
    while(true){
        char* ptr = line;
        int offset = 0;
        inputFile.getline(line, 64);
        if(line[0] == '\0'){
            break;
        }
        while(ptr[offset] != '-'){
            ++ offset;
        }
        ptr[offset] = '\0';
        unsigned long long time = atoll(ptr);
        if(last_time != 0){
            Sleep(time - last_time);
        }
        ptr += offset + 1;
        offset = 0;
        while(ptr[offset] != '-'){
            ++ offset;
        }
        ptr[offset] = '\0';
        char* type = ptr;
        ptr += offset + 1;
        unsigned int scan_key = atoi(ptr);
        printf("%llu %s %u\n", time, type, scan_key);
        if(!strcmp(type, "KeyBoard")){
            monitor->SendScanCode(scan_key);
        }
        last_time = time;
    }
}


#endif