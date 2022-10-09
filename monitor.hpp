#ifndef _KEYBOARD_MONITOR_H
#define _KEYBOARD_MONITOR_H
#include "keyboard_config.hpp"
#include "time_utils.hpp"
#include <vector>
#include <fstream>

// Port
#define CMDPORT 0x64
#define DATAPORT 0x60


// function pointer for winio dll
typedef bool (_stdcall *InstallDrive)(PSTR, bool);
typedef bool (_stdcall *GetPortVal)(WORD wPortAddr,PDWORD dwPortVal,BYTE bSize);
typedef bool (_stdcall *SetPortVal)(WORD wPortAddr,DWORD dwPortVal,BYTE bSize);
typedef bool (_stdcall *InitWinIO)();
typedef bool (_stdcall *RemoveWinIoDriver)();
typedef bool (_stdcall *ShutDownWinIO)();


class Monitor{
    private:
        static bool isInit;
        static const HMODULE winIO; // WinIO Core
        static InitWinIO funcInitWinIO;
        static ShutDownWinIO funcShutDownWinIO;
        static RemoveWinIoDriver funcRemoveWinIoDriver;
        static InstallDrive funcInstallDriver;
        static GetPortVal funcGetPortVal;
        static SetPortVal funcSetPortVal;
        std::vector<DWORD> keyPressed = std::vector<DWORD>(32);
        bool WinIOEnvCheck() const;
        bool InstallDriver() const;
        void WaitForBuf() const;
    public:
        ~Monitor(){
            funcShutDownWinIO();
        }
        Monitor(){
            if(WinIOEnvCheck()){
                isInit = true;
            }else{
                isInit = false;
            }
        }
        bool is_init() const{
            return isInit;
        }
        void SendScanCode(DWORD) const;
        void SendKeyPress(DWORD) const;
        void SendKeyRelease(DWORD) const;
        void SetKeyPress(DWORD);
        void SetKeyRelease(DWORD);
        void StartRecord(const char*);
};

bool Monitor::isInit = false;
const HMODULE Monitor::winIO = LoadLibrary(TEXT("WinIo64.dll")); // Load WinIO dll
#define GET_FUNC(NAME) GetProcAddress(winIO, NAME)
InitWinIO Monitor::funcInitWinIO = (InitWinIO) GET_FUNC("InitializeWinIo");  
ShutDownWinIO Monitor::funcShutDownWinIO = (ShutDownWinIO) GET_FUNC("ShutdownWinIo");  
RemoveWinIoDriver Monitor::funcRemoveWinIoDriver = (RemoveWinIoDriver) GET_FUNC("RemoveWinIoDriver");
InstallDrive Monitor::funcInstallDriver = (InstallDrive) GET_FUNC("InstallWinIoDriver"); 
GetPortVal Monitor::funcGetPortVal = (GetPortVal) GET_FUNC("GetPortVal");
SetPortVal Monitor::funcSetPortVal = (SetPortVal) GET_FUNC("SetPortVal");


bool Monitor::WinIOEnvCheck() const{
    if(winIO == NULL){
        printf("error: can't not load WinIo64.dll! \n");
        return false;
    }
    if(!funcInitWinIO()){
        printf("error: can't not init WinIO! \n");
        return false;
    }
    printf("WinIO init. \n");
    return true;
}


bool Monitor::InstallDriver() const{
    if(funcInstallDriver(PSTR("WinIo64.sys"), false)){
        printf("driver is installed. \n");
    }else{
        printf("error: you should run this program in administrative mode! \n");
        return false;
    }
    return true;
}


void Monitor::WaitForBuf() const{
    DWORD stageReg = 0;
    funcGetPortVal(CMDPORT, &stageReg, 1);
    do{
        funcGetPortVal(CMDPORT, &stageReg, 1);
    }while((stageReg & 0x02));
}

void Monitor::SendKeyPress(DWORD key) const{
    DWORD scanCode=MapVirtualKey(key,0);
    SendScanCode(scanCode);
}

void Monitor::SendScanCode(DWORD scanCode) const{
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD2, 1); // send write command to command port.
    WaitForBuf();
    funcSetPortVal(DATAPORT, scanCode, 1);
}

void Monitor::SendKeyRelease(DWORD key) const{
    DWORD scanCodeBreak=MapVirtualKey(key,0) | 0x80;
    SendScanCode(scanCodeBreak);
}

void Monitor::SetKeyPress(DWORD key){
    bool isPressed = false;
    for(DWORD _key : keyPressed){
        if(_key == key){
            isPressed = true;
            break;
        }
    }
    if(!isPressed){
        keyPressed.push_back(key);
    }
    
}

void Monitor::SetKeyRelease(DWORD key){
    for(std::vector<DWORD>::iterator i = keyPressed.begin(); i != keyPressed.end(); ++ i){
        if(*i == key){
            keyPressed.erase(i);
            break;
        }
    }
}

void Monitor::StartRecord(const char* path){
    std::ofstream outputFile(path, std::ios::out);
    bool pause = true;
    bool ignoreNext = false;
    while(true){ // start record until we press END_KEY.
        DWORD cmdPortDword = 0;
        DWORD dataPortDword = 0;
        funcGetPortVal(CMDPORT, &cmdPortDword, 1);
        if(cmdPortDword & 0x1){
            if(!ignoreNext){
                funcGetPortVal(DATAPORT, &dataPortDword, 1);
                UINT vK = MapVirtualKey(dataPortDword, MAPVK_VSC_TO_VK);
                if(vK == START_KEY){
                    pause = false;
                }else if(vK == PAUSE_KEY){
                    pause = true;
                }else if(vK == END_KEY){
                    break;
                }else if(!pause){
                    ignoreNext = true;
                    SendScanCode(dataPortDword);  // resend the scancode.
                    char result[64];
                    int length = sprintf(result ,"%llu-KeyBoard-%d\n", GetTime(), dataPortDword);
                    outputFile.write(result, length);
                    outputFile.flush();
                    printf("Get ScanCode: %d\n", dataPortDword);
                }
            }else{
                ignoreNext = false;
            }
        }
    }
    outputFile.close();
}

#endif