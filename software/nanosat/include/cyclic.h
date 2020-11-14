#ifndef _CYCLIC_H
#define _CYCLIC_H

#include "globals.h"
#include "senddata.h"
#include "rcommand.h"
#include "spislave.h"
#include "bmesensor.h"
#include "display.h"
#include "sds011read.h"
#include "sdcard.h"

extern Ticker housekeeper;

void housekeeping(void);
void doHousekeeping(void);
uint64_t uptime(void);
void reset_counters(void);
uint32_t getFreeRAM();

#endif
