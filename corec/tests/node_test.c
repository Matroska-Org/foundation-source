#include "corec/node/node.h"

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
    node* p[10000];
    int i;
    nodecontext Context;
    NodeContext_Init(&Context,NULL,NULL,NULL);

    for (i=0;i<10000;++i)
        p[i] = NodeCreate(&Context,NODE_CLASS);

    for (i=0;i<1000000;++i)
        NodeDelete(NodeCreate(&Context,NODE_CLASS));

    for (i=0;i<10000;++i)
        NodeDelete(p[i]);

    NodeContext_Done(&Context);
    return 0;
}
