#include "monitor.hpp"
#include "replayer.hpp"

#define STREQ(A,B) !strcmp(A,B)

int main(int argc, char* argv[]){
    if(argc > 1){
        Monitor monitor;
        int ptr = 1;
        while(ptr < argc){
            if(STREQ(argv[ptr], "-m")){
                if(!monitor.is_init()){
                    return 1;
                }
                if(++ptr >= argc){
                    printf("args error!");
                    return 1;
                }
                printf("Press F1 to start recording. The log will store in %s. \n", argv[ptr]);
                monitor.StartRecord(argv[ptr]);
            }else if(STREQ(argv[ptr], "-r")){
                if(++ptr >= argc){
                    printf("args error!");
                    return 1;
                }
                Sleep(4000);
                printf("Replay %s. \n", argv[ptr]);
                Replayer replay(argv[ptr], &monitor);
                replay.StartReplay();
            }
            ++ ptr;
        }
    }else{
        printf("Help:\n  -m <path>: Start monitoring\n  -r <path>: Start replaying");
    }
    return 0;
}