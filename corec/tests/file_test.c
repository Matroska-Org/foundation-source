#include "corec/helpers/file/file.h"

#include <stdio.h>
void DebugMessage(const tchar_t* Msg,...)
{
    va_list Args;
    tchar_t Buffer[1024];

    va_start(Args,Msg);
    vstprintf_s(Buffer,TSIZEOF(Buffer), Msg, Args);
    va_end(Args);
    tcscat_s(Buffer,TSIZEOF(Buffer),T("\r\n"));

#ifdef UNICODE
    fprintf(stderr, "%ls", Buffer);
#else
    fprintf(stderr, "%s", Buffer);
#endif
}

int main(int argc,char** argv)
{
    nodecontext Context;
    NodeContext_Init(&Context,NULL,NULL,NULL);

    NodeContext_Done(&Context);
    return 0;
}
