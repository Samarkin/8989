#include "stdafx.h"
#include "alphabet.h"

#define BUFSIZE 4096
#define DefaultAlphabet "+A-Za-z\r\n\
+\\x0410-\\x042F\\x0430-\\x044F\\x0401\\x451\r\n\
+0-9\r\n\
-\\x000A\\x000D\\x0009\r\n\
-\"'`\r\n\
-+\\-*\\\\/=\r\n\
- ,.?!:;\r\n\
-&_~@#$%^|\r\n\
-()<>[]{}"


void LoadAlphabet(BYTE* charmap) {
	HANDLE hFile = CreateFile(AlphabetFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(hFile == INVALID_HANDLE_VALUE) {
		// Generate new alphabet file
		hFile = CreateFile(AlphabetFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, 0, 0);
		FatalError(L"creating alphabet file");
		DWORD written;
		WriteFile(hFile, DefaultAlphabet, sizeof(DefaultAlphabet)-1, &written, NULL);
		SetEndOfFile(hFile);
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	}
	char* lpBuf = malloc(BUFSIZE), ch;
	wchar_t prev;
	DWORD read;
	BOOL lineBreak = TRUE;
	BOOL inBreak = FALSE;
	BOOL isLetter;
	BOOL inEscape = FALSE;
	BOOL inRange = FALSE;
	BOOL wasFail = FALSE;
	int  n = 0;
	wchar_t newch = 0;
	while(ReadFile(hFile, lpBuf, BUFSIZE, &read, NULL)) {
		if(!read) break;
		for(unsigned int i = 0; i < read; i++) {
			ch = lpBuf[i];
			if(lineBreak) {
				lineBreak = FALSE;
				if(ch == '-')
					isLetter = FALSE;
				else if(ch == '+')
					isLetter = TRUE;
			} // lineBreak
			else {
				if(inEscape) {
					inEscape = FALSE;
					if(!inRange) {
						if(ch == 'x') {
							n = 4;
							newch = 0;
							wasFail = FALSE;
						}
						else {
							prev = ch;
							charmap[ch] = TRUE + isLetter;
						}
					} // !inRange
					else {
						if(ch == 'x') {
							n = 4;
							newch = 0;
							wasFail = FALSE;
						}
						else {
							inRange = FALSE;
							BYTE tmp = TRUE + isLetter;
							FastFillMemory(charmap + prev, newch - prev + 1, tmp);
						} // ch != 'x'
					} // inRange
				} // inEscape
				else {
					if(n) {
						n--;
						if(ch >= 'a' && ch <= 'f') {
							newch = (newch << 4) + ch - 'a' + 10;
						}
						else if(ch >= 'A' && ch <= 'F') {
							newch = (newch << 4) + ch - 'A' + 10;
						}
						else if(ch >= '0' && ch <= '9') {
							newch = (newch << 4) + ch - '0';
						}
						else {
							wasFail = TRUE;
						}
						if(n == 0 && !wasFail) {
							// parsed new \x???? character
							if(!inRange) {
								prev = newch;
								charmap[newch] = TRUE + isLetter;
							} // !inRange
							else {
								inRange = FALSE;
								BYTE tmp = TRUE + isLetter;
								FastFillMemory(charmap + prev, newch - prev + 1, tmp);
							} // inRange
						}
					} // n != 0
					else {
						if(ch == '-') {
							inRange = TRUE;
						}
						else if(ch == '\\') {
							inEscape = TRUE;
						}
						else if(ch == '\n') {
							if(inBreak) {
								inBreak = FALSE;
								lineBreak = TRUE;
							}
						}
						else if(ch == '\r') {
							inBreak = TRUE;
						}
						else if(ch < 0x80) {
							if(!inRange) {
								prev = ch;
								charmap[ch] = TRUE + isLetter;
							} // !inRange
							else {
								inRange = FALSE;
								BYTE tmp = TRUE + isLetter;
								for(WCHAR a = prev; a <= ch; a++) {
									charmap[a] = tmp;
								}
							} // inRange
						}
					} // n == 0
				} // !inEscape
			} // !lineBreak
		} // for
	} // while
	CloseHandle(hFile);
}