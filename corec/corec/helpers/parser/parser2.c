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
#include "buffer.h"
#include "strtypes.h"
#include <corec/helpers/file/streams.h>
#include <corec/helpers/charconvert/charconvert.h>

struct parser
{
    parsercontext *Context;
    struct stream* Stream;
    buffer Buffer;
    struct charconv* CC;
    struct charconv* EscapeCC;
    tchar_t *BigLine;
    boolmem_t Element;
    boolmem_t ElementEof;
    boolmem_t URL;
    boolmem_t OwnCC;
    boolmem_t Error;

};

#define Parser_Context(p) ((parsercontext*)Node_Context(p))

extern const nodemeta LangStr_Class[];

static const tchar_t* ExternalStr(nodecontext* p,fourcc_t Class,int Id)
{
    return StrTab_Find(&((parsercontext*)p)->StrTab,Class,Id);
}

void ParserContext_Init(parsercontext* p,const nodemeta* Custom, const cc_memheap* Heap, const cc_memheap* ConstHeap)
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

    NodeRegisterClassEx(&p->Base.Base,LangStr_Class);
    CoreC_FileInit(&p->Base.Base);
}

void ParserContext_Done(parsercontext* p)
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

void Node_ToUTF8(anynode* p, char* Out,size_t OutLen, const tchar_t* In)
{
    CharConvST(Parser_Context(p)->ToUTF8,Out,OutLen,In);
}

void Node_FromUTF8(anynode* p, tchar_t* Out,size_t OutLen, const char* In)
{
    CharConvTS(Parser_Context(p)->FromUTF8,Out,OutLen,In);
}

void Node_ToStr(anynode* p, char* Out,size_t OutLen, const tchar_t* In)
{
    CharConvST(Parser_Context(p)->ToStr,Out,OutLen,In);
}

void Node_FromStr(anynode* p, tchar_t* Out,size_t OutLen, const char* In)
{
    CharConvTS(Parser_Context(p)->FromStr,Out,OutLen,In);
}

void Node_ToWcs(anynode* p, wchar_t* Out,size_t OutLen, const tchar_t* In)
{
    CharConvWT(Parser_Context(p)->ToWcs,Out,OutLen,In);
}

void Node_FromWcs(anynode* p, tchar_t* Out,size_t OutLen, const wchar_t* In)
{
    CharConvTW(Parser_Context(p)->FromWcs,Out,OutLen,In);
}

void Node_ToUTF16(anynode* p, utf16_t* Out,size_t OutLen, const tchar_t* In)
{
    CharConvUT(Parser_Context(p)->ToUtf16,Out,OutLen,In);
}

void Node_FromUTF16(anynode* p, tchar_t* Out,size_t OutLen, const utf16_t* In)
{
    CharConvTU(Parser_Context(p)->FromUtf16,Out,OutLen,In);
}

static NOINLINE err_t ParserFill(parser* p,size_t Needed) // non-blocking stream could return ERR_NEED_MORE_DATA
{
    // Needed may be zero (see http.c EnumDir())

    // pack buffer if more then half is done
    if (p->Buffer.Read > p->Buffer.Begin + (p->Buffer.End - p->Buffer.Begin)/2)
        BufferPack(&p->Buffer,0);

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
    ParserFill(p,1); \
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

static bool_t ParserIsToken(parser* p, const tchar_t* Token) // case insensitive, ascii
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

static void ParserHTMLChars(parser *p, tchar_t *Out, size_t OutLen)
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
                Node_FromUTF16(p->Context,Out,OutLen,UChar);
                for (Index=0;Index<OutLen && Out[Index];++Index) {}
                memmove(Out+Index,i+1,sizeof(tchar_t)*tcslen(i));
                Out += Index - 1; // -1 for the ++ of the loop
                OutLen -= Index - 1;
            }
        }
    }
}

static void ParserHTMLToURL(tchar_t* URL, size_t OutLen)
{
    for (;*URL && OutLen>0;++URL,--OutLen)
        if (URL[0]=='%' && Hex(URL[1])>=0 && Hex(URL[2])>=0)
        {
            *URL = (tchar_t)((Hex(URL[1])<<4)+Hex(URL[2]));
            memmove(URL+1,URL+3,sizeof(tchar_t)*(tcslen(URL+3)+1));
        }
}

intptr_t ParserReadUntil(parser* p, tchar_t* Out, size_t OutLen, int Delimiter)
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
                    Found = ParserIsToken(p,T("]]>"));
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

            if (Delimiter != '\n') // not ParserLine
                ParserHTMLChars(p,Out,OutLen);

            if (p->URL)
                ParserHTMLToURL(Out,OutLen);
        }
    }
    return n;
}

static NOINLINE void ParserElementSkip(parser* p)
{
    while (ParserIsAttrib(p,NULL,0))
           ParserAttribSkip(p);
}

/** Skip all the attributes of the current element and position after the '>' */
static NOINLINE void ParserElementSkipNested(parser* p)
{
    ParserElementSkip(p);
    while (ParserIsElementNested(p,NULL,0))
        ParserElementSkipNested(p);
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

bool_t ParserIsElementNested(parser* p, tchar_t* Name, size_t NameLen)
{
    bool_t Start = 1;

    if (p->Element)
        ParserElementSkipNested(p);

    if (p->ElementEof || !(Start = ElementStart(p)) || IsToken(p,T("/")))
    {
        if (!Start)
            p->Error = 1;
        p->ElementEof = 0;
        p->Element = 0;
    }
    else
        p->Element = (boolmem_t)(ParserReadUntil(p,Name,NameLen,'>')>0);

    return p->Element;
}

bool_t ParserIsAttrib(parser* p, tchar_t* Name, size_t NameLen)
{
    if (!p->Element)
        return 0;

    // skip spaces by ParserIsToken
    p->ElementEof = (boolmem_t)ParserIsToken(p,T("/>"));
    if (p->ElementEof || IsToken(p,T(">")) || IsToken(p,T("?>")))
        p->Element = 0;
    else
        p->Element = (boolmem_t)(ParserReadUntil(p,Name,NameLen,'=')>0);
    return p->Element;
}

bool_t ParserAttribString(parser* p, tchar_t* Out, size_t OutLen)
{
    int Delimiter;

    // skip spaces by ParserIsToken
    if (!ParserIsToken(p,T("=")))
        return 0;

    // skip spaces by ParserIsToken
    if (ParserIsToken(p,T("\"")))
        Delimiter = '\"';
    else if (IsToken(p,T("'")))
        Delimiter = '\'';
    else
        Delimiter = '>';

    return ParserReadUntil(p,Out,OutLen,Delimiter)>=0;
}

void ParserAttribSkip(parser* p)
{
    ParserAttribString(p,NULL,0);
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
        Base = NodeSingleton(Context,StringToFourCC(Token,1));
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

static bool_t PinToString(tchar_t* Value, size_t ValueLen, const pin* Data, node* Base)
{
    if (NodeToString(Value,ValueLen,Data->Node,Base) && Data->Node)
    {
        size_t n;
        if (Value[0])
            tcscat_s(Value,ValueLen,T("."));
        n = tcslen(Value);
        NodeParamName(Data->Node,Data->Id,Value+n,ValueLen-n);
    }

    return 1;
}

bool_t NodeToString(tchar_t* Value, size_t ValueLen, node* Node, node* UNUSED_PARAM(Base))
{
    Value[0]=0;

    if (Node)
    {
        if (NodeClassFlags(Node) & CFLAG_SINGLETON)
            FourCCToString(Value,ValueLen,Node_ClassId(Node));
        else
        {
            //TODO: use Base... (maybe NULL)
            if (Node_Get(Node,NODE_ID,Value,ValueLen*sizeof(tchar_t))!=ERR_NONE)
                Value[0] = 0;
        }
    }

    return 1;
}

err_t TextPrintf(textwriter* p, const tchar_t* Msg,...)
{
    tchar_t s[MAXLINE];
    va_list Arg;
    va_start(Arg,Msg);
    vstprintf_s(s,TSIZEOF(s),Msg,Arg);
    va_end(Arg);
    return TextWrite(p,s);
}

err_t Stream_Printf(stream* Stream, const tchar_t* Msg,...)
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
    return TextWrite(&p,s);
}

err_t TextWrite(textwriter* p, const tchar_t* Msg)
{
    size_t i;
    char s[MAXLINE];
    CharConvST(p->CC,s,sizeof(s),Msg);

    i = strlen(s);

#if defined(TARGET_WIN)
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
    TextAttrib(Text,Name,&Node,TYPE_PTR);

    if (Node)
    {
        fourcc_t ClassId = Node_ClassId(Node);
        tcscpy_s(Value,ValueLen,Name);
        tcscat_s(Value,ValueLen,T("_Class"));
        TextAttrib(Text,Value,&ClassId,TYPE_FOURCC);
    }

    if (Id)
    {
        int i = (int)Id;
        tcscpy_s(Value,ValueLen,Name);
        tcscat_s(Value,ValueLen,T("_Id"));
        TextAttrib(Text,Value,&i,TYPE_INT);
    }
}

NOINLINE bool_t DataToString(tchar_t* Value, size_t ValueLen, const void* Data, size_t Size, dataflags Type)
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
        RGBToString(Value,ValueLen,*(rgbval_t*)Data);
        break;

    case TYPE_TICK:
        TickToString(Value,ValueLen,*(tick_t*)Data,0,1,Display);
        break;

    case TYPE_SIZE:
#if SIZE_MAX > INT32_MAX
        Int64ToString(Value,ValueLen,*(size_t*)Data,0);
#else
        IntToString(Value,ValueLen,*(size_t*)Data,0);
#endif
        break;

    case TYPE_INT:
        if ((Type & TUNIT_MASK)==TUNIT_PERCENT)
        {
            f.Num = *(int*)Data;
            f.Den = PERCENT_ONE;
            FractionToString(Value,ValueLen,&f,-1,1);
        }
        else
        if (Display && (Type & TUNIT_MASK)==TUNIT_BYTERATE)
            ByteRateToString(Value,ValueLen,*(int*)Data);
        else
        {
            IntToString(Value,ValueLen,*(int*)Data,0);
            if (Display && (Type & TUNIT_MASK)==TUNIT_KBYTE)
                tcscat_s(Value,ValueLen,T(" KB"));
        }
        break;

    case TYPE_INT8:
        IntToString(Value,ValueLen,*(uint8_t*)Data,0);
        break;

    case TYPE_INT16:
        IntToString(Value,ValueLen,*(int16_t*)Data,0);
        break;

    case TYPE_GUID:
        GUIDToString(Value,ValueLen,(const cc_guid*)Data);
        break;

    case TYPE_DBNO: //TODO: support for 64 bits
    case TYPE_DATETIME:
    case TYPE_INT32:
        IntToString(Value,ValueLen,*(int32_t*)Data,(Type & TUNIT_MASK)==TUNIT_HEX);
        break;

    case TYPE_INT64:
        Int64ToString(Value,ValueLen,*(int64_t*)Data,(Type & TUNIT_MASK)==TUNIT_HEX);
        break;

    case TYPE_BOOL_BIT:
    case TYPE_BOOLEAN:
        IntToString(Value,ValueLen,(int32_t)*(bool_t*)Data,0);
        break;

    case TYPE_FOURCC:
        FourCCToString(Value,ValueLen,*(fourcc_t*)Data);
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
        FractionToString(Value,ValueLen,&f,0,3);
        break;

    case TYPE_FRACTION:
        if ((Type & TUNIT_MASK)==TUNIT_PERCENT)
            FractionToString(Value,ValueLen,(cc_fraction*)Data,Display?1:-1,2);
        else
        if (Display)
            FractionToString(Value,ValueLen,(cc_fraction*)Data,0,3); //fps needs 3 decimal
        else
            stprintf_s(Value,ValueLen,T("%d:%d"),((cc_fraction*)Data)->Num,((cc_fraction*)Data)->Den);
        break;

    case TYPE_PIN:
        PinToString(Value,ValueLen,(pin*)Data,NULL);
        break;

    case TYPE_NODE:
        NodeToString(Value,ValueLen,*(node**)Data,NULL);
        break;

    case TYPE_STRING:
        tcscpy_s(Value,ValueLen,(tchar_t*)Data);
        break;
    }

    return 1;
}

void TextAttribEx(textwriter* Text, const tchar_t* Name, const void* Data, size_t Size, dataflags Type)
{
    tchar_t Value[MAXDATA+64];
    tchar_t *c;
    size_t i;

    if (DataToString(Value,TSIZEOF(Value),Data,Size,Type))
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

           TextPrintf(Text,T(" %s=\"%s\""),Name,Value);
    }
}

void TextAttrib(textwriter* Text, const tchar_t* Name, const void* Data, dataflags Type)
{
    TextAttribEx(Text,Name,Data,0,Type);
}

void TextElementBegin(textwriter* Out, textwriter* In, const tchar_t* Element)
{
    if (!In->HasChild)
    {
        In->HasChild = 1;
        if (In->InsideContent)
            TextWrite(In,T(">"));
        else
            TextWrite(In,T(">\n"));
    }
    Out->CC = In->CC;
    Out->Stream = In->Stream;
    Out->HasChild = 0;
    Out->SafeFormat = In->SafeFormat;
    Out->InsideContent = In->Deep==0;
    Out->Deep = In->InsideContent ? 0 : In->Deep+2;
    Out->Element = Element;
    TextPrintf(Out,T("%*c%s"),In->Deep?Out->Deep:0, '<',Element);
}

void TextElementEnd(textwriter* Text)
{
    if (Text->HasChild)
    {
        if (Text->InsideContent)
            TextPrintf(Text,T("</%s> "),Text->Element);
        else
            TextPrintf(Text,T("%*c/%s>\n"),Text->Deep,'<',Text->Element);
    }
    else
    {
        if (Text->SafeFormat)
            TextPrintf(Text,T("></%s>"),Text->Element);
        else
            TextWrite(Text,T("/>"));
        if (!Text->InsideContent)
            TextWrite(Text,T("\n"));
    }
}

void TextElementEndData(textwriter* Text, const tchar_t *Value)
{
    if (Text->HasChild)
        TextPrintf(Text,T("%s%*c/%s>\n"),Value, Text->Deep,'<',Text->Element);
    else
        TextPrintf(Text,T(">%s</%s>\n"), Value, Text->Element);
}

void TextElementAppendData(textwriter* Text, const tchar_t *Value)
{
    if (Text->Deep==1 && tcsisame_ascii(Value,T(" ")))
        return; // root element
    if (Text->HasChild)
        TextWrite(Text,Value);
    else {
        TextPrintf(Text,T(">%s"), Value);
        Text->HasChild = 1;
        Text->Deep = 0;
    }
}

void TextElementXML(parsercontext *Context, textwriter* Text, const tchar_t* Root)
{
    assert(Text->Stream);
    Text->CC = Context->ToUTF8;
    Text->Element = Root;
    Text->Deep = 1;
    Text->HasChild = 0;
    Text->InsideContent = 0;
    Text->SafeFormat = 0;
    TextPrintf(Text,T("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<%s"),Root);
}

NOINLINE bool_t ExprSkipAfter(const tchar_t** p,int ch)
{
    const tchar_t* s = *p;
    for (;*s;++s)
        if ((ch && *s == ch) || (!ch && IsSpace(*s)))
        {
            *p = s+1;
            if (!ch) ExprSkipSpace(p); // skip other spaces too
            return 1;
        }

    *p = s;
    return 0;
}

NOINLINE void ExprSkipSpace(const tchar_t** p)
{
    const tchar_t* s = *p;
    while (IsSpace(*s))
        ++s;
    *p = s;
}

NOINLINE void ExprTrimSpace(tchar_t** p)
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

static NOINLINE bool_t ReadHex(const tchar_t** p,intptr_t* Out,bool_t RGB, bool_t Neg);

NOINLINE bool_t ExprIsTokenEx(const tchar_t** p,const tchar_t* Name,...)
{
    const tchar_t* s = *p;
    bool_t Long = 0;
    va_list Arg;
    va_start(Arg, Name);

    if (!*Name)
        return 0;

    ExprSkipSpace(&s);

    while (*Name && *s)
    {
        if (*Name == ' ')
        {
            ExprSkipSpace(&s);
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
                    if ((!IntSize && !ExprIsInt64(&s,va_arg(Arg,int64_t*)))||(IntSize && !ExprIsInt64Ex(&s,IntSize,va_arg(Arg,int64_t*))))
                        break;
                }
                else
                {
                    if ((!IntSize && !ExprIsInt(&s,va_arg(Arg,intptr_t*)))||(IntSize && !ExprIsIntEx(&s,IntSize,va_arg(Arg,intptr_t*))))
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

NOINLINE bool_t ExprIsToken(const tchar_t** p,const tchar_t* Name)
{
    return ExprIsTokenEx(p,Name);
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

bool_t BufferToBase64(tchar_t *Out, size_t OutLen, const uint8_t *Buffer, size_t BufferSize)
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

NOINLINE size_t ExprIsBase64(const tchar_t** p,uint8_t* Out,size_t OutSize)
{
    size_t n;
    size_t v=0;
    uint8_t* Out0 = Out;

    ExprSkipSpace(p);
    for (n=0;**p;++n,++(*p))
    {
        if (**p == '=')
        {
            while (**p == '=') ++(*p);
            break;
        }

        if (**p < 43 || **p >= (tchar_t)(43+sizeof(Base64)) || Base64[**p-43] == 0x80)
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

NOINLINE bool_t ExprIsName(const tchar_t** p,tchar_t* Out,size_t OutLen, const tchar_t* Delimiter)
{
    bool_t Found = 0;

    ExprSkipSpace(p);
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

NOINLINE bool_t ExprIsSymbol(const tchar_t** p,int ch)
{
    if (**p)
    {
        ExprSkipSpace(p);
        if (**p == ch)
        {
            ++(*p);
            return 1;
        }
    }
    return 0;
}

NOINLINE bool_t ExprIsSymbol2(const tchar_t** p,int ch, int ch2)
{
    ExprSkipSpace(p);
    if ((*p)[0] == ch && (*p)[1]==ch2)
    {
        (*p)+=2;
        return 1;
    }
    return 0;
}

NOINLINE void ExprParamEnd(const tchar_t** p)
{
    ExprIsSymbol(p,')');
}

NOINLINE bool_t ExprParamNext(const tchar_t** p)
{
    if (ExprIsSymbol(p,','))
        return 1;
    ExprParamEnd(p);
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

NOINLINE bool_t ExprIsHex(const tchar_t** p,intptr_t* Out)
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

NOINLINE bool_t ExprIsRGB(const tchar_t** p,intptr_t* Out)
{
    const tchar_t* s = *p;
    if (*(s++) == '#' && ReadHex(&s,Out,1,0))
    {
        *p = s;
        return 1;
    }
    return 0;
}

NOINLINE bool_t ExprIsFourCC(const tchar_t** p,intptr_t* Out)
{
    const tchar_t* s = *p;
    if (*(s++) == '@' && *(s++) == '\'' && ExprSkipAfter(&s,'\'') && (s <= *p+7))
    {
        tchar_t v[16];
        tcsncpy_s(v,TSIZEOF(v),*p+2,s-*p-3);
        *Out = StringToFourCC(v,0);
        *p = s;
        return 1;
    }
    return 0;
}

NOINLINE bool_t ExprIsIntEx(const tchar_t** p,int Size,intptr_t* Out)
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

NOINLINE bool_t ExprIsInt64Ex(const tchar_t** p,int Size,int64_t* Out)
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

NOINLINE bool_t ExprIsInt(const tchar_t** p,intptr_t* Out)
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

NOINLINE bool_t ExprIsInt64(const tchar_t** p,int64_t* Out)
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

NOINLINE bool_t ExprIsFrac(const tchar_t** p,cc_fraction* Out)
{
    int64_t Num,Den;
    intptr_t v;

    if (!ExprIsInt(p,&v))
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

    SimplifyFrac(Out,Num,Den);
    return 1;
}

bool_t ExprIsPoint(const tchar_t** p, cc_point* Out)
{
    intptr_t x,y;
    if (ExprIsTokenEx(p,T("{ %ld , %ld }"),&x,&y))
    {
        Out->x = (int)x;
        Out->y = (int)y;
        return 1;
    }
    return 0;
}

#define MAX_GCD_ITER 128

void SimplifyFrac(cc_fraction* f, int64_t Num, int64_t Den)
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

void DoubleToFrac(cc_fraction* f,int64_t v)
{
    if (!v)
    {
        f->Num = 0;
        f->Den = 1;
    }
    else
    {
        int e = (int)(((v >> 52) & 0xFFF) - 1023);
        SimplifyFrac(f,(v & LL(0xFFFFFFFFFFFFF))|LL(0x10000000000000),LL(1)<<(52-e));
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
        FourCCToString(Name,NameLen,(fourcc_t)Id);
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
        Id = StringToInt(Token+1,1);
    else
    {
        Id = Node_FindParam(p,Token);
        if (!Id)
            Id = StringToFourCC(Token,0);
    }
    return NodeDataDef(p,Id,DataDef);
}

void TextSerializeNode(textwriter* Text, node* p, uint_fast32_t Mask, uint_fast32_t Filter)
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

                    TextAttribEx(Text,Name,Data,Size,i->Flags);

                    /* TODO: support dumping packetformat information (example for crash.txt)
                    if ((i->Flags & TYPE_MASK) == TYPE_PACKET && Node_Get(p,i->Id|PIN_FORMAT,Data,sizeof(packetformat)) == ERR_NONE)
                    {
                        tcscat_s(Name,TSIZEOF(Name),T("_Format"));
                        TextAttrib(Text,Name,Data,TYPE_PACKETFORMAT);
                    }
                    */
                }
            }
        }

    ArrayClear(&List);
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
    ClassId = StringToFourCC(Token,0);
    No = StringToInt(Token+4,1);
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
        FourCCToString(Element,TSIZEOF(Element),Node_ClassId(p));
        TextElementBegin(&Level,Text,Element);
        TextAttrib(&Level,T("Addr"),&p,TYPE_PTR);
        TextSerializeNode(&Level,p,TFLAG_NODUMP,0);
        ArrayInit(&Children);
        if (Node_GET(p,NODE_CHILDREN,&Children)==ERR_NONE)
            for (i=ARRAYBEGIN(Children,node*);i!=ARRAYEND(Children,node*);++i)
                NodeDumpObject(*i,&Level);
        ArrayClear(&Children);
        TextElementEnd(&Level);
    }
    TRY_END
}

void NodeDump(nodecontext* p,textwriter* Text)
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
