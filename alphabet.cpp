#include "stdafx.h"
#include "alphabet.h"

#define BUFSIZE 4096

void LoadAlphabet(BYTE* charmap) {
	HANDLE hFile = CreateFile(AlphabetFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(!hFile) {
		ErrorReport(L"openning file");
	}
	char* lpBuf = new char[BUFSIZE], ch;
	wchar_t prev;
	DWORD read;
	bool lineBreak = true;
	bool inBreak = false;
	bool isLetter;
	bool inEscape = false;
	bool inRange = false;
	bool wasFail = false;
	int  n = 0;
	wchar_t newch = 0;
	while(ReadFile(hFile, lpBuf, BUFSIZE, &read, NULL)) {
		if(!read) break;
		for(int i = 0; i < read; i++) {
			ch = lpBuf[i];
			if(lineBreak) {
				lineBreak = false;
				if(ch == '-')
					isLetter = false;
				else if(ch == '+')
					isLetter = true;
			} // lineBreak
			else {
				if(inEscape) {
					inEscape = false;
					if(!inRange) {
						if(ch == 'x') {
							n = 4;
							newch = 0;
							wasFail = false;
						}
						else {
							prev = ch;
							charmap[ch] = true + isLetter;
						}
					} // !inRange
					else {
						if(ch == 'x') {
							n = 4;
							newch = 0;
							wasFail = false;
						}
						else {
							inRange = false;
							BYTE tmp = true + isLetter;
							for(WCHAR a = prev; a <= ch; a++) {
								charmap[a] = tmp;
							}
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
							wasFail = true;
						}
						if(n == 0 && !wasFail) {
							// parsed new \x???? character
							if(!inRange) {
								prev = newch;
								charmap[newch] = true + isLetter;
							} // !inRange
							else {
								inRange = false;
								BYTE tmp = true + isLetter;
								for(WCHAR a = prev; a <= newch; a++) {
									charmap[a] = tmp;
								}
							} // inRange
						}
					} // n != 0
					else {
						if(ch == '-') {
							inRange = true;
						}
						else if(ch == '\\') {
							inEscape = true;
						}
						else if(ch == '\n') {
							if(inBreak) {
								inBreak = false;
								lineBreak = true;
							}
						}
						else if(ch == '\r') {
							inBreak = true;
						}
						else if(ch < 0x80) {
							if(!inRange) {
								prev = ch;
								charmap[ch] = true + isLetter;
							} // !inRange
							else {
								inRange = false;
								BYTE tmp = true + isLetter;
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