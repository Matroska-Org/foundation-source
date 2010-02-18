/*****************************************************************************
 * 
 * Copyright (c) 2008, CoreCodec, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of CoreCodec, Inc. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CoreCodec, Inc. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CoreCodec, Inc. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "boot.h"

#include <kernel.h>
#include <sifrpc.h>
#include <graph.h>
#include <dma.h>
#include <loadfile.h>
#include <stdio.h>
#include <libcdvd.h>
#include <sbv_patches.h>
#include <iopheap.h>
#include <iopcontrol.h>
#include <osd_config.h>

#define INTC_TIM1  	10
#define T1_COUNT        (volatile u32 *)0x10000800
#define T1_MODE         (volatile u32 *)0x10000810
#define T1_COMP         (volatile u32 *)0x10000820

#define MAX_THREADSLEEP 8

int TvScreenType=TV_SCREEN_43;
int FileSemaId=-1;
int TimeTick=0;

static s32 TimeHandler=-1;
static int TimeCounter[MAX_THREADSLEEP]={0};
static s32 TimeThreadId[MAX_THREADSLEEP];

static int Timer_Interrupt(int Cause)
{
    size_t i;

	++TimeTick;
    for (i=0;i<MAX_THREADSLEEP;++i)
        if (TimeCounter[i]>0 && --TimeCounter[i]==0)
            iWakeupThread(TimeThreadId[i]);

	*T1_COUNT = 0;
	*T1_MODE |= (1 << 10);

	__asm__ volatile("sync.l; ei");
	return 0;
}

static void resetIOP()
{
    cdInit(CDVD_INIT_EXIT);
    SifExitIopHeap();
    SifLoadFileExit();
    SifExitRpc();
    SifIopReset("rom0:UDNL rom0:EELOADCNF", 0);
    while (!SifIopSync()) {}
    printf("IOP synced.");
    SifInitRpc(0);
    SifLoadFileInit();
}

void Boot_Init(const char* Path)
{
    ee_sema_t Sema;

    SifInitRpc(0);

    if (Path)
    {
        int i = strcspn(Path, ":");
        if (strncmp(Path, "host", i) != 0)
             resetIOP();
    }

    sbv_patch_enable_lmb();
    sbv_patch_disable_prefix_check();

    SifLoadModule("rom0:SIO2MAN", 0, NULL);
    SifLoadModule("rom0:MCSERV", 0, NULL);
    SifLoadModule("rom0:MCMAN", 0, NULL);
    SifLoadModule("rom0:PADMAN", 0, NULL);
	SifLoadModule("rom0:LIBSD", 0, NULL);

    fioInit();

    dma_initialize();
    dma_channel_initialize(DMA_CHANNEL_GIF, NULL, DMA_FLAG_CHAIN);

    graph_initialize();
    graph_set_mode(GRAPH_MODE_AUTO, GRAPH_PSM_32, GRAPH_PSM_32);
    graph_set_displaybuffer(0);
    graph_set_drawbuffer(0);
    graph_set_clearbuffer(0,0,0);

    ChangeThreadPriority(GetThreadId(),64);

	TimeHandler = AddIntcHandler(INTC_TIM1, Timer_Interrupt, 0);
	EnableIntc(INTC_TIM1);

	*T1_COUNT = 0;
	*T1_COMP = 586; /* 150MHZ / 256 / 1000 */
	*T1_MODE = 2 | (0<<2) | (0<<6) | (1<<7) | (1<<8);

    Sema.init_count = 1;
    Sema.max_count = 1;
    Sema.option = 0;
    Sema.attr = 0;
    FileSemaId = CreateSema(&Sema);

    TvScreenType = configGetTvScreenType();
}

void Boot_Done()
{
	DisableIntc(INTC_TIM1);
	if (TimeHandler >= 0)
	{
		RemoveIntcHandler(INTC_TIM1, TimeHandler);
		TimeHandler = -1;
	}

    if (FileSemaId >= 0)
    {
        DeleteSema(FileSemaId);
        FileSemaId = -1;
    }

    graph_shutdown();
    dma_shutdown();
}

void ThreadSleep(int Time)
{
    if (Time)
    {
        size_t i;
        s32 ThreadId = GetThreadId();
        CancelWakeupThread(ThreadId);
        DI();
        for (i=0;i<MAX_THREADSLEEP;++i)
            if (!TimeCounter[i])
            {
                TimeThreadId[i] = ThreadId;
                TimeCounter[i] = Time;
                break;
            }
        EI();
        if (i<MAX_THREADSLEEP)
        {
            SleepThread();
            return;
        }
    }
    RotateThreadReadyQueue(64);
}

