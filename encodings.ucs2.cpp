#include "stdafx.h"
#include "encodings.h"

INT FetchUcs2LEChar(CHAR* buf, WCHAR& ch, int bytes)
{
	if(bytes >= 2)
	{
		ch = *(WCHAR*)(buf + BUFLEN - 2);
		return 2;
	}
	else
	{
		return FALSE;
	}
}

// Decodes null-terminated UTF-16 Little Endian string
LPWSTR DecodeFromUcs2LE(LPSTR lpStr)
{
	return (LPWSTR)lpStr;
}
