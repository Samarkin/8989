#include "stdafx.h"
#include "encodings.h"

WCHAR EasyFetch(CHAR ch);
WCHAR win1251[64] = {0x402, 0x403, 0x201A, 0x453, 0x201E, 0x2026, 0x2020, 0x2021, 0x20AC, 0x2030, 0x409, 0x2039, 0x40A, 0x40C, 0x40B, 0x40F, 0x452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x0, 0x2122, 0x459, 0x203A, 0x45A, 0x45C, 0x45B, 0x45F, 0xA0, 0x40E, 0x45E, 0x408, 0xA4, 0x490, 0xA6, 0xA7, 0x401, 0xA9, 0x404, 0xAB, 0xAC, 0xAD, 0xAE, 0x407, 0xB0, 0xB1, 0x406, 0x456, 0x491, 0xB5, 0xB6, 0xB7, 0x451, 0x2116, 0x454, 0xBB, 0x458, 0x405, 0x455, 0x457};

// fetch char from utf-8 byte sequence
//   buf - sequence
//   ch - resulting symbol (!)
//   bytes - maximum ammount of available bytes
BOOL FetchWin1251Char(CHAR* buf, WCHAR& wch, int bytes) {
	if(bytes == 0)
		return FALSE;
	wch = EasyFetch(buf[BUFLEN-1]);
	return TRUE;
}

WCHAR EasyFetch(CHAR ch) {
	unsigned char uch = ch;
	if(uch < 0x80) {
		return (WCHAR)uch;
	}
	else if(uch < 0xC0) {
		return win1251[uch-0x80];
	}
	else {
		return (WCHAR)uch + 0x410 - 0xC0;
	}
}

// Decodes null-terminated UTF-8 string
LPWSTR DecodeSzFromWin1251(LPSTR lpStr) {
	int len = 0;
	for(char* lpc = lpStr; *lpc; lpc++) len++;
	return DecodeFromWin1251(lpStr, len);
}

// Decodes not null-terminated Windows-1251 string
LPWSTR DecodeFromWin1251(LPSTR lpStr, int len) {
	WCHAR* ans = new WCHAR[len+1];
	for(int i = 0; i < len; i++) {
		ans[i] = EasyFetch(lpStr[i]);
	}
	ans[len] = L'\0';
	return ans;
}