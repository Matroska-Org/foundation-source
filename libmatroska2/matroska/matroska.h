/*
 * $Id: matroska.h 1315 2008-08-24 14:52:42Z robux4 $
 * Copyright (c) 2008, Matroska Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Matroska Foundation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY The Matroska Foundation ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL The Matroska Foundation BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ebml/ebml.h"

#if defined(MATROSKA2_EXPORTS)
#define MATROSKA_DLL DLLEXPORT
#elif defined(MATROSKA2_IMPORTS)
#define MATROSKA_DLL DLLIMPORT
#else
#define MATROSKA_DLL
#endif

EBML_DLL err_t MATROSKA_Init(nodecontext *p);
EBML_DLL err_t MATROSKA_Done(nodecontext *p);

// EBML contexts
extern const ebml_context MATROSKA_ContextStream;

extern const ebml_context MATROSKA_ContextSegment;

extern const ebml_context MATROSKA_ContextSeekHead;
extern const ebml_context MATROSKA_ContextSegmentInfo;
extern const ebml_context MATROSKA_ContextCluster;
extern const ebml_context MATROSKA_ContextTracks;
extern const ebml_context MATROSKA_ContextCues;
extern const ebml_context MATROSKA_ContextAttachments;
extern const ebml_context MATROSKA_ContextChapters;
extern const ebml_context MATROSKA_ContextTags;

extern const ebml_context MATROSKA_ContextChapterAtom;