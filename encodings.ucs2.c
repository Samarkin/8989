#include "stdafx.h"
#include "encodings.h"

INT FetchUcs2LEChar(CHAR* buf, WCHAR* ch, int bytes)
{
	if(bytes >= 2)
	{
		*ch = *(WCHAR*)(buf + BUFLEN - 2);
		return 2;
	}
	else
	{
		return FALSE;
	}
}

// Decodes null-terminated UTF-16 Little Endian string
LPWSTR DecodeSzFromUcs2LE(LPSTR lpStr)
{
	return (LPWSTR)lpStr;
}

// Decodes UTF-16 Little Endian string
LPWSTR DecodeFromUcs2LE(LPSTR lpStr, int len)
{
	// num of widechars + null-symbol
	int wlen = len/2 + 1;
	WCHAR* t = malloc(sizeof(WCHAR)*wlen);
	for(int i = 0; i < wlen-1; i++) {
		t[i] = *(((WCHAR*)lpStr) + i);
	}
	t[wlen-1] = L'\0';
	return t;
}
