/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#include "charconvert.h"
#include <corec/str/str.h>

#if defined(TARGET_OSX)

//#include <iconv.h>
#include <locale.h>
#include <wchar.h>
#include <errno.h>

#ifndef ICONV_CONST
#if defined(__GLIBC__)
#define ICONV_CONST
#else
#define ICONV_CONST const
#endif
#endif

#include <CoreFoundation/CoreFoundation.h>

#define LOSSY_CHAR '.'

static char *Current = NULL;

typedef struct charconv_osx {
    CFStringBuiltInEncodings EncFrom;
	CFStringBuiltInEncodings EncTo;
} charconv_osx;

// char to char conversion
void CharConvSS(charconv* Conv, char* Out, size_t OutLen, const char* In)
{
    if (OutLen>0)
    {
        charconv_osx *CC = (charconv_osx *)Conv;
        if (CC)
        {
            CFStringRef TmpIn = CFStringCreateWithCString(NULL, In, CC->EncFrom);
            CFIndex Read;
            CFRange		r;
            r.location = 0;
            r.length = CFStringGetLength(TmpIn);
            CFStringGetBytes(TmpIn, r, CC->EncTo,
                    LOSSY_CHAR, /* no lossy conversion */
                    0, /* not external representation */
                    (UInt8*)Out, OutLen, &Read);
            CFRelease(TmpIn);
            Out[Read]=0;
        }
        else
        {
            size_t n = MIN(In?strlen(In):0,OutLen-1);
            memcpy(Out,In,n*sizeof(char));
            Out[n] = 0;
        }
    }
}

// char to utf16_t conversion
void CharConvUS(charconv* Conv, utf16_t* Out, size_t OutLen, const char* In)
{
	if (OutLen>0)
	{
        // assume wchar_t is 16 bits even if it's not true
        charconv_osx *CC = (charconv_osx *)Conv;
        if (CC)
        {
            CFStringRef TmpIn = CFStringCreateWithCString(NULL, In, CC->EncFrom);
            assert(TmpIn);
            if (TmpIn)
            {
                CFIndex Read;
                CFRange		r;
                r.location = 0;
                r.length = CFStringGetLength(TmpIn);
                CFStringGetBytes(TmpIn, r, CC->EncTo,
                        LOSSY_CHAR, /* no lossy conversion */
                        0, /* not external representation */
                        (UInt8*)Out, OutLen, &Read);
                CFRelease(TmpIn);
                memset((UInt8*)Out+Read,0,sizeof(uint16_t));
            }
            else
            {
                Out[0]=0;
            }
        }
        else
        {
            fprintf(stderr, "Not supported yet: %s with no CC\n", __FUNCTION__);
        }
    }
}

// utf16_t to char conversion
void CharConvSU(charconv* Conv, char* Out, size_t OutLen, const utf16_t* In)
{
	if (OutLen>0)
	{
        // assume wchar_t is 16 bits even if it's not true
        CFStringEncoding EncFrom,EncTo;
        if (Conv)
        {
            EncFrom = ((charconv_osx *)Conv)->EncFrom;
            EncTo = ((charconv_osx *)Conv)->EncTo;
        }
        else
        {
            EncFrom = kCFStringEncodingUTF16;
            EncTo = kCFStringEncodingUTF8;
        }

        CFStringRef TmpIn = CFStringCreateWithBytes(NULL, (const UInt8*)In, (utf16len(In)+1)*sizeof(utf16_t), EncFrom, false);
        CFIndex Read;
        CFRange		r;
        r.location = 0;
        r.length = CFStringGetLength(TmpIn);
        CFStringGetBytes(TmpIn, r, EncTo,
                LOSSY_CHAR, /* no lossy conversion */
                0, /* not external representation */
                (UInt8*)Out, OutLen, &Read);
        CFRelease(TmpIn);
        Out[Read]=0;
	}
}

void CharConvSW(charconv* UNUSED_PARAM(Conv), char* UNUSED_PARAM(Out), size_t UNUSED_PARAM(OutLen), const wchar_t* UNUSED_PARAM(In))
{
    fprintf(stderr, "Not supported yet: %s with no CC\n", __FUNCTION__);
}

void CharConvWS(charconv* UNUSED_PARAM(Conv), wchar_t* UNUSED_PARAM(Out), size_t UNUSED_PARAM(OutLen), const char* UNUSED_PARAM(In))
{
    fprintf(stderr, "Not supported yet: %s with no CC\n", __FUNCTION__);
}

void CharConvUW(charconv* UNUSED_PARAM(Conv), utf16_t* UNUSED_PARAM(Out), size_t UNUSED_PARAM(OutLen), const wchar_t* UNUSED_PARAM(In))
{
    fprintf(stderr, "Not supported yet: %s with no CC\n", __FUNCTION__);
}

void CharConvWU(charconv* UNUSED_PARAM(Conv), wchar_t* UNUSED_PARAM(Out), size_t UNUSED_PARAM(OutLen), const utf16_t* UNUSED_PARAM(In))
{
    fprintf(stderr, "Not supported yet: %s with no CC\n", __FUNCTION__);
}

// wchar_t to wchar_t conversion
void CharConvWW(charconv* Conv, wchar_t* Out, size_t OutLen, const wchar_t* In)
{
	if (OutLen>0)
	{
        // assume wchar_t is 16 bits even if it's not true
        charconv_osx *CC = (charconv_osx *)Conv;
        if (CC)
        {
            CFStringRef TmpIn = CFStringCreateWithBytes(NULL, (const UInt8*)In, (utf16len((const uint16_t*)In)+1)*sizeof(uint16_t), CC->EncFrom, false);
            assert(TmpIn);
            CFIndex Read;
            CFRange		r;
            r.location = 0;
            r.length = CFStringGetLength(TmpIn);
            CFStringGetBytes(TmpIn, r, CC->EncTo,
                    LOSSY_CHAR, /* no lossy conversion */
                    0, /* not external representation */
                    (UInt8*)Out, OutLen, &Read);
            CFRelease(TmpIn);
            memset((UInt8*)Out+Read,0,sizeof(uint16_t));
        }
        else
        {
            fprintf(stderr, "Not supported yet: %s with no CC\n", __FUNCTION__);
        }
    }
}

static NOINLINE void GetDefault(void)
{
    if (!Current)
    {
        setlocale(LC_ALL,""); // set default for all
        Current = setlocale(LC_CTYPE,"");
		if (strcmp(Current,"C")==0)
			Current = "ASCII";
        if (Current)
            Current = strrchr(Current,'.');
        if (Current)
           ++Current;
        else
           Current = "";
    }
}

static CFStringBuiltInEncodings GetEncoding(const tchar_t* From)
{
    if (!From)
		return kCFStringEncodingUTF8; // use UTF-8 internally
    else if (!From[0])
		return kCFStringEncodingASCII; // regular/default strings are ASCII
	else if (tcsicmp(From,T("ASCII"))==0)
		return kCFStringEncodingASCII;
	else if (tcsicmp(From,T("UTF-8"))==0)
		return kCFStringEncodingUTF8;
	else if (tcsicmp(From,T("UTF-16"))==0)
		return kCFStringEncodingUTF16;
	else if (tcsicmp(From,T("UTF-32"))==0)
		return kCFStringEncodingUTF32;
	else if (tcsnicmp(From,T("CP"),2)==0)
		return CFStringConvertWindowsCodepageToEncoding(atoi(From+2));

	return (CFStringBuiltInEncodings)kCFStringEncodingInvalidId;
}

charconv* CharConvOpen(const tchar_t* From, const tchar_t* To)
{
	charconv_osx TmpConv;
	charconv_osx *CC;

	TmpConv.EncFrom = GetEncoding(From);
	TmpConv.EncTo = GetEncoding(To);

	if (TmpConv.EncFrom == kCFStringEncodingInvalidId || TmpConv.EncTo == kCFStringEncodingInvalidId || TmpConv.EncFrom == TmpConv.EncTo)
		return NULL;

	CC = malloc(sizeof(*CC));
	CC->EncFrom = TmpConv.EncFrom;
	CC->EncTo = TmpConv.EncTo;
    return (charconv*)CC;
}

void CharConvClose(charconv* p)
{
    if (p)
		free(p);
}

void CharConvDefault(tchar_t* Out, size_t OutLen)
{
    GetDefault();

    tcscpy_s(Out,OutLen,Current);
}

#endif
