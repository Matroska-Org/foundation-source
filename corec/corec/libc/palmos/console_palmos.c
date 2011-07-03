#include "corec/corec.h"
#include "pace.h"

extern int main(int,char**);
extern int __cdecl vsprintf( char *buffer, const char *format, va_list argptr );

static void putline(const char* s,int n)
{
    RectangleType r;
    WinHandle h = WinGetActiveWindow();

    r.topLeft.x = 0;
    r.topLeft.y = 15;
    r.extent.x = 160;
    r.extent.y = 160-15; 

    WinCopyRectangle(h,h,&r,0,0,winPaint);

    r.topLeft.x = 0;
    r.topLeft.y = 160-15;
    r.extent.x = 160;
    r.extent.y = 15; 

    WinEraseRectangle(&r, 0);
    WinDrawChars(s, (Int16)n, 1, 160-16+1);
}

int __cdecl puts(const char* s)
{
    int n=strlen(s);
    while (*s)
    {
        const char* x = strchr(s,10);
        if (!x) x = s+strlen(s);
        putline(s,x-s);
        if (*x==10) ++x;
        s=x;
    }
    return n;
}

int __cdecl printf(const char* fmt,...)
{
    char s[1024];
	va_list Args;
	va_start(Args,fmt);
	vsprintf(s,fmt,Args);
	va_end(Args);
    return puts(s);
}

UInt32 PilotMain(UInt16 launchCode, MemPtr launchParameters, UInt16 launchFlags)
{
	if (launchCode == sysAppLaunchCmdNormalLaunch ||
		launchCode == sysAppLaunchCmdOpenDB ||
		launchCode == sysAppLaunchCmdCustomBase)
    {
        char* argv[1] = {NULL};
        RectangleType r;
        r.topLeft.x = 0;
        r.topLeft.y = 0;
        r.extent.x = 160;
        r.extent.y = 160; 
        WinEraseRectangle(&r, 0);
        main(0,argv);
    	SysTaskDelay(100);
	}
	else
	if (launchCode == sysAppLaunchCmdNotify)
    {
    }
	return errNone;
}
