#include "corec/node/node.h"

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
