#pragma once

// fetch char from byte sequence
//   buf - sequence
//   wch - resulting symbol
//   bytes - maximum ammount of available bytes
typedef INT (*FETCHCHAR)(CHAR* buf, WCHAR& wch, int bytes);

// Decodes null terminated string starting with the buf addr
typedef LPWSTR (*DECODESZSTRING)(LPSTR);

// Decodes string with specified length (in bytes) starting with the buf addr
typedef LPWSTR (*DECODESTRING)(LPSTR, int);
