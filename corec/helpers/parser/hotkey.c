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

typedef struct hotkeyname
{
    uint32_t Key;
    const tchar_t* XML;

} hotkeyname;

static const hotkeyname HotKeyName[] =
{
    // first the masks
    { HOTKEY_SHIFT,             T("Shift")},
    { HOTKEY_CTRL,              T("Ctrl")},
    { HOTKEY_ALT,               T("Alt")},
    { HOTKEY_WIN,               T("Win")},
    { HOTKEY_HOLD,              T("Hold")},

    { HOTKEY_MEDIA_STOP,        T("Stop")},
    { HOTKEY_MEDIA_PLAY,		T("Play")},
    { HOTKEY_MEDIA_PREV,		T("Prev")},
    { HOTKEY_MEDIA_NEXT,		T("Next")},
    { HOTKEY_MEDIA_VOLUME_UP,  	T("VolUp")},
    { HOTKEY_MEDIA_VOLUME_DOWN, T("VolDown")},
    { HOTKEY_MEDIA_MUTE,		T("Mute")},
    { HOTKEY_MEDIA_MOVEFWD,		T("MoveFwd")},
    { HOTKEY_MEDIA_MOVEBACK,	T("MoveBack")},

    { HOTKEY_DPAD_ACTION,		T("Action")},
    { HOTKEY_DPAD_LEFT, 		T("Left")},
    { HOTKEY_DPAD_RIGHT,		T("Right")},
    { HOTKEY_DPAD_UP,		    T("Up")},
    { HOTKEY_DPAD_DOWN,         T("Down")},

    { HOTKEY_ENTER,             T("Enter")},
    { HOTKEY_SPACE,             T("Space")},
    { HOTKEY_ESCAPE,            T("Escape")},
    { HOTKEY_TABNEXT,           T("TabNext")},
    { HOTKEY_TABPREV,           T("TabPrev")},
    { HOTKEY_BACKSPACE,         T("BackSpace")},
    { HOTKEY_DELETE,            T("Delete")},

    { HOTKEY_SELECT,            T("Select")},
    { HOTKEY_START,             T("Start")},
    { HOTKEY_TRIANGLE,          T("Triangle")},
    { HOTKEY_SQUARE,            T("Square")},
    { HOTKEY_CIRCLE,            T("Circle")},
    { HOTKEY_CROSS,             T("Cross")},

    {0,                         NULL},

    // should be right after {0}
    { HOTKEY_APP_FIRST,         T("App%d")},
};

static const tchar_t* GetHotKeyName(const hotkeyname* i, hotkeygetname GetName, void* GetNameParam)
{
    if (GetName)
        return GetName(GetNameParam,i->Key);
    return i->XML;
}

void HotKeyToString(tchar_t* Out, size_t OutLen, uint32_t HotKey, hotkeygetname GetName, void* GetNameParam)
{
    const hotkeyname* i = HotKeyName;
    if (!GetName)
    {
        // force #<N> XML exporting to make import safer...
        stprintf_s(Out,OutLen,T("#%x"),(int)HotKey);
        return;
    }

    *Out = 0;

    for (;i->Key > HOTKEY_MASK;++i)
        if (HotKey & i->Key)
        {
            tcscat_s(Out,OutLen,GetHotKeyName(i,GetName,GetNameParam));
            tcscat_s(Out,OutLen,T("+"));
        }

    HotKey &= HOTKEY_MASK;

    for (;i->Key;++i)
        if (HotKey == i->Key)
        {
            tcscat_s(Out,OutLen,GetHotKeyName(i,GetName,GetNameParam));
            return;
        }

	if ((HotKey >= '0' && HotKey <= '9') || (HotKey >= 'A' && HotKey <= 'Z'))
		stcatprintf_s(Out,OutLen,T("%c"),HotKey);
    else
    if (HotKey >= HOTKEY_FUNC_FIRST && HotKey <= HOTKEY_FUNC_LAST)
		stcatprintf_s(Out,OutLen,T("F%d"),HotKey-HOTKEY_FUNC_FIRST+1);
    else
    if (HotKey >= HOTKEY_APP_FIRST && HotKey <= HOTKEY_APP_LAST)
        stcatprintf_s(Out,OutLen,GetHotKeyName(i+1,GetName,GetNameParam),HotKey-HOTKEY_APP_FIRST+1);
    else
		stcatprintf_s(Out,OutLen,T("#%02X"),HotKey);
}

uint32_t StringToHotKey(const tchar_t* In)
{
    const tchar_t* m;
    const hotkeyname* i;
    uint32_t HotKey = 0;
    size_t n;

    ExprSkipSpace(&In);

    while ((m = tcschr(In,'+')) != NULL)
    {
        n = m-In;
        while (n>0 && IsSpace(In[n-1]))
            --n;

        for (i = HotKeyName;i->Key > HOTKEY_MASK;++i)
            if (tcsnicmp(In,i->XML,n)==0)
            {
                HotKey |= i->Key;
                break;
            }

        In = m+1;
    }

    if (!In[0])
        return 0;

    for (i = HotKeyName;i->Key;++i)
        if (tcsisame_ascii(In,i->XML))
            return HotKey | i->Key;

    if ((In[0] == 'f' || In[0] == 'F') && IsDigit(In[1]))
        return HotKey | (HOTKEY_FUNC_FIRST + StringToInt(In+1,0) - 1);

    if (In[0] == '#')
        return HotKey | StringToInt(In+1,1);

    n = tcslen(In);
    if (n>1 && IsDigit(In[n-1]))
    {
        while (n>0 && IsDigit(In[n-1]))
            --n;
        return HotKey | (HOTKEY_APP_FIRST + StringToInt(In+n,0) - 1);
    }

	return HotKey | In[0];
}
