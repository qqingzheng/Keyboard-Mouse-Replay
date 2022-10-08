#include "WinIO.hpp"
#include "KeyConfig.hpp"
void startListener(){
    bool pause = true;
    bool ignoreNext = false;
    while(1){
        DWORD cmdPortDword = 0;
        DWORD dataPortDword = 0;
        FuncGetPortVal(cmdPort, &cmdPortDword, 1);
        if(cmdPortDword & 0x1){
            if(!ignoreNext){
                FuncGetPortVal(dataPort, &dataPortDword, 1);
                UINT vK = MapVirtualKey(dataPortDword, MAPVK_VSC_TO_VK);
                if(vK == START_KEY){
                    pause = false;
                }else if(vK == PAUSE_KEY){
                    pause = true;
                }else if(vK == END_KEY){
                    break;
                }else if(!pause){
                    ignoreNext = true;
                    KeySendScanCode(dataPortDword);
                    char result[32];
                    if(FindVKName(vK, result)){
                        printf("%s\n", result);
                    }else{
                        printf("%d\n", dataPortDword);
                    }
                    
                }
            }else{
                ignoreNext = false;
            }
        }
    }
}


int main(){
    // printf("%d",InstallDriver());
    if(!WinIOEnvCheck()){
        return 1;
    }
    startListener();
    ShutDownWinIO();
    return 0;
}