#include "corec/helpers/file/file.h"

int main(int argc,char** argv)
{
    nodecontext Context;
    NodeContext_Init(&Context,NULL,NULL,NULL);

    NodeContext_Done(&Context);
    return 0;
}
