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

#ifndef __PARSER_H
#define __PARSER_H

#include "file.h"
#include "charconvert.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "dataheap.h"
#include "strtab.h"
#include "strtypes.h"
#include "hotkey.h"
#include "buffer.h"
#include "nodelookup.h"
#include "urlpart.h"

#define LANG_STRINGS_ID  FOURCC('L','N','G','S')

#define MAXTOKEN		32
#define MAXLINE			1024
#define BIGLINE			16 * 1024

#define Parser_Context(p) ((parsercontext*)Node_Context(p))

typedef struct parsercontext
{
    nodecontext Base;
	charconv* ToUTF8;
	charconv* FromUTF8;
	charconv* ToStr;
	charconv* FromStr;
	charconv* ToWcs;
	charconv* FromWcs;
	charconv* ToUtf16;
	charconv* FromUtf16;
	strtab StrTab;
	bool_t LowStartMemory;

} parsercontext;

NODE_DLL void NODE_ParserContext_Init(parsercontext* p,const nodemeta* Custom, const cc_memheap* Heap, const cc_memheap* ConstHeap);
NODE_DLL void NODE_ParserContext_Done(parsercontext* p);

NODE_DLL void NODE_ToUTF8(anynode*, char* Out,size_t OutLen, const tchar_t*);
NODE_DLL void NODE_FromUTF8(anynode*, tchar_t* Out,size_t OutLen, const char*);
NODE_DLL void NODE_ToStr(anynode*, char* Out,size_t OutLen, const tchar_t*);
NODE_DLL void NODE_FromStr(anynode*, tchar_t* Out,size_t OutLen, const char*);
NODE_DLL void NODE_ToWcs(anynode*, wchar_t* Out,size_t OutLen, const tchar_t*);
NODE_DLL void NODE_FromWcs(anynode*, tchar_t* Out,size_t OutLen, const wchar_t*);
NODE_DLL void NODE_ToUTF16(anynode*, utf16_t* Out,size_t OutLen, const tchar_t*);
NODE_DLL void NODE_FromUTF16(anynode*, tchar_t* Out,size_t OutLen, const utf16_t*);

typedef bool_t (*exprmacro)(void* Cookie, const tchar_t* Name, size_t NameLen, tchar_t* Out, size_t OutLen);

typedef struct exprstate
{
    nodecontext* Context;
    node* Base;
    cc_point CoordScale;
    fourcc_t ClassId;
    const tchar_t* EnumList;
    exprmacro MacroFunc;
    void* MacroCookie;
    array NodeLookup;

} exprstate;

NODE_DLL void NODE_ExprState(exprstate* State, node*, dataid Id, dataflags Flags);

typedef err_t (*parserexpradd)(node* Node, const datadef* DataDef, exprstate* State, const tchar_t* Expr, bool_t Save);

typedef struct parser
{
    parsercontext *Context;
	stream* Stream;
	buffer Buffer;
	charconv* CC;
    charconv* EscapeCC;
    tchar_t *BigLine;
    boolmem_t Element;
    boolmem_t ElementEof;
    boolmem_t URL;
    boolmem_t OwnCC;
    boolmem_t Error;

} parser;

NODE_DLL err_t NODE_ParserStream(parser*, stream* Stream, parsercontext* Context);
NODE_DLL err_t NODE_ParserStreamXML(parser*, stream* Stream, parsercontext* Context, const tchar_t* Root, bool_t NeedRootAttribs);
NODE_DLL void NODE_ParserCC(parser*, charconv* CC, bool_t OwnCC);
NODE_DLL void NODE_ParserBOM(parser*);

NODE_DLL err_t NODE_ParserFill(parser*,size_t AdditionalNeeded); // non-blocking stream could return ERR_NEED_MORE_DATA
NODE_DLL err_t NODE_ParserFillLine(parser*); // non-blocking stream could return ERR_NEED_MORE_DATA
NODE_DLL void NODE_ParserDataFeed(parser* p,const void* Ptr,size_t Len);
NODE_DLL err_t NODE_ParserSkip(parser*, intptr_t* Skip);
NODE_DLL err_t NODE_ParserRead(parser*, void* Data, size_t Size, size_t* Readed);
NODE_DLL err_t NODE_ParserReadEx(parser*, void* Data, size_t Size, size_t* Readed, bool_t Fill);
NODE_DLL intptr_t NODE_ParserReadUntil(parser* p, tchar_t* Out, size_t OutLen, int Delimiter);
NODE_DLL void NODE_ParserSkipAfter(parser* p, int Delimiter);
NODE_DLL bool_t NODE_ParserLine(parser*, tchar_t* Out, size_t OutLen);
NODE_DLL bool_t NODE_ParserBigLine(parser*);
NODE_DLL bool_t NODE_ParserIsToken(parser*, const tchar_t* Token); // case insensitive, ascii
NODE_DLL bool_t NODE_ParserIsFormat(parser*, const tchar_t* Name, void *Value);
NODE_DLL const uint8_t* NODE_ParserPeek(parser*, size_t Len);
NODE_DLL const uint8_t* NODE_ParserPeekEx(parser*, size_t Len, bool_t Fill, err_t*);

NODE_DLL bool_t NODE_ParserIsRootElement(parser*, tchar_t* Root, size_t RootLen);
NODE_DLL bool_t NODE_ParserIsElement(parser*, tchar_t* Name, size_t NameLen);
NODE_DLL bool_t NODE_ParserIsElementNested(parser*, tchar_t* Name, size_t NameLen);
NODE_DLL bool_t NODE_ParserElementContent(parser*, tchar_t* Out, size_t OutLen);
/** Skip all the attributes of the current element and position after the '>' */
NODE_DLL void NODE_ParserElementSkip(parser* p);
NODE_DLL void NODE_ParserElementSkipNested(parser* p);

NODE_DLL bool_t NODE_ParserIsAttrib(parser*, tchar_t* Name, size_t NameLen);
NODE_DLL bool_t NODE_ParserAttribString(parser*, tchar_t* Out, size_t OutLen);
NODE_DLL bool_t NODE_ParserAttrib(parser*, void* Data, size_t Size, dataflags Flags, exprstate* State);
NODE_DLL bool_t NODE_ParserAttribData(parser*, node* Node, const datadef* DataDef, exprstate* State, parserexpradd ExprAdd, bool_t ExprSave);
NODE_DLL bool_t NODE_ParserValueData(const tchar_t* Value, node* Node, const datadef* DataDef, exprstate* State, parserexpradd ExprAdd, bool_t ExprSave);
NODE_DLL bool_t NODE_ParserAttribLangStr(parser* p, parsercontext* Context, fourcc_t Class, dataid Id);
NODE_DLL void NODE_ParserAttribSkip(parser*);


NODE_DLL void NODE_ParserHTMLChars(parser*,tchar_t*,size_t);
NODE_DLL void NODE_ParserHTMLToURL(tchar_t*,size_t);

NODE_DLL void NODE_ParserURLToHTML(tchar_t*,size_t);

NODE_DLL void NODE_ParserImport(parser* Parser,node* Node);
NODE_DLL void NODE_ParserImportNested(parser* Parser,node* Node);

typedef struct textwriter
{
	stream* Stream;
	charconv* CC;
	bool_t SafeFormat; // use <tag></tag> instead of <tag/>

	// private
	const tchar_t* Element;
	int Deep;
	bool_t HasChild;
    bool_t InsideContent;

} textwriter;

NODE_DLL err_t NODE_TextWrite(textwriter*, const tchar_t*);
NODE_DLL err_t NODE_TextPrintf(textwriter*, const tchar_t* Msg,...)
#if defined(__GNUC__) && !defined(_MSC_VER)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
NODE_DLL void NODE_TextElementXML(parsercontext *Context, textwriter* Text, const tchar_t* Root);
NODE_DLL void NODE_TextElementBegin(textwriter* Out, textwriter* In, const tchar_t* Element);
NODE_DLL void NODE_TextElementEnd(textwriter*);
NODE_DLL void NODE_TextElementAppendData(textwriter* Text, const tchar_t *Value);
NODE_DLL void NODE_TextElementEndData(textwriter* Text, const tchar_t *Value);
NODE_DLL void NODE_TextAttrib(textwriter*, const tchar_t* Name, const void* Data, dataflags Type);
NODE_DLL void NODE_TextAttribEx(textwriter*,const tchar_t* Name, const void* Data, size_t Size, dataflags Type);
NODE_DLL void NODE_TextSerializeNode(textwriter* Text, node* p, uint_fast32_t Mask, uint_fast32_t Filter);

NODE_DLL bool_t NODE_StringToPin(pin* Data, datadef* DataDef, exprstate* State, const tchar_t** Expr);
NODE_DLL bool_t NODE_StringToNode(node** Data, exprstate* State, const tchar_t** Expr);
NODE_DLL bool_t NODE_StringToData(void* Data, size_t Size, dataflags Type, exprstate* State, const tchar_t* Value);
NODE_DLL bool_t NODE_DataToString(tchar_t* Value, size_t ValueLen, const void* Data, size_t Size, dataflags Type);
NODE_DLL bool_t NODE_PinToString(tchar_t* Value, size_t ValueLen, const pin* Data, node* Base);
NODE_DLL bool_t NODE_NodeToString(tchar_t* Value, size_t ValueLen, node* Node, node* Base);

NODE_DLL void NODE_ExprSkipSpace(const tchar_t** p);
NODE_DLL void NODE_ExprTrimSpace(tchar_t** p);
NODE_DLL bool_t NODE_ExprCmd(const tchar_t** Expr, tchar_t* Out, size_t OutLen);
NODE_DLL bool_t NODE_ExprSkipAfter(const tchar_t** p,int ch); //ch=0 for general space
NODE_DLL bool_t NODE_ExprIsToken(const tchar_t** p,const tchar_t* Name);
NODE_DLL bool_t NODE_ExprIsTokenEx(const tchar_t** p,const tchar_t* Name,...)
#if defined(__GNUC__) && !defined(_MSC_VER)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
NODE_DLL bool_t NODE_ExprIsSymbol(const tchar_t** p,int ch);
NODE_DLL bool_t NODE_ExprIsSymbol2(const tchar_t** p,int ch, int ch2);
NODE_DLL void NODE_ExprParamEnd(const tchar_t** p);
NODE_DLL bool_t NODE_ExprParamNext(const tchar_t** p);
NODE_DLL bool_t NODE_ExprIsFrac(const tchar_t** p,cc_fraction*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsInt(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsInt64(const tchar_t** p,int64_t*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsIntEx(const tchar_t** p,int size,intptr_t*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsInt64Ex(const tchar_t** p,int size,int64_t*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsHex(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsRGB(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsFourCC(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t NODE_ExprIsPoint(const tchar_t** p,cc_point*);
NODE_DLL bool_t NODE_ExprIsName(const tchar_t** p,tchar_t* Out,size_t OutLen, const tchar_t* Delimiter);
NODE_DLL size_t NODE_ExprIsBase64(const tchar_t** p,uint8_t* Out,size_t OutSize);

NODE_DLL void NODE_SimplifyFrac(cc_fraction*, int64_t Num, int64_t Den);
NODE_DLL void NODE_DoubleToFrac(cc_fraction*, int64_t v);

NODE_DLL bool_t NODE_BufferToBase64(tchar_t *Out, size_t OutLen, const uint8_t *Buffer, size_t BufferSize);
NODE_DLL uint32_t NODE_StringToIP(const tchar_t *Addr);

NODE_DLL err_t NODE_Stream_Printf(stream*, const tchar_t* Msg,...)
#if defined(__GNUC__) && !defined(_MSC_VER)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
NODE_DLL void NODE_Dump(nodecontext*,textwriter*);

#ifdef __cplusplus
}
#endif

#endif
