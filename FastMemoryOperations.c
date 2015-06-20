#include "stdafx.h"

VOID FastFillMemory(LPVOID addr, SIZE_T len, BYTE value) {
	__asm {
		mov edi, addr
		mov ecx, len
		mov al, value
__zerobyte:
		mov [edi], al
		inc edi
		loop __zerobyte
	}
}

LPVOID FastCopyMemory(LPVOID addr, SIZE_T len) {
	LPVOID newAddr = HeapAlloc(GetProcessHeap(), 0, len);
	__asm {
		mov eax, newAddr
		/*
		// push len
		mov ecx, len
		push ecx
		// push  0
		xor eax,eax
		push eax
		// push GetProcessHeap()
		call [GetProcessHeap]
		push eax
		// heap alloc
		call [HeapAlloc]
		// eax now stores ptr to new mem
		// clear stack
		add esp, 12
			*/
		// copy
		mov esi, addr
		mov edi, eax
		mov ecx, len
		cld
		rep movsb
		// eax is still pointing to new mem
	}
}

VOID FastZeroMemory(LPVOID addr, SIZE_T len) {
	FastFillMemory(addr, len, 0);
}