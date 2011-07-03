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

#ifndef __HOTKEY_H
#define __HOTKEY_H

#define HOTKEY_MASK					0x0000FFFF
#define HOTKEY_SHIFT				0x00010000
#define HOTKEY_CTRL					0x00020000
#define	HOTKEY_ALT					0x00040000
#define	HOTKEY_WIN					0x00080000
#define	HOTKEY_HOLD					0x00100000

#define HOTKEY_MEDIA_STOP			0x00009000
#define HOTKEY_MEDIA_PLAY			0x00009001
#define HOTKEY_MEDIA_PREV			0x00009002
#define HOTKEY_MEDIA_NEXT			0x00009003
#define HOTKEY_MEDIA_VOLUME_UP  	0x00009004
#define HOTKEY_MEDIA_VOLUME_DOWN  	0x00009005
#define HOTKEY_MEDIA_MUTE         	0x00009006
#define HOTKEY_MEDIA_MOVEFWD       	0x00009007
#define HOTKEY_MEDIA_MOVEBACK      	0x00009008

#define HOTKEY_DPAD_ACTION			0x00009010
#define HOTKEY_DPAD_LEFT 			0x00009011
#define HOTKEY_DPAD_RIGHT 			0x00009012
#define HOTKEY_DPAD_UP		        0x00009013
#define HOTKEY_DPAD_DOWN            0x00009014

#define HOTKEY_ENTER                0x00009020
#define HOTKEY_SPACE                0x00009021
#define HOTKEY_ESCAPE               0x00009022
#define HOTKEY_TABNEXT              0x00009023
#define HOTKEY_TABPREV              0x00009024
#define HOTKEY_DELETE               0x00009025
#define HOTKEY_BACKSPACE            0x00009026

#define HOTKEY_FUNC_FIRST           0x00009030
#define HOTKEY_FUNC_LAST            0x0000903F

#define HOTKEY_APP_FIRST            0x00009040
#define HOTKEY_APP_LAST             0x0000904F

#define HOTKEY_MOD_SHIFT	    	0x00009050
#define HOTKEY_MOD_CTRL				0x00009051
#define	HOTKEY_MOD_ALT				0x00009052
#define	HOTKEY_MOD_WIN				0x00009053

#define KEYPAD_KEY_FIRST            0x00009060
#define KEYPAD_KEY_LAST             0x0000906F

#define HOTKEY_SELECT               0x00009080
#define HOTKEY_START                0x00009081
#define HOTKEY_TRIANGLE             0x00009082
#define HOTKEY_SQUARE               0x00009083
#define HOTKEY_CIRCLE               0x00009084
#define HOTKEY_CROSS                0x00009085

typedef	const tchar_t* (*hotkeygetname)(void* Param,uint32_t Key);

NODE_DLL void HotKeyToString(tchar_t* Out, size_t OutLen, uint32_t HotKey, hotkeygetname GetName, void* GetNameParam);
NODE_DLL uint32_t StringToHotKey(const tchar_t* In);

#endif
