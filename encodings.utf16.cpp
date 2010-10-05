#include "stdafx.h"
#include "encodings.h"

INT FetchUtf16LEChar(CHAR* buf, WCHAR& ch, int bytes)
{
	if(bytes >= 2)
	{
		ch = ((short)buf[BUFLEN-1] << 8) + buf[BUFLEN-2];
		return 2;
	}
	else
	{
		return FALSE;
	}
}

// Decodes null-terminated UTF-16 Little Endian string
LPWSTR DecodeFromUtf16LE(LPSTR lpStr)
{
	return (LPWSTR)lpStr;
}
