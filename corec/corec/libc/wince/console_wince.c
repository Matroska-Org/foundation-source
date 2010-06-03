#define WIN32_LEAN_AND_MEAN
#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

extern int main(int argc,char** argv);

int __cdecl wprintf(const WCHAR* fmt,...)
{
    WCHAR sw[1024];
	int i;
	va_list Args;
	va_start(Args,fmt);
	i=vswprintf(sw,fmt,Args);
	va_end(Args);
    OutputDebugString(sw);
    return i;
}

int __cdecl printf(const char* fmt,...)
{
    WCHAR sw[1024];
    char s[1024];
	int i;
	va_list Args;
	va_start(Args,fmt);
	i=vsprintf(s,fmt,Args);
	va_end(Args);
    MultiByteToWideChar(CP_ACP,0,s,-1,sw,1024);
    OutputDebugString(sw);
    return i;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hParent,WCHAR* Cmd,int CmdShow)
{
    char *i,*j;
    char Cmd8[1024];
    int n=1;
    char* argv[64];

    argv[0] = "";

    WideCharToMultiByte(CP_ACP,0,Cmd,-1,Cmd8,sizeof(Cmd8),0,0);
    for (i=Cmd8;*i;)
    {
        while (isspace(*i)) ++i;
        if (*i)
        {
            int quote = 0;
            argv[n++] = i;
            j = i;
            for (;*i && (quote || !isspace(*i));++i)
            {
                if (*i=='"')
                    quote = !quote;
                else
                    *(j++) = *i;
            }
            if (*i) ++i;
            *j = 0;
        }
    }

    argv[n] = NULL;

	//_wfreopen(L"\\stdin",L"rb",stdin);
	//_wfreopen(L"\\stdout",L"w+b",stdout);

    return main(n,argv);
}
