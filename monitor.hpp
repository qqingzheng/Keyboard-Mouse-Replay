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
        char keyPressed[128] = {0};
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
        bool IsMarkScanCode(DWORD) const;
        void MoveMouseX(char) const;
        void MoveMouseY(char) const;
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

bool Monitor::IsMarkScanCode(DWORD key) const{
    return key < 128;
}

void Monitor::MoveMouseX(char x) const{
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, 0x08, 1);
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, x, 1);
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, 0, 1);
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, 0, 1);
}

void Monitor::MoveMouseY(char y) const{
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, 0x08, 1);
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, 0, 1);
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, y, 1);
    WaitForBuf();
    funcSetPortVal(CMDPORT, 0xD3, 1);
    funcSetPortVal(DATAPORT, 0, 1);
}

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
    keyPressed[key] = 1;   
}

void Monitor::SetKeyRelease(DWORD key){
    keyPressed[key] = 0;
}

void Monitor::StartRecord(const char* path){
    std::ofstream outputFile(path, std::ios::out);
    bool pause = true;
    bool ignoreNext = false;
    bool resend = true;
    while(true){ // start record until we press END_KEY.
        DWORD cmdPortDword = 0;
        DWORD dataPortDword = 0;
        funcGetPortVal(CMDPORT, &cmdPortDword, 1);
        if(cmdPortDword & 0x1){
            if(!ignoreNext){
                funcGetPortVal(DATAPORT, &dataPortDword, 1);
                if(dataPortDword == START_KEY){
                    printf("Start recording. \n");
                    pause = false;
                }else if(dataPortDword == PAUSE_KEY){
                    MoveMouseX(30);
                    printf("Record pause. \n");
                    pause = true;
                }else if(dataPortDword == END_KEY){
                    printf("Record end. \n");
                    break;
                }else if(dataPortDword == RESEND_KEY){
                    if(resend){
                        resend = false;
                        printf("resend stop. \n");
                    }else{
                        resend = true;
                        printf("resend start. \n");
                    }
                }else if(!pause){
                    if(!IsMarkScanCode(dataPortDword)){
                        if(resend){
                            SendScanCode(dataPortDword);  // resend the scancode.
                        }
                        char result[64];
                        int length = sprintf(result ,"%llu-K-%d\n", GetTime(), dataPortDword);
                        outputFile.write(result, length);
                        SetKeyRelease(dataPortDword & 0x7F);
                        printf("%d release\n", dataPortDword  & 0x7F);
                    }else if(!keyPressed[dataPortDword]){
                        ignoreNext = true;
                        if(resend){
                            SendScanCode(dataPortDword);  // resend the scancode.
                        }
                        char result[64];
                        int length = sprintf(result ,"%llu-K-%d\n", GetTime(), dataPortDword);
                        outputFile.write(result, length);
                        SetKeyPress(dataPortDword);
                        printf("%d press\n", dataPortDword);
                    }
                }else{
                    ignoreNext = true;
                    SendScanCode(dataPortDword);
                    Sleep(1);
                }
            }else{
                ignoreNext = false;
            }
        }
    }
    outputFile.flush();
    outputFile.close();
}

#endif