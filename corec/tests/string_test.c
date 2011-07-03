#include "corec/str/str.h"

#if defined(UNICODE)
#include "corec/helpers/charconvert/charconvert.h"
charconv *ToStr;
charconv *FromStr;
#endif

#include <stdio.h>

void test_float(const char *Format, double Value)
{
    char String[512];
    tchar_t tString[512];
    const tchar_t *format;
    char *corec;
#if defined(UNICODE)
    tchar_t tFormat[64];
    char corecString[512];
#endif

    sprintf(String,Format,Value);
#if defined(UNICODE)
    CharConvTS(FromStr,tFormat,TSIZEOF(tFormat),Format);
    format = tFormat;
#else
    format = Format;
#endif
    stprintf_s(tString,TSIZEOF(tString),format,Value);

#if defined(UNICODE)
    CharConvST(ToStr,corecString,sizeof(corecString),tString);
    corec = corecString;
#else
    corec = tString;
#endif

    printf("Format %-8s: ",Format);
    if (stricmp(String,corec) != 0)
        printf("failed '%s' != '%s'\n",String,corec);
    else
        printf("passed '%s'\n",String);
}

void test_integer(const char *Format, int Value)
{
    char String[512];
    tchar_t tString[512];
    const tchar_t *format;
    char *corec;
#if defined(UNICODE)
    tchar_t tFormat[64];
    char corecString[512];
#endif

    sprintf(String,Format,Value);
#if defined(UNICODE)
    CharConvTS(FromStr,tFormat,TSIZEOF(tFormat),Format);
    format = tFormat;
#else
    format = Format;
#endif
    stprintf_s(tString,TSIZEOF(tString),format,Value);

#if defined(UNICODE)
    CharConvST(ToStr,corecString,sizeof(corecString),tString);
    corec = corecString;
#else
    corec = tString;
#endif

    printf("Format %-8s: ",Format);
    if (stricmp(String,corec) != 0)
        printf("failed '%s' != '%s'\n",String,corec);
    else
        printf("passed '%s'\n",String);
}

void test_ptr(const char *Format, intptr_t Value)
{
    char String[512];
    tchar_t tString[512];
    const tchar_t *format;
    char *corec;
#if defined(UNICODE)
    tchar_t tFormat[64];
    char corecString[512];
#endif

    sprintf(String,Format,Value);
#if defined(UNICODE)
    CharConvTS(FromStr,tFormat,TSIZEOF(tFormat),Format);
    format = tFormat;
#else
    format = Format;
#endif
    stprintf_s(tString,TSIZEOF(tString),format,Value);

#if defined(UNICODE)
    CharConvST(ToStr,corecString,sizeof(corecString),tString);
    corec = corecString;
#else
    corec = tString;
#endif

    printf("Format %-8s: ",Format);
    if (stricmp(String,corec) != 0)
        printf("failed '%s' != '%s'\n",String,corec);
    else
        printf("passed '%s'\n",String);
}

int main(int argc,char** argv)
{
#if defined(UNICODE)
    ToStr = CharConvOpen(NULL,CHARSET_DEFAULT);
    FromStr = CharConvOpen(CHARSET_DEFAULT,NULL);
#endif

    test_ptr("%p",(intptr_t)argv);
    test_ptr("%p",0xA1234);
    test_ptr("%10p",0xA1234);
    test_ptr("%-10p",0xA1234);

    test_float("%g",100.25);
    test_float("%f",100.25);
    test_float("%f",0.25);
    test_float("%.014g",100.25);
    test_float("%14g",100.25);
    test_float("%15f",99.2500000000001); // fail
    test_float("%15g",10.2500000000001); // fail
    test_float("%14g",-0.25); // fail
    test_float("%14.4g",0.25); // fail
    test_float("%14.4g",0.2512345); // fail
    test_float("%14.0g",0.25); // fail
    test_float("%015.5g",0.25); // fail
    test_float("%.014g",1000000000.25);
    test_float("%03.014g",10.25);
    test_float("%.14f",10.250000000000005); // fail
    test_float("%.15f",10.250000000000005); // fail
    test_float("%.1f",.99); // fail
    test_float("%03.1f",10.25); // fail
    test_float("%03.1f",-10.25); // fail
    test_float("%03.1g",-10.25); // fail

    test_integer("%6d",25);
    test_integer("%06d",25);
    test_integer("%06d",-25);

#if 0 // integer too long
    test_integer("%15d",25);
    test_integer("%015d",25);
#endif

#if 0 // length align not supported with numbers
    test_integer("%-6d",25);
    test_integer("%-15d",25);
    test_float("%-14g",0.25);
#endif

#if 0 // infinite loops !
    test_float("%e",100.25);
    test_float("%E",100.25);
#endif

#if defined(UNICODE)
    CharConvClose(ToStr);
    CharConvClose(FromStr);
#endif

    return 0;
}
