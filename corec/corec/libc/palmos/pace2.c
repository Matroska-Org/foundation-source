/*****************************************************************************
 * 
 * Copyright (c) 2008-2010, CoreCodec, Inc.
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

#include "corec/corec.h"

#if defined(TARGET_PALMOS)

#include <CoreTraps.h>
#include <LibTraps.h>
#include <IntlMgr.h>

#define NavSelectorFrmNavDrawFocusRing		7
#define NavSelectorFrmNavRemoveFocusRing	8
#define NavSelectorFrmNavGetFocusRingInfo	9
#define NavSelectorFrmNavObjectTakeFocus    10

#define sysTrapVFSMgr	                    sysTrapFileSystemDispatch
#define vfsTrapFileCreate				    2
#define vfsTrapFileOpen					    3
#define vfsTrapFileClose			    	4
#define vfsTrapFileReadData		        	5
#define vfsTrapFileRead				    	6
#define vfsTrapFileWrite			    	7
#define vfsTrapFileDelete			    	8
#define vfsTrapFileRename			    	9
#define vfsTrapFileSeek				    	10
#define vfsTrapFileEOF				    	11
#define vfsTrapFileTell				    	12
#define vfsTrapFileResize			    	13
#define vfsTrapFileGetAttributes	    	14
#define vfsTrapFileSetAttributes	    	15
#define vfsTrapFileGetDate			    	16
#define vfsTrapFileSetDate			    	17
#define vfsTrapFileSize				    	18
#define vfsTrapDirCreate			    	19
#define vfsTrapDirEntryEnumerate	    	20
#define vfsTrapGetDefaultDirectory	        21
#define vfsTrapRegisterDefaultDirectory	    22
#define vfsTrapUnregisterDefaultDirectory	23
#define vfsTrapVolumeFormat	    	    	24
#define vfsTrapVolumeMount		    		25
#define vfsTrapVolumeUnmount		    	26
#define vfsTrapVolumeEnumerate	        	27
#define vfsTrapVolumeInfo		    		28
#define vfsTrapVolumeGetLabel	    		29
#define vfsTrapVolumeSetLabel	    		30
#define vfsTrapVolumeSize		    		31
#define vfsTrapInstallFSLib			        32
#define vfsTrapRemoveFSLib			    	33
#define vfsTrapImportDatabaseFromFile	    34
#define vfsTrapExportDatabaseToFile		    35
#define vfsTrapFileDBGetResource		    36
#define vfsTrapFileDBInfo				    37
#define vfsTrapFileDBGetRecord		        38
#define vfsTrapImportDatabaseFromFileCustom	39
#define vfsTrapExportDatabaseToFileCustom	40

#define HDSelectorWinSetCoordinateSystem	8
#define HDSelectorWinGetCoordinateSystem	9
#define HDSelectorWinScalePoint				10
#define HDSelectorWinUnscalePoint			11
#define HDSelectorWinScaleRectangle			12
#define HDSelectorWinUnscaleRectangle		13
#define HDSelectorWinScreenGetAttribute		14
#define HDSelectorWinScaleCoord				18
#define HDSelectorWinUnscaleCoord		    19

#define	pinPINSetInputAreaState			    0
#define pinPINGetInputAreaState			    1
#define pinPINSetInputTriggerState		    2
#define pinPINGetInputTriggerState	 	    3
#define pinWinSetConstraintsSize		    13
#define pinFrmSetDIAPolicyAttr			    14
#define pinFrmGetDIAPolicyAttr			    15
#define pinStatHide						    16
#define pinStatShow						    17
#define pinStatGetAttribute				    18
#define pinSysGetOrientation                19
#define pinSysSetOrientation                20
#define pinSysGetOrientationTriggerState    21
#define pinSysSetOrientationTriggerState    22

#define netLibTrapAddrINToA					(sysLibTrapCustom+0)
#define netLibTrapAddrAToIN					(sysLibTrapCustom+1)

#define netLibTrapSocketOpen				(sysLibTrapCustom+2)
#define netLibTrapSocketClose				(sysLibTrapCustom+3)
#define netLibTrapSocketOptionSet			(sysLibTrapCustom+4)
#define netLibTrapSocketOptionGet			(sysLibTrapCustom+5)
#define netLibTrapSocketBind				(sysLibTrapCustom+6)
#define netLibTrapSocketConnect				(sysLibTrapCustom+7)
#define netLibTrapSocketListen				(sysLibTrapCustom+8)
#define netLibTrapSocketAccept				(sysLibTrapCustom+9)
#define netLibTrapSocketShutdown			(sysLibTrapCustom+10)

#define netLibTrapSendPB					(sysLibTrapCustom+11)
#define netLibTrapSend						(sysLibTrapCustom+12)
#define netLibTrapReceivePB					(sysLibTrapCustom+13)
#define netLibTrapReceive					(sysLibTrapCustom+14)
#define netLibTrapDmReceive					(sysLibTrapCustom+15)
#define netLibTrapSelect					(sysLibTrapCustom+16)

#define netLibTrapPrefsGet					(sysLibTrapCustom+17)
#define netLibTrapPrefsSet					(sysLibTrapCustom+18)

#define netLibTrapGetHostByName				(sysLibTrapCustom+22)
#define netLibTrapGetHostByAddr				(sysLibTrapCustom+34)
#define netLibTrapGetServByName				(sysLibTrapCustom+35)
#define netLibTrapSocketAddr				(sysLibTrapCustom+36)


#include "arm/pealstub.h"

typedef struct emustate
{
	uint32_t instr;
	uint32_t regD[8];
	uint32_t regA[8];
	uint32_t regPC;

} emustate;

#define PACE_ADD16          1,
#define PACE_ADD32          2,
#define PACE_TRAP(Trap)     3,PceNativeTrapNo(Trap) & 255,PceNativeTrapNo(Trap)>>8,
#define PACE_TRAP32(Trap)   3,PceNativeTrapNo(Trap) & 255,PceNativeTrapNo(Trap)>>8,
#define PACE_TRAPPTR(Trap)  4,PceNativeTrapNo(Trap) & 255,PceNativeTrapNo(Trap)>>8,
#define PACE_TRAP16(Trap)   5,PceNativeTrapNo(Trap) & 255,PceNativeTrapNo(Trap)>>8,
#define PACE_DATA16(n)      6,n,
#define PACE_DATA32(n)      7,n,
#define PACE_ADD8           8,
#define PACE_SWAP_BEGIN(n)  9,n,
#define PACE_SWAP16         2,
#define PACE_SWAP32         4,
#define PACE_SWAP_SKIP(n)   (n)^1,
#define PACE_SWAP_END       0,
#define PACE_COPY(n,size)   10,n,size,
#define PACE_SEL(sel)		11,(sel)&255,(sel)>>8,
#define PACE_TRAP8(Trap)    12,PceNativeTrapNo(Trap) & 255,PceNativeTrapNo(Trap)>>8,
#define PACE_EVENT_ARM_TO_M68K(n) 13,n,
#define PACE_ADDCOPY        14,
#define PACE_EVENT_M68K_TO_ARM(n) 15,n,
#define PACE_UNPOPPED(c)     16,(c)&255,(c)>>8,
#define PACE_SWAP32LIST16(n,m) 17,n,m,
#define PACE_SWAP16DATA(n,m) 18,n,m,
#define PACE_SWAP32LISTPTR32(n,m) 19,n,m,

#define PACE_BEGIN          @@error@@

#define PACE_BEGIN0(Name)   uint32_t Name() { static const uint8_t cmd[] = {
#define PACE_END0           0}; return PaceParse(NULL,cmd); }

#define PACE_BEGIN1(Name)   uint32_t Name(uint32_t a) { static const uint8_t cmd[] = {
#define PACE_END1           0}; return PaceParse_1(a,cmd); }

#define PACE_BEGIN2(Name)   uint32_t Name(uint32_t a,uint32_t b) { static const uint8_t cmd[] = {
#define PACE_END2           0}; return PaceParse_2(a,b,cmd); }

#define PACE_BEGIN3(Name)   uint32_t Name(uint32_t a,uint32_t b,uint32_t c) { static const uint8_t cmd[] = {
#define PACE_END3           0}; return PaceParse_3(a,b,c,cmd); }

#define PACE_BEGIN4(Name)   uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d) { static const uint8_t cmd[] = {
#define PACE_END4           0}; return PaceParse_4(a,b,c,d,cmd); }

#define PACE_BEGIN5(Name)   uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e) { static const uint8_t cmd[] = {
#define PACE_END5           0}; return PaceParse_5(a,b,c,d,e,cmd); }

#define PACE_BEGIN6(Name)   uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f) { static const uint8_t cmd[] = {
#define PACE_END6           0}; return PaceParse_6(a,b,c,d,e,f,cmd); }

#define PACE_BEGIN7(Name)   uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g) { static const uint8_t cmd[] = {
#define PACE_END7           0}; return PaceParse_7(a,b,c,d,e,f,g,cmd); }

#define PACE_BEGIN8(Name)   uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h) { static const uint8_t cmd[] = {
#define PACE_END8           0}; return PaceParse_8(a,b,c,d,e,f,g,h,cmd); }

#define PACE_BEGIN9(Name)   uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h,uint32_t i) { static const uint8_t cmd[] = {
#define PACE_END9           0}; return PaceParse_9(a,b,c,d,e,f,g,h,i,cmd); }

#define PACE_BEGIN10(Name)  uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h,uint32_t i,uint32_t j) { static const uint8_t cmd[] = {
#define PACE_END10          0}; return PaceParse_10(a,b,c,d,e,f,g,h,i,j,cmd); }

#define PACE_BEGIN13(Name)  uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h,uint32_t i,uint32_t j,uint32_t k,uint32_t l,uint32_t m) { static const uint8_t cmd[] = {
#define PACE_END13          0}; return PaceParse_13(a,b,c,d,e,f,g,h,i,j,k,l,m,cmd); }

#define PACE_BEGIN16(Name)  uint32_t Name(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h,uint32_t i,uint32_t j,uint32_t k,uint32_t l,uint32_t m,uint32_t n,uint32_t p,uint32_t q) { static const uint8_t cmd[] = {
#define PACE_END16          0}; return PaceParse_16(a,b,c,d,e,f,g,h,i,j,k,l,m,n,p,q,cmd); }

#define MAX_COPY    32
#define MAX_ARGS    32

void Event_M68K_To_ARM(const uint16_t* In,void* Out);
void Event_ARM_To_M68K(const void* In,uint16_t* Out);

uint32_t Call68K(uint32_t trapOrFunction, const void *argsOnStackP, uint32_t argsSizeAndwantA0);

static NOINLINE uint32_t PaceParse(uint32_t* param, const uint8_t* cmd)
{
    uint16_t args[MAX_ARGS];
    uint16_t* pos = args;
    uint32_t* i = param;
    uint32_t result = 0;
    uint8_t* ptr8;
    uint16_t* ptr16;
    uint32_t* ptr32;
    uint32_t* cnt32;
    uint8_t copy[MAX_COPY];
    uint8_t* copypos = copy;
    size_t num;

    for (;*cmd;++cmd)
    {
        switch (*cmd)
        {
        case 1:
            *(pos++) = SWAP16(*i);
            ++i;
            assert(pos<=args+MAX_ARGS);
            break;
        case 2:
            *(pos++) = SWAP16(*i >> 16);
            *(pos++) = SWAP16(*i);
            ++i;
            assert(pos<=args+MAX_ARGS);
            break;
        case 3:
            result = Call68K(cmd[1]|(cmd[2]<<8),args,((uint8_t*)pos - (uint8_t*)args));
            cmd += 2;
            break;
        case 4:
            result = Call68K(cmd[1]|(cmd[2]<<8),args,((uint8_t*)pos - (uint8_t*)args) | kPceNativeWantA0);
            cmd += 2;
            break;
        case 5:
            result = (uint16_t)Call68K(cmd[1]|(cmd[2]<<8),args,((uint8_t*)pos - (uint8_t*)args));
            cmd += 2;
            break;
        case 12:
            result = (uint8_t)Call68K(cmd[1]|(cmd[2]<<8),args,((uint8_t*)pos - (uint8_t*)args));
            cmd += 2;
            break;
        case 6:
            ptr16 = (uint16_t*)param[*(++cmd)];
            if (ptr16)
                *ptr16 = SWAP16(*ptr16);
            break;
        case 7:
            ptr32 = (uint32_t*)param[*(++cmd)];
            if (ptr32)
                *ptr32 = SWAP32(*ptr32);
            break;
        case 8: //PACE_ADD8
            *(pos++) = (uint8_t)(*i);
            ++i;
            break;
        case 9: //PACE_SWAP_BEGIN
            ptr8 = (uint8_t*)param[*(++cmd)];
            while (*(++cmd))
                if (ptr8)
                {
                    if (*cmd & 1)
                    {
                        // skip
                        if (*cmd == 1)
                            ++ptr8;
                        else
                            ptr8 += *cmd ^ 1;
                    }
                    else
                    {
                        // swap
                        switch (*cmd)
                        {
                        case 4: 
                            *((uint32_t*)ptr8) = SWAP32(*((uint32_t*)ptr8));
                            break;
                        case 2:
                            *((uint16_t*)ptr8) = SWAP16(*((uint16_t*)ptr8));
                            break;
                        }
                        ptr8 += *cmd;
                    }
                }
            break;
        case 10:
            ptr8 = (uint8_t*)param[cmd[1]];
            if (ptr8)
            {
                memcpy(copypos,ptr8,cmd[2]);
                param[cmd[1]] = (uint32_t)copypos;
                copypos += cmd[2];
                assert(copypos<copy+MAX_COPY);
            }
            cmd += 2;
            break;
        case 11:
            ((emustate*)PceCall.State)->regD[2] = cmd[1]|(cmd[2]<<8);
            cmd += 2;
            break;
        case 13:
            Event_ARM_To_M68K((void*)param[*(++cmd)],(uint16_t*)copypos);
            param[*cmd] = (uint32_t)copypos;
            copypos += 12*2;
            assert(copypos<copy+MAX_COPY);
            break;
        case 14:
            *(pos++) = SWAP16((uint32_t)copy >> 16);
            *(pos++) = SWAP16((uint32_t)copy);
            ++i;
            assert(pos<=args+MAX_ARGS);
            break;
        case 15:
            Event_M68K_To_ARM((uint16_t*)copy,(void*)param[*(++cmd)]);
            break;
        case 16:
            ((emustate*)PceCall.State)->regA[7] -= 2;
            *(pos++) = (uint16_t)(cmd[2]|(cmd[1]<<8));
            cmd += 2;
            assert(pos<=args+MAX_ARGS);
            break;
        case 17:
            ptr32 = (uint32_t*)param[*(++cmd)];
            num = (uint16_t)param[*(++cmd)];
            if (ptr32)
            {
                size_t i;
                for (i=0;i<num;++i,++ptr32)
                    *ptr32 = SWAP32(*ptr32);
            }
            break;
        case 18:
            ptr16 = (uint16_t*)param[*(++cmd)];
            num = (uint16_t)param[*(++cmd)];
            if (ptr16)
            {
                size_t i;
                for (i=0;i<num;i+=2,++ptr16)
                    *ptr16 = SWAP16(*ptr16);
            }
            break;
        case 19:
            ptr32 = (uint32_t*)param[*(++cmd)];
            cnt32 = (uint32_t*)param[*(++cmd)];
            if (ptr32 && cnt32)
            {
                ptr32 = (uint32_t*)*ptr32;
                if (ptr32)
                {
                    size_t i;
                    for (i=0;i<*cnt32;++i,++ptr32)
                        *ptr32 = SWAP32(*ptr32);
                }
            }
            break;
        }
    }
    return result;
}

static NOINLINE uint32_t PaceParse_1(uint32_t a, const uint8_t* cmd)
{
    uint32_t param[1];
    param[0] = a;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_2(uint32_t a,uint32_t b, const uint8_t* cmd)
{
    uint32_t param[2];
    param[0] = a;
    param[1] = b;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_3(uint32_t a,uint32_t b,uint32_t c, const uint8_t* cmd)
{
    uint32_t param[3];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_4(uint32_t a,uint32_t b,uint32_t c,uint32_t d, const uint8_t* cmd)
{
    uint32_t param[4];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_5(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e, const uint8_t* cmd)
{
    uint32_t param[5];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_6(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f, const uint8_t* cmd)
{
    uint32_t param[6];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    param[5] = f;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_7(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g, const uint8_t* cmd)
{
    uint32_t param[7];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    param[5] = f;
    param[6] = g;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_8(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h, const uint8_t* cmd)
{
    uint32_t param[8];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    param[5] = f;
    param[6] = g;
    param[7] = h;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_9(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h,uint32_t i, const uint8_t* cmd)
{
    uint32_t param[9];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    param[5] = f;
    param[6] = g;
    param[7] = h;
    param[8] = i;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_10(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e,uint32_t f,uint32_t g,uint32_t h,uint32_t i,uint32_t j, const uint8_t* cmd)
{
    uint32_t param[10];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    param[5] = f;
    param[6] = g;
    param[7] = h;
    param[8] = i;
    param[9] = j;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_13(uint32_t a,uint32_t b,uint32_t c,uint32_t d, 
                                    uint32_t e,uint32_t f,uint32_t g,uint32_t h,
                                    uint32_t i,uint32_t j,uint32_t k,uint32_t l,
                                    uint32_t m,const uint8_t* cmd)
{
    uint32_t param[13];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    param[5] = f;
    param[6] = g;
    param[7] = h;
    param[8] = i;
    param[9] = j;
    param[10] = k;
    param[11] = l;
    param[12] = m;
    return PaceParse(param,cmd);
}

static NOINLINE uint32_t PaceParse_16(uint32_t a,uint32_t b,uint32_t c,uint32_t d, 
                                    uint32_t e,uint32_t f,uint32_t g,uint32_t h,
                                    uint32_t i,uint32_t j,uint32_t k,uint32_t l,
                                    uint32_t m,uint32_t n,uint32_t p,uint32_t q,const uint8_t* cmd)
{
    uint32_t param[16];
    param[0] = a;
    param[1] = b;
    param[2] = c;
    param[3] = d;
    param[4] = e;
    param[5] = f;
    param[6] = g;
    param[7] = h;
    param[8] = i;
    param[9] = j;
    param[10] = k;
    param[11] = l;
    param[12] = m;
    param[13] = n;
    param[14] = p;
    param[15] = q;
    return PaceParse(param,cmd);
}

PACE_BEGIN2(WinPaintPixel)
PACE_ADD16
PACE_ADD16
PACE_TRAP(sysTrapWinPaintPixel)
PACE_END2

PACE_BEGIN4(WinDrawLine)
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_TRAP(sysTrapWinDrawLine)
PACE_END4

PACE_BEGIN13(DmDatabaseInfo)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapDmDatabaseInfo)
PACE_DATA16(3)
PACE_DATA16(4) 
PACE_DATA32(5) 
PACE_DATA32(6) 
PACE_DATA32(7) 
PACE_DATA32(8)
PACE_DATA32(9) 
PACE_DATA32(10) 
PACE_DATA32(11) 
PACE_DATA32(12)
PACE_END13

PACE_BEGIN2(MemHeapID)
PACE_ADD16
PACE_ADD16
PACE_TRAP16(sysTrapMemHeapID)
PACE_END2

PACE_BEGIN1(MemPtrNew)
PACE_ADD32
PACE_TRAPPTR(sysTrapMemPtrNew)
PACE_END1

PACE_BEGIN1(MemChunkFree)
PACE_ADD32
PACE_TRAP16(sysTrapMemChunkFree)
PACE_END1

PACE_BEGIN1(MemPtrSize)
PACE_ADD32
PACE_TRAP32(sysTrapMemPtrSize)
PACE_END1

PACE_BEGIN2(MemPtrResize)
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapMemPtrResize)
PACE_END2

PACE_BEGIN3(MemHeapFreeBytes)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapMemHeapFreeBytes)
PACE_DATA32(1)
PACE_DATA32(2)
PACE_END3

PACE_BEGIN1(MemHeapCheck)
PACE_ADD16
PACE_TRAP16(sysTrapMemHeapCheck)
PACE_END1

/*
PACE_BEGIN2(FrmGetGadgetData)
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapFrmGetGadgetData)
PACE_END2

PACE_BEGIN3(FrmSetGadgetData)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP(sysTrapFrmSetGadgetData)
PACE_END3
*/

PACE_BEGIN6(FrmNewGadget)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_DATA32(0)
PACE_TRAPPTR(sysTrapFrmNewGadget)
PACE_DATA32(0)
PACE_END6

PACE_BEGIN2(_FrmRemoveObject)
PACE_ADD32
PACE_ADD16
PACE_DATA32(0)
PACE_TRAP16(sysTrapFrmRemoveObject)
PACE_DATA32(0)
PACE_END2

PACE_BEGIN2(FrmGetFormBounds)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapFrmGetFormBounds)
PACE_SWAP_BEGIN(1)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END2

PACE_BEGIN0(FrmCloseAllForms)
PACE_TRAP(sysTrapFrmCloseAllForms)
PACE_END0

PACE_BEGIN2(FrmGetObjectId)
PACE_ADD32
PACE_ADD16
PACE_TRAP16(sysTrapFrmGetObjectId)
PACE_END2

PACE_BEGIN1(FrmGetNumberOfObjects)
PACE_ADD32
PACE_TRAP16(sysTrapFrmGetNumberOfObjects)
PACE_END1

PACE_BEGIN0(FrmGetActiveFormID)
PACE_TRAP16(sysTrapFrmGetActiveFormID)
PACE_END0

PACE_BEGIN1(FrmGotoForm)
PACE_ADD16
PACE_TRAP(sysTrapFrmGotoForm)
PACE_END1

PACE_BEGIN2(CtlSetUsable)
PACE_ADD32
PACE_ADD8
PACE_TRAP(sysTrapCtlSetUsable)
PACE_END2

PACE_BEGIN1(CtlGetValue)
PACE_ADD32
PACE_TRAP16(sysTrapCtlGetValue)
PACE_END1

PACE_BEGIN2(CtlSetValue)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapCtlSetValue)
PACE_END2

PACE_BEGIN2(CtlSetLabel)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapCtlSetLabel)
PACE_END2

PACE_BEGIN4(WinScrollRectangle)
PACE_COPY(0,4*2)
PACE_SWAP_BEGIN(1)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_ADD32
PACE_ADD8
PACE_ADD16
PACE_ADD32
PACE_TRAP(sysTrapWinScrollRectangle)
PACE_SWAP_BEGIN(3)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END4

PACE_BEGIN2(WinGetBounds)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapWinGetBounds)
PACE_SWAP_BEGIN(1)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END2

PACE_BEGIN2(WinSetBounds)
PACE_COPY(1,4*2)
PACE_SWAP_BEGIN(1)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapWinSetBounds)
PACE_END2

PACE_BEGIN3(FrmSetObjectBounds)
PACE_COPY(2,4*2)
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP(sysTrapFrmSetObjectBounds)
PACE_END3

PACE_BEGIN2(WinDrawRectangle)
PACE_COPY(0,4*2)
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapWinDrawRectangle)
PACE_END2

PACE_BEGIN3(WinDrawBitmap)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_TRAP(sysTrapWinDrawBitmap)
PACE_END3

PACE_BEGIN2(WinEraseRectangle)
PACE_COPY(0,4*2)
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapWinEraseRectangle)
PACE_END2

PACE_BEGIN4(WinDrawChars)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_TRAP(sysTrapWinDrawChars)
PACE_END4

PACE_BEGIN2(SysGetAppInfo)
PACE_ADD32
PACE_ADD32
PACE_TRAPPTR(sysTrapSysGetAppInfo) 
PACE_END2

PACE_BEGIN3(MemChunkNew)
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapMemChunkNew)
PACE_END3

PACE_BEGIN2(VFSVolumeEnumerate)
PACE_ADD32
PACE_ADD32
PACE_DATA32(1)
PACE_SEL(vfsTrapVolumeEnumerate)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA16(0) 
PACE_DATA32(1)
PACE_END2

PACE_BEGIN3(VFSVolumeGetLabel)
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_SEL(vfsTrapVolumeGetLabel)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END3

PACE_BEGIN2(VFSVolumeInfo)
PACE_ADD16
PACE_ADD32
PACE_SEL(vfsTrapVolumeInfo)
PACE_TRAP16(sysTrapVFSMgr)
PACE_SWAP_BEGIN(1)
PACE_SWAP32 // attributes
PACE_SWAP32 // fsType
PACE_SWAP32 // fsCreator
PACE_SWAP32 // mountClass
PACE_SWAP16 // slotLibRefNum
PACE_SWAP16 // slotRefNum
PACE_SWAP32 // mediaType
PACE_SWAP32 // reverved
PACE_SWAP_END
PACE_END2

PACE_BEGIN2(FtrPtrFree)
PACE_ADD32
PACE_ADD16
PACE_TRAP16(sysTrapFtrPtrFree)
PACE_END2

PACE_BEGIN3(FtrGet)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysTrapFtrGet)
PACE_DATA32(2)
PACE_END3

PACE_BEGIN3(FtrSet)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysTrapFtrSet)
PACE_END3

PACE_BEGIN4(FtrPtrNew)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapFtrPtrNew)
PACE_DATA32(3)
PACE_END4

PACE_BEGIN4(DmWrite)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapDmWrite)
PACE_END4

PACE_BEGIN2(DmGetResource)
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapDmGetResource)
PACE_END2

PACE_BEGIN1(DmReleaseResource)
PACE_ADD32
PACE_TRAP16(sysTrapDmReleaseResource)
PACE_END1

PACE_BEGIN1(MemHandleSize)
PACE_ADD32
PACE_TRAP32(sysTrapMemHandleSize)
PACE_END1

PACE_BEGIN1(MemHandleLock)
PACE_ADD32
PACE_TRAPPTR(sysTrapMemHandleLock)
PACE_END1

PACE_BEGIN1(MemHandleUnlock)
PACE_ADD32
PACE_TRAP16(sysTrapMemHandleUnlock)
PACE_END1

PACE_BEGIN1(MemPtrUnlock)
PACE_ADD32
PACE_TRAP16(sysTrapMemPtrUnlock)
PACE_END1

PACE_BEGIN7(DmGetNextDatabaseByTypeCreator)
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_DATA32(6) //dbIDP
PACE_TRAP16(sysTrapDmGetNextDatabaseByTypeCreator)
PACE_DATA16(5) //cardNoP
PACE_DATA32(6) //dbIDP
PACE_END7

PACE_BEGIN5(DmDatabaseSize)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapDmDatabaseSize)
PACE_DATA32(2) //numRecordsP
PACE_DATA32(3) //totalBytesP
PACE_DATA32(4) //dataBytesP
PACE_END5

PACE_BEGIN3(DmOpenDatabaseByTypeCreator)
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapDmOpenDatabaseByTypeCreator)
PACE_END3

PACE_BEGIN1(DmCloseDatabase)
PACE_ADD32
PACE_TRAP16(sysTrapDmCloseDatabase)
PACE_END1

PACE_BEGIN2(DmGetRecord)
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapDmGetRecord)
PACE_END2

PACE_BEGIN3(DmReleaseRecord)
PACE_ADD32
PACE_ADD16
PACE_ADD8
PACE_TRAP16(sysTrapDmReleaseRecord)
PACE_END3

PACE_BEGIN1(DmNumRecords)
PACE_ADD32
PACE_TRAP16(sysTrapDmNumRecords)
PACE_END1

PACE_BEGIN2(DmDeleteDatabase)
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysTrapDmDeleteDatabase)
PACE_END2

PACE_BEGIN13(DmSetDatabaseInfo)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_DATA16(3)
PACE_DATA16(4) 
PACE_DATA32(5) 
PACE_DATA32(6) 
PACE_DATA32(7) 
PACE_DATA32(8) 
PACE_DATA32(9) 
PACE_DATA32(10) 
PACE_DATA32(11) 
PACE_DATA32(12)
PACE_TRAP16(sysTrapDmSetDatabaseInfo)
PACE_DATA16(3)
PACE_DATA16(4) 
PACE_DATA32(5) 
PACE_DATA32(6) 
PACE_DATA32(7) 
PACE_DATA32(8) 
PACE_DATA32(9) 
PACE_DATA32(10) 
PACE_DATA32(11) 
PACE_DATA32(12)
PACE_END13

PACE_BEGIN6(FileOpen)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAPPTR(sysTrapFileOpen)
PACE_DATA16(5) //errP
PACE_END6

PACE_BEGIN2(FileDelete)
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysTrapFileDelete)
PACE_END2

PACE_BEGIN1(FileClose)
PACE_ADD32
PACE_TRAP16(sysTrapFileClose)
PACE_END1

PACE_BEGIN2(FileTruncate)
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapFileTruncate)
PACE_END2

PACE_BEGIN5(FileWrite)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP32(sysTrapFileWrite)
PACE_DATA16(4) //errP
PACE_END5

PACE_BEGIN7(FileReadLow)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP32(sysTrapFileReadLow)
PACE_DATA16(6) //errP
PACE_END7

PACE_BEGIN3(FileSeek)
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_TRAP16(sysTrapFileSeek)
PACE_END3

PACE_BEGIN3(FileTell)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP32(sysTrapFileTell)
PACE_DATA32(1) //fileSizeP
PACE_DATA16(2) //errP
PACE_END3

PACE_BEGIN1(WinSetDrawMode)
PACE_ADD8
PACE_TRAP8(sysTrapWinSetDrawMode)
PACE_END1

PACE_BEGIN4(WinPaintChars)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_TRAP(sysTrapWinPaintChars)
PACE_END4

PACE_BEGIN2(WinDeleteWindow)
PACE_ADD32
PACE_ADD8
PACE_TRAP(sysTrapWinDeleteWindow)
PACE_END2

PACE_BEGIN6(WinCopyRectangle)
PACE_COPY(2,2*4)
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD8
PACE_TRAP(sysTrapWinCopyRectangle)
PACE_END6

PACE_BEGIN2(WinCreateBitmapWindow)
PACE_ADD32
PACE_ADD32
PACE_TRAPPTR(sysTrapWinCreateBitmapWindow)
PACE_DATA16(1)
PACE_END2

PACE_BEGIN4(WinCreateOffscreenWindow)
PACE_ADD16
PACE_ADD16
PACE_ADD8
PACE_ADD32
PACE_TRAPPTR(sysTrapWinCreateOffscreenWindow)
PACE_DATA16(3)
PACE_END4

PACE_BEGIN1(WinGetClip)
PACE_ADD32
PACE_TRAP(sysTrapWinGetClip)
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END1

PACE_BEGIN1(WinSetClip)
PACE_COPY(0,2*4)
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_ADD32
PACE_TRAP(sysTrapWinSetClip)
PACE_END1

PACE_BEGIN0(WinPushDrawState)
PACE_TRAP(sysTrapWinPushDrawState)
PACE_END0

PACE_BEGIN0(WinPopDrawState)
PACE_TRAP(sysTrapWinPopDrawState)
PACE_END0

PACE_BEGIN5(SclSetScrollBar)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_TRAP(sysTrapSclSetScrollBar)
PACE_END5

PACE_BEGIN2(WinScalePoint)
PACE_ADD32
PACE_ADD8
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_SEL(HDSelectorWinScalePoint)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END2

PACE_BEGIN1(WinSetCoordinateSystem)
PACE_ADD16
PACE_SEL(HDSelectorWinSetCoordinateSystem)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_END1

PACE_BEGIN4(FrmCustomAlert)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapFrmCustomAlert)
PACE_END4

PACE_BEGIN3(FrmGetObjectBounds)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP(sysTrapFrmGetObjectBounds)
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END3

PACE_BEGIN2(WinGetDisplayExtent)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapWinGetDisplayExtent)
PACE_DATA16(0)
PACE_DATA16(1)
PACE_END2

/*
PACE_BEGIN2(WinScaleCoord)
PACE_ADD16
PACE_ADD8
PACE_SEL(HDSelectorWinScaleCoord)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_END2

PACE_BEGIN2(WinUnscaleCoord)
PACE_ADD16
PACE_ADD8
PACE_SEL(HDSelectorWinUnscaleCoord)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_END2

PACE_BEGIN1(WinUnscaleRectangle)
PACE_ADD32
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_SEL(HDSelectorWinUnscaleRectangle)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END1
*/

PACE_BEGIN2(WinUnscalePoint)
PACE_ADD32
PACE_ADD8
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_SEL(HDSelectorWinUnscalePoint)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_SWAP_BEGIN(0)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_END2

PACE_BEGIN0(WinGetCoordinateSystem)
PACE_SEL(HDSelectorWinGetCoordinateSystem)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_END0

PACE_BEGIN2(WinScreenGetAttribute)
PACE_ADD8
PACE_ADD32
PACE_SEL(HDSelectorWinScreenGetAttribute)
PACE_TRAP16(sysTrapHighDensityDispatch)
PACE_DATA32(1)
PACE_END2

PACE_BEGIN5(WinScreenMode)
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_DATA32(1)
PACE_DATA32(2)
PACE_DATA32(3)
PACE_TRAP16(sysTrapWinScreenMode)
PACE_DATA32(1)
PACE_DATA32(2)
PACE_DATA32(3)
PACE_END5

PACE_BEGIN4(WinPalette)
PACE_ADD8
PACE_ADD16
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysTrapWinPalette)
PACE_END4

PACE_BEGIN0(WinGetDisplayWindow)
PACE_TRAPPTR(sysTrapWinGetDisplayWindow)
PACE_END0

PACE_BEGIN0(WinGetActiveWindow)
PACE_TRAPPTR(sysTrapWinGetActiveWindow)
PACE_END0

PACE_BEGIN1(WinGetBitmap)
PACE_ADD32
PACE_TRAPPTR(sysTrapWinGetBitmap)
PACE_END1

PACE_BEGIN4(BmpGetDimensions)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapBmpGetDimensions)
PACE_DATA16(1) 
PACE_DATA16(2) 
PACE_DATA16(3)
PACE_END4

PACE_BEGIN1(BmpGetBits)
PACE_ADD32
PACE_TRAPPTR(sysTrapBmpGetBits)
PACE_END1

PACE_BEGIN2(WinSetTextColorRGB)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapWinSetTextColorRGB)
PACE_END2

PACE_BEGIN2(WinSetBackColorRGB)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapWinSetBackColorRGB)
PACE_END2

PACE_BEGIN3(VFSRegisterDefaultDirectory)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapRegisterDefaultDirectory)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END3

PACE_BEGIN4(VFSGetDefaultDirectory)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapGetDefaultDirectory) 
PACE_DATA16(3)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA16(3)
PACE_END4

PACE_BEGIN4(VFSImportDatabaseFromFile)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapImportDatabaseFromFile)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA16(2)
PACE_DATA32(3)
PACE_END4

PACE_BEGIN3(VFSFileRename)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileRename)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END3

PACE_BEGIN2(VFSFileDelete)
PACE_ADD16
PACE_ADD32
PACE_SEL(vfsTrapFileDelete)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END2

PACE_BEGIN2(VFSDirCreate)
PACE_ADD16
PACE_ADD32
PACE_SEL(vfsTrapDirCreate)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END2

PACE_BEGIN4(VFSFileOpen)
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_SEL(vfsTrapFileOpen)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(3)
PACE_END4

PACE_BEGIN1(VFSFileClose)
PACE_ADD32
PACE_SEL(vfsTrapFileClose)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END1

PACE_BEGIN2(VFSFileSize)
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileSize)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(1)
PACE_END2

PACE_BEGIN3(VFSFileGetDate)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_SEL(vfsTrapFileGetDate)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(2)
PACE_END3

PACE_BEGIN3(VFSDirEntryEnumerate)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapDirEntryEnumerate)
PACE_DATA32(1)
PACE_SWAP_BEGIN(2)
PACE_SWAP32
PACE_SWAP32
PACE_SWAP16
PACE_SWAP_END
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(1)
PACE_SWAP_BEGIN(2)
PACE_SWAP32
PACE_SWAP32
PACE_SWAP16
PACE_SWAP_END
PACE_END3

PACE_BEGIN2(VFSFileGetAttributes)
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileGetAttributes)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(1)
PACE_END2

PACE_BEGIN2(VFSFileSetAttributes)
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileSetAttributes)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END2

PACE_BEGIN5(VFSFileReadData)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileReadData)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(4)
PACE_END5

PACE_BEGIN4(VFSFileRead)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileRead)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(3)
PACE_END4

PACE_BEGIN4(VFSFileWrite)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileWrite)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(3)
PACE_END4

PACE_BEGIN3(VFSFileSeek)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_SEL(vfsTrapFileSeek)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END3

PACE_BEGIN2(VFSFileTell)
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileTell)
PACE_TRAP16(sysTrapVFSMgr)
PACE_DATA32(1)
PACE_END2

PACE_BEGIN2(VFSFileResize)
PACE_ADD32
PACE_ADD32
PACE_SEL(vfsTrapFileResize)
PACE_TRAP16(sysTrapVFSMgr)
PACE_END2

PACE_BEGIN1(WinScreenLock)
PACE_ADD8
PACE_TRAPPTR(sysTrapWinScreenLock)
PACE_END1

PACE_BEGIN0(WinScreenUnlock)
PACE_TRAP(sysTrapWinScreenUnlock)
PACE_END0

PACE_BEGIN3(LstSetListChoices)
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_SWAP32LIST16(1,2)
PACE_TRAP(sysTrapLstSetListChoices)
PACE_END3

PACE_BEGIN9(_LstNewList)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD8
PACE_ADD16
PACE_ADD16
PACE_DATA32(0)
PACE_TRAP16(sysTrapLstNewList)
PACE_DATA32(0)
PACE_END9

PACE_BEGIN1(LstPopupList)
PACE_ADD32
PACE_TRAP16(sysTrapLstPopupList)
PACE_END1

PACE_BEGIN2(LstSetHeight)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapLstSetHeight)
PACE_END2

PACE_BEGIN2(LstSetSelection)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapLstSetSelection)
PACE_END2

PACE_BEGIN2(LstGetSelectionText)
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapLstGetSelectionText)
PACE_END2

PACE_BEGIN1(MemHandleNew)
PACE_ADD32
PACE_TRAPPTR(sysTrapMemHandleNew)
PACE_END1
							
PACE_BEGIN1(MemHandleFree)
PACE_ADD32
PACE_TRAP16(sysTrapMemHandleFree)
PACE_END1

PACE_BEGIN16(_FldNewField)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD8 // font
PACE_ADD32 // maxchars
PACE_ADD8 // editable
PACE_ADD8 // underlined
PACE_ADD8 // suingleline
PACE_ADD8 // dynamicsize
PACE_ADD8 // justification
PACE_ADD8 // autoshift
PACE_ADD8 // hasScrollBar
PACE_ADD8 // numeric
PACE_DATA32(0)
PACE_TRAPPTR(sysTrapFldNewField)
PACE_DATA32(0)
PACE_END16

PACE_BEGIN1(FldReleaseFocus)
PACE_ADD32
PACE_TRAP(sysTrapFldReleaseFocus)
PACE_END1

PACE_BEGIN1(FldGrabFocus)
PACE_ADD32
PACE_TRAP(sysTrapFldGrabFocus)
PACE_END1

PACE_BEGIN2(FldRecalculateField)
PACE_ADD32
PACE_ADD8
PACE_TRAP(sysTrapFldRecalculateField)
PACE_END2

PACE_BEGIN3(FldGetSelection)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapFldGetSelection)
PACE_DATA16(1)
PACE_DATA16(2)
PACE_END3

PACE_BEGIN1(FldGetTextPtr)
PACE_ADD32
PACE_TRAPPTR(sysTrapFldGetTextPtr)
PACE_END1

PACE_BEGIN1(FldGetInsPtPosition)
PACE_ADD32
PACE_TRAP16(sysTrapFldGetInsPtPosition)
PACE_END1

PACE_BEGIN2(FldSetInsPtPosition)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapFldSetInsPtPosition)
PACE_END2

PACE_BEGIN2(FldSetTextHandle)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapFldSetTextHandle)
PACE_END2

PACE_BEGIN1(FldGetTextHandle)
PACE_ADD32
PACE_TRAPPTR(sysTrapFldGetTextHandle)
PACE_END1

PACE_BEGIN1(SndPlaySystemSound)
PACE_ADD8
PACE_TRAP(sysTrapSndPlaySystemSound)
PACE_END1

PACE_BEGIN9(SndStreamCreate)
PACE_ADD32
PACE_ADD8
PACE_ADD32
PACE_ADD16
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_TRAP16(sysTrapSndStreamCreate)
PACE_DATA32(0)
PACE_END9

PACE_BEGIN1(SndStreamDelete)
PACE_ADD32
PACE_TRAP16(sysTrapSndStreamDelete)
PACE_END1

PACE_BEGIN1(SndStreamStart)
PACE_ADD32
PACE_TRAP16(sysTrapSndStreamStart)
PACE_END1

PACE_BEGIN2(SndStreamPause)
PACE_ADD32
PACE_ADD8
PACE_TRAP16(sysTrapSndStreamPause)
PACE_END2

PACE_BEGIN1(SndStreamStop)
PACE_ADD32
PACE_TRAP16(sysTrapSndStreamStop)
PACE_END1

PACE_BEGIN2(SndStreamSetPan)
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapSndStreamSetPan)
PACE_END2

PACE_BEGIN2(SndStreamSetVolume)
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapSndStreamSetVolume)
PACE_END2

PACE_BEGIN2(SndStreamGetVolume)
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapSndStreamGetVolume)
PACE_DATA32(1)
PACE_END2

PACE_BEGIN1(CtlDrawControl)
PACE_ADD32
PACE_TRAP(sysTrapCtlDrawControl)
PACE_END1

PACE_BEGIN3(FrmCopyLabel)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP(sysTrapFrmCopyLabel)
PACE_END3

PACE_BEGIN1(FldDrawField)
PACE_ADD32
PACE_TRAP(sysTrapFldDrawField)
PACE_END1

PACE_BEGIN2(FldSetTextPtr)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapFldSetTextPtr)
PACE_END2

PACE_BEGIN1(FrmGetFormPtr)
PACE_ADD16
PACE_TRAPPTR(sysTrapFrmGetFormPtr)
PACE_END1

PACE_BEGIN0(FrmGetActiveForm)
PACE_TRAPPTR(sysTrapFrmGetActiveForm)
PACE_END0

PACE_BEGIN2(FrmHideObject)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapFrmHideObject)
PACE_END2

PACE_BEGIN2(FrmShowObject)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapFrmShowObject)
PACE_END2

PACE_BEGIN3(_FrmSetGadgetHandler)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP(sysTrapFrmSetGadgetHandler)
PACE_END3

PACE_BEGIN2(_FrmSetEventHandler)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapFrmSetEventHandler)
PACE_END2

PACE_BEGIN1(_FrmDeleteForm)
PACE_ADD32
PACE_TRAP(sysTrapFrmDeleteForm)
PACE_END1

PACE_BEGIN1(_FrmReturnToForm)
PACE_ADD16
PACE_TRAP(sysTrapFrmReturnToForm)
PACE_END1

PACE_BEGIN3(EvtEnqueueKey)
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_TRAP16(sysTrapEvtEnqueueKey)
PACE_END3

PACE_BEGIN0(FntLineHeight)
PACE_TRAP16(sysTrapFntLineHeight)
PACE_END0

PACE_BEGIN2(FntLineWidth)
PACE_ADD32
PACE_ADD16
PACE_TRAP16(sysTrapFntLineWidth)
PACE_END2

PACE_BEGIN2(FntCharsWidth)
PACE_ADD32
PACE_ADD16
PACE_TRAP16(sysTrapFntCharsWidth)
PACE_END2

PACE_BEGIN1(FntSetFont)
PACE_ADD8
PACE_TRAP8(sysTrapFntSetFont)
PACE_END1

PACE_BEGIN0(KeyCurrentState)
PACE_TRAP32(sysTrapKeyCurrentState)
PACE_END0

PACE_BEGIN0(TimGetSeconds)
PACE_TRAP32(sysTrapTimGetSeconds)
PACE_END0

PACE_BEGIN1(TimDateTimeToSeconds)
PACE_ADD32
PACE_SWAP_BEGIN(0)
PACE_SWAP16 //second
PACE_SWAP16 //minute
PACE_SWAP16 //hour
PACE_SWAP16 //day
PACE_SWAP16 //month
PACE_SWAP16 //year
PACE_SWAP16 //weekDay
PACE_SWAP_END
PACE_TRAP32(sysTrapTimDateTimeToSeconds)
PACE_SWAP_BEGIN(0)
PACE_SWAP16 //second
PACE_SWAP16 //minute
PACE_SWAP16 //hour
PACE_SWAP16 //day
PACE_SWAP16 //month
PACE_SWAP16 //year
PACE_SWAP16 //weekDay
PACE_SWAP_END
PACE_END1

PACE_BEGIN2(TimSecondsToDateTime)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapTimSecondsToDateTime)
PACE_SWAP_BEGIN(1)
PACE_SWAP16 //second
PACE_SWAP16 //minute
PACE_SWAP16 //hour
PACE_SWAP16 //day
PACE_SWAP16 //month
PACE_SWAP16 //year
PACE_SWAP16 //weekDay
PACE_SWAP_END
PACE_END2

PACE_BEGIN1(SysLibOpen)
PACE_ADD16
PACE_TRAP16(sysLibTrapOpen)
PACE_END1

PACE_BEGIN1(SysLibClose)
PACE_ADD16
PACE_TRAP16(sysLibTrapClose)
PACE_END1

PACE_BEGIN1(SysHandleEvent)
PACE_EVENT_ARM_TO_M68K(0)
PACE_ADD32
PACE_TRAP8(sysTrapSysHandleEvent)
PACE_END1

PACE_BEGIN2(FrmHandleEvent)
PACE_EVENT_ARM_TO_M68K(1)
PACE_ADD32
PACE_ADD32
PACE_TRAP8(sysTrapFrmHandleEvent)
PACE_END2

PACE_BEGIN1(_FrmDispatchEvent)
PACE_EVENT_ARM_TO_M68K(0)
PACE_ADD32
PACE_TRAP8(sysTrapFrmDispatchEvent)
PACE_END1

PACE_BEGIN3(EvtAddUniqueEventToQueue)
PACE_EVENT_ARM_TO_M68K(0)
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_TRAP(sysTrapEvtAddUniqueEventToQueue)
PACE_END3

PACE_BEGIN1(EvtAddEventToQueue)
PACE_EVENT_ARM_TO_M68K(0)
PACE_ADD32
PACE_TRAP(sysTrapEvtAddEventToQueue)
PACE_END1

PACE_BEGIN2(EvtGetEvent)
PACE_ADDCOPY
PACE_ADD32
PACE_TRAP(sysTrapEvtGetEvent)
PACE_EVENT_M68K_TO_ARM(0)
PACE_END2

PACE_BEGIN1(MenuHideItem)
PACE_ADD16
PACE_TRAP8(sysTrapMenuHideItem)
PACE_END1

PACE_BEGIN4(MenuAddItem)
PACE_ADD16
PACE_ADD16
PACE_ADD8
PACE_ADD32
PACE_TRAP16(sysTrapMenuAddItem)
PACE_END4

PACE_BEGIN0(MenuGetActiveMenu)
PACE_TRAPPTR(sysTrapMenuGetActiveMenu)
PACE_END0

PACE_BEGIN3(MenuHandleEvent)
PACE_EVENT_ARM_TO_M68K(1)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP8(sysTrapMenuHandleEvent)
PACE_DATA16(2)
PACE_END3

PACE_BEGIN1(EvtEnableGraffiti)
PACE_ADD8
PACE_TRAP(sysTrapEvtEnableGraffiti)
PACE_END1

PACE_BEGIN0(EvtResetAutoOffTimer)
PACE_TRAP16(sysTrapEvtResetAutoOffTimer)
PACE_END0

PACE_BEGIN0(FrmGetFirstForm)
PACE_TRAPPTR(sysTrapFrmGetFirstForm)
PACE_END0

PACE_BEGIN2(FrmGetObjectPtr)
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapFrmGetObjectPtr)
PACE_END2

#if defined(IX86)
// simulator freezes many times. bypassing...
Boolean EvtEventAvail() { return 1; }
#else
PACE_BEGIN0(EvtEventAvail)
PACE_TRAP8(sysTrapEvtEventAvail)
PACE_END0
#endif

PACE_BEGIN3(EvtGetPen)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapEvtGetPen)
PACE_DATA16(0)
PACE_DATA16(1)
PACE_END3

PACE_BEGIN1(EvtSysEventAvail)
PACE_ADD8
PACE_TRAP8(sysTrapEvtSysEventAvail)
PACE_END1

PACE_BEGIN5(CtlSetSliderValues)
PACE_COPY(1,2)
PACE_COPY(2,2)
PACE_COPY(3,2)
PACE_COPY(4,2)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_DATA16(1)
PACE_DATA16(2)
PACE_DATA16(3)
PACE_DATA16(4)
PACE_TRAP(sysTrapCtlSetSliderValues)
PACE_END5								   

PACE_BEGIN1(WinSetDrawWindow)
PACE_ADD32
PACE_TRAPPTR(sysTrapWinSetDrawWindow)
PACE_END1

PACE_BEGIN0(WinGetDrawWindow)
PACE_TRAPPTR(sysTrapWinGetDrawWindow)
PACE_END0

PACE_BEGIN1(UIColorGetTableEntryIndex)
PACE_ADD8
PACE_TRAP8(sysTrapUIColorGetTableEntryIndex)
PACE_END1

PACE_BEGIN1(WinSetForeColor)
PACE_ADD8
PACE_TRAP8(sysTrapWinSetForeColor)
PACE_END1

PACE_BEGIN1(WinSetBackColor)
PACE_ADD8
PACE_TRAP8(sysTrapWinSetBackColor)
PACE_END1

PACE_BEGIN1(WinSetTextColor)
PACE_ADD8
PACE_TRAP8(sysTrapWinSetTextColor)
PACE_END1

PACE_BEGIN4(DmNewResource)
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAPPTR(sysTrapDmNewResource)
PACE_END4

PACE_BEGIN4(DmFindResource)
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysTrapDmFindResource)
PACE_END4

PACE_BEGIN2(DmFindDatabase)
PACE_ADD16
PACE_ADD32
PACE_TRAP32(sysTrapDmFindDatabase)
PACE_END2

PACE_BEGIN5(DmCreateDatabase)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_TRAP16(sysTrapDmCreateDatabase)
PACE_END5

PACE_BEGIN3(DmOpenDatabase)
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_TRAPPTR(sysTrapDmOpenDatabase)
PACE_END3

PACE_BEGIN2(DmRemoveResource)
PACE_ADD32
PACE_ADD16
PACE_TRAP16(sysTrapDmRemoveResource)
PACE_END2

PACE_BEGIN2(FrmSetMenu)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapFrmSetMenu)
PACE_END2

PACE_BEGIN1(FrmDoDialog)
PACE_ADD32
PACE_TRAP16(sysTrapFrmDoDialog)
PACE_END1

PACE_BEGIN1(FrmGetFormId)
PACE_ADD32
PACE_TRAP16(sysTrapFrmGetFormId)
PACE_END1

PACE_BEGIN1(FrmPopupForm)
PACE_ADD16
PACE_TRAP(sysTrapFrmPopupForm)
PACE_END1

PACE_BEGIN1(FrmDrawForm)
PACE_ADD32
PACE_TRAP(sysTrapFrmDrawForm)
PACE_END1

PACE_BEGIN1(FrmEraseForm)
PACE_ADD32
PACE_TRAP(sysTrapFrmEraseForm)
PACE_END1

PACE_BEGIN2(FrmGetObjectIndex)
PACE_ADD32
PACE_ADD16
PACE_TRAP16(sysTrapFrmGetObjectIndex)
PACE_END2

PACE_BEGIN6(SysNotifyRegister)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_ADD32
PACE_TRAP16(sysTrapSysNotifyRegister)
PACE_END6

PACE_BEGIN1(FrmVisible)
PACE_ADD32
PACE_TRAP8(sysTrapFrmVisible)
PACE_END1

PACE_BEGIN1(FrmSetActiveForm)
PACE_ADD32
PACE_TRAP(sysTrapFrmSetActiveForm)
PACE_END1

PACE_BEGIN1(FrmInitForm)
PACE_ADD16
PACE_TRAPPTR(sysTrapFrmInitForm)
PACE_END1

PACE_BEGIN1(FrmGetWindowHandle)
PACE_ADD32
PACE_TRAPPTR(sysTrapFrmGetWindowHandle)
PACE_END1

PACE_BEGIN4(SysNotifyUnregister)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_TRAP16(sysTrapSysNotifyUnregister)
PACE_END4

PACE_BEGIN2(SysCurAppDatabase)
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapSysCurAppDatabase)
PACE_DATA16(0)
PACE_DATA32(1)
PACE_END2

PACE_BEGIN0(GetOEMSleepMode)
PACE_SEL(263)
PACE_TRAP32(sysTrapOEMDispatch2)
PACE_END0

PACE_BEGIN1(SetOEMSleepMode)
PACE_SEL(264)
PACE_ADD32
PACE_TRAP16(sysTrapOEMDispatch2)
PACE_END1

PACE_BEGIN0(SysGetOrientation)
PACE_SEL(pinSysGetOrientation)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END0

PACE_BEGIN3(WinGetPixelRGB)
PACE_ADD16
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysTrapWinGetPixelRGB)
PACE_END3

PACE_BEGIN2(WinSetForeColorRGB)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapWinSetForeColorRGB)
PACE_END2

PACE_BEGIN2(WinDrawPixel)
PACE_ADD16
PACE_ADD16
PACE_TRAP(sysTrapWinDrawPixel)
PACE_END2

PACE_BEGIN1(FrmGetFocus)
PACE_ADD32
PACE_TRAP16(sysTrapFrmGetFocus)
PACE_END1

PACE_BEGIN2(FrmSetFocus)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapFrmSetFocus)
PACE_END2

PACE_BEGIN2(FrmSetTitle)
PACE_ADD32
PACE_ADD32
PACE_TRAP(sysTrapFrmSetTitle)
PACE_END2

PACE_BEGIN2(FrmGetObjectType)
PACE_ADD32
PACE_ADD16
PACE_TRAP8(sysTrapFrmGetObjectType)
PACE_END2

PACE_BEGIN6(FrmNewLabel)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD8
PACE_DATA32(0)
PACE_TRAPPTR(sysTrapFrmNewLabel)
PACE_DATA32(0)
PACE_END6

PACE_BEGIN10(FrmNewForm)
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD8
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_TRAPPTR(sysTrapFrmNewForm)
PACE_END10

PACE_BEGIN7(WinSetConstraintsSize)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_SEL(pinWinSetConstraintsSize)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END7

PACE_BEGIN1(SysSetOrientation)
PACE_ADD16
PACE_SEL(pinSysSetOrientation)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END1

PACE_BEGIN0(PINGetInputAreaState)
PACE_SEL(pinPINGetInputAreaState)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END0

PACE_BEGIN1(PINSetInputTriggerState)
PACE_ADD16
PACE_SEL(pinPINSetInputTriggerState)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END1

PACE_BEGIN2(FrmSetDIAPolicyAttr)
PACE_ADD32
PACE_ADD16
PACE_SEL(pinFrmSetDIAPolicyAttr)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END2

PACE_BEGIN1(PINSetInputAreaState)
PACE_ADD16
PACE_SEL(pinPINSetInputAreaState)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END1

PACE_BEGIN1(SysSetOrientationTriggerState)
PACE_ADD16
PACE_SEL(pinSysSetOrientationTriggerState)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END1
	
PACE_BEGIN2(StatGetAttribute)
PACE_ADD16
PACE_ADD32
PACE_SEL(pinStatGetAttribute)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_DATA32(1)
PACE_END2
	
PACE_BEGIN0(StatHide)
PACE_SEL(pinStatHide)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END0

PACE_BEGIN0(StatShow)
PACE_SEL(pinStatShow)
PACE_TRAP16(sysTrapPinsDispatch)
PACE_END0

PACE_BEGIN1(SysSetAutoOffTime)
PACE_ADD16
PACE_TRAP16(sysTrapSysSetAutoOffTime)
PACE_END1

PACE_BEGIN1(SysTaskDelay)
PACE_ADD32
PACE_TRAP16(sysTrapSysTaskDelay)
PACE_END1

PACE_BEGIN0(SysTicksPerSecond)
PACE_TRAP16(sysTrapSysTicksPerSecond)
PACE_END0

PACE_BEGIN0(TimGetTicks)
PACE_TRAP32(sysTrapTimGetTicks)
PACE_END0

PACE_BEGIN1(PrefGetPreference)
PACE_ADD8
PACE_TRAP32(sysTrapPrefGetPreference)
PACE_END1

PACE_BEGIN5(PrefGetAppPreferences)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_DATA16(3)
PACE_TRAP16(sysTrapPrefGetAppPreferences)
PACE_DATA16(3)
PACE_END5

PACE_BEGIN6(PrefSetAppPreferences)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_ADD8
PACE_TRAP(sysTrapPrefSetAppPreferences)
PACE_END6

//PACE_BEGIN2(PceNativeCall)
//PACE_ADD32
//PACE_ADD32
//PACE_TRAP32(sysTrapPceNativeCall)
//PACE_END2

PACE_BEGIN1(RotationMgrGetLibAPIVersion)
PACE_ADD16
PACE_TRAP32((sysLibTrapCustom+0))
PACE_END1

PACE_BEGIN3(RotationMgrAttributeGet)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_TRAP16((sysLibTrapCustom+5))
PACE_DATA32(2)
PACE_END3

PACE_BEGIN7(SysBatteryInfo)
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapSysBatteryInfo)
PACE_DATA16(1)
PACE_DATA16(2)
PACE_DATA16(3)
PACE_END7

#ifdef HAVE_PALMONE_SDK

#include <68K/System/HsExtTraps.h>

PACE_BEGIN5(HsNavGetFocusRingInfo)
PACE_UNPOPPED(hsSelNavGetFocusRingInfo)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapOEMDispatch)
PACE_DATA16(1) 
PACE_DATA16(2) 
PACE_SWAP_BEGIN(3)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_DATA16(4) 
PACE_END5

PACE_BEGIN6(HsNavDrawFocusRing)
PACE_COPY(3,2*4)
PACE_SWAP_BEGIN(3)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_UNPOPPED(hsSelNavDrawFocusRing)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_ADD8
PACE_TRAP16(sysTrapOEMDispatch)
PACE_END6

PACE_BEGIN1(HsNavRemoveFocusRing)
PACE_UNPOPPED(hsSelNavRemoveFocusRing)
PACE_ADD32
PACE_TRAP16(sysTrapOEMDispatch)
PACE_END1

PACE_BEGIN2(HsNavObjectTakeFocus)
PACE_UNPOPPED(hsSelNavObjectTakeFocus)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapOEMDispatch)
PACE_END2

PACE_BEGIN2(HsLightCircumstance)
PACE_UNPOPPED(hsSelLightCircumstance)
PACE_ADD8
PACE_ADD16
PACE_TRAP16(sysTrapOEMDispatch)
PACE_END2

PACE_BEGIN0(HsCurrentLightCircumstance)
PACE_UNPOPPED(hsGetCurrentLightCircumstance)
PACE_TRAP16(sysTrapOEMDispatch)
PACE_END0

PACE_BEGIN3(HsAttrGet)
PACE_UNPOPPED(hsSelAttrGet)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapOEMDispatch)
PACE_DATA32(2)
PACE_END3

PACE_BEGIN3(HsAttrSet)
PACE_UNPOPPED(hsSelAttrSet)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_DATA32(2)
PACE_TRAP16(sysTrapOEMDispatch)
PACE_DATA32(2)
PACE_END3

PACE_BEGIN2(HWUBlinkLED)
PACE_ADD16
PACE_ADD8
PACE_TRAP16((sysLibTrapCustom+0))
PACE_END2

PACE_BEGIN2(HWUSetBlinkRate)
PACE_ADD16
PACE_ADD16
PACE_TRAP16((sysLibTrapCustom+1))
PACE_END2

PACE_BEGIN2(HWUEnableDisplay)
PACE_ADD16
PACE_ADD8
PACE_TRAP16((sysLibTrapCustom+2))
PACE_END2

PACE_BEGIN1(HWUGetDisplayState)
PACE_ADD16
PACE_TRAP8((sysLibTrapCustom+3))
PACE_END1

PACE_BEGIN1(DexGetDisplayAddress)
PACE_ADD16
PACE_TRAPPTR((sysLibTrapCustom+4))
PACE_END1

PACE_BEGIN4(DexGetDisplayDimensions)
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP((sysLibTrapCustom+5))
PACE_DATA16(1) 
PACE_DATA16(2) 
PACE_DATA16(3)
PACE_END4

#endif

#ifdef HAVE_SONY_SDK

PACE_BEGIN1(VskGetAPIVersion)
PACE_ADD16
PACE_TRAP32((sysLibTrapCustom+3))
PACE_END1

PACE_BEGIN3(VskSetState)
PACE_ADD16
PACE_ADD16
PACE_ADD16
PACE_TRAP16((sysLibTrapCustom+6))
PACE_END3

PACE_BEGIN3(VskGetState)
PACE_ADD16
PACE_ADD16
PACE_ADD32
PACE_TRAP16((sysLibTrapCustom+7))
PACE_DATA16(2)
PACE_END3

#endif

PACE_BEGIN3(SysLibLoad)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapSysLibLoad)
PACE_DATA16(2)
PACE_END3

PACE_BEGIN2(SysLibFind)
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapSysLibFind)
PACE_DATA16(1)
PACE_END2

PACE_BEGIN5(FrmNavGetFocusRingInfo)
PACE_UNPOPPED(NavSelectorFrmNavGetFocusRingInfo)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_TRAP16(sysTrapNavSelector)
PACE_DATA16(1)
PACE_DATA16(2)
PACE_SWAP_BEGIN(3)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_DATA16(4)
PACE_END5

PACE_BEGIN6(FrmNavDrawFocusRing)
PACE_COPY(3,2*4)
PACE_SWAP_BEGIN(3)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP16
PACE_SWAP_END
PACE_UNPOPPED(NavSelectorFrmNavDrawFocusRing)
PACE_ADD32
PACE_ADD16
PACE_ADD16
PACE_ADD32
PACE_ADD16
PACE_ADD8
PACE_TRAP16(sysTrapNavSelector)
PACE_END6

PACE_BEGIN1(FrmNavRemoveFocusRing)
PACE_UNPOPPED(NavSelectorFrmNavRemoveFocusRing)
PACE_ADD32
PACE_TRAP16(sysTrapNavSelector)
PACE_END1

PACE_BEGIN2(FrmNavObjectTakeFocus)
PACE_UNPOPPED(NavSelectorFrmNavObjectTakeFocus)
PACE_ADD32
PACE_ADD16
PACE_TRAP(sysTrapNavSelector)
PACE_END2

PACE_BEGIN2(NetLibOpen)
PACE_ADD16
PACE_ADD32
PACE_TRAP16(sysLibTrapOpen)
PACE_DATA16(1)
PACE_END2

PACE_BEGIN2(NetLibClose)
PACE_ADD16
PACE_ADD16
PACE_TRAP16(sysLibTrapClose)
PACE_END2

PACE_BEGIN6(NetLibSocketOpen)
PACE_ADD16 //libRefnum
PACE_ADD8 //domain
PACE_ADD8 //type
PACE_ADD16 //protocol
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_TRAP16(netLibTrapSocketOpen)
PACE_DATA16(5)
PACE_END6

PACE_BEGIN5(NetLibSocketShutdown)
PACE_ADD16 //libRefnum
PACE_ADD16 //socket
PACE_ADD16 //direction
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_TRAP16(netLibTrapSocketShutdown)
PACE_DATA16(4)
PACE_END5

PACE_BEGIN4(NetLibSocketClose)
PACE_ADD16 //libRefnum
PACE_ADD16 //socket
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_TRAP16(netLibTrapSocketClose)
PACE_DATA16(3)
PACE_END4

PACE_BEGIN3(NetLibAddrINToA)
PACE_ADD16 //libRefnum
PACE_ADD32 //inet
PACE_ADD32 //spaceP
PACE_TRAPPTR(netLibTrapAddrINToA)
PACE_END3

PACE_BEGIN2(NetLibAddrAToIN)
PACE_ADD16 //libRefnum
PACE_ADD32 //addr
PACE_TRAP32(netLibTrapAddrAToIN)
PACE_END2

PACE_BEGIN6(NetLibSocketBind)
PACE_ADD16 //libRefnum
PACE_ADD16 //socket
PACE_ADD32 //sockAddrP
PACE_ADD16 //addrLen
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_TRAP16(netLibTrapSocketBind)
PACE_DATA16(5)
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_END6

PACE_BEGIN6(NetLibSocketConnect)
PACE_ADD16 //libRefnum
PACE_ADD16 //socket
PACE_ADD32 //sockAddrP
PACE_ADD16 //addrLen
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_TRAP16(netLibTrapSocketConnect)
PACE_DATA16(5)
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_END6

PACE_BEGIN9(NetLibReceive)
PACE_ADD16 //libRefNum
PACE_ADD16 //socket
PACE_ADD32 //bufP
PACE_ADD16 //bufLen
PACE_ADD16 //flags
PACE_ADD32 //fromAddrP
PACE_ADD32 //fromLenP
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_DATA16(6)
PACE_SWAP_BEGIN(5)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_TRAP16(netLibTrapReceive)
PACE_DATA16(6)
PACE_SWAP_BEGIN(5)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_DATA16(8)
PACE_END9

PACE_BEGIN9(NetLibSend)
PACE_ADD16 //libRefNum
PACE_ADD16 //socket
PACE_ADD32 //bufP
PACE_ADD16 //bufLen
PACE_ADD16 //flags
PACE_ADD32 //toAddrP
PACE_ADD16 //toLen
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_SWAP_BEGIN(5)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_TRAP16(netLibTrapSend)
PACE_SWAP_BEGIN(5)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_DATA16(8)
PACE_END9

PACE_BEGIN8(NetLibSocketAddr)
PACE_ADD16 //libRefnum
PACE_ADD16 //socketRef
PACE_ADD32 //locAddrP
PACE_ADD32 //locAddrLenP
PACE_ADD32 //remAddrP
PACE_ADD32 //remAddrLenP
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_DATA16(5)
PACE_DATA16(3)
PACE_TRAP16(netLibTrapSocketAddr)
PACE_DATA16(5)
PACE_SWAP_BEGIN(4)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_DATA16(3)
PACE_SWAP_BEGIN(2)
PACE_SWAP16
PACE_SWAP16
PACE_SWAP32
PACE_SWAP_END
PACE_DATA16(7)
PACE_END8

PACE_BEGIN5(NetLibGetHostByName)
PACE_ADD16 //libRefNum
PACE_ADD32 //nameP
PACE_ADD32 //bufP
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_TRAPPTR(netLibTrapGetHostByName)
PACE_DATA16(4)
PACE_SWAP_BEGIN(2)
PACE_SWAP32 // nameP
PACE_SWAP32 // nameAliasesP
PACE_SWAP16 // addrType
PACE_SWAP16 // addrLen
PACE_SWAP32 // addrListP
PACE_SWAP_SKIP(128) // name[netDNSMaxDomainName+1]
PACE_SWAP_SKIP(128) // name[netDNSMaxDomainName+1]
PACE_SWAP32 // aliasList[0]
PACE_SWAP32 // aliasList[1]
PACE_SWAP_SKIP(128) // aliases[0][netDNSMaxDomainName+1]
PACE_SWAP_SKIP(128) // aliases[0][anetDNSMaxDomainName+1]
PACE_SWAP32 // addressList[0]
PACE_SWAP32 // addressList[1]
PACE_SWAP32 // addressList[2]
PACE_SWAP32 // addressList[3]
PACE_SWAP32 // address[0]
PACE_SWAP32 // address[1]
PACE_SWAP32 // address[2]
PACE_SWAP32 // address[3]
PACE_SWAP_END
PACE_END5

PACE_BEGIN7(NetLibSelect)
PACE_ADD16 //libRefNum
PACE_ADD16 //width
PACE_ADD32 //readFDs
PACE_ADD32 //writeFDs
PACE_ADD32 //exceptFDs
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_DATA32(2)
PACE_DATA32(3)
PACE_DATA32(4)
PACE_TRAP16(netLibTrapSelect)
PACE_DATA32(2)
PACE_DATA32(3)
PACE_DATA32(4)
PACE_DATA16(6)
PACE_END7

PACE_BEGIN8(NetLibSocketOptionSet)
PACE_ADD16 //libRefnum
PACE_ADD16 //socket,
PACE_ADD16 //level
PACE_ADD16 //option
PACE_ADD32 //optValueP
PACE_ADD16 //optValueLen
PACE_ADD32 //timeout
PACE_ADD32 //errP
PACE_SWAP16DATA(4,5)
PACE_TRAP16(netLibTrapSocketOptionSet)
PACE_SWAP16DATA(4,5)
PACE_DATA16(7)
PACE_END8

PACE_BEGIN6(TxtCompare)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_SEL(intlTxtCompare)
PACE_TRAP16(sysTrapIntlDispatch)
PACE_DATA16(2)
PACE_DATA16(5)
PACE_END6

PACE_BEGIN6(TxtCaselessCompare)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_SEL(intlTxtCaselessCompare)
PACE_TRAP16(sysTrapIntlDispatch)
PACE_DATA16(2)
PACE_DATA16(5)
PACE_END6

PACE_BEGIN5(TxtTransliterate)
PACE_ADD32
PACE_ADD16
PACE_ADD32
PACE_ADD32
PACE_ADD16
PACE_DATA16(3)
PACE_SEL(intlTxtTransliterate)
PACE_TRAP16(sysTrapIntlDispatch)
PACE_DATA16(3)
PACE_END5

PACE_BEGIN1(TxtNameToEncoding)
PACE_ADD32
PACE_SEL(intlTxtNameToEncoding)
PACE_TRAP8(sysTrapIntlDispatch)
PACE_END1

PACE_BEGIN10(TxtConvertEncoding)
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_ADD32
PACE_ADD32
PACE_ADD8
PACE_ADD32
PACE_ADD16
PACE_DATA16(3)
PACE_DATA16(6)
PACE_SEL(intlTxtConvertEncoding)
PACE_TRAP16(sysTrapIntlDispatch)
PACE_DATA16(3)
PACE_DATA16(6)
PACE_END10

PACE_BEGIN1(TxtEncodingName)
PACE_ADD8
PACE_SEL(intlTxtEncodingName)
PACE_TRAPPTR(sysTrapIntlDispatch)
PACE_END1

PACE_BEGIN6(DlkGetSyncInfo)
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_ADD32
PACE_DATA32(5)
PACE_TRAP16(sysTrapDlkGetSyncInfo)
PACE_DATA32(0)
PACE_DATA32(1)
PACE_DATA32(5)
PACE_END6

PACE_BEGIN3(ExgGetDefaultApplication)
PACE_ADD32 //creatorID
PACE_ADD16 //typeId
PACE_ADD32 //dataTypesP
PACE_TRAP16(sysTrapExgGetDefaultApplication)
PACE_DATA32(0)
PACE_END3

PACE_BEGIN3(ExgSetDefaultApplication)
PACE_ADD32 //creatorID
PACE_ADD16 //typeId
PACE_ADD32 //dataTypesP
PACE_TRAP16(sysTrapExgSetDefaultApplication)
PACE_END3

PACE_BEGIN5(ExgRegisterDatatype)
PACE_ADD32 //creatorID
PACE_ADD16 //typeId
PACE_ADD32 //dataTypesP
PACE_ADD32 //descriptionsP
PACE_ADD16 //flags
PACE_TRAP16(sysTrapExgRegisterDatatype)
PACE_END5

PACE_BEGIN6(ExgGetRegisteredApplications)
PACE_ADD32 //creatorIDsP
PACE_ADD32 //numAppsP
PACE_ADD32 //namesP
PACE_ADD32 //descriptionsP
PACE_ADD16 //typeId
PACE_ADD32 //dataTypesP
PACE_TRAP16(sysTrapExgGetRegisteredApplications)
PACE_DATA32(0)
PACE_DATA32(1)
PACE_DATA32(2)
PACE_DATA32(3)
PACE_SWAP32LISTPTR32(0,1)
PACE_END6

PACE_BEGIN3(TimTimeZoneToUTC)
PACE_ADD32 // seconds
PACE_ADD16 // timeZone
PACE_ADD16 // daylightSavingAdjustment
PACE_TRAP32(sysTrapTimTimeZoneToUTC)
PACE_END3

PACE_BEGIN3(TimUTCToTimeZone)
PACE_ADD32 // seconds
PACE_ADD16 // timeZone
PACE_ADD16 // daylightSavingAdjustment
PACE_TRAP32(sysTrapTimUTCToTimeZone)
PACE_END3

#endif
