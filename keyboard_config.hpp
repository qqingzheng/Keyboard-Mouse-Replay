#ifndef _WINDOWS_H
#define _WINDOWS_H
#include <Windows.h>
#endif

#ifndef _KEYBOARD_CONFIG_H
#define _KEYBOARD_CONFIG_H
#include <cstdio>
#define START_KEY 59
#define PAUSE_KEY 60
#define END_KEY 61
#define RESEND_KEY 62
typedef struct{
    unsigned int virtualKey;
    const char* name;
} VirtualKeyToName;


static VirtualKeyToName mapList[] = {
    {VK_SPACE, "Space"},
    {VK_SHIFT, "Shift"},
    {VK_ESCAPE, "Esc"},
    {VK_LCONTROL, "LeftControl"},
    {VK_CONTROL, "Control"},
    {VK_MENU, "Alt"},
    {VK_TAB, "Tab"}};

static bool FindVKName(UINT virtualKey, char* result){
    if(virtualKey >= 0x41 && virtualKey <= 0x5A){
        sprintf(result, "%c", virtualKey);
        return true;
    }
    if(virtualKey >= 0x30 && virtualKey <= 0x39){
        sprintf(result, "%c", virtualKey);
        return true;
    }
    for(VirtualKeyToName line : mapList){
        if(line.virtualKey == virtualKey){
            strcpy(result,line.name);
            return true;
        }
    }
    return false;
}
#endif