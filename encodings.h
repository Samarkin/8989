#pragma once

// fetch char from utf-8 byte sequence
//   buf - sequence
//   wch - resulting symbol
//   bytes - maximum ammount of available bytes
INT FetchUtf8Char(CHAR* buf, WCHAR& wch, int bytes);

LPWSTR DecodeFromWin1251(LPSTR);
LPWSTR DecodeFromUtf8(LPSTR);
LPWSTR DecodeFromUtf16LE(LPSTR);
/*
LPSTR EncodeToWin1251(LPWSTR lpStr);
LPSTR EncodeToUtf8(LPWSTR lpStr);
LPSTR EncodeToUnicode(LPWSTR lpStr);
*/