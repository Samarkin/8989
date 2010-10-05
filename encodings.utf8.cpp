#include "stdafx.h"
#include "encodings.h"

// Decodes null-terminated Windows-1251 string
LPWSTR DecodeFromWin1251(LPSTR lpStr) {
	// TODO: Implement
	return NULL;
}

// fetch char from utf-8 byte sequence
//   buf - sequence
//   ch - resulting symbol (!)
//   bytes - maximum ammount of available bytes
BOOL FetchUtf8Char(CHAR* buf, WCHAR& ch, int bytes) {
	if(bytes == 0) return FALSE;
	if(buf[BUFLEN-1] & 0x80) { // 1xxxxxxx
		if(buf[BUFLEN-1] & 0x40) { // 11xxxxxx
			// first utf-8 symbol here??? this is wrong
			return FALSE;
		}
		else { // 10xxxxxx
			// ok, it was utf-8 sequell byte, moving forward
			if(bytes >= 2 && buf[BUFLEN-2] & 0x80 ) { // 1xxxxxxx 10xxxxxx
				if(buf[BUFLEN-2] & 0x40) { // 11xxxxxx 10xxxxxx
					// so far so good
					if(buf[BUFLEN-2] & 0x20) { // 111xxxxx 10xxxxxx
						// wrong!
						return FALSE;
					}
					else { // 110xxxxx 10xxxxxx
						// canonical two-byte utf-8 char
						ch = (buf[BUFLEN-1] & 0x3f) // last six bits of last byte
						 + ((buf[BUFLEN-2] & 0x1f) << 6); // last five bits of first byte
						return 2;
					}
				}
				else { // 10xxxxxx 10xxxxxxx
					// next!
					if(bytes >= 3 && buf[BUFLEN-3] & 0x80) { // 1xxxxxxx 10xxxxxx 10xxxxxx
						if(buf[BUFLEN-3] & 0x40) { // 11xxxxxx 10xxxxxx 10xxxxxxx
							if(buf[BUFLEN-3] & 0x20) { // 111xxxxx 10xxxxxx 10xxxxxx
								// no-no-no
								return FALSE;
							}
							else { // 110xxxxx 10xxxxxx 10xxxxxx
								ch = (buf[BUFLEN-1] & 0x3f)			// last six bits of last octet
								 + ((int)(buf[BUFLEN-2] & 0x3f) << 6)	// last six bits of pre-last octet
								 + ((int)(buf[BUFLEN-3] & 0xf) << 12);	// last four bits of first octet
								return 3;
							}
						}
						else { // 10xxxxxx 10xxxxxx 10xxxxxx
							// uhm... also a unicode char... but can not fit in wchar, so fuck it =)
							return FALSE;
						}
					}
					else { // 0xxxxxxx 10xxxxxxx 10xxxxxx or no bytes left
						// anyway, bad for them
						return FALSE;
					}
				}
			}
			else { // 0xxxxxx or no bytes left
				// one-byte char?? here??
				return FALSE;
			}
		}
	}
	else { // 0xxxxxxx
		// one byte character
		ch = (WCHAR)buf[BUFLEN-1];
		return 1;
	}
}

// Decodes null-terminated UTF-8 string
LPWSTR DecodeFromUtf8(LPSTR lpStr) {
	int len = 0;
	for(char* lpc = lpStr; *lpc; lpc++) {
		if(!(*lpc & 0x80) // if first bit is zero
			|| (*lpc & 0xc0) == 0xc0) { // or first two is 1
				// then this is the beginning of symbol
				len++;
		}
	}
	WCHAR* buf = new WCHAR[len], *buf1 = buf;
	int bytes = 0;
	for(char* lpc = lpStr; *lpc; lpc++) {
		if(bytes < 3) ++bytes;
		if(FetchUtf8Char(lpc-BUFLEN+1, *buf1, bytes)) {
			buf1++;
		}
	}
	*buf1 = (WCHAR)0;
	return buf;
}
