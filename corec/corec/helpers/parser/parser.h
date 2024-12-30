/*****************************************************************************
 *
 * Copyright (c) 2008-2010, CoreCodec, Inc.
 * Copyright (C) CoreCodec, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 ****************************************************************************/

#ifndef __PARSER_H
#define __PARSER_H

#include <corec/node/node.h>
#include <corec/array/array.h>

#ifdef __cplusplus
extern "C" {
#endif


#define LANG_STRINGS_ID  FOURCC('L','N','G','S')

#define MAXTOKEN		32
#define MAXLINE			1024
#define BIGLINE			16 * 1024

typedef struct strtab
{
    const struct cc_memheap* Heap;
    array Table;

} strtab;

typedef struct parsercontext
{
    nodecontext Base;
    struct charconv* ToUTF8;
    struct charconv* FromUTF8;
    struct charconv* ToStr;
    struct charconv* FromStr;
    struct charconv* ToWcs;
    struct charconv* FromWcs;
    struct charconv* ToUtf16;
    struct charconv* FromUtf16;
	strtab StrTab;

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

typedef struct stream stream;

typedef struct textwriter
{
	stream* Stream;
    struct charconv* CC;
	bool_t SafeFormat; // use <tag></tag> instead of <tag/>

	// private
	const tchar_t* Element;
	int Deep;
	bool_t HasChild;
    bool_t InsideContent;

} textwriter;

NODE_DLL err_t TextWrite(textwriter*, const tchar_t*);
NODE_DLL err_t TextPrintf(textwriter*, const tchar_t* Msg,...)
#if defined(__GNUC__) && !defined(_MSC_VER) && !defined(UNICODE)
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

NODE_DLL bool_t DataToString(tchar_t* Value, size_t ValueLen, const void* Data, size_t Size, dataflags Type);
NODE_DLL bool_t NodeToString(tchar_t* Value, size_t ValueLen, node* Node, node* Base);

void ExprSkipSpace(const tchar_t** p);
NODE_DLL void ExprTrimSpace(tchar_t** p);
NODE_DLL bool_t ExprSkipAfter(const tchar_t** p,int ch); //ch=0 for general space
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
NODE_DLL bool_t ExprIsName(const tchar_t** p,tchar_t* Out,size_t OutLen, const tchar_t* Delimiter);
NODE_DLL size_t ExprIsBase64(const tchar_t** p,uint8_t* Out,size_t OutSize);

NODE_DLL void SimplifyFrac(cc_fraction*, int64_t Num, int64_t Den);
NODE_DLL void DoubleToFrac(cc_fraction*, int64_t v);

NODE_DLL bool_t BufferToBase64(tchar_t *Out, size_t OutLen, const uint8_t *Buffer, size_t BufferSize);

NODE_DLL err_t Stream_Printf(stream*, const tchar_t* Msg,...)
#if defined(__GNUC__) && !defined(_MSC_VER) && !defined(UNICODE)
    __attribute__ ((format(printf, 2, 3)))
#endif
    ;
NODE_DLL void NodeDump(nodecontext*,textwriter*);

#ifdef __cplusplus
}
#endif

#endif
