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
        inputFile.getline(line, 64);
        line[13] = '\0';
        line[15] = '\0';
        if(line[0] == '\0'){
            break;
        }
        unsigned long long time = atoll(line);
        if(last_time != 0){
            Sleep(time - last_time);
        }
        char* type = line + 14;
        if(*type == 'K'){
            unsigned int scan_key = atoi(line+16);
            monitor->SendScanCode(scan_key);
        }
        last_time = time;
    }
}


#endif