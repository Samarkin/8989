#pragma once

const int BUFLEN = 3;

INT FetchWin1251Char(CHAR*, WCHAR&, int);
INT FetchUtf8Char(CHAR*, WCHAR&, int);
INT FetchUtf16LEChar(CHAR*, WCHAR&, int);

LPWSTR DecodeFromWin1251(LPSTR);
LPWSTR DecodeFromUtf8(LPSTR);
LPWSTR DecodeFromUtf16LE(LPSTR);
