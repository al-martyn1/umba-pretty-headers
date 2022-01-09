#pragma once


#if defined(WIN32) || defined(_WIN32)

    #include <windows.h>
    #include <debugapi.h>

    #if defined(min)
        #undef min
    #endif

    #if defined(max)
        #undef max
    #endif

#endif



