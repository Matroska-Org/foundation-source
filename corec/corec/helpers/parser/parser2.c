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

#include "parser.h"
#include "corec/corec_exports.h"

static const tchar_t* ExternalStr(nodecontext* p,fourcc_t Class,int Id)
{
    return StrTab_Find(&((parsercontext*)p)->StrTab,Class,Id);
}

void NODE_ParserContext_Init(parsercontext* p,const nodemeta* Custom, const cc_memheap* Heap, const cc_memheap* ConstHeap)
{
    NodeContext_Init(&p->Base,Custom,Heap,ConstHeap);
    p->Base.ExternalStr = ExternalStr;
    StrTab_Init(&p->StrTab,p->Base.NodeConstHeap,5120);
	p->ToUTF8    = CharConvOpen(NULL,CHARSET_UTF8);
	p->FromUTF8  = CharConvOpen(CHARSET_UTF8,NULL);
    p->ToStr     = CharConvOpen(NULL,CHARSET_DEFAULT);
    p->FromStr   = CharConvOpen(CHARSET_DEFAULT,NULL);
	p->ToWcs     = CharConvOpen(NULL,CHARSET_WCHAR);
	p->FromWcs   = CharConvOpen(CHARSET_WCHAR,NULL);
	p->ToUtf16   = CharConvOpen(NULL,CHARSET_UTF16);
	p->FromUtf16 = CharConvOpen(CHARSET_UTF16,NULL);
}

void NODE_ParserContext_Done(parsercontext* p)
{
	CharConvClose(p->ToUTF8);
	CharConvClose(p->FromUTF8);
	CharConvClose(p->ToStr);
	CharConvClose(p->FromStr);
	CharConvClose(p->ToWcs);
	CharConvClose(p->FromWcs);
	CharConvClose(p->ToUtf16);
	CharConvClose(p->FromUtf16);
    p->ToUTF8 = NULL;
    p->FromUTF8 = NULL;
    p->ToStr = NULL;
    p->FromStr = NULL;
    p->ToWcs = NULL;
    p->FromWcs = NULL;
    p->ToUtf16 = NULL;
    p->FromUtf16 = NULL;
    StrTab_Done(&p->StrTab);
    NodeContext_Done(&p->Base);
}

void NODE_ToUTF8(anynode* p, char* Out,size_t OutLen, const tchar_t* In)
{
    CharConvST(Parser_Context(p)->ToUTF8,Out,OutLen,In);
}

void NODE_FromUTF8(anynode* p, tchar_t* Out,size_t OutLen, const char* In)
{
    CharConvTS(Parser_Context(p)->FromUTF8,Out,OutLen,In);
}

void NODE_ToStr(anynode* p, char* Out,size_t OutLen, const tchar_t* In)
{
    CharConvST(Parser_Context(p)->ToStr,Out,OutLen,In);
}

void NODE_FromStr(anynode* p, tchar_t* Out,size_t OutLen, const char* In)
{
    CharConvTS(Parser_Context(p)->FromStr,Out,OutLen,In);
}

void NODE_ToWcs(anynode* p, wchar_t* Out,size_t OutLen, const tchar_t* In)
{
    CharConvWT(Parser_Context(p)->ToWcs,Out,OutLen,In);
}

void NODE_FromWcs(anynode* p, tchar_t* Out,size_t OutLen, const wchar_t* In)
{
    CharConvTW(Parser_Context(p)->FromWcs,Out,OutLen,In);
}

void NODE_ToUTF16(anynode* p, utf16_t* Out,size_t OutLen, const tchar_t* In)
{
    CharConvUT(Parser_Context(p)->ToUtf16,Out,OutLen,In);
}

void NODE_FromUTF16(anynode* p, tchar_t* Out,size_t OutLen, const utf16_t* In)
{
    CharConvTU(Parser_Context(p)->FromUtf16,Out,OutLen,In);
}

void NODE_ParserDataFeed(parser* p,const void* Ptr,size_t Len)
{
	NODE_BufferWrite(&p->Buffer,Ptr,Len,4096);
}

NOINLINE err_t NODE_ParserFill(parser* p,size_t Needed)
{
    // Needed may be zero (see http.c EnumDir())

    // pack buffer if more then half is done
	if (p->Buffer.Read > p->Buffer.Begin + (p->Buffer.End - p->Buffer.Begin)/2)
		NODE_BufferPack(&p->Buffer,0);

	for (;;)
    {
        size_t Readed,n;
        err_t Err;

        n = p->Buffer.End - p->Buffer.Write;
	    if (n==0 || !p->Stream)
            return (Needed>0)?ERR_NEED_MORE_DATA:ERR_NONE;

        Err = Stream_ReadOneOrMore(p->Stream,p->Buffer.Write,n,&Readed);
        if (!Readed)
            return Err;

		p->Buffer.Write += Readed;

        if (Readed >= Needed)
            break;

        Needed -= Readed;
    }

    return ERR_NONE;
}

NOINLINE err_t NODE_ParserFillLine(parser* p)
{
    err_t Err;
    do
    {
        const uint8_t* i;
        for (i=p->Buffer.Read;i!=p->Buffer.Write;++i)
            if (*i == '\n')
                return ERR_NONE;

        Err = NODE_ParserFill(p,1);

    } while (Err==ERR_NONE);
    return Err;
}

void NODE_ParserBOM(parser* p)
{
    //TODO: use BOM detection in more places (playlist,xml,...)
    //TODO: support 16bit LE/BE encodings
    const uint8_t* BOM = NODE_ParserPeek(p,3);
    if (BOM)
    {
        if (BOM[0]==0xEF && BOM[1]==0xBB && BOM[2]==0xBF)
        {
            intptr_t Skip = 3;
            NODE_ParserSkip(p,&Skip);
            if (p->Context)
                NODE_ParserCC(p,p->Context->FromUTF8,0);
        }
    }
}

NOINLINE void NODE_ParserCC(parser* p, charconv* CC, bool_t OwnCC)
{
    if (p->CC && p->OwnCC)
        CharConvClose(p->CC);
    p->CC = CC;
    p->OwnCC = (boolmem_t)OwnCC;
}

err_t NODE_ParserStream(parser* p, stream* Stream, parsercontext* Context)
{
    NODE_ParserCC(p,Context ? Context->FromStr:NULL, 0);

	p->Stream = Stream;
    p->Element = 0;
    p->ElementEof = 0;
    p->Error = 0;
    p->URL = 0;
    p->Context = Context;

	if (Stream)
	{
		if (!p->Buffer.Begin)
			if (!NODE_BufferAlloc(&p->Buffer,4096,1))
                return ERR_OUT_OF_MEMORY;
	}
    else {
		NODE_BufferClear(&p->Buffer);
        if (p->BigLine) {
            free(p->BigLine);
            p->BigLine = NULL;
        }
    }
    return ERR_NONE;
}

err_t NODE_ParserSkip(parser* p, intptr_t* Skip)
{
    intptr_t n = min(*Skip,p->Buffer.Write - p->Buffer.Read);
    if (n>0)
    {
        *Skip -= n;
        p->Buffer.Read += n;
    }
    return Stream_Skip(p->Stream,Skip);
}

err_t NODE_ParserReadEx(parser* p, void* Data, size_t Size, size_t* Readed, bool_t Fill)
{
    if (!Fill)
    {
        size_t n = 0;
        if (p->Buffer.Write > p->Buffer.Read)
        {
            n = min(Size,(size_t)(p->Buffer.Write-p->Buffer.Read));
            memcpy(Data,p->Buffer.Read,n);
            p->Buffer.Read += n;
            Size -= n;
        }
        if (Readed)
            *Readed = n;
        return !Size ? ERR_NONE:ERR_NEED_MORE_DATA;
    }
    return NODE_ParserRead(p,Data,Size,Readed);
}

err_t NODE_ParserRead(parser* p, void* Data, size_t Size, size_t* Readed)
{
    if (p->Buffer.Write > p->Buffer.Read)
    {
        err_t Err = ERR_NONE;
        size_t n = min(Size,(size_t)(p->Buffer.Write-p->Buffer.Read));
        memcpy(Data,p->Buffer.Read,n);
        p->Buffer.Read += n;
        Size -= n;
        if (Size>0)
        {
            Err = Stream_Read(p->Stream,(uint8_t*)Data+n,Size,&Size);
            n += Size;
        }
        if (Readed)
            *Readed = n;
        return Err;
    }

    return Stream_Read(p->Stream,Data,Size,Readed);
}

const uint8_t* NODE_ParserPeek(parser* p, size_t Len)
{
	if (p->Buffer.Write < p->Buffer.Read+Len)
	{
		NODE_ParserFill(p,p->Buffer.Read+Len-p->Buffer.Write);
		if (p->Buffer.Write < p->Buffer.Read+Len)
			return NULL;
	}
	return p->Buffer.Read;
}

const uint8_t* NODE_ParserPeekEx(parser* p, size_t Len, bool_t Fill, err_t* Err)
{
	if (p->Buffer.Write < p->Buffer.Read+Len)
	{
        if (!Fill)
        {
            *Err = ERR_NEED_MORE_DATA;
            return NULL;
        }
		*Err = NODE_ParserFill(p,p->Buffer.Read+Len-p->Buffer.Write);
		if (p->Buffer.Write < p->Buffer.Read+Len)
			return NULL;
	}
    *Err = ERR_NONE;
	return p->Buffer.Read;
}

#define PARSER_BEGIN(p) \
    uint8_t* Read = p->Buffer.Read; \
    uint8_t* Write = p->Buffer.Write; \

#define PARSER_RESTART(p) \
    Read = p->Buffer.Read; \
    Write = p->Buffer.Write; \

#define PARSER_SAVE(p) \
    p->Buffer.Read = Read; 

#define PARSER_FILL(p) \
    PARSER_SAVE(p) \
    NODE_ParserFill(p,1); \
    PARSER_RESTART(p)

static bool_t SkipAfter(parser* p, int ch)
{
    PARSER_BEGIN(p)
    for (;;++Read)
    {
        if (Read>=Write)
        {
            PARSER_FILL(p)
            if (Read>=Write)
                return 0;
        }

        if (*Read==(uint8_t)ch)
            break;
    }
    ++Read;
    PARSER_SAVE(p);
    return 1;
}

bool_t NODE_ParserIsToken(parser* p, const tchar_t* Token)
{
    PARSER_BEGIN(p)
    // space skipping
    for (;;++Read)
    {
        if (Read>=Write)
        {
            PARSER_FILL(p)
            if (Read>=Write)
                return 0;
        }
        if (*Read!=' ' && *Read!=9 && *Read!=10 && *Read!=13)
            break;
    }
    PARSER_SAVE(p)
    for (;*Token;++Read,++Token)
    {
        if (Read>=Write)
        {
            PARSER_FILL(p)
            if (Read>=Write)
                return 0;
        }
        if (*Read!=(uint8_t)*Token)
            return 0;
    }

    PARSER_SAVE(p)
    return 1;
}

NOINLINE bool_t NODE_ParserIsFormat(parser* p,const tchar_t* Token, void *Value)
{
    size_t i,j=0;
    tchar_t tBuffer[MAXDATA];

    PARSER_BEGIN(p)
    if (!*Token || *Token!='%')
        return 0;

    // space skipping
    for (;;++Read)
    {
        if (Read>=Write)
        {
            PARSER_FILL(p)
            if (Read>=Write)
                return 0;
        }
        if (*Read!=' ' && *Read!=9 && *Read!=10 && *Read!=13)
            break;
    }
    PARSER_SAVE(p)

    for (;j<TSIZEOF(tBuffer);++Read)
    {
        if (Read>=Write)
        {
            PARSER_FILL(p)
            if (Read>=Write)
                return 0;
        }
        tBuffer[j] = (tchar_t)p->Buffer.Read[j++];
        i=j;
        if (!stscanf_s(tBuffer,&i,Token,Value))
            break;
        if (i != j)
            break;
    }
    if (j>1) // recover the last working config
    {
        --j;
        stscanf_s(tBuffer,&j,Token,Value);
    }

    PARSER_SAVE(p)
    return (j!=0);
}

static bool_t IsToken(parser* p, const tchar_t* Token)
{
    PARSER_BEGIN(p)
    for (;*Token;++Read,++Token)
    {
        if (Read>=Write)
        {
            PARSER_FILL(p)
            if (Read>=Write)
                return 0;
        }
        if (*Read!=(uint8_t)*Token)
            return 0;
    }
    PARSER_SAVE(p)
    return 1;
}

typedef struct htmlchar
{
	uint8_t ch;
	tchar_t sym[6+1];

} htmlchar;

static const htmlchar HTMLChar[] =
{
	{34,T("quot")},
	{38,T("amp")},	
    {39,T("apos")},
	{60,T("lt")},
	{62,T("gt")},
	{160,T("nbsp")},
	{161,T("iexcl")},
	{162,T("cent")},
	{163,T("pound")},
	{164,T("curren")},
	{165,T("yen")},
	{166,T("brvbar")},
	{167,T("sect")},
	{168,T("uml")},
	{169,T("copy")},
	{170,T("ordf")},
	{171,T("laquo")},
	{172,T("not")},
	{173,T("shy")},
	{174,T("reg")},
	{175,T("hibar")},
	{176,T("deg")},
	{177,T("plusmn")},
	{185,T("sup1")},
	{178,T("sup2")},
	{179,T("sup3")},
	{180,T("acute")},
	{181,T("micro")},
	{182,T("para")},
	{183,T("middot")},
	{184,T("cedil")},
	{186,T("ordm")},
	{187,T("raquo")},
	{188,T("frac14")},
	{189,T("frac12")},
	{190,T("frac34")},
	{191,T("iquest")},
	{192,T("Agrave")},
	{193,T("Aacute")},
	{194,T("Acirc")},
	{195,T("Atilde")},
	{196,T("Auml")},
	{197,T("Aring")},
	{198,T("AElig")},
	{199,T("Ccedil")},
	{200,T("Egrave")},
	{201,T("Eacute")},
	{202,T("Ecirc")},
	{203,T("Euml")},
	{204,T("Igrave")},
	{205,T("Iacute")},
	{206,T("Icirc")},
	{207,T("Iuml")},
	{208,T("ETH")},
	{209,T("Ntilde")},
	{210,T("Ograve")},
	{211,T("Oacute")},
	{212,T("Ocirc")},
	{213,T("Otilde")},
	{214,T("Ouml")},
	{215,T("times")},
	{216,T("Oslash")},
	{217,T("Ugrave")},
	{218,T("Uacute")},
	{219,T("Ucirc")},
	{220,T("Uuml")},
	{221,T("Yacute")},
	{222,T("THORN")},
	{223,T("szlig")},
	{224,T("agrave")},
	{225,T("aacute")},
	{226,T("acirc")},
	{227,T("atilde")},
	{228,T("auml")},
	{229,T("aring")},
	{230,T("aelig")},
	{231,T("ccedil")},
	{232,T("egrave")},
	{233,T("eacute")},
	{234,T("ecirc")},
	{235,T("euml")},
	{236,T("igrave")},
	{237,T("iacute")},
	{238,T("icirc")},
	{239,T("iuml")},
	{240,T("eth")},
	{241,T("ntilde")},
	{242,T("ograve")},
	{243,T("oacute")},
	{244,T("ocirc")},
	{245,T("otilde")},
	{246,T("ouml")},
	{247,T("divide")},
	{248,T("oslash")},
	{249,T("ugrave")},
	{250,T("uacute")},
	{251,T("ucirc")},
	{252,T("uuml")},
	{253,T("yacute")},
	{254,T("thorn")},
	{255,T("yuml")},
	{0,T("")}
};

void NODE_ParserHTMLChars(parser *p, tchar_t *Out, size_t OutLen)
{
	const tchar_t* i;
    utf16_t ch;
    utf16_t UChar[2];
    UChar[1] = 0;
	for (;*Out;++Out,--OutLen)
    {
		if (*Out==T('&') && (i=tcschr(Out,T(';')))!=NULL)
		{
            ch = 0;
            if (Out[1]=='#')
            {
                if (IsDigit(Out[2])) // decimal encoding
                {
                    const tchar_t* s;
                    for (s = Out+2;s<i;++s)
                        ch = (utf16_t)(ch*10 + (*s - '0'));
                }
                else if (Out[2] == 'x' || Out[2] == 'X') // hexadecimal encoding
                {
                    const tchar_t* s;
                    for (s = Out+3;s<i;++s)
                        ch = (utf16_t)(ch*16 + Hex(*s));
                }
            }
            else
            {
			    const htmlchar* c;
			    size_t n = i-Out-1;
			    for (c=HTMLChar;c->ch;++c)
				    if (tcsnicmp(c->sym,Out+1,n)==0 && c->sym[n]==0)
				    {
                        ch = c->ch;
					    break;
				    }
            }

            if (ch)
            {
                size_t Index=0;
                UChar[0] = ch;
                NODE_FromUTF16(p->Context,Out,OutLen,UChar);
                for (Index=0;Index<OutLen && Out[Index];++Index) {}
			    memmove(Out+Index,i+1,sizeof(tchar_t)*tcslen(i));
                Out += Index - 1; // -1 for the ++ of the loop
                OutLen -= Index - 1;
            }
		}
    }
}

void NODE_ParserHTMLToURL(tchar_t* URL, size_t OutLen)
{
	for (;*URL && OutLen>0;++URL,--OutLen)
		if (URL[0]=='%' && Hex(URL[1])>=0 && Hex(URL[2])>=0)
		{
			*URL = (tchar_t)((Hex(URL[1])<<4)+Hex(URL[2]));
			memmove(URL+1,URL+3,sizeof(tchar_t)*(tcslen(URL+3)+1));
		}
}

void NODE_ParserURLToHTML(tchar_t* p,size_t n)
{
	for (;*p && n>0;++p,--n)
		if (*p == ' ' && n>=4)
		{
            size_t i = min(n-4,tcslen(p+1));
			memmove(p+3,p+1,i*sizeof(tchar_t));
            p[3+i] = 0;

			p[0] = '%';
            p[1] = '2';
            p[2] = '0';
		}
}

void NODE_ParserSkipAfter(parser* p, int Delimiter)
{
    tchar_t Del[2];
    NODE_ParserReadUntil(p,NULL,0,Delimiter);
    Del[0] = (tchar_t)Delimiter;
    Del[1] = 0;
    IsToken(p,Del);
}

intptr_t NODE_ParserReadUntil(parser* p, tchar_t* Out, size_t OutLen, int Delimiter)
{
    char* s = alloca(OutLen);
    size_t n=0;

    PARSER_BEGIN(p)
    if (Delimiter == '>' || Delimiter == '=')
    {
        // ' ','\t','\n','/','>' as additional delimiters
        for (;;++Read)
        {
            if (Read>=Write)
            {
                PARSER_FILL(p)
                if (Read>=Write)
                {
                    if (!n) return -1;
                    break;
                }
            }

            if (*Read<0x40) // fast prefilter
            {
               if (*Read==(uint8_t)Delimiter || *Read==' ' || *Read=='\t' || *Read=='\n' || *Read=='/' || *Read=='>')
                    break;
            }

            if (*Read!='\r' && ++n<OutLen) // skip the \r character
                s[n-1] = *Read;
        }
    }
    else
    {
        for (;;++Read)
        {
            if (Read>=Write)
            {
                PARSER_FILL(p)
                if (Read>=Write)
                {
                    if (!n) return -1;
                    break;
                }
            }

            if (*Read==(uint8_t)Delimiter)
            {
                if (Delimiter == ']')
                {
                    bool_t Found;
                    PARSER_SAVE(p);
                    Found = NODE_ParserIsToken(p,T("]]>"));
                    PARSER_RESTART(p);
                    if (Found)
                        break;
                }
                else
                {
                    if (Delimiter != '<') // skip delimiter (except for '<')
                        ++Read;
                    break;
                }
            }

            if (*Read!='\r' && ++n<OutLen) // skip the \r character
                s[n-1] = *Read;
        }
    }
    PARSER_SAVE(p);

    if (OutLen>0)
    {
        if (!n)
            *Out = 0;
        else
        {
            s[min(n,OutLen-1)]=0;
            CharConvTS(p->CC,Out,OutLen,s);

            if (Delimiter != '\n') // not NODE_ParserLine
                NODE_ParserHTMLChars(p,Out,OutLen);
            
            if (p->URL)
                NODE_ParserHTMLToURL(Out,OutLen);
        }
    }
    return n;
}

bool_t NODE_ParserLine(parser* p, tchar_t* Out, size_t OutLen)
{
	return NODE_ParserReadUntil(p,Out,OutLen,'\n')>=0;
}

bool_t NODE_ParserBigLine(parser* p)
{
    if (!p->BigLine) {
        p->BigLine = (tchar_t *) malloc(BIGLINE * sizeof(tchar_t));
        if (!p->BigLine)
            return 0;
    }
    return NODE_ParserLine(p, p->BigLine, BIGLINE);
}

NOINLINE void NODE_ParserElementSkip(parser* p)
{
    while (NODE_ParserIsAttrib(p,NULL,0))
   		NODE_ParserAttribSkip(p);
}

NOINLINE void NODE_ParserElementSkipNested(parser* p)
{
    NODE_ParserElementSkip(p);
    while (NODE_ParserIsElementNested(p,NULL,0))
        NODE_ParserElementSkipNested(p);
}

static NOINLINE bool_t ElementStart(parser* p)
{
    for (;;)
    {
        if (!SkipAfter(p,'<'))
            break;

        if (IsToken(p,T("!--")))
        {
            do
            {
                if (!SkipAfter(p,'-'))
                    return 0;
            }
            while (!IsToken(p,T("->")));
        }
        else if (IsToken(p,T("![CDATA[")))
        {
            do
            {
                if (!SkipAfter(p,']'))
                    return 0;
            }
            while (!IsToken(p,T("]>")));
        }
        else return 1;
    }
    return 0;
}

bool_t NODE_ParserIsElementNested(parser* p, tchar_t* Name, size_t NameLen)
{
    bool_t Start = 1;

    if (p->Element)
        NODE_ParserElementSkipNested(p);

    if (p->ElementEof || !(Start = ElementStart(p)) || IsToken(p,T("/")))
    {
        if (!Start)
            p->Error = 1;
        p->ElementEof = 0;
        p->Element = 0;
    }
    else
        p->Element = (boolmem_t)(NODE_ParserReadUntil(p,Name,NameLen,'>')>0);

    return p->Element;
}

bool_t NODE_ParserIsElement(parser* p, tchar_t* Name, size_t NameLen)
{
    NODE_ParserElementSkip(p);

	if (!ElementStart(p))
    {
        p->Element = 0;
    }
    else
    {
	    if (IsToken(p,T("/")) && NameLen>0)
	    {
		    *(Name++) = '/';
		    --NameLen;
	    }

        p->Element = (boolmem_t)(NODE_ParserReadUntil(p,Name,NameLen,'>')>0);
    }

    return p->Element;
}

bool_t NODE_ParserElementContent(parser* p, tchar_t* Out, size_t OutLen)
{
	NODE_ParserElementSkip(p);
    if (NODE_ParserReadUntil(p,Out,OutLen,'<')<0)
        return 0;
    if (NODE_ParserIsToken(p,T("![CDATA[")))
    	return NODE_ParserReadUntil(p,Out,OutLen,']')>=0;
    return 1;
}

bool_t NODE_ParserIsAttrib(parser* p, tchar_t* Name, size_t NameLen)
{
    if (!p->Element)
        return 0;

    // skip spaces by NODE_ParserIsToken
    p->ElementEof = (boolmem_t)NODE_ParserIsToken(p,T("/>"));
	if (p->ElementEof || IsToken(p,T(">")) || IsToken(p,T("?>")))
        p->Element = 0;
	else
        p->Element = (boolmem_t)(NODE_ParserReadUntil(p,Name,NameLen,'=')>0);
    return p->Element;
}

bool_t NODE_ParserAttribLangStr(parser* p, parsercontext* Context, fourcc_t Class, dataid Id)
{
	tchar_t Value[MAXDATA/sizeof(tchar_t)+64];
	if (!NODE_ParserAttribString(p,Value,TSIZEOF(Value)))
		return 0;
	StrTab_Add(&Context->StrTab,1,Class,(int32_t)Id,Value);
	return 1;
}

bool_t NODE_ParserAttribString(parser* p, tchar_t* Out, size_t OutLen)
{
	int Delimiter;

    // skip spaces by NODE_ParserIsToken
	if (!NODE_ParserIsToken(p,T("=")))
		return 0;

    // skip spaces by NODE_ParserIsToken
	if (NODE_ParserIsToken(p,T("\"")))
		Delimiter = '\"';
	else if (IsToken(p,T("'")))
		Delimiter = '\'';
	else
		Delimiter = '>';

	return NODE_ParserReadUntil(p,Out,OutLen,Delimiter)>=0;
}

void NODE_ParserAttribSkip(parser* p)
{
	NODE_ParserAttribString(p,NULL,0);
}

bool_t NODE_ParserIsRootElement(parser *p, tchar_t* Root, size_t RootLen)
{
	tchar_t Token[MAXTOKEN];

	while (NODE_ParserIsElement(p,Token,TSIZEOF(Token)))
	{
		if (tcsisame_ascii(Token,T("?xml")))
		{
			while (NODE_ParserIsAttrib(p,Token,TSIZEOF(Token)))
			{
				if (tcsisame_ascii(Token,T("encoding")))
				{
					NODE_ParserAttribString(p,Token,TSIZEOF(Token));
                    NODE_ParserCC(p,CharConvOpen(Token,NULL),1);
				}
				else
					NODE_ParserAttribSkip(p);
			}
		}
		else
        if (tcsisame_ascii(Token,T("!DOCTYPE")) || Token[0]==T('?'))
        {
            NODE_ParserElementSkip(p);
        }
		else 
        {
            tcscpy_s(Root,RootLen,Token);
            return 1;
        }
	}
	return 0;
}

err_t NODE_ParserStreamXML(parser* p, stream* s, parsercontext* Context, const tchar_t* Root, bool_t NeedRootAttribs)
{
    err_t Result=ERR_NONE;
    tchar_t FoundRoot[MAXPATH];

    if (!Root || !Root[0])
        return ERR_INVALID_PARAM;
    Result = NODE_ParserStream(p,s,Context);
    if (Result==ERR_NONE)
    {
        if (Context)
            NODE_ParserCC(p,Context->FromUTF8, 0);
        if (!NODE_ParserIsRootElement(p, FoundRoot, TSIZEOF(FoundRoot)))
        {
            Result = ERR_INVALID_DATA;
        }
        else
        {
            if (tcsisame_ascii(FoundRoot,Root) && !NeedRootAttribs)
                NODE_ParserElementSkip(p);
        }
    }
    return Result;
}

static NOINLINE bool_t IsName(int ch)
{
     return ch== '_' || IsAlpha(ch) || IsDigit(ch);
}

typedef struct findpin
{
    fourcc_t ClassId;
    datadef* DataDef;
    node* Node;
    const tchar_t* Expr;

} findpin;

static bool_t FindParam(node* Base, findpin* Find, nodecontext*);

static INLINE bool_t FindChild(nodetree* i, const tchar_t* Token, findpin* Find)
{
    nodetree* Child = NodeTree_FindChild(i,Token);
    if (Child && FindParam((node*)Child,Find,Node_Context(Child)))
        return 1;
    return 0;
}

static NOINLINE bool_t ReadName(findpin* Find, tchar_t* Token, size_t TokenLen)
{
    size_t i=0;
    while (IsName((Find->Expr)[0]))
    {
        if (i+1<TokenLen)
            Token[i++] = (Find->Expr)[0];
        ++(Find->Expr);
    }
    Token[i] = 0;

    if ((Find->Expr)[0] == '.')
    {
        ++(Find->Expr);
        return 1;
    }

    return 0;
}

static NOINLINE bool_t FindParam(node* Base, findpin* Find, nodecontext* Context)
{
    const tchar_t* Expr0 = Find->Expr;
    tchar_t Token[MAXTOKEN];

    if (!ReadName(Find,Token,TSIZEOF(Token)))
    {
        if (Token[0])
        {
            // check for param
            if (Base && Find->DataDef && NodeFindDef(Base,Token,Find->DataDef))
            {
                Find->Node = Base;
                return 1;
            }
        }
        else
        {
            // check for default param
            if (Base && Find->DataDef && NodeDataDef(Base,NODE_DEFAULT_DATA,Find->DataDef))
            {
                Find->Node = Base;
                return 1;
            }

            // check for classid
            if (Base && Node_IsPartOf(Base,Find->ClassId))
            {
                Find->Node = Base;
                return 1;
            }
            Find->Expr = Expr0;
            return 0;
        }
    }

    if (!Base)
    {
        // try singleton
        Base = NodeSingleton(Context,STR_StringToFourCC(Token,1));
        if (Base && FindParam(Base,Find,Context))
            return 1;
    }
    else
    if (Node_IsPartOf(Base,NODETREE_CLASS))
    {
        if (tcsisame_ascii(Token,T("parent")))
        {
            // go parent
            Base = (node*)((nodetree*)Base)->Parent;
            if (Base && FindParam(Base,Find,Context))
                return 1;
        }
        else
        if (FindChild((nodetree*)Base,Token,Find))
            return 1;
    }

    Find->Expr = Expr0;
    return 0;
}

static NOINLINE bool_t FindPin(node* Base, findpin* Find, nodecontext* Context)
{
    // Base == NULL, when only singletons are accepted
    while (Base)
    {
        if (FindParam(Base,Find,Context))
            return 1;

        if (Node_IsPartOf(Base,NODETREE_CLASS))
            Base = (node*)((nodetree*)Base)->Parent;
        else
            Base = NULL;
    }

    // try singleton
    return FindParam(NULL,Find,Context);
}

bool_t NODE_StringToPin(pin* Data, datadef* DataDef, exprstate* State, const tchar_t** Expr)
{
    findpin Find;
    Find.ClassId = 0;
    Find.DataDef = DataDef;
    Find.Expr = *Expr;
    Find.Node = NULL;

    if (!ARRAYEMPTY(State->NodeLookup))
    {
        tchar_t Id[MAXTOKEN];
        tchar_t Token[MAXTOKEN];
        node* Node;

        Id[0]=0;
        while (ReadName(&Find,Token,TSIZEOF(Token)))
            tcscpy_s(Id,TSIZEOF(Id),Token);

        Node = NodeLookup_FindUnique(&State->NodeLookup,Id);
        if (Node)
        {
            if (NodeFindDef(Node,Token,DataDef))
            {
                Data->Id = DataDef->Id;
                Data->Node = Node;
                *Expr = Find.Expr;
                return 1;
            }
            else
            if (Node_IsPartOf(Node,NODETREE_CLASS) && FindChild((nodetree*)Node,Token,&Find))
            {
                Data->Id = DataDef->Id;
                Data->Node = Find.Node;
                *Expr = Find.Expr;
                return 1;
            }
            else
            {
                Data->Id = 0;
                Data->Node = NULL;
                return 0;
            }
        }

        Find.Expr = *Expr;
    }

    if (!FindPin(State->Base,&Find,State->Context))
    {
        if (State->Context && ARRAYEMPTY(State->NodeLookup))
        {
            bool_t Result;
            NodeLookup_AddSingletons(&State->NodeLookup,State->Context);
            Result = NODE_StringToPin(Data,DataDef,State,Expr);
            ArrayClear(&State->NodeLookup);
            return Result;
        }
        else
        {
            Data->Id = 0;
            Data->Node = NULL;
            return 0;
        }
    }

    Data->Id = DataDef->Id;
    Data->Node = Find.Node;
    *Expr = Find.Expr;
    return 1;
}

bool_t NODE_StringToNode(node** Data, exprstate* State, const tchar_t** Expr)
{
    findpin Find;
    Find.ClassId = State->ClassId?State->ClassId:NODE_CLASS;
    Find.DataDef = NULL;
    Find.Expr = *Expr;
    Find.Node = NULL;

    if (!ARRAYEMPTY(State->NodeLookup))
    {
        tchar_t Id[MAXTOKEN];
        node* Node;

        while (ReadName(&Find,Id,TSIZEOF(Id))) {}

        Node = NodeLookup_FindUnique(&State->NodeLookup,Id);
        if (Node)
        {
            if (Node_IsPartOf(Node,Find.ClassId))
            {
                *Data = Node;
                *Expr = Find.Expr;
                return 1;
            }
            else
            {
                *Data = NULL;
                return 0;
            }
        }

        Find.Expr = *Expr;
    }


    if (!FindPin(State->Base,&Find,State->Context))
    {
        *Data = NULL;
        return 0;
    }

    *Data = Find.Node;
    *Expr = Find.Expr;
    return 1;
}

bool_t NODE_PinToString(tchar_t* Value, size_t ValueLen, const pin* Data, node* Base)
{
    if (NODE_NodeToString(Value,ValueLen,Data->Node,Base) && Data->Node)
    {
        size_t n;
        if (Value[0])
            tcscat_s(Value,ValueLen,T("."));
        n = tcslen(Value);
        NodeParamName(Data->Node,Data->Id,Value+n,ValueLen-n);
    }

    return 1;
}

bool_t NODE_NodeToString(tchar_t* Value, size_t ValueLen, node* Node, node* UNUSED_PARAM(Base))
{
    Value[0]=0;

    if (Node)
    {
        if (NodeClassFlags(Node) & CFLAG_SINGLETON)
    		STR_FourCCToString(Value,ValueLen,Node_ClassId(Node));
        else
        {
            //TODO: use Base... (maybe NULL)
            if (Node_Get(Node,NODE_ID,Value,ValueLen*sizeof(tchar_t))!=ERR_NONE)
                Value[0] = 0;
        }
    }

    return 1;
}

static NOINLINE int StringToIntEx(const tchar_t* Value, dataflags Flags, exprstate* State)
{
    int v;
    if (State && State->EnumList)
    {
        v = StrListIndex(Value,State->EnumList);
        if (v>=0)
            return v;
    }

    v = STR_StringToInt(Value,-1);

    if (State)
    {
        switch (Flags & TUNIT_MASK)
        {
        case TUNIT_XCOORD:
            if (State->CoordScale.x)
                v = (v*State->CoordScale.x+(1<<15)) >> 16;
            break;

        case TUNIT_YCOORD:
            if (State->CoordScale.y)
                v = (v*State->CoordScale.y+(1<<15)) >> 16;
            break;
        }
    }

    return v;
}

NOINLINE bool_t ExprToData(void* Data, size_t* Size, dataflags Flags, exprstate* State, const tchar_t** Expr)
{
    cc_point v;

	switch (Flags & TYPE_MASK)
	{
    case TYPE_POINT:
        if (State && NODE_ExprIsPoint(Expr,&v) && *Size>=sizeof(cc_point))
        {
            *(cc_point*)Data = v;
            *Size = sizeof(cc_point);
            return 1;
        }
    case TYPE_POINT16:
        if (State && NODE_ExprIsPoint(Expr,&v) && *Size>=sizeof(cc_point16))
        {
            if ((Flags & TUNIT_MASK)==TUNIT_COORD)
            {
                if (State->CoordScale.x)
                    v.x = (v.x*State->CoordScale.x+(1<<15)) >> 16;
                if (State->CoordScale.y)
                    v.y = (v.y*State->CoordScale.y+(1<<15)) >> 16;
            }

            ((cc_point16*)Data)->x = (int16_t)v.x;
            ((cc_point16*)Data)->y = (int16_t)v.y;
            *Size = sizeof(cc_point16);
            return 1;
        }
        break;
    }

    return 0;
}

NOINLINE bool_t NODE_StringToData(void* Data, size_t Size, dataflags Flags, exprstate* State, const tchar_t* Value)
{
    datadef DataDef;
    cc_fraction f;
    size_t i;
    int a,b;

	switch (Flags & TYPE_MASK)
	{
    case TYPE_STRING:
        tcscpy_s(Data,Size/sizeof(tchar_t),Value);
        break;

    case TYPE_BINARY:
       	for (i=0;i<Size && (a=Hex(Value[i*2]))>=0 && (b=Hex(Value[i*2+1]))>=0;++i)
            ((uint8_t*)Data)[i] = (uint8_t)(a*16+b);
        break;

    case TYPE_RGB:
        *(rgbval_t*)Data = STR_StringToRGB(Value);
        break;

	case TYPE_TICK:
		*(tick_t*)Data = STR_StringToTick(Value);
		break;

	case TYPE_SIZE:
        *(size_t*)Data = StringToIntEx(Value,Flags,State);
        break;

	case TYPE_INT:
        if ((Flags & TUNIT_MASK)==TUNIT_PERCENT)
        {
            cc_fraction f;
            STR_StringToFraction(Value,&f,1);
            *(int*)Data = ScaleRound(PERCENT_ONE,f.Num,f.Den);
        }
        else
        if ((Flags & TUNIT_MASK)==TUNIT_HOTKEY)
            *(int*)Data = KEY_StringToHotKey(Value);
        else
    		*(int*)Data = StringToIntEx(Value,Flags,State);
		break;

	case TYPE_INT8:
		*(uint8_t*)Data = (uint8_t)StringToIntEx(Value,Flags,State);
		break;

	case TYPE_INT16:
		*(uint16_t*)Data = (uint16_t)StringToIntEx(Value,Flags,State);
		break;

    case TYPE_GUID:
        STR_StringToGUID(Value,(cc_guid*)Data);
        break;

	case TYPE_DBNO: //TODO: support for 64 bits
	case TYPE_DATETIME:
	case TYPE_INT32:
		*(int32_t*)Data = (int32_t)StringToIntEx(Value,Flags,State);
		break;

	case TYPE_INT64:
		*(int64_t*)Data = STR_StringToInt64(Value);
		break;

    case TYPE_BOOL_BIT:
	case TYPE_BOOLEAN:
		*(bool_t*)Data = STR_StringToInt(Value,-1);
		break;
		
	case TYPE_FOURCC:
		*(fourcc_t*)Data = STR_StringToFourCC(Value,(Flags & TUNIT_MASK)==TUNIT_UPPER);
		break;

    case TYPE_FIX16:
        STR_StringToFraction(Value,&f,0);
        *(int*)Data = Scale32(FIX16_UNIT,f.Num,f.Den);
        break;

	case TYPE_FRACTION:
		((cc_fraction*)Data)->Num = 0;
		((cc_fraction*)Data)->Den = 0;
        if (tcschr(Value,':'))
        {
    		stscanf(Value,T("%d:%d"),&a,&b);
            ((cc_fraction*)Data)->Num = a;
            ((cc_fraction*)Data)->Den = b;
        }
        else
            STR_StringToFraction(Value,(cc_fraction*)Data,(Flags & TUNIT_MASK)==TUNIT_PERCENT);
		break;

    case TYPE_NODE:
        if (!State)
            return 0;
        return NODE_StringToNode((node**)Data,State,&Value);

    case TYPE_PIN:
        if (!State)
            return 0;
        return NODE_StringToPin((pin*)Data,&DataDef,State,&Value);

    case TYPE_POINT:
    case TYPE_POINT16:
        return ExprToData(Data,&Size,Flags,State,&Value);
        
	default:
		return 0;
	}
	return 1;
}

NOINLINE bool_t NODE_ParserAttrib(parser* p, void* Data, size_t Size, dataflags Flags, exprstate* State)
{
	tchar_t Value[MAXDATA+64];
	if (!NODE_ParserAttribString(p,Value,TSIZEOF(Value)))
		return 0;

    return NODE_StringToData(Data,Size,Flags,State,Value);
}

void NODE_ExprState(exprstate* State, node* Node, dataid Id, dataflags Flags)
{
    if (Flags & TFLAG_ENUM)
        State->EnumList = (const tchar_t*)Node_Meta(Node,Id,META_PARAM_ENUMNAME);
    else
        State->EnumList = NULL;

    if ((Flags & TYPE_MASK) == TYPE_NODE)
        State->ClassId = (fourcc_t)Node_Meta(Node,Id,META_PARAM_CLASS);
}

bool_t NODE_ParserValueData(const tchar_t* Value, node* Node, const datadef* DataDef, exprstate* State, parserexpradd ExprAdd, bool_t ExprSave)
{
    uint8_t Data[MAXDATA];
    size_t Size = Node_MaxDataSize(Node,DataDef->Id,DataDef->Flags,META_PARAM_SET);
    datatype Type = DataDef->Flags & TYPE_MASK;

    State->Context = Node_Context(Node);
    NODE_ExprState(State,Node,DataDef->Id,DataDef->Flags);

    if (Type == TYPE_ARRAY)
    {
        bool_t Result;
        array Array;
        const tchar_t* Expr = Value;
        dataflags Flags = (dataflags)Node_Meta(Node,DataDef->Id,META_PARAM_ARRAY_TYPE);

        ArrayInit(&Array);

        for (;;)
        {
            size_t Size = sizeof(Data);

            if (!ExprToData(Data,&Size,Flags,State,&Expr))
                break;
            
            if (!ArrayAppend(&Array,Data,Size,0))
                break;

            NODE_ExprIsSymbol(&Expr,',');
        }

        Result = Node_SET(Node,DataDef->Id,&Array) == ERR_NONE;

        ArrayClear(&Array);
        return Result;
    }

    if (ExprAdd &&
        !State->EnumList &&
        (Type == TYPE_INT ||
        Type == TYPE_STRING ||
        Type == TYPE_FRACTION ||
        Type == TYPE_FIX16 ||
        Type == TYPE_RGB ||
        Type == TYPE_BOOLEAN ||
        Type == TYPE_BOOL_BIT ||
        Type == TYPE_FOURCC ||
        Type == TYPE_DBNO ||
        Type == TYPE_TICK ||
        Type == TYPE_SIZE ||
        Type == TYPE_PIN ||
        Type == TYPE_EXPR))
    {
        // try to detect constants to skip expression evaluation...
        const tchar_t* s = Value;
        if (Type != TYPE_STRING && Type != TYPE_EXPR && Type != TYPE_FOURCC)
        {
            if (*s=='-' || *s=='+') ++s;
            for (;*s;++s)
                if (IsAlpha(*s) || tcschr(T("+-<>()=*/!%"),*s)!=NULL)
                    break;
        }

        if (*s)
            return ExprAdd(Node,DataDef,State,Value,ExprSave) == ERR_NONE;
    }

    if (!NODE_StringToData(Data,sizeof(Data),DataDef->Flags,State,Value))
    {
        // try reference resolving
        if (Type == TYPE_NODE && ExprAdd)
            return ExprAdd(Node,DataDef,State,Value,ExprSave) == ERR_NONE;

        // save for later
        return ExprAdd && ExprSave && ExprAdd(Node,DataDef,NULL,Value,1) == ERR_NONE;
    }

    return Node_Set(Node,DataDef->Id,Data,Size) == ERR_NONE;
}

bool_t NODE_ParserAttribData(parser* p, node* Node, const datadef* DataDef, exprstate* State, parserexpradd ExprAdd, bool_t ExprSave)
{
	tchar_t Value[MAXDATA+64];

    if (!NODE_ParserAttribString(p,Value,TSIZEOF(Value)))
		return 0;

    return NODE_ParserValueData(Value,Node,DataDef,State,ExprAdd,ExprSave);
}

err_t NODE_TextPrintf(textwriter* p, const tchar_t* Msg,...)
{
	tchar_t s[MAXLINE];
	va_list Arg;
	va_start(Arg,Msg);
	vstprintf_s(s,TSIZEOF(s),Msg,Arg);
	va_end(Arg);
	return NODE_TextWrite(p,s);
}

err_t NODE_Stream_Printf(stream* Stream, const tchar_t* Msg,...)
{
    tchar_t s[MAXLINE];
	va_list Arg;
    textwriter p;
    if (!Stream)
        return ERR_INVALID_PARAM;
    p.Stream = Stream;
    p.CC = Parser_Context(Stream)->ToUTF8;
	va_start(Arg,Msg);
	vstprintf_s(s,TSIZEOF(s),Msg,Arg);
	va_end(Arg);
    return NODE_TextWrite(&p,s);
}

err_t NODE_TextWrite(textwriter* p, const tchar_t* Msg)
{
	size_t i;
	char s[MAXLINE];
	CharConvST(p->CC,s,sizeof(s),Msg);
		
	i = strlen(s);
	
#if defined(TARGET_WIN) || defined(TARGET_SYMBIAN)
	{
		char* nl = s;
		while (i+1<sizeof(s) && (nl = strchr(nl,10))!=NULL)
		{
			memmove(nl+1,nl,i+1-(nl-s));
			*nl = 13;
			nl += 2;
			++i;
		}
	}
#endif

	return Stream_Write(p->Stream,s,i,NULL);
}

static NOINLINE void DumpPtr(textwriter* Text, const tchar_t* Name, node* Node, dataid Id, tchar_t* Value, size_t ValueLen)
{
    NODE_TextAttrib(Text,Name,&Node,TYPE_PTR);

    if (Node)
    {
        fourcc_t ClassId = Node_ClassId(Node);
        tcscpy_s(Value,ValueLen,Name);
        tcscat_s(Value,ValueLen,T("_Class"));
        NODE_TextAttrib(Text,Value,&ClassId,TYPE_FOURCC);
    }

    if (Id)
    {
        int i = (int)Id;
        tcscpy_s(Value,ValueLen,Name);
        tcscat_s(Value,ValueLen,T("_Id"));
        NODE_TextAttrib(Text,Value,&i,TYPE_INT);
    }
}

NOINLINE bool_t NODE_DataToString(tchar_t* Value, size_t ValueLen, const void* Data, size_t Size, dataflags Type)
{
    cc_fraction f;
	size_t i;
    bool_t Display = (Type & TFLAG_DISPLAY)!=0;

    if (!Size)
        Size = NodeTypeSize(Type);

	if (Type & TFLAG_DEFAULT)
	{
		if ((Type & TYPE_MASK) == TYPE_STRING)
		{
			if (((const tchar_t*)Data)[0]==0)
				return 0;
		}
		else
		{
			for (i=0;i<Size;++i)
				if (((const uint8_t*)Data)[i])
					break;
			if (i==Size)
				return 0;
		}
	}

	Value[0] = 0;

	switch (Type & TYPE_MASK)
	{
    case TYPE_BINARY:
       	for (i=0;i<Size;++i)
    		stcatprintf_s(Value,ValueLen,T("%02X"),((uint8_t*)Data)[i]);
        break;

    case TYPE_RGB:
        STR_RGBToString(Value,ValueLen,*(rgbval_t*)Data);
        break;

	case TYPE_TICK:
		STR_TickToString(Value,ValueLen,*(tick_t*)Data,0,1,Display);
		break;

    case TYPE_SIZE:
#if defined(SIZE_MAX) && SIZE_MAX > 0xFFFFFFFF
        STR_Int64ToString(Value,ValueLen,*(size_t*)Data,0);
#else
        STR_IntToString(Value,ValueLen,*(size_t*)Data,0);
#endif
        break;

	case TYPE_INT:
        if ((Type & TUNIT_MASK)==TUNIT_PERCENT)
        {
            cc_fraction f;
            f.Num = *(int*)Data;
            f.Den = PERCENT_ONE;
            STR_FractionToString(Value,ValueLen,&f,-1,1);
        }
        else
        if ((Type & TUNIT_MASK)==TUNIT_HOTKEY)
            KEY_HotKeyToString(Value,ValueLen,*(int*)Data,NULL,NULL);
        else
        if (Display && (Type & TUNIT_MASK)==TUNIT_BYTERATE)
            STR_ByteRateToString(Value,ValueLen,*(int*)Data);
        else
        {
		    STR_IntToString(Value,ValueLen,*(int*)Data,0);
            if (Display && (Type & TUNIT_MASK)==TUNIT_KBYTE)
		        tcscat_s(Value,ValueLen,T(" KB"));
        }
		break;

	case TYPE_INT8:
		STR_IntToString(Value,ValueLen,*(uint8_t*)Data,0);
		break;

	case TYPE_INT16:
		STR_IntToString(Value,ValueLen,*(int16_t*)Data,0);
		break;

    case TYPE_GUID:
        STR_GUIDToString(Value,ValueLen,(const cc_guid*)Data);
        break;

	case TYPE_DBNO: //TODO: support for 64 bits
	case TYPE_DATETIME:
	case TYPE_INT32:
		STR_IntToString(Value,ValueLen,*(int32_t*)Data,(Type & TUNIT_MASK)==TUNIT_HEX);
		break;

	case TYPE_INT64:
		STR_Int64ToString(Value,ValueLen,*(int64_t*)Data,(Type & TUNIT_MASK)==TUNIT_HEX);
		break;

    case TYPE_BOOL_BIT:
	case TYPE_BOOLEAN:
		STR_IntToString(Value,ValueLen,(int32_t)*(bool_t*)Data,0);
		break;
		
	case TYPE_FOURCC:
		STR_FourCCToString(Value,ValueLen,*(fourcc_t*)Data);
		break;

	case TYPE_POINT:
		stprintf_s(Value,ValueLen,T("%d,%d"),((cc_point*)Data)->x,((cc_point*)Data)->y);
		break;

	case TYPE_RECT:
		stprintf_s(Value,ValueLen,T("%d,%d,%d,%d"),((cc_rect*)Data)->x,((cc_rect*)Data)->y,((cc_rect*)Data)->Width,((cc_rect*)Data)->Height);
		break;

    case TYPE_FIX16:
        f.Num = *(int*)Data;
        f.Den = FIX16_UNIT;
        STR_FractionToString(Value,ValueLen,&f,0,3);
        break;

	case TYPE_FRACTION:
        if ((Type & TUNIT_MASK)==TUNIT_PERCENT)
            STR_FractionToString(Value,ValueLen,(cc_fraction*)Data,Display?1:-1,2);
        else
        if (Display)
            STR_FractionToString(Value,ValueLen,(cc_fraction*)Data,0,3); //fps needs 3 decimal
        else
		    stprintf_s(Value,ValueLen,T("%d:%d"),((cc_fraction*)Data)->Num,((cc_fraction*)Data)->Den);
		break;

    case TYPE_PIN:
        NODE_PinToString(Value,ValueLen,(pin*)Data,NULL);
        break;

    case TYPE_NODE:
        NODE_NodeToString(Value,ValueLen,*(node**)Data,NULL);
        break;

    case TYPE_STRING:
        tcscpy_s(Value,ValueLen,(tchar_t*)Data);
        break;
	}

    return 1;
}

void NODE_TextAttribEx(textwriter* Text, const tchar_t* Name, const void* Data, size_t Size, dataflags Type)
{
	tchar_t Value[MAXDATA+64];
	tchar_t *c;
	size_t i;

    if (NODE_DataToString(Value,TSIZEOF(Value),Data,Size,Type))
    {
        switch (Type & TYPE_MASK)
        {
        case TYPE_PIN:
        case TYPE_PACKET:
            DumpPtr(Text,Name,((pin*)Data)->Node,((pin*)Data)->Id,Value,TSIZEOF(Value));
            return;
	    
        case TYPE_NODE:
            DumpPtr(Text,Name,*(node**)Data,0,Value,TSIZEOF(Value));
            return;

	    case TYPE_STRING:
		    for (i=0,c=(tchar_t*)Data;*c && i<TSIZEOF(Value)-1;++c)
		    {
			    const tchar_t* s;
			    switch (*c)
			    {
                // don't use &apos; to be compatible with html
			    case '&': s = T("&amp;"); break;
			    case '"': s = T("&quot;"); break;
			    case '<': s = T("&lt;"); break;
			    case '>': s = T("&gt;"); break;
			    default: s = NULL; break;
			    }
			    if (s)
			    {
				    tcscpy_s(Value+i,TSIZEOF(Value)-i,s);
				    i += tcslen(Value+i);
			    }
			    else
				    Value[i++] = *c;
		    }
		    assert(i<TSIZEOF(Value)-1);
            if (i>TSIZEOF(Value)-1)
                i=TSIZEOF(Value)-1;
		    Value[i] = 0;
		    break;

	    case TYPE_PTR:
		    stprintf_s(Value,TSIZEOF(Value),T("%08x"),*(uint32_t*)Data);
            break;
        }

       	NODE_TextPrintf(Text,T(" %s=\"%s\""),Name,Value);
    }
}

void NODE_TextAttrib(textwriter* Text, const tchar_t* Name, const void* Data, dataflags Type)
{
    NODE_TextAttribEx(Text,Name,Data,0,Type);
}

void NODE_TextElementBegin(textwriter* Out, textwriter* In, const tchar_t* Element)
{
	if (!In->HasChild)
	{
		In->HasChild = 1;
        if (In->InsideContent)
            NODE_TextWrite(In,T(">"));
        else
		    NODE_TextWrite(In,T(">\n"));
	}
    Out->CC = In->CC;
	Out->Stream = In->Stream;
	Out->HasChild = 0;
	Out->SafeFormat = In->SafeFormat;
    Out->InsideContent = In->Deep==0;
    Out->Deep = In->InsideContent ? 0 : In->Deep+2;
	Out->Element = Element;
    NODE_TextPrintf(Out,T("%*c%s"),In->Deep?Out->Deep:0, '<',Element);
}

void NODE_TextElementEnd(textwriter* Text)
{
    if (Text->HasChild)
	{
        if (Text->InsideContent)
            NODE_TextPrintf(Text,T("</%s> "),Text->Element);
        else
            NODE_TextPrintf(Text,T("%*c/%s>\n"),Text->Deep,'<',Text->Element);
    }
	else
	{
		if (Text->SafeFormat)
			NODE_TextPrintf(Text,T("></%s>"),Text->Element);
		else
			NODE_TextWrite(Text,T("/>"));
		if (!Text->InsideContent)
			NODE_TextWrite(Text,T("\n"));
	}
}

void NODE_TextElementEndData(textwriter* Text, const tchar_t *Value)
{
	if (Text->HasChild)
		NODE_TextPrintf(Text,T("%s%*c/%s>\n"),Value, Text->Deep,'<',Text->Element);
	else
		NODE_TextPrintf(Text,T(">%s</%s>\n"), Value, Text->Element);
}

void NODE_TextElementAppendData(textwriter* Text, const tchar_t *Value)
{
    if (Text->Deep==1 && tcsisame_ascii(Value,T(" ")))
        return; // root element
    if (Text->HasChild) 
		NODE_TextWrite(Text,Value);
    else {
		NODE_TextPrintf(Text,T(">%s"), Value);
        Text->HasChild = 1;
        Text->Deep = 0;
    }
}

void NODE_TextElementXML(parsercontext *Context, textwriter* Text, const tchar_t* Root)
{
	assert(Text->Stream);
	Text->CC = Context->ToUTF8;
	Text->Element = Root;
	Text->Deep = 1;
	Text->HasChild = 0;
    Text->InsideContent = 0;
	Text->SafeFormat = 0;
	NODE_TextPrintf(Text,T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<%s"),Root);
}

NOINLINE bool_t NODE_ExprSkipAfter(const tchar_t** p,int ch)
{
    const tchar_t* s = *p;
    for (;*s;++s)
        if ((ch && *s == ch) || (!ch && IsSpace(*s)))
        {
            *p = s+1;
            if (!ch) NODE_ExprSkipSpace(p); // skip other spaces too
            return 1;
        }

    *p = s;
    return 0;
}

NOINLINE void NODE_ExprSkipSpace(const tchar_t** p)
{
    const tchar_t* s = *p;
    while (IsSpace(*s))
        ++s;
    *p = s;
}

NOINLINE void NODE_ExprTrimSpace(tchar_t** p)
{
    if (*p[0])
    {
        tchar_t* s = *p+tcslen(*p)-1;
        while (s>=*p && IsSpace(*s))
        {
            *s = 0;
            s--;
        }
    }
}

NOINLINE bool_t NODE_ExprCmd(const tchar_t** Expr, tchar_t* Out, size_t OutLen)
{
    const tchar_t* s;
    bool_t Quote=0;

    NODE_ExprSkipSpace(Expr);

    s = *Expr;
    if (*s == '\0')
        return 0;

    assert(*s && (Quote || !IsSpace(*s)));

    for (;*s && (Quote || !IsSpace(*s));++s)
    {
        if (*s == '"')
        {
            Quote = !Quote;
            continue;
        }

        if (OutLen>1)
        {
            *(Out++) = *s;
            --OutLen;
        }
    }

    *Expr = s;

    if (OutLen>0)
        *Out=0;

    return 1;
}

static NOINLINE bool_t ReadHex(const tchar_t** p,intptr_t* Out,bool_t RGB, bool_t Neg);

NOINLINE bool_t NODE_ExprIsTokenEx(const tchar_t** p,const tchar_t* Name,...)
{
    const tchar_t* s = *p;
    bool_t Long = 0;
	va_list Arg;
	va_start(Arg, Name);

    if (!*Name)
        return 0;

	NODE_ExprSkipSpace(&s);

    while (*Name && *s)
    {
        if (*Name == ' ')
        {
            NODE_ExprSkipSpace(&s);
            ++Name;
        }
        else
        if (*Name == '%')
        {
            int IntSize = 0;
            ++Name;
            if (IsDigit(*Name))
            {
                IntSize = *Name - '0';
                ++Name;
            }
            while (*Name == 'l')
            {
                Long=1;
				++Name; // long
            }
            if (*Name=='I' && *(Name+1)=='6' && *(Name+2)=='4')
            {
                Long=1;
				Name += 3;
            }
            if (*Name == 'd')
            {
                if (Long)
                {
                    if ((!IntSize && !NODE_ExprIsInt64(&s,va_arg(Arg,int64_t*)))||(IntSize && !NODE_ExprIsInt64Ex(&s,IntSize,va_arg(Arg,int64_t*))))
                        break;
                }
                else
                {
                    if ((!IntSize && !NODE_ExprIsInt(&s,va_arg(Arg,intptr_t*)))||(IntSize && !NODE_ExprIsIntEx(&s,IntSize,va_arg(Arg,intptr_t*))))
                        break;
                }
                ++Name;
            }
            if (*Name == 'x')
            {
                if (!ReadHex(&s,va_arg(Arg,intptr_t*),0,0))
                    break;
                ++Name;
            }
            else if (*Name == 's')
            {
                tchar_t *Out = va_arg(Arg,tchar_t*);
                size_t OutLen = va_arg(Arg,size_t);
                ++Name;
                while (OutLen>1 && *s && (*s != *Name && (*Name || *s != ' ')))
                {
                    *Out++ = *s++;
                    OutLen--;
                }
                *Out = 0;
            }
            else if (*Name == '%')
            {
				if (*s == '%') {
					++s;
					++Name;
				}
			}
        }
        else
        {
            size_t n;
            for (n=1;Name[n] && Name[n]!=' ' && Name[n]!='%';++n) {}

	        if (tcsnicmp_ascii(s,Name,n)!=0)
                break;

            Name += n;
            s += n;
        }
    }

	va_end(Arg);

    if (*Name==0 && (!IsAlpha(Name[-1]) || !IsAlpha(*s)))
    {
        *p = s;
        return 1;
    }
    return 0;
}

NOINLINE bool_t NODE_ExprIsToken(const tchar_t** p,const tchar_t* Name)
{
    return NODE_ExprIsTokenEx(p,Name);
}

uint32_t NODE_StringToIP(const tchar_t *Address)
{
    uint32_t Result = 0;
    tchar_t *s;
    while ((s = tcschr(Address,T('.')))!=NULL)
    {
        *s++ = 0;
        Result <<= 8;
        Result |= STR_StringToInt(Address,0);
        Address = s;
    }
    Result <<= 8;
    Result |= STR_StringToInt(Address,0);
    return Result;
}

static const uint8_t Base64[] =
{
    0x3e,0x80,0x80,0x80,0x3f,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x80,
    0x80,0x80,0x80,0x80,0x80,0x80,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
    0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
    0x80,0x80,0x80,0x80,0x80,0x80,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,0x21,0x22,0x23,
    0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33
};

static tchar_t Base64Char(int Value)
{
    if (Value < 26)
        return (tchar_t)(Value + 'A');
    if (Value < 52)
        return (tchar_t)(Value - 26 + 'a');
    if (Value < 62)
        return (tchar_t)(Value - 52 + '0');
    if (Value == 62)
        return '+';
    assert(Value == 63);
    return '/';
}

bool_t NODE_BufferToBase64(tchar_t *Out, size_t OutLen, const uint8_t *Buffer, size_t BufferSize)
{
    while (BufferSize >= 3 && OutLen > 4) // treat packets of 3 bytes at once
    {
        Out[0] = Base64Char(Buffer[0] >> 2);
        Out[1] = Base64Char(((Buffer[0]&0x03)<<4) | (Buffer[1]>>4));
        Out[2] = Base64Char(((Buffer[1]&0x0F)<<2) | (Buffer[2]>>6));
        Out[3] = Base64Char(Buffer[2]&0x3F);
        BufferSize -= 3;
        Buffer += 3;
        OutLen -= 4;
        Out += 4;
    }

    if (BufferSize--)
    {
        if (OutLen < 5)
            return 0; // not enough room in the output buffer
        Out[0] = Base64Char(Buffer[0] >> 2);

        if (BufferSize--)
            Out[1] = Base64Char(((Buffer[0]&0x03)<<4) | (Buffer[1]>>4));
        else
            Out[1] = Base64Char((Buffer[0]&0x03)<<4);

        if (BufferSize == (size_t)-1)
            Out[2] = '=';
        else if (BufferSize--)
            Out[2] = Base64Char(((Buffer[1]&0x0F)<<2) | (Buffer[2]>>6));
        else
            Out[2] = Base64Char((Buffer[1]&0x0F)<<2);

        if (BufferSize == (size_t)-1)
            Out[3] = '=';
        else
            Out[3] = Base64Char(Buffer[2]&0x3F);
        Out += 4;
    }
    Out[0] = 0;
    return 1;
}

NOINLINE size_t NODE_ExprIsBase64(const tchar_t** p,uint8_t* Out,size_t OutSize)
{
    size_t n;
    size_t v=0;
    uint8_t* Out0 = Out;

	NODE_ExprSkipSpace(p);
    for (n=0;**p;++n,++(*p))
    {
        if (**p == '=')
        {
            while (**p == '=') ++(*p);
            break;
        }

        if (**p < 43 || **p >= 43+sizeof(Base64) || Base64[**p-43] == 0x80)
            break;

        v = (v<<6) + Base64[**p-43];

        if ((n & 3) && OutSize>0)
        {
            --OutSize;
            *(Out++) = (uint8_t)(v >> (6 - (n & 3)*2));
        }
    }

    return Out-Out0;
}

NOINLINE bool_t NODE_ExprIsName(const tchar_t** p,tchar_t* Out,size_t OutLen, const tchar_t* Delimiter)
{
    bool_t Found = 0;

	NODE_ExprSkipSpace(p);
    for (;**p && !IsSpace(**p) && tcschr(Delimiter,**p)==NULL;++(*p))
        if (OutLen>1)
        {
            --OutLen;
            *(Out++) = **p;
            Found = 1;
        }

    if (OutLen)
        *Out = 0;

    return Found;
}

NOINLINE bool_t NODE_ExprIsSymbol(const tchar_t** p,int ch)
{
    if (**p)
    {
	    NODE_ExprSkipSpace(p);
	    if (**p == ch)
	    {
		    ++(*p);
		    return 1;
	    }
    }
	return 0;
}

NOINLINE bool_t NODE_ExprIsSymbol2(const tchar_t** p,int ch, int ch2)
{
	NODE_ExprSkipSpace(p);
	if ((*p)[0] == ch && (*p)[1]==ch2)
	{
		(*p)+=2;
		return 1;
	}
	return 0;
}

NOINLINE void NODE_ExprParamEnd(const tchar_t** p)
{
	NODE_ExprIsSymbol(p,')');
}

NOINLINE bool_t NODE_ExprParamNext(const tchar_t** p)
{
	if (NODE_ExprIsSymbol(p,','))
		return 1;
	NODE_ExprParamEnd(p);
	return 0;
}

static NOINLINE bool_t ReadHex(const tchar_t** p,intptr_t* Out,bool_t RGB, bool_t Neg)
{
    const tchar_t* s = *p;
    intptr_t v = 0;
    intptr_t w;
    size_t n = 0;

    if ((w = Hex(*s))<0)
        return 0;

    do
    {
        ++n;
        v = v*16+w;
    } while ((w = Hex(*(++s)))>=0);

    if (RGB)
    {
        if (n<=6)
            v <<= 8;
        v = INT32BE(v);
    }

    if (Neg)
        v = -v;

    *Out = v;
    *p = s;
    return 1;
}

NOINLINE bool_t NODE_ExprIsHex(const tchar_t** p,intptr_t* Out)
{
    const tchar_t* s = *p;

    bool_t Pos = *s == '+';
    bool_t Neg = *s == '-';
    if (Neg || Pos) ++s;

	if (s[0]=='0' && s[1]=='x')
    {
        s += 2;
        if (ReadHex(&s,Out,0,Neg))
        {
            *p = s;
            return 1;
        }
    }
    return 0;
}

NOINLINE bool_t NODE_ExprIsRGB(const tchar_t** p,intptr_t* Out)
{
    const tchar_t* s = *p;
    if (*(s++) == '#' && ReadHex(&s,Out,1,0))
    {
        *p = s;
        return 1;
    }
    return 0;
}

NOINLINE bool_t NODE_ExprIsFourCC(const tchar_t** p,intptr_t* Out)
{
    const tchar_t* s = *p;
    if (*(s++) == '@' && *(s++) == '\'' && NODE_ExprSkipAfter(&s,'\'') && (s <= *p+7))
    {
        tchar_t v[16];
        tcsncpy_s(v,TSIZEOF(v),*p+2,s-*p-3);
        *Out = STR_StringToFourCC(v,0);
        *p = s;
        return 1;
    }
    return 0;
}

NOINLINE bool_t NODE_ExprIsIntEx(const tchar_t** p,int Size,intptr_t* Out)
{
    const tchar_t* s = *p;
    intptr_t v = 0;

    bool_t Pos = *s == '+';
    bool_t Neg = *s == '-';
    if (Neg || Pos) ++s;

	if (!IsDigit(*s))
        return 0;

	do
    {
		v = v*10 + (*s-'0');
    }
    while (--Size && IsDigit(*(++s)));

    if (Size!=0)
        return 0;

	if (Neg)
		v = -v;

    *Out = v;
    *p = s+1;
    return 1;
}

NOINLINE bool_t NODE_ExprIsInt64Ex(const tchar_t** p,int Size,int64_t* Out)
{
    const tchar_t* s = *p;
    int64_t v = 0;

    bool_t Pos = *s == '+';
    bool_t Neg = *s == '-';
    if (Neg || Pos) ++s;

	if (!IsDigit(*s))
        return 0;

	do
    {
		v = v*10 + (*s-'0');
    }
    while (--Size && IsDigit(*(++s)));

    if (Size!=0)
        return 0;

	if (Neg)
		v = -v;

    *Out = v;
    *p = s+1;
    return 1;
}

NOINLINE bool_t NODE_ExprIsInt(const tchar_t** p,intptr_t* Out)
{
    const tchar_t* s = *p;
    intptr_t v = 0;

    bool_t Pos = *s == '+';
    bool_t Neg = *s == '-';
    if (Neg || Pos) ++s;

	if (!IsDigit(*s))
        return 0;

	do
    {
		v = v*10 + (*s-'0');
    }
    while (IsDigit(*(++s)));

	if (Neg)
		v = -v;

    *Out = v;
    *p = s;
    return 1;
}

NOINLINE bool_t NODE_ExprIsInt64(const tchar_t** p,int64_t* Out)
{
    const tchar_t* s = *p;
    int64_t v = 0;

    bool_t Pos = *s == '+';
    bool_t Neg = *s == '-';
    if (Neg || Pos) ++s;

	if (!IsDigit(*s))
        return 0;

	do
    {
		v = v*10 + (*s-'0');
    }
    while (IsDigit(*(++s)));

	if (Neg)
		v = -v;

    *Out = v;
    *p = s;
    return 1;
}

NOINLINE bool_t NODE_ExprIsFrac(const tchar_t** p,cc_fraction* Out)
{
    int64_t Num,Den;
    intptr_t v;

    if (!NODE_ExprIsInt(p,&v))
        return 0;

    Num = v;
    Den = 1;

	if (**p == '.' || **p == ',')
	{
		++(*p);
        for (;IsDigit(**p);++(*p))
        {
		    Num = Num*10 + (**p-'0');
            Den *= 10;
        }
	}

    NODE_SimplifyFrac(Out,Num,Den);
    return 1;
}

bool_t NODE_ExprIsPoint(const tchar_t** p, cc_point* Out)
{
    intptr_t x,y;
    if (NODE_ExprIsTokenEx(p,T("{ %d , %d }"),&x,&y))
    {
        Out->x = (int)x;
        Out->y = (int)y;
        return 1;
    }
    return 0;
}

#define MAX_GCD_ITER 128

void NODE_SimplifyFrac(cc_fraction* f, int64_t Num, int64_t Den)
{
    bool_t Sign = (Num<0) != (Den<0);

    Num = _abs(Num);
    Den = _abs(Den);

    if (Num>INT_MAX || Den>INT_MAX)
    {
        int64_t a = Num;
        int64_t b = Den;
        size_t n;

        for (n=0;n<MAX_GCD_ITER && a>0 && b>0;++n)
        {
            int64_t c = a;
            a = b % a;
            b = c;
        }

        if (n<MAX_GCD_ITER && b>1)
        {
            Num /= b;
            Den /= b;
        }

        while (Num>INT_MAX || Den>INT_MAX)
        {
            Num >>= 1;
            Den >>= 1;
        }
    }
    
    f->Num = (int)(Sign?-Num:Num);
    f->Den = (int)Den;
}

void NODE_DoubleToFrac(cc_fraction* f,int64_t v)
{
    if (!v)
    {
        f->Num = 0;
        f->Den = 1;
    }
    else
    {
        int e = (int)(((v >> 52) & 0xFFF) - 1023);
        NODE_SimplifyFrac(f,(v & LL(0xFFFFFFFFFFFFF))|LL(0x10000000000000),LL(1)<<(52-e));
        if (v<0)
            f->Num = -f->Num;
    }
}

void NodeParamName(node* p, dataid Id, tchar_t* Name, size_t NameLen)
{
	const tchar_t* ShortName = (const tchar_t*)Node_Meta(p,Id,META_PARAM_NAME);
	if (ShortName)
		tcscpy_s(Name,NameLen,ShortName);
	else
	if (Id>(1<<24))
		STR_FourCCToString(Name,NameLen,(fourcc_t)Id);
	else
    if (Node_Get(p,Id|DATA_DYNNAME,Name,NameLen*sizeof(tchar_t))!=ERR_NONE)
		stprintf_s(Name,NameLen,T("_%04x"),Id);
}

static bool_t CheckHex(const tchar_t* s)
{
    for (;*s;++s)
        if (Hex(*s)<0)
            return 0;
    return 1;
}

bool_t NodeFindDef(node* p,const tchar_t* Token,datadef* DataDef)
{
    dataid Id;
	if (Token[0]=='_' && CheckHex(Token+1))
        Id = STR_StringToInt(Token+1,1);
    else
    {
        Id = Node_FindParam(p,Token);
        if (!Id)
            Id = STR_StringToFourCC(Token,0);
    }
    return NodeDataDef(p,Id,DataDef);
}

void NODE_TextSerializeNode(textwriter* Text, node* p, uint_fast32_t Mask, uint_fast32_t Filter)
{
    array List;
	datadef* i;

    NodeEnumDef(p,&List);

	for (i=ARRAYBEGIN(List,datadef);i!=ARRAYEND(List,datadef);++i)
		if ((i->Flags & Mask)==Filter)
	    {
		    tchar_t Name[32];
		    uint8_t Data[MAXDATA];
            size_t Size = Node_MaxDataSize(p,i->Id,i->Flags,META_PARAM_GET);
            assert((i->Id & ~DATA_ENUM)==i->Id);
            assert(Size <= sizeof(Data));
            if (Size)
            {
		        err_t Result = Node_Get(p,i->Id,Data,Size);

		        if (Result == ERR_NONE || Result == ERR_NOT_SUPPORTED) // ERR_NOT_SUPPORTED for subclasses that don't support a param of the main class
		        {
                    NodeParamName(p,i->Id,Name,TSIZEOF(Name));

			        NODE_TextAttribEx(Text,Name,Data,Size,i->Flags);

                    /* TODO: support dumping packetformat information (example for crash.txt)
                    if ((i->Flags & TYPE_MASK) == TYPE_PACKET && Node_Get(p,i->Id|PIN_FORMAT,Data,sizeof(packetformat)) == ERR_NONE)
                    {
                        tcscat_s(Name,TSIZEOF(Name),T("_Format"));
				        NODE_TextAttrib(Text,Name,Data,TYPE_PACKETFORMAT);
                    }
                    */
		        }
            }
	    }

    ArrayClear(&List);
}

void NODE_ParserImport(parser* Parser,node* Node)
{
	tchar_t Token[MAXTOKEN];
    exprstate State;
    memset(&State,0,sizeof(State));

	while (NODE_ParserIsAttrib(Parser,Token,MAXTOKEN))
	{
        datadef DataDef;
        if (NodeFindDef(Node,Token,&DataDef))
            NODE_ParserAttribData(Parser,Node,&DataDef,&State,NULL,0);
        else
            NODE_ParserAttribSkip(Parser);
	}
}

void NODE_ParserImportNested(parser* Parser,node* Node)
{
    if (Node)
        NODE_ParserImport(Parser,Node);

    if (Node && Node_IsPartOf(Node,NODETREE_CLASS))
    {
		tchar_t Token[MAXTOKEN];
		while (NODE_ParserIsElementNested(Parser,Token,MAXTOKEN))
        {
            node* Child = NodeCreate(Node,STR_StringToFourCC(Token,0));
            NODE_ParserImportNested(Parser,Child);
            if (Child)
                NodeTree_SetParent(Child,Node,NULL);
        }
    }
	else
        NODE_ParserElementSkipNested(Parser);
}

#define LANG_STRINGS_OFFSET  0x100

static err_t GetStr(node* p,dataid Id, tchar_t *Data, size_t Size)
{
    if (Id-LANG_STRINGS_OFFSET < DATA_ENUM)
        tcscpy_s(Data,Size/sizeof(tchar_t),StrTab_GetPos(&Parser_Context(p)->StrTab,Id-LANG_STRINGS_OFFSET));
    else if ((Id-LANG_STRINGS_OFFSET) & DATA_DYNNAME)
        StrTab_GetPosName(&Parser_Context(p)->StrTab,(Id-LANG_STRINGS_OFFSET) & ~DATA_DYNNAME, Data, Size/sizeof(tchar_t));
    else
        return ERR_INVALID_PARAM;
    return ERR_NONE;
}

static uintptr_t MetaStr(node* p,dataid Id,datameta Meta)
{
    if (Meta==META_PARAM_TYPE)
        return TYPE_STRING|TFLAG_RDONLY;
    else if (Meta == META_PARAM_NAME)
        return 0; // get it with Id|DATA_DYNNAME

    return INHERITED(p,node_vmt,LANG_STRINGS_ID)->Meta(p,Id-LANG_STRINGS_OFFSET,Meta);
}

static dataid FindStrParam(node* p,const tchar_t* Token)
{
    fourcc_t ClassId;
    int No;
    if (tcslen(Token)!=8)
        return 0;
    ClassId = STR_StringToFourCC(Token,0);
    No = STR_StringToInt(Token+4,1);
    return (dataid)StrTab_Pos(&Parser_Context(p)->StrTab,ClassId,No) + LANG_STRINGS_OFFSET;
}

static void EnumStr(node *p,array* List)
{
    dataid Id,*i;
    size_t StrSize = ArraySize(&Parser_Context(p)->StrTab.Table);
    StrSize /= STRTAB_ITEMSIZE;

    ArrayInit(List);
    ArrayResize(List,StrSize*sizeof(dataid),0);
    i = ARRAYBEGIN(*List,dataid);
    for (Id = 0;Id < StrSize;++Id,++i)
        *i = Id+LANG_STRINGS_OFFSET;
}

static void NodeDumpObject(node* p,textwriter* Text)
{
	textwriter Level;
    array Children;
	tchar_t Element[8];
    node** i;

	TRY_BEGIN 
    if (!(NodeClassFlags(p) & CFLAG_LOCAL)) // example skin UI stuff
    {
        STR_FourCCToString(Element,TSIZEOF(Element),Node_ClassId(p));
        NODE_TextElementBegin(&Level,Text,Element);
        NODE_TextAttrib(&Level,T("Addr"),&p,TYPE_PTR);
		NODE_TextSerializeNode(&Level,p,TFLAG_NODUMP,0);
        ArrayInit(&Children);
        if (Node_GET(p,NODE_CHILDREN,&Children)==ERR_NONE)
            for (i=ARRAYBEGIN(Children,node*);i!=ARRAYEND(Children,node*);++i)
                NodeDumpObject(*i,&Level);
        ArrayClear(&Children);
        NODE_TextElementEnd(&Level);
    }
    TRY_END
}

void NODE_Dump(nodecontext* p,textwriter* Text)
{
	//no NodeLock, because may be stayed locked when dumping after crash 
	node **i;
	for (i=ARRAYBEGIN(p->NodeSingleton,node*);i!=ARRAYEND(p->NodeSingleton,node*);++i)
		if (*i)
            NodeDumpObject(*i,Text);
}

META_START(LangStr_Class,LANG_STRINGS_ID)
META_CLASS(FLAGS,CFLAG_SINGLETON|CFLAG_LOCAL)
META_PARAM(STRING,NODE_ID,T("LangStr"))
META_VMT(TYPE_FUNC,node_vmt,Get,GetStr)
META_VMT(TYPE_FUNC,node_vmt,Meta,MetaStr)
META_VMT(TYPE_FUNC,node_vmt,Enum,EnumStr)
META_VMT(TYPE_FUNC,node_vmt,FindParam,FindStrParam)
META_END(NODE_CLASS)
