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

#include "corec/helpers/file/file.h"
#include "corec/helpers/charconvert/charconvert.h"

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

NODE_DLL void ParserContext_Init(parsercontext* p,const nodemeta* Custom, const cc_memheap* Heap, const cc_memheap* ConstHeap);
NODE_DLL void ParserContext_Done(parsercontext* p);

NODE_DLL void Node_ToUTF8(anynode*, char* Out,size_t OutLen, const tchar_t*);
NODE_DLL void Node_FromUTF8(anynode*, tchar_t* Out,size_t OutLen, const char*);
NODE_DLL void Node_ToStr(anynode*, char* Out,size_t OutLen, const tchar_t*);
NODE_DLL void Node_FromStr(anynode*, tchar_t* Out,size_t OutLen, const char*);
NODE_DLL void Node_ToWcs(anynode*, wchar_t* Out,size_t OutLen, const tchar_t*);
NODE_DLL void Node_FromWcs(anynode*, tchar_t* Out,size_t OutLen, const wchar_t*);
NODE_DLL void Node_ToUTF16(anynode*, utf16_t* Out,size_t OutLen, const tchar_t*);
NODE_DLL void Node_FromUTF16(anynode*, tchar_t* Out,size_t OutLen, const utf16_t*);

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

NODE_DLL void ExprState(exprstate* State, node*, dataid Id, dataflags Flags);

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

NODE_DLL err_t ParserStream(parser*, stream* Stream, parsercontext* Context);
NODE_DLL err_t ParserStreamXML(parser*, stream* Stream, parsercontext* Context, const tchar_t* Root, bool_t NeedRootAttribs);
NODE_DLL void ParserCC(parser*, charconv* CC, bool_t OwnCC);
NODE_DLL void ParserBOM(parser*);

NODE_DLL err_t ParserFill(parser*,size_t AdditionalNeeded); // non-blocking stream could return ERR_NEED_MORE_DATA
NODE_DLL err_t ParserFillLine(parser*); // non-blocking stream could return ERR_NEED_MORE_DATA
NODE_DLL void ParserDataFeed(parser* p,const void* Ptr,size_t Len);
NODE_DLL err_t ParserSkip(parser*, intptr_t* Skip);
NODE_DLL err_t ParserRead(parser*, void* Data, size_t Size, size_t* Readed);
NODE_DLL err_t ParserReadEx(parser*, void* Data, size_t Size, size_t* Readed, bool_t Fill);
NODE_DLL intptr_t ParserReadUntil(parser* p, tchar_t* Out, size_t OutLen, int Delimiter);
NODE_DLL void ParserSkipAfter(parser* p, int Delimiter);
NODE_DLL bool_t ParserLine(parser*, tchar_t* Out, size_t OutLen);
NODE_DLL bool_t ParserBigLine(parser*);
NODE_DLL bool_t ParserIsToken(parser*, const tchar_t* Token); // case insensitive, ascii
NODE_DLL bool_t ParserIsFormat(parser*, const tchar_t* Name, void *Value);
NODE_DLL const uint8_t* ParserPeek(parser*, size_t Len);
NODE_DLL const uint8_t* ParserPeekEx(parser*, size_t Len, bool_t Fill, err_t*);

NODE_DLL bool_t ParserIsRootElement(parser*, tchar_t* Root, size_t RootLen);
NODE_DLL bool_t ParserIsElement(parser*, tchar_t* Name, size_t NameLen);
NODE_DLL bool_t ParserIsElementNested(parser*, tchar_t* Name, size_t NameLen);
NODE_DLL bool_t ParserElementContent(parser*, tchar_t* Out, size_t OutLen);
NODE_DLL void ParserElementSkip(parser* p);
NODE_DLL void ParserElementSkipNested(parser* p);

NODE_DLL bool_t ParserIsAttrib(parser*, tchar_t* Name, size_t NameLen);
NODE_DLL bool_t ParserAttribString(parser*, tchar_t* Out, size_t OutLen);
NODE_DLL bool_t ParserAttrib(parser*, void* Data, size_t Size, dataflags Flags, exprstate* State);
NODE_DLL bool_t ParserAttribData(parser*, node* Node, const datadef* DataDef, exprstate* State, parserexpradd ExprAdd, bool_t ExprSave);
NODE_DLL bool_t ParserValueData(const tchar_t* Value, node* Node, const datadef* DataDef, exprstate* State, parserexpradd ExprAdd, bool_t ExprSave);
NODE_DLL bool_t ParserAttribLangStr(parser* p, parsercontext* Context, fourcc_t Class, dataid Id);
NODE_DLL void ParserAttribSkip(parser*);


NODE_DLL void ParserHTMLChars(parser*,tchar_t*,size_t);
NODE_DLL void ParserHTMLToURL(tchar_t*,size_t);

NODE_DLL void ParserURLToHTML(tchar_t*,size_t);

NODE_DLL void ParserImport(parser* Parser,node* Node);
NODE_DLL void ParserImportNested(parser* Parser,node* Node);

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

NODE_DLL err_t TextWrite(textwriter*, const tchar_t*);
NODE_DLL err_t TextPrintf(textwriter*, const tchar_t* Msg,...)
#if defined(__GNUC__)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
NODE_DLL void TextElementXML(parsercontext *Context, textwriter* Text, const tchar_t* Root);
NODE_DLL void TextElementBegin(textwriter* Out, textwriter* In, const tchar_t* Element);
NODE_DLL void TextElementEnd(textwriter*);
NODE_DLL void TextElementAppendData(textwriter* Text, const tchar_t *Value);
NODE_DLL void TextElementEndData(textwriter* Text, const tchar_t *Value);
NODE_DLL void TextAttrib(textwriter*, const tchar_t* Name, const void* Data, dataflags Type);
NODE_DLL void TextAttribEx(textwriter*,const tchar_t* Name, const void* Data, size_t Size, dataflags Type);
NODE_DLL void TextSerializeNode(textwriter* Text, node* p, uint_fast32_t Mask, uint_fast32_t Filter);

NODE_DLL bool_t StringToPin(pin* Data, datadef* DataDef, exprstate* State, const tchar_t** Expr);
NODE_DLL bool_t StringToNode(node** Data, exprstate* State, const tchar_t** Expr);
NODE_DLL bool_t StringToData(void* Data, size_t Size, dataflags Type, exprstate* State, const tchar_t* Value);
NODE_DLL bool_t DataToString(tchar_t* Value, size_t ValueLen, const void* Data, size_t Size, dataflags Type);
NODE_DLL bool_t PinToString(tchar_t* Value, size_t ValueLen, const pin* Data, node* Base);
NODE_DLL bool_t NodeToString(tchar_t* Value, size_t ValueLen, node* Node, node* Base);

NODE_DLL void ExprSkipSpace(const tchar_t** p);
NODE_DLL void ExprTrimSpace(tchar_t** p);
NODE_DLL bool_t ExprCmd(const tchar_t** Expr, tchar_t* Out, size_t OutLen);
NODE_DLL bool_t ExprSkipAfter(const tchar_t** p,int ch); //ch=0 for general space
NODE_DLL bool_t ExprIsToken(const tchar_t** p,const tchar_t* Name);
NODE_DLL bool_t ExprIsTokenEx(const tchar_t** p,const tchar_t* Name,...)
#if defined(__GNUC__)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
NODE_DLL bool_t ExprIsSymbol(const tchar_t** p,int ch);
NODE_DLL bool_t ExprIsSymbol2(const tchar_t** p,int ch, int ch2);
NODE_DLL void ExprParamEnd(const tchar_t** p);
NODE_DLL bool_t ExprParamNext(const tchar_t** p);
NODE_DLL bool_t ExprIsFrac(const tchar_t** p,cc_fraction*); // no space skipping!
NODE_DLL bool_t ExprIsInt(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t ExprIsInt64(const tchar_t** p,int64_t*); // no space skipping!
NODE_DLL bool_t ExprIsIntEx(const tchar_t** p,int size,intptr_t*); // no space skipping!
NODE_DLL bool_t ExprIsInt64Ex(const tchar_t** p,int size,int64_t*); // no space skipping!
NODE_DLL bool_t ExprIsHex(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t ExprIsRGB(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t ExprIsFourCC(const tchar_t** p,intptr_t*); // no space skipping!
NODE_DLL bool_t ExprIsPoint(const tchar_t** p,cc_point*);
NODE_DLL bool_t ExprIsName(const tchar_t** p,tchar_t* Out,size_t OutLen, const tchar_t* Delimiter);
NODE_DLL size_t ExprIsBase64(const tchar_t** p,uint8_t* Out,size_t OutSize);

NODE_DLL void SimplifyFrac(cc_fraction*, int64_t Num, int64_t Den);
NODE_DLL void DoubleToFrac(cc_fraction*, int64_t v);

NODE_DLL bool_t BufferToBase64(tchar_t *Out, size_t OutLen, const uint8_t *Buffer, size_t BufferSize);
NODE_DLL uint32_t StringToIP(const tchar_t *Addr);

NODE_DLL err_t Stream_Printf(stream*, const tchar_t* Msg,...)
#if defined(__GNUC__)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
NODE_DLL void NodeDump(nodecontext*,textwriter*);

#ifdef __cplusplus
}
#endif

#endif
