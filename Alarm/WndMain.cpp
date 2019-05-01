#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "AlarmLib.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnClickListener(HINSTANCE Instance, HWND hWnd, int type);

HINSTANCE gInstance;
LPCWSTR lpszClassN = L"Park Alarm";
uPOINT mainCursor;

int FocusWnd = 0;
bool bAddMenu = false;
bool AddMenuFirstMotion = true;
bool bModifyMenu = false;

ALARM *HeadNode = NULL;
ALARM *NewNode = NULL;
int PrintNodePoint = 0;

TIME *tSelectedTime = (TIME*)calloc(1, sizeof(TIME));
LPWSTR MemoData = (LPWSTR)calloc(MEMO_MAXBUF, sizeof(wchar_t));
RECT MemoRect = { MEB_LEFT, MEB_TOP, MEB_RIGHT, MEB_BOTTOM };
int MemoDataLen = 0;
int NumOfAlarm = 0;


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) {
	HWND hWnd;
	MSG Msg;
	WNDCLASS WndClass;
	gInstance = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClassN;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClassN, L"", WS_THICKFRAME,	//  | WS_SYSMENU | WS_CAPTION
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Msg, 0, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

void TimeProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
	if (HeadNode != NULL) {
		ALARM *Current = HeadNode;
		time_t t = time(NULL);
		struct tm CurrentTime = *localtime(&t);

		while (Current != NULL) {
			if (Current->OnOff && Current->time.RepeatWeek[CurrentTime.tm_wday] && Current->time.Hou == CurrentTime.tm_hour && Current->time.Min == CurrentTime.tm_min) {
				Current->OnOff = false;
				MessageBox(hWnd, Current->MemoData, L"Park Alarm.", MB_OK);
			}

			Current = Current->NextAlarm;
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	int x, y;
	
	switch (iMessage) {

	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, (TIMERPROC)TimeProc);

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 700;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 700;
		return 0;

	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		PrintMainDisplay(gInstance, hWnd, hdc, &ps);
		PrintAlarmList(HeadNode, gInstance, hdc, PrintNodePoint);
		if (bAddMenu) AppearAddMenu(gInstance, hWnd, hdc, *tSelectedTime, MemoData, &NewNode, &AddMenuFirstMotion, &FocusWnd);
		if (bModifyMenu);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_MOUSEMOVE:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		UpdateSelectedButton(gInstance, hWnd, CheckingMousePos(&NewNode, x, y, FocusWnd, false), &FocusWnd);
		return 0;

	case WM_LBUTTONDOWN:
		mainCursor.x = LOWORD(lParam);
		mainCursor.y = HIWORD(lParam);
		OnClickListener(gInstance, hWnd, CheckingMousePos(&NewNode, mainCursor.x, mainCursor.y, FocusWnd, true));
		SendMessage(hWnd, WM_MOUSEMOVE, 0, 0);
		return 0;

	case WM_CHAR:
		if (wParam == VK_BACK) {
			if (MemoDataLen - 1 >= 0) MemoData[--MemoDataLen] = 0;
		} else if (wParam == VK_RETURN) {
			return 0;
		} else if (MemoDataLen < MEMO_MAXBUF) {
			MemoData[MemoDataLen] = wParam;
			MemoData[++MemoDataLen] = 0;
		}
		InvalidateRect(hWnd, &MemoRect, true);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void OnClickListener(HINSTANCE Instance, HWND hWnd, int type) {
	HDC hdc;
	static int OldHourType = 0;
	static int OldMinuteType = 0;

	if (FocusWnd == 0) {
		switch (type) {
		case 1:
			bAddMenu = true;
			InvalidateRect(hWnd, NULL, false);
			break;

		case 2:
			MessageBox(hWnd, L"Click Modify button!!", L"From. Park Alarm", MB_OK);
			break;

		case 3:
			MessageBox(hWnd, L"Click Delete button!!", L"From. Park Alarm", MB_OK);
			break;

		case 4:
			MessageBox(hWnd, L"Click Copy button!!", L"From. Park Alarm", MB_OK);
			break;

		case 5:
			if (PrintNodePoint > 0) PrintNodePoint--;
			InvalidateRect(hWnd, NULL, true);
			break;

		case 6:
			if (NumOfAlarm - PrintNodePoint > 10) PrintNodePoint++;
			InvalidateRect(hWnd, NULL, true);
			break;

		case 7:
			int i, t = 0;
			for (i = PrintNodePoint + 1; i <= NumOfAlarm - PrintNodePoint; i++) {
				if (mainCursor.x > ALARMTABLE_LEFT && mainCursor.x < ALARMTABLE_RIGHT && mainCursor.y > ALARMTABLE_TOP + (t * 49) && mainCursor.y < ALARMTABLE_TOP + (++t * 49)) break;
			}
			if (t == 2) MessageBox(hWnd, L"Click Alarm Table area!!", L"From. Park Alarm", MB_OK);
			
			break;
		}
	} else if (FocusWnd == 1) {
		switch (type) {
		// Hour
		case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
		case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43:
			PrintSelectedButton(Instance, hWnd, OldHourType, &FocusWnd, false);
			PrintSelectedButton(Instance, hWnd, type, &FocusWnd, true);
			OldHourType = type;
			tSelectedTime->Hou = type;
			break;

		// Minute
		case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61:
		case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73:
		case 74: case 75: case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85:
		case 86: case 87: case 88: case 89: case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97:
		case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109:
			PrintSelectedButton(Instance, hWnd, OldMinuteType, &FocusWnd, false);
			PrintSelectedButton(Instance, hWnd, type, &FocusWnd, true);
			OldMinuteType = type;
			tSelectedTime->Min = type;
			break;

		// RepeatWeek
		case 120: case 121: case 122: case 123: case 124: case 125: case 126:
			if (tSelectedTime->RepeatWeek[type - 120] == 0) tSelectedTime->RepeatWeek[type - 120] = 1;
			else tSelectedTime->RepeatWeek[type - 120] = 0;
			InvalidateRect(hWnd, NULL, true);
			break;

		// Button for Create and Cancel.
		case 150: case 151:
			if (type == 150) {
				CreateAlarm(tSelectedTime, MemoData, &NewNode);
				NumOfAlarm = AppendNode(&HeadNode, NewNode);
				if (NumOfAlarm > 10) PrintNodePoint++;
				NewNode = NULL;
			} else if (type == 151) {
				free(NewNode); NewNode = NULL;
			}

			FocusWnd = 0;
			bAddMenu = false;
			AddMenuFirstMotion = true;
			memset(tSelectedTime, 0, sizeof(TIME));
			memset(MemoData, 0, MEMO_MAXBUF * sizeof(wchar_t));
			MemoDataLen = 0;

			InvalidateRect(hWnd, NULL, true);
			break;
		}
	}
}
