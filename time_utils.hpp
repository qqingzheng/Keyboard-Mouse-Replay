#ifndef _TIME_UTIL_H
#define _TIME_UTIL_H
#include <ctime>
#include <sys/timeb.h>
static unsigned long long GetTime(){
    timeb t;
    ftime(&t);
    unsigned long long now = t.time * 1000 + t.millitm;
    return now;
}


#endif

