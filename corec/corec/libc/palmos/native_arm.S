@*****************************************************************************
@* 
@* Copyright (c) 2008-2010, CoreCodec, Inc.
@* All rights reserved.
@*
@* Redistribution and use in source and binary forms, with or without
@* modification, are permitted provided that the following conditions are met:
@*     * Redistributions of source code must retain the above copyright
@*       notice, this list of conditions and the following disclaimer.
@*     * Redistributions in binary form must reproduce the above copyright
@*       notice, this list of conditions and the following disclaimer in the
@*       documentation and/or other materials provided with the distribution.
@*     * Neither the name of CoreCodec, Inc. nor the
@*       names of its contributors may be used to endorse or promote products
@*       derived from this software without specific prior written permission.
@*
@* THIS SOFTWARE IS PROVIDED BY CoreCodec, Inc. ``AS IS'' AND ANY
@* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
@* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
@* DISCLAIMED. IN NO EVENT SHALL CoreCodec, Inc. BE LIABLE FOR ANY
@* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
@* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
@* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
@* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
@* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
@* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
@*
@*****************************************************************************

.globl	SonyInvalidateDCache
.globl	SonyCleanDCache
.globl	SysGetEntryAddresses
.globl	SysFindModule
.globl  HALDelay
.globl  HALDisplayWake
.globl  HALDisplayOff_TREO650
.globl	SysLoadModule
.globl	SysUnloadModule
.globl  PceCall
.globl  PalmCall
.globl  PalmCall2

AddrPceCall: .DC.L PceCall

@r0,r1
SonyCleanDCache:
	stmdb sp!, {r9, lr}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	swi	0xb5
	ldmia sp!, {r9, pc}  

@r0,r1
SonyInvalidateDCache:
	stmdb sp!, {r9, lr}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	swi	0xb4
	ldmia sp!, {r9, pc}  

@r0,r1,r2,r3
SysGetEntryAddresses:
	stmdb sp!, {r9, lr}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	ldr	ip, [r9, #-8]
	mov lr, pc
	ldr	pc, [ip, #2104]
	ldmia sp!, {r9, pc}  

@r0,r1,r2,r3,[sp]
SysFindModule:
	ldr ip, [sp, #0]
	stmdb sp!, {r9, lr}
	stmdb sp!, {ip}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	ldr	ip, [r9, #-8]
	mov lr, pc
	ldr	pc, [ip, #2092]
	add sp, sp, #4
	ldmia sp!, {r9, pc}  

@r0,r1,r2,r3,[sp]
SysLoadModule:
	ldr ip, [sp, #0]
	stmdb sp!, {r9, lr}
	stmdb sp!, {ip}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	ldr	ip, [r9, #-8]
	mov lr, pc
	ldr	pc, [ip, #2176]
	add sp, sp, #4
	ldmia sp!, {r9, pc}  

@r0
SysUnloadModule:
	stmdb sp!, {r9, lr}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	ldr	ip, [r9, #-8]
	mov lr, pc
	ldr	pc, [ip, #2312]
	ldmia sp!, {r9, pc}  

@r0
HALDelay:
	stmdb sp!, {r9, lr}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	ldr	ip, [r9, #-4]
	mov lr, pc
	ldr	pc, [ip, #0x28]
	ldmia sp!, {r9, pc}  

HALDisplayWake:
	stmdb sp!, {r9, lr}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	ldr	ip, [r9, #-4]
	mov lr, pc
	ldr	pc, [ip, #0x40]
	ldmia sp!, {r9, pc}  

HALDisplayOff_TREO650:
	stmdb sp!, {r9, lr}
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	ldr	ip, [r9, #-4]
	mov lr, pc
	ldr	pc, [ip, #0x358]
	ldmia sp!, {r9, pc}  

PalmCall:
	stmdb sp!, {r9, lr}
	mov ip, r0
	mov r0, r1
	mov r1, r2
	mov r2, r3
	ldr r3, [sp, #8]
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	mov lr, pc
	mov pc, ip
	ldmia sp!, {r9, pc}  

PalmCall2:
	stmdb sp!, {r9, lr}

	sub sp, sp, #16
	ldr ip, [sp, #8+16+4]
	ldr r9, [sp, #8+16+8]
	str ip, [sp, #0]
	str r9, [sp, #4]
	ldr ip, [sp, #8+16+12]
	ldr r9, [sp, #8+16+16]
	str ip, [sp, #8]
	str r9, [sp, #12]

	mov ip, r0
	mov r0, r1
	mov r1, r2
	mov r2, r3
	ldr r3, [sp, #8+16]
	ldr r9, AddrPceCall
	ldr r9, [r9, #0]
	mov lr, pc
	mov pc, ip

	add sp, sp, #16
	ldmia sp!, {r9, pc}  
