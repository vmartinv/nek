//
//  config.h
//  Cedille
//
//  Created by Corwin McKnight on 6/17/14.
//  Copyright (c) 2014 Corwin Lee McKnight. All rights reserved.
//

#ifndef CEDILLE_CONFIG_H
#define CEDILLE_CONFIG_H

#define CONFIG_VERSION 1

#define DEBUG 1

#define PROFILER_COL 60
#define PROFILER_ROW 10
#define PROFILER_BLOCKS PROFILER_COL * PROFILER_ROW
#define PROFILER_C_YES   0xE
#define PROFILER_C_NO    0xC
#define PROFILER_C_DEBUG 0x7

#ifdef DEBUG
#ifndef DISABLE_PROFILER
#define START_PROFILE
#else
#define START_PROFILE return;
#endif
#endif


#endif
