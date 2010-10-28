#include "stdafx.h"
#include "FileProcessor.h"

#define BLOCKBITS 12
#define BLOCKSIZE (1 << BLOCKBITS)

DWORD WINAPI ProcessFile(LPVOID arg) {
	// TODO: Strange Panic in utf-8
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
#ifdef DEBUG
		DWORD err = ErrorReport(L"determining size of the file", false);
#else
		DWORD err = GetLastError();
#endif
		pf->callback(NULL);
		delete pf;
		ExitThread(err);
		return err;
	}
	int sz32 = (sz.LowPart >> BLOCKBITS)
		+ (sz.HighPart << (32-BLOCKBITS));
	if(sz.HighPart >> BLOCKBITS) {
		MessageBox(0, L"File is too big!", NULL, MB_OK);
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
	bool endFile = false;
	// block reading
	while(!endFile && ReadFile(file, buf, BLOCKSIZE, &read, 0)) {
		// update progress
		if(pf->progressUpdated) pf->progressUpdated(blocks++);
		if(!read) {
			endFile = true;
			read = 1;
			buf[0] = '\0';
		}

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
			isLetter = bPrev && pf->charmap[wch] == ISLETTER;
			// still moving prev
			isStr[0] = isStr[1];
			isStr[1] = isStr[2];
			isStr[2] = isStr[3];
			isStr[3] = bPrev && pf->charmap[wch];
			if(!bPrev) bPrev = 1;
			bool nullTerm = wch == L'\0';
			if(isStr[3-bPrev] && (nullTerm || (!pf->nullTerminated && !isStr[3])))
			{
				len += bPrev;
				// if string has no letter or too short - fuck it
				if(contLetters && effLen >= pf->minLength) {
					// the simpliest variant - if entire string is already in memory
					if(len <= i) {
						// just use it
						LPSTR ptr = (CHAR*)(buf+(i-len+1));
						LPWSTR tmp = nullTerm && !endFile
							? pf->decodeSzString(ptr)
							: pf->decodeString(ptr, len-bPrev);
						pf->callback(tmp);
						if(ptr != (CHAR*)tmp) delete tmp;
					}
					// well... uhm... here we need to re-read
					else {
						// seek for len characters back
						if(SetFilePointer(file, -((int)read-i + len - endFile) + 1, NULL, FILE_CURRENT)
							== INVALID_SET_FILE_POINTER) {
								goto __loop_end;
						}
						// allocate memory
						CHAR* tmp = new CHAR[len];
						DWORD j;
						// read memory block
						ReadFile(file, tmp, len, &j, NULL);
						// decode it
						LPWSTR wtmp = nullTerm && !endFile
							? pf->decodeSzString(tmp)
							: pf->decodeString(tmp, len-bPrev);
						// process decoded result and clear memory
						pf->callback(wtmp);
						if(tmp != (CHAR*)wtmp) delete wtmp;
						delete tmp;
						// return file pointer back only if it is not endFile
						if(!endFile && SetFilePointer(file, (int)read - i, NULL, FILE_CURRENT)
							== INVALID_SET_FILE_POINTER) {
								goto __loop_end;
						}
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

__loop_end:
	delete buf;
	ErrorReport(L"reading file",false);
	if(!CloseHandle(file)) {
#ifdef DEBUG
		ErrorReport(L"closing file", false);
#endif
	}
	// 
	pf->callback(NULL);
	//delete pf->fileName;
	delete pf;
	ExitThread(0);
	return 0;
}