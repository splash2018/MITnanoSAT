#ifndef _SDCARD_H
#define _SDCARD_H

#include <globals.h>
#include <stdio.h>
#include <SPI.h>
#include <mySD.h>

#include "sds011read.h"

#define SDCARD_FILE_NAME       "paxcount.%02d"
#define SDCARD_FILE_HEADER     "date, time, wifi, bluet"

bool sdcard_init( void );
void sdcardWriteData( uint16_t, uint16_t);

#endif
