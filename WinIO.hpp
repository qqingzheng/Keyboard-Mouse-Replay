#include <Windows.h>
#include <cstdio>

static HMODULE WinIO = LoadLibrary(TEXT("WinIo64.dll"));
#define GET_FUNC(NAME) GetProcAddress(WinIO, NAME)
typedef bool (_stdcall *InstallDrive)(PSTR, bool);
typedef bool (_stdcall *GetPortVal)(WORD wPortAddr,PDWORD dwPortVal,BYTE bSize);
typedef bool (_stdcall *SetPortVal)(WORD wPortAddr,DWORD dwPortVal,BYTE bSize);
typedef bool (_stdcall *InitWinIO)();
typedef bool (_stdcall *RemoveWinIoDriver)();
typedef bool (_stdcall *ShutDownWinIO)();
static InitWinIO FuncInitWinIO = (InitWinIO) GET_FUNC("InitializeWinIo");  // The InitializeWinIo function must be called before using any other function in the library. 
static ShutDownWinIO FuncShutDownWinIO = (ShutDownWinIO) GET_FUNC("ShutdownWinIo");  // The ShutdownWinIo function must be called before terminating the application or in case the WinIo library is no longer required.
static RemoveWinIoDriver FuncRemoveWinIoDriver = (RemoveWinIoDriver) GET_FUNC("RemoveWinIoDriver");
static InstallDrive FuncInstallDriver = (InstallDrive) GET_FUNC("InstallWinIoDriver"); // The InstallWinIoDriver function installs the WinIo driver and configures it to load automatically when Windows starts. This function fails if called from a non-administrative account. 
static GetPortVal FuncGetPortVal = (GetPortVal) GET_FUNC("GetPortVal");
static SetPortVal FuncSetPortVal = (SetPortVal) GET_FUNC("SetPortVal");

WORD cmdPort = 0x64;
WORD dataPort =  0x60;

bool WinIOEnvCheck(){
    if(WinIO == NULL){
        printf("error: can't not load WinIo64.dll! \n");
        return false;
    }
    if(!FuncInitWinIO()){
        printf("error: can't not init WinIO! \n");
        return false;
    }

    printf("WinIO init. \n");
    return true;
}

bool InstallDriver(){
    if(FuncInstallDriver(PSTR("WinIo64.sys"), false)){
        printf("driver is installed. \n");
    }else{
        printf("error: you should run this program in administrative mode! \n");
        return false;
    }
    return true;
}

void WaitForBuf(){
    DWORD stageReg = 0;
    FuncGetPortVal(cmdPort, &stageReg, 1);
    do{
        FuncGetPortVal(cmdPort, &stageReg, 1);
    }while((stageReg & 0x02));
}

void KeyPress(DWORD key){
    DWORD scanCode=MapVirtualKey(key,0);
    WaitForBuf();
    FuncSetPortVal(cmdPort, 0xD2, 1); // send write command to command port.
    WaitForBuf();
    FuncSetPortVal(dataPort, scanCode, 1);
}
void KeySendScanCode(DWORD scanCode){
    WaitForBuf();
    FuncSetPortVal(cmdPort, 0xD2, 1); // send write command to command port.
    WaitForBuf();
    FuncSetPortVal(dataPort, scanCode, 1);
}
void KeyRelease(DWORD key){
    DWORD scanCodeBreak=MapVirtualKey(key,0) | 0x80;
    WaitForBuf();
    FuncSetPortVal(cmdPort, 0xD2, 1); // send write command to command port.
    WaitForBuf();
    FuncSetPortVal(dataPort, scanCodeBreak, 1);
}

void ReadOutputPort(){
    DWORD stageReg = 0;
    for(int i = 0; i < 10000; ++i){
        FuncGetPortVal(cmdPort, &stageReg, 1);
        printf("%u\n", stageReg & 0x02);
    }
}