#include "stdafx.h"

DWORD ErrorReportInternal(LPWSTR lpszWhere, BOOL isFatal);

DWORD FatalError(LPWSTR lpszWhere) {
	return ErrorReportInternal(lpszWhere, TRUE);
}

DWORD ErrorReport(LPWSTR lpszWhere) {
	return ErrorReportInternal(lpszWhere, FALSE);
}

DWORD ErrorReportInternal(LPWSTR lpszWhere, BOOL isFatal) {
	// Retrieve the system error message for the last-error code
	WCHAR* lpMsgBuf;
	WCHAR* lpDisplayBuf;
	DWORD dw = GetLastError();
	if(!dw) return 0;

	// Fetch error message
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf,
		0, NULL);
	int res = GetLastError();

	// Decorate it
	DWORD_PTR pArgs[] = {
		(DWORD_PTR)dw,
		(DWORD_PTR)lpszWhere,
		(DWORD_PTR)lpMsgBuf
	};
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_ARGUMENT_ARRAY |
		FORMAT_MESSAGE_FROM_STRING,
		lpszWhere
			? L"Error %1!d! while %2: %3"
			: L"Error %1!d!: %3",
		0, 0,
		(LPWSTR)&lpDisplayBuf,
		0, (va_list*)pArgs);
	res = GetLastError();

	// Show It
	MessageBox(NULL, lpDisplayBuf, NULL, MB_ICONERROR | MB_OK); 

	// GTFO
	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	if(isFatal) ExitProcess(dw); 
	return dw;
}

/*
BOOL HeapTruncate(LPVOID ptr, SIZE_T size) {
	LPVOID smth = HeapReAlloc(GetProcessHeap(), 0, ptr, size);
	if(!smth) {
		MessageBox(0, L"ќй пиздец", NULL, MB_OK);
	}
	return TRUE;
}
*/
/*
unsigned int widelen(const wchar_t *Str) {
	__asm {
		mov esi, Str
		xor eax, eax // eax=0
wcslen_m1:
		mov bx, [esi]
		// move pointer
		inc esi
		inc esi
		// inc counter
		inc eax
		test bx, bx
		jnz wcslen_m1
		// do not count the last
		dec eax
		// eax now holds the answer
	}
}
*/
wchar_t* widecat(wchar_t* s1, const wchar_t* s2) {
	wchar_t* s = s1;
	while(*s) ++s;
	do { *(s++) = *s2; } while(*(s2++));
	return s1;
}
/*
void* memset(void* ptr, char value, size_t num) {
	FastFillMemory(ptr, num, value);
	return ptr;
}
*/
void*__cdecl malloc(size_t size) {
	return HeapAlloc(GetProcessHeap(), 0, size);
}

void __cdecl free(void* ptr) {
	HeapFree(GetProcessHeap(), 0, ptr);
}

// itow - converts number to wide-string
LPWSTR itow(long a) {
	// prepare
	const static SIZE_T BUF_SIZE = 11;
	LPWSTR buf = malloc(BUF_SIZE);
	size_t len = 0;

	// build
	do {
		buf[len++] = (a % 10) + '0';
		a /= 10;
	} while(a > 0 && len < BUF_SIZE);

	// invert
	WCHAR tmp;
	for(unsigned int i = 0, j = len-1; i < j; i++, j--) {
		tmp = buf[i];
		buf[i] = buf[j];
		buf[j] = tmp;
	}

	// attach zero byte
	buf[len++] = '\0';

	// return
	return buf;
}