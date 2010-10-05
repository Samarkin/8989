#include "stdafx.h"
#include "FileProcessor.h"

#define BLOCKBITS 12
#define BLOCKSIZE (1 << BLOCKBITS)

DWORD WINAPI ProcessFile(LPVOID arg) {
	// TODO: Strange Panic
	PPROCESSFILE pf = (PPROCESSFILE)arg;
	if(!pf->callback || !pf->fetchChar || !pf->decodeString) {
		delete pf;
		ExitThread(-1);
		return -1;
	}
	HANDLE file = CreateFile(pf->fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(file == INVALID_HANDLE_VALUE) {
		DWORD err = ErrorReport(L"openning file", false);
		pf->callback(NULL);
		delete pf;
		ExitThread(err);
		return err;
	}

	LARGE_INTEGER sz;
	if(!GetFileSizeEx(file, &sz)) {
		DWORD err = ErrorReport(L"determining size of the file", false);
		pf->callback(NULL);
		delete pf;
		ExitThread(err);
		return err;
	}
	int sz32 = (sz.LowPart >> BLOCKBITS)
		+ (sz.HighPart << (32-BLOCKBITS));
	if(sz.HighPart >> BLOCKBITS) {
		MessageBox(0, L"File is too big!", L"", MB_OK);
		pf->callback(NULL);
		delete pf;
		ExitThread(-1);
		return -1;
	}
	if(pf->setJobSize) pf->setJobSize(sz32);
	CHAR* buf = new CHAR[BLOCKSIZE];
	DWORD read = 0;
	long len = 0;
	long effLen = 0;
	bool isLetter, contLetters = false;
	char prev[3];
	bool isStr[4];
	int bytes = 0;
	int blocks = 0;
	// block reading
	while(ReadFile(file, buf, BLOCKSIZE, &read, 0)) {
		if(pf->progressUpdated) pf->progressUpdated(blocks++);
		if(!read) break;
		// update progress (but not very often)

		// block scanning
		for(int i = 0; i < read; i++) {
			if(bytes < 3) ++bytes;
			char ch = buf[i];
			// move prev
			prev[0] = prev[1];
			prev[1] = prev[2];
			prev[2] = ch;
			// fetch char
			WCHAR wch;
			int bPrev = pf->fetchChar(prev, wch, bytes);
			isLetter = (wch >= L'a' && wch <= L'z') || (wch >= L'A' && wch <= L'Z')
				|| (wch >= L'à' && wch <= L'ÿ') || (wch >= L'À' && wch <= L'ß')
				|| (wch >= L'0' && wch <= L'9');
			// still moving prev
			isStr[0] = isStr[1];
			isStr[1] = isStr[2];
			isStr[2] = isStr[3];
			isStr[3] = bPrev &&
				(isLetter || wch == '(' || wch == ')' || wch == '$' || wch == '_'
				|| wch == ',' || wch == '-' || wch == L' ');
			if(!bPrev) bPrev = 1;
			if(isStr[3-bPrev] && wch == L'\0') {
				len += bPrev;
				// if string has no letter or too short - fuck it
				if(contLetters && effLen >= pf->minLength) {
					// the simpliest variant - if entire string is already in memory
					if(len <= i) {
						// just use it (with null-symbol)
						LPWSTR tmp = pf->decodeString((CHAR*)(buf+(i-len+1)));
						pf->callback(tmp);
						//delete tmp;
					}
					// well... uhm... here we need to re-read
					else {
						// seek for len characters back
						SetFilePointer(file, (-(int)read+i)-len+1, NULL, FILE_CURRENT);
						// allocate memory
						CHAR* tmp = new CHAR[len];
						DWORD i;
						// read memory block
						ReadFile(file, tmp, len, &i, NULL);
						// decode it
						LPWSTR wtmp = pf->decodeString(tmp);
						// process decoded result and clear memory
						delete tmp;
						pf->callback(wtmp);
						//delete wtmp;
						// read next block
						break;
					}
				}
			}
			// next symbol
			else if(isStr[3] && isStr[3-bPrev]) {
				len+= bPrev;
				contLetters |= isLetter;
				effLen++;
			}
			// first symbol
			else if(isStr[3]) {
				len = bPrev;
				contLetters = isLetter;
				effLen = 1;
			}
		}
	}
	delete buf;
	ErrorReport(L"reading file",false);
	if(!CloseHandle(file)) {
		ErrorReport(L"closing file", false);
	}
	// 
	pf->callback(NULL);
	//delete pf->fileName;
	delete pf;
	ExitThread(0);
	return 0;
}