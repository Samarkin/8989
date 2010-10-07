#pragma once

const int BUFLEN = 3;

INT FetchWin1251Char(CHAR*, WCHAR&, int);
INT FetchUtf8Char(CHAR*, WCHAR&, int);
INT FetchUcs2LEChar(CHAR*, WCHAR&, int);

LPWSTR DecodeFromWin1251(LPSTR);
LPWSTR DecodeFromUtf8(LPSTR);
LPWSTR DecodeFromUcs2LE(LPSTR);
