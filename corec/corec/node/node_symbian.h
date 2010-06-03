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

#ifndef NODE_SYMBIAN__H
#define NODE_SYMBIAN__H

/* the symbian headers are not compatible with C */
#if defined(__cplusplus)

#undef T
#undef NULL

#include <e32std.h>
#include <e32hal.h>
#include <f32file.h>
#include <flogger.h>
#include <hal.h>
#include <utf.h>
#include <eikfutil.h>
#include <mdaaudiooutputstream.h>
#include <mda\common\audio.h>

#ifdef SYMBIAN90
#include <CommDB.h>
#endif

#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikmenup.h>
#include <eikon.hrh>
#include <eikmenub.h>
#include <eikedwin.h>
#include <eiktbar.h>

#if !defined(SERIES90)
#include <eikspane.h>
#endif

#if defined(SERIES60)
#include <aknapp.h>
#include <aknenv.h>
#include <eikdoc.h>
#include <stdio.h>
#include <aknnotewrappers.h>
#include <aknindicatorcontainer.h>

#define CAppUi CAknAppUi
#define CDocument CEikDocument
typedef CAknApplication CApplication;

#elif defined(SERIES80) || defined(SERIES90)
#include <eikenv.h> 
#include <eikappui.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikbtgpc.h> 

#define CAppUi CEikAppUi
#define CDocument CEikDocument
typedef CEikApplication CApplication;

#elif defined(UIQ)
#include <qikappui.h>
#include <qikapplication.h>
#include <qikdocument.h>
#if defined(SYMBIAN90)
#include <QikViewBase.h>
#include <QikCommand.h>
#endif

#define CAppUi CQikAppUi 
#define CDocument CQikDocument
typedef CQikApplication CApplication;

#else
#error Unknown Symbian SDK
#endif

#ifdef UNICODE
#define T(a) L ## a
#else
#define T(a) a
#endif

#ifndef SYMBIAN90
static INLINE int64_t FromInt64(const TInt64& i)
{
	return i.Low() | ((int64_t)i.High() << 32);
}
static INLINE TInt64 ToInt64(int64_t i)
{
    return TInt64((int32_t)(i>>32),(uint32_t)i);
}
#else
#define FromInt64(a) (a)
#define ToInt64(a) (a)
#endif

#endif /* __cplusplus */

#endif /* NODE_SYMBIAN__H */
