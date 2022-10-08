#define START_KEY 112
#define PAUSE_KEY 113
#define END_KEY 114

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

bool FindVKName(UINT virtualKey, char* result){
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