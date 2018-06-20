
#ifndef _ATCA_LIB_H
#define _ATCA_LIB_H

#include <stddef.h>
#include <string.h>

//slm add
#include "atca_iface.h"
#include "hal/hal_timer.h"

#include "atca_status.h"
#include "atca_command.h"
#include "basic/atca_basic.h"
//#include "basic/atca_helpers.h"

#ifdef ATCAPRINTF
    #include <stdio.h>
//#define BREAK(status, message) {printf(__FUNCTION__": "message" -- Status: %02X\r\n", status); break;}
    #define BREAK(status, message) { printf(": "message " -- Status: %02X\r\n", status); break; }
    #define RETURN(status, message) { printf(": "message " -- Status: %02X\r\n", status); return status; }
    #define PRINTSTAT(status, message) { printf(": "message " -- Status: %02X\r\n", status); }
    #define PRINT(message) { printf(": "message "\r\n"); break; }
    #define DBGOUT(message) { printf(": "message "\r\n"); break; }
#else
    #define BREAK(status, message) { break; }
    #define RETURN(status, message) { return status; }
    #define PRINT(message) { break; }
    #define DBGOUT(message) { break; }
#endif

#endif
