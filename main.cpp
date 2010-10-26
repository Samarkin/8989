// 8989.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "main.h"
#include "FileOpenDialog.h"
#include "FileProcessor.h"
#include "commctrl.h"
#include "shellapi.h"
#include "encodings.h"
#include "alphabet.h"

#define MAX_LOADSTRING 100
#define IDM_LISTBOX 201
#define IDM_TEXTBOX 202
#define IDM_STATUSBAR 203
#define IDM_STARTBUTTON 204

#define SIDE_SPACE 10							// minimal distance between corner and the closest element

// Global Variables:
HINSTANCE	hInst;								// current instance
TCHAR		szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR		szWindowClass[MAX_LOADSTRING];		// the main window class name

HWND		hWnd;								// main window handle
HWND		hListBox;							// main listbox handle
HWND		hTextBox;							// info textbox handle
HWND		hStatusBar;							// status bar handle
HWND		hStartButton;						// боепде button handle
HWND		hProgressBar;						// progress bar handle

DWORD		dwEncoding;							// selected encoding
HANDLE		hThread;							// child thread handle
LPWSTR		lpFileName;							// name of openned file

bool		bNullTerm;							// is nullterm checked

// command line arguments
INT			argc;
WCHAR**		argv;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
VOID				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
VOID				openFile(LPWSTR);
VOID				window_Create(HWND);

int main()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	int nCmdShow = SW_SHOWNORMAL;

	// get command line
	LPWSTR cmdLine = GetCommandLine();
	argv = CommandLineToArgvW(cmdLine, &argc);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MY8989, szWindowClass, MAX_LOADSTRING);
	if(!MyRegisterClass(hInstance))
	{
		ErrorReport(L"registering class");
	}

	// Perform application initialization:
	InitInstance (hInstance, nCmdShow);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY8989));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ExitProcess(msg.wParam);
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	HANDLE hShell32 = LoadLibraryEx(L"Shell32.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
	HICON hIcon = LoadIcon((HINSTANCE)hShell32, MAKEINTRESOURCE(23));

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hIcon;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MY8989);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
VOID InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindowEx(WS_EX_ACCEPTFILES, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		100, 100, 600, 440, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		ErrorReport(L"creating main window");
	}

	window_Create(hWnd);

	if(argc > 1) {
		openFile(argv[1]);
	}

	// Main menu
	HMENU hMenu = GetMenu(hWnd);
	dwEncoding = ID_ENCODING_UCS2LE;
	if(!CheckMenuRadioItem(hMenu, ID_ENCODING_UTF, ID_ENCODING_KOI8, dwEncoding, MF_BYCOMMAND)) {
		ErrorReport(L"creating menu");
	}
	bNullTerm = false;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
}

void SetStartButtonState(bool start)
{
	// change text
	WCHAR* buf = new WCHAR[MAX_LOADSTRING];
	if(start)
		LoadString(hInst, IDS_STARTBUTTON, buf, MAX_LOADSTRING);
	else
		LoadString(hInst, IDS_STOPBUTTON, buf, MAX_LOADSTRING);
	SetWindowText(hStartButton, buf);
	delete buf;

	// enable/disable Start button
	LONG style = GetWindowLong(hStartButton, GWL_STYLE);
	SetWindowLong(hStartButton, GWL_STYLE, style & (~WS_DISABLED));

	// update button state
	SetFocus(hStartButton);
}

VOID CALLBACK ProcessorCallback(WCHAR* message)
{
	if(message) {
		SendMessage(hListBox, LB_ADDSTRING, (WPARAM)-1, (LPARAM)message);
	} else {
		SetWindowText(hStatusBar, L"Child thread finished");
		// enable start button
		SetStartButtonState(true);
		// clear memory
		CloseHandle(hThread);
		hThread = NULL;
	}
}

VOID CALLBACK ProcessorJobSize(int size)
{
	SendMessage(hProgressBar, PBM_SETRANGE32, 0, size-1);
}

VOID CALLBACK ProcessorProgress(int progress)
{
	SendMessage(hProgressBar, PBM_SETPOS, progress, 0);
}

void startProcess()
{
	if(hThread) {
		TerminateThread(hThread, -1);
		CloseHandle(hThread);
		hThread = NULL;
		SetStartButtonState(true);
		return;
	}
	// clear listbox
	SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
	// process itself
	PROCESSFILE* pf = new PROCESSFILE;
	FastFillMemory(pf, sizeof(PROCESSFILE), 0);
	pf->fileName = lpFileName;
	pf->minLength = 3;
	pf->callback = &ProcessorCallback;
	pf->progressUpdated = &ProcessorProgress;
	pf->setJobSize = &ProcessorJobSize;
	pf->nullTerminated = bNullTerm;

	LoadAlphabet(pf->charmap);

	// Encodings
	switch(dwEncoding) {
	case ID_ENCODING_UTF:
		pf->fetchChar = &FetchUtf8Char;
		pf->decodeSzString = &DecodeSzFromUtf8;
		pf->decodeString = &DecodeFromUtf8;
		break;
	case ID_ENCODING_UCS2LE:
		pf->fetchChar = &FetchUcs2LEChar;
		pf->decodeString = &DecodeFromUcs2LE;
		pf->decodeSzString = &DecodeSzFromUcs2LE;
		break;
	case ID_ENCODING_WINDOWS:
		pf->fetchChar = &FetchWin1251Char;
		pf->decodeString = &DecodeFromWin1251;
		pf->decodeSzString = &DecodeSzFromWin1251;
		break;
	case ID_ENCODING_KOI8:
		pf->fetchChar = & FetchKoi8Char;
		pf->decodeString = &DecodeFromKoi8;
		pf->decodeSzString = &DecodeSzFromKoi8;
		break;
	default:
		{
			WCHAR* szMessageText = new WCHAR[MAX_LOADSTRING];
			LoadString(hInst, IDS_NOTSUPPORTED, szMessageText, MAX_LOADSTRING);
			MessageBox(hWnd, szMessageText, NULL, MB_OK);
			delete szMessageText;
		}
		delete pf;
		return;
	}
	
	hThread = CreateThread(NULL, 0, &ProcessFile, pf, 0, NULL);
	// disable start button
	SetStartButtonState(false);
	// verbose
	SetWindowText(hStatusBar, L"Child thread started");
}

void openFile(LPWSTR lpszFileName)
{
	if(hThread) {
		WCHAR* szMessageTitle = new WCHAR[MAX_LOADSTRING],
			 * szMessageText = new WCHAR[MAX_LOADSTRING];
		LoadString(hInst, IDS_OPENFILE, szMessageTitle, MAX_LOADSTRING);
		LoadString(hInst, IDS_TERMINATETHREAD, szMessageText, MAX_LOADSTRING);
		int res = MessageBox(hWnd, szMessageText, szMessageTitle, MB_YESNO);
		delete szMessageText;
		delete szMessageTitle;

		if(res == IDYES) {
			TerminateThread(hThread, -1);
			CloseHandle(hThread);
			hThread = NULL;
		}
		else return;
	}
	// clear previously allocated memory
	if(lpFileName) delete lpFileName;
	// save new file name
	lpFileName = lpszFileName;
	// change window title
	WCHAR* buf = new WCHAR[MAX_PATH + MAX_LOADSTRING + 4];
	buf[0] = L'\0';
	widecat(buf, lpFileName);
	widecat(buf, L" - ");
	widecat(buf, szTitle);
	SetWindowText(hWnd, buf);
	delete buf;
	if(hStartButton) {
		SetStartButtonState(true);
	}
}

void sel_Changed()
{
	int idx;
	if((idx = SendMessage(hListBox, LB_GETCURSEL, 0, 0)) != LB_ERR) {
		// if any element selected
		SIZE_T len = SendMessage(hListBox, LB_GETTEXTLEN, idx, 0);
		WCHAR* buf = new WCHAR[len+1];
		SendMessage(hListBox, LB_GETTEXT, idx, (LPARAM)buf);
		SetWindowText(hTextBox, buf);
	}
}

void nullTerm()
{
	HMENU hMenu = GetMenu(hWnd);
	DWORD state = GetMenuState(hMenu, ID_NULLTERM, MF_BYCOMMAND);

	if(state & MF_CHECKED) {
		if(GetMenuState(hMenu, ID_ENCODING_UTF, MF_BYCOMMAND) & MF_CHECKED) {
			WCHAR* szMessageText = new WCHAR[MAX_LOADSTRING];
			LoadString(hInst, IDS_NOTSUPPORTED, szMessageText, MAX_LOADSTRING);
			MessageBox(hWnd, szMessageText, NULL, MB_OK);
			delete szMessageText;
		} else {
			bNullTerm = false;
			CheckMenuItem(hMenu, ID_NULLTERM, MF_UNCHECKED | MF_BYCOMMAND);
		}
	}
	else {
		bNullTerm = true;
		CheckMenuItem(hMenu, ID_NULLTERM, MF_CHECKED | MF_BYCOMMAND);
	}
}

#define FIRST_ROW	25
#define PROGRESS_HEIGHT	15
void window_Resized(int width, int height)
{
	int stHeight = 15;

	int colWidth = (width - 4*SIDE_SPACE)/2;

	// Rearrage elements
	MoveWindow(hStartButton, SIDE_SPACE, SIDE_SPACE, colWidth, FIRST_ROW, TRUE);
	MoveWindow(hListBox, SIDE_SPACE, SIDE_SPACE*2 + FIRST_ROW, colWidth, height - stHeight - 3*SIDE_SPACE - FIRST_ROW, TRUE);

	MoveWindow(hTextBox, SIDE_SPACE*3 + colWidth, SIDE_SPACE+1, (width - 4*SIDE_SPACE)/2, FIRST_ROW-2, TRUE);
	MoveWindow(hProgressBar, SIDE_SPACE*3 + colWidth, height - stHeight - 2*SIDE_SPACE - PROGRESS_HEIGHT, colWidth, PROGRESS_HEIGHT, TRUE);

	// Resize status bar
	SendMessage(hStatusBar, WM_SIZE, 0, 0);
}

void window_Create(HWND hWnd)
{
	// List Box
	hListBox = CreateWindow(L"LISTBOX", L"",
		WS_BORDER | WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | LBS_NOTIFY | WS_VSCROLL,
		0, 0, 0, 0, hWnd, (HMENU)IDM_LISTBOX, hInst, NULL);
	if(!hListBox) ErrorReport(L"creating list box");

	// Text Box
	hTextBox = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_READONLY,
		0, 0, 0, 0, hWnd, (HMENU)IDM_TEXTBOX, hInst, NULL);
	if(!hTextBox) ErrorReport(L"creating text box");

	// Status Bar
	INITCOMMONCONTROLSEX lpInitCtrls;
	lpInitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	lpInitCtrls.dwICC = ICC_BAR_CLASSES;
	int t = InitCommonControlsEx(&lpInitCtrls);
	hStatusBar = CreateWindowEx(0, STATUSCLASSNAME, NULL,
		WS_VISIBLE | WS_CHILD | SBARS_SIZEGRIP,
		0, 0, 0, 0, hWnd, (HMENU)IDM_STATUSBAR, hInst, NULL);
	if(!hStatusBar) ErrorReport(L"creating status bar");

	// Progress Bar
	hProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL,
		WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0, hWnd, 0, hInst, NULL);

	// Start Button
	WCHAR* buf = new WCHAR[MAX_LOADSTRING];
	LoadString(hInst, IDS_STARTBUTTON, buf, MAX_LOADSTRING);
	hStartButton = CreateWindow(L"BUTTON", buf,
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON |
		(lpFileName ? 0 : WS_DISABLED),
		0, 0, 0, 0, hWnd, (HMENU)IDM_STARTBUTTON, hInst, NULL);
	delete buf;
	if(!hStartButton) ErrorReport(L"creating start button");
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_DROPFILES: {
		WCHAR* lpszFile = new WCHAR[MAX_PATH];
		HDROP hDrop = (HDROP)wParam;
		DragQueryFile(hDrop, 0, lpszFile, MAX_PATH);
		openFile(lpszFile);
		} break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the control's messages:
		switch (wmId)
		{
		case IDM_LISTBOX:
			if(wmEvent == LBN_SELCHANGE)
				sel_Changed();
			break;
		case IDM_STARTBUTTON:
			startProcess();
			break;
		case IDM_OPEN:
			// open file
			openFile(FileOpenDialog(hWnd, ALLFILESMASK));
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		// encodings
		case ID_ENCODING_UTF:
			bNullTerm = true;
			CheckMenuItem(GetMenu(hWnd), ID_NULLTERM, MF_CHECKED | MF_BYCOMMAND);
		case ID_ENCODING_UCS2LE:
		case ID_ENCODING_UCS2BE:
		case ID_ENCODING_WINDOWS:
		case ID_ENCODING_KOI8:
			dwEncoding = wmId;
			CheckMenuRadioItem(GetMenu(hWnd), ID_ENCODING_UTF, ID_ENCODING_KOI8, wmId, MF_BYCOMMAND);
			break;
		case ID_NULLTERM:
			nullTerm();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		window_Resized(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}