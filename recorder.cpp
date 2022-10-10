#include "monitor.hpp"
#include "replayer.hpp"

void StartReplay(){
    Monitor monitor;
    Replayer replay("records/test.log", &monitor);
    replay.StartReplay();
}


int main(){
    // Monitor monitor;
    // if(!monitor.is_init()){
    //     return 1;
    // }
    // monitor.StartRecord("records/test.log");
    Sleep(3000);
    StartReplay();
    return 0;
}