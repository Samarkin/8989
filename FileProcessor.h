#include "encodings.interface.h"

typedef void (CALLBACK *RESULTCALLBACK)(WCHAR* result);
typedef void (CALLBACK *PROGRESSCALLBACK)(int progress);

typedef BYTE CHARSTATE;
typedef enum tagCHARSTATE {
	ISNOTCHAR = 0,
	ISCHAR = 1,
	ISLETTER = 2
};

typedef struct tagPROCESSFILE
{
	LPWSTR fileName;
	int minLength;
	BOOL nullTerminated;
	RESULTCALLBACK callback;
	PROGRESSCALLBACK progressUpdated;
	PROGRESSCALLBACK setJobSize;
	FETCHCHAR fetchChar;
	DECODESTRING decodeString;
	DECODESZSTRING decodeSzString;
	CHARSTATE charmap[1 << (sizeof(WCHAR)*8)];
} PROCESSFILE, *PPROCESSFILE;

DWORD WINAPI ProcessFile(LPVOID arg);
