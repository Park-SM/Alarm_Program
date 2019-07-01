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
bool ModifyMenuFirstMotion = true;
bool bModifyMenu = false;

ALARM *HeadNode = NULL;
ALARM *NewNode = NULL;
ALARM *SelectedNode = NULL;
int PrintNodePoint = 0;

TIME *tSelectedTime = (TIME*)calloc(1, sizeof(TIME));
LPWSTR MemoData = (LPWSTR)calloc(MEMO_MAXBUF, sizeof(wchar_t));
RECT MemoRect = { MEB_LEFT, MEB_TOP, MEB_RIGHT, MEB_BOTTOM };
RECT ModifyRect = { MEB_LEFT + 150, MEB_TOP, MEB_RIGHT + 150, MEB_BOTTOM };
RECT AlarmTableRect = { ALARMTABLE_LEFT, ALARMTABLE_TOP, ALARMTABLE_RIGHT, ALARMTABLE_BOTTOM };
RECT SoundPathRect_add = { BR_RIGHT + 40, BR_TOP + 10, 349, BR_BOTTOM };
RECT SoundPathRect_modify = { BR_RIGHT + 40 + 150, BR_TOP + 10, 500, BR_BOTTOM };
OPENFILENAME ofn;
WCHAR szFileName[512];
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
	int CheckedType;
	
	switch (iMessage) {

	case WM_CREATE:
		SetTimer(hWnd, 1, 1000, (TIMERPROC)TimeProc);
		AlarmFileReader(&HeadNode, &NumOfAlarm);

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
		if (bModifyMenu) AppearModifyMenu(gInstance, hWnd, hdc, tSelectedTime, MemoData, SelectedNode, &ModifyMenuFirstMotion, &FocusWnd);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_MOUSEMOVE:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (FocusWnd == 0) CheckedType = CheckingMousePos(NULL, NULL, x, y, FocusWnd, false);
		else if (FocusWnd == 1) CheckedType = CheckingMousePos(NewNode, NULL, x, y, FocusWnd, false);
		else if (FocusWnd == 2) CheckedType = CheckingMousePos(NULL, tSelectedTime, x, y, FocusWnd, false);
		UpdateSelectedButton(gInstance, hWnd, CheckedType, FocusWnd);
		return 0;

	case WM_LBUTTONDOWN:
		mainCursor.x = LOWORD(lParam);
		mainCursor.y = HIWORD(lParam);
		if (FocusWnd == 0) CheckedType = CheckingMousePos(NULL, NULL, mainCursor.x, mainCursor.y, FocusWnd, true);
		else if (FocusWnd == 1) CheckedType = CheckingMousePos(NewNode, NULL, mainCursor.x, mainCursor.y, FocusWnd, true);
		else if (FocusWnd == 2) CheckedType = CheckingMousePos(NULL, tSelectedTime, mainCursor.x, mainCursor.y, FocusWnd, true);
		OnClickListener(gInstance, hWnd, CheckedType);
		SendMessage(hWnd, WM_MOUSEMOVE, 0, 0);
		return 0;

	case WM_MOUSEWHEEL:
		if (FocusWnd == 0) {
			if ((short)HIWORD(wParam) > 0) {
				if (PrintNodePoint > 0) PrintNodePoint--;
				InvalidateRect(hWnd, &AlarmTableRect, false);
			}
			else {
				if (NumOfAlarm - PrintNodePoint > 10) PrintNodePoint++;
				InvalidateRect(hWnd, &AlarmTableRect, false);
			}
		}
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
		if (FocusWnd == 1) InvalidateRect(hWnd, &MemoRect, true);
		else if (FocusWnd == 2) InvalidateRect(hWnd, &ModifyRect, true);
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void OnClickListener(HINSTANCE Instance, HWND hWnd, int type) {
	static bool IsFirstH = true;	// Using for FocusWnd2
	static bool IsFirstM = true;	// Using for FocusWnd2
	static WCHAR OldFileName[100];	// Using for FocusWnd2
	static WCHAR OldFilePath[512];	// Using for FocusWnd2
	static int OldHourType = 0;
	static int OldMinuteType = 0;

	if (FocusWnd == 0) {
		switch (type) {
		case 1:
			bAddMenu = true;
			InvalidateRect(hWnd, NULL, false);
			break;

		case 2:
			if (HeadNode != NULL) {
				SelectedNode = HeadNode;
				while (SelectedNode != NULL && SelectedNode->Selected != true) SelectedNode = SelectedNode->NextAlarm;
				if (SelectedNode != NULL) {
					tSelectedTime->Hou = SelectedNode->time.Hou + 219;
					tSelectedTime->Min = SelectedNode->time.Min + 250;
					wcsncpy(MemoData, SelectedNode->MemoData, wcslen(SelectedNode->MemoData) + 1);
					MemoDataLen = wcslen(SelectedNode->MemoData);
					for (int i = 0; i < 7; i++) tSelectedTime->RepeatWeek[i] = SelectedNode->time.RepeatWeek[i];
					wcsncpy(OldFileName, SelectedNode->szSoundFileName, wcslen(SelectedNode->szSoundFileName) + 1);
					wcsncpy(OldFilePath, SelectedNode->szSoundFilePath, wcslen(SelectedNode->szSoundFilePath) + 1);
					bModifyMenu = true;
					InvalidateRect(hWnd, NULL, false);
				}
			}
			//MessageBox(hWnd, L"Click Modify button!!", L"From. Park Alarm", MB_OK);
			break;

		case 3:
			DeleteAlarm(&HeadNode, &NumOfAlarm);
			AlarmFileWriter(HeadNode);
			InvalidateRect(hWnd, NULL, true);
			break;

		case 4:
			SelectedNode = HeadNode;
			while (SelectedNode != NULL && SelectedNode->Selected != true) SelectedNode = SelectedNode->NextAlarm;
			if (SelectedNode != NULL) {
				NewNode = (ALARM*)calloc(1, sizeof(ALARM));
				wcsncpy(NewNode->MemoData, SelectedNode->MemoData, wcslen(SelectedNode->MemoData) + 1);
				wcsncpy(NewNode->szSoundFileName, SelectedNode->szSoundFileName, wcslen(SelectedNode->szSoundFileName) + 1);
				wcsncpy(NewNode->szSoundFilePath, SelectedNode->szSoundFilePath, wcslen(SelectedNode->szSoundFilePath) + 1);
				NewNode->NextAlarm = NULL;
				NewNode->OnOff = true;
				NewNode->Selected = false;
				NewNode->time.Hou = SelectedNode->time.Hou;
				NewNode->time.Min = SelectedNode->time.Min;
				for (int i = 0; i < 7; i++) NewNode->time.RepeatWeek[i] = SelectedNode->time.RepeatWeek[i];

				NumOfAlarm = AppendNode(&HeadNode, NewNode);
				if (NumOfAlarm > 10) PrintNodePoint++;
				AlarmFileWriter(HeadNode);
				NewNode = NULL;
			}
			SelectedNode = NULL;
			InvalidateRect(hWnd, NULL, false);
			//MessageBox(hWnd, L"Click Copy button!!", L"From. Park Alarm", MB_OK);
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
			int i = 0;
			ALARM *Current = NULL;
			for (Current = HeadNode; Current != NULL; Current = Current->NextAlarm)
				if (Current->Selected) Current->Selected = false;

			Current = HeadNode;
			for (int t = 0; t < PrintNodePoint; t++) Current = Current->NextAlarm;

			for (; Current != NULL && (i < 10); Current = Current->NextAlarm) {
				if (mainCursor.x > ALARMTABLE_LEFT && mainCursor.x < ALARMTABLE_RIGHT && mainCursor.y > ALARMTABLE_TOP + (i * 49) && mainCursor.y < ALARMTABLE_TOP + (++i * 49)) {
					Current->Selected = true;
					break;
				}
			}
			InvalidateRect(hWnd, &AlarmTableRect, false);
			break;
		}
	}
	else if (FocusWnd == 1) {
		switch (type) {
			// Hour
		case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31:
		case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43:
			PrintSelectedButton(Instance, hWnd, OldHourType, FocusWnd, false);
			PrintSelectedButton(Instance, hWnd, type, FocusWnd, true);
			OldHourType = type;
			tSelectedTime->Hou = type;
			break;

			// Minute
		case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61:
		case 62: case 63: case 64: case 65: case 66: case 67: case 68: case 69: case 70: case 71: case 72: case 73:
		case 74: case 75: case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85:
		case 86: case 87: case 88: case 89: case 90: case 91: case 92: case 93: case 94: case 95: case 96: case 97:
		case 98: case 99: case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109:
			PrintSelectedButton(Instance, hWnd, OldMinuteType, FocusWnd, false);
			PrintSelectedButton(Instance, hWnd, type, FocusWnd, true);
			OldMinuteType = type;
			tSelectedTime->Min = type;
			break;

			// RepeatWeek
		case 120: case 121: case 122: case 123: case 124: case 125: case 126:
			if (tSelectedTime->RepeatWeek[type - 120] == 0) tSelectedTime->RepeatWeek[type - 120] = 1;
			else tSelectedTime->RepeatWeek[type - 120] = 0;
			InvalidateRect(hWnd, NULL, true);
			break;

			// Button for Browse.
		case 7:
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = L"All Files(*.*)\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = 512;

			if (GetOpenFileName(&ofn))
			{
				int i;
				int nlpstrFile = wcslen(ofn.lpstrFile);
				WCHAR tChar = ofn.lpstrFile[nlpstrFile];

				memset(NewNode->szSoundFileName, 0, sizeof(NewNode->szSoundFileName));
				memset(NewNode->szSoundFilePath, 0, sizeof(NewNode->szSoundFilePath));
				wcsncpy(NewNode->szSoundFilePath, ofn.lpstrFile, nlpstrFile);
				for (i = nlpstrFile; tChar != '\\' && i >= 0; i--) tChar = ofn.lpstrFile[i];
				i += 2;
				wcsncpy(NewNode->szSoundFileName, &ofn.lpstrFile[i], nlpstrFile - i);
				
			}

			InvalidateRect(hWnd, &SoundPathRect_add, false);
			break;

			// Button for Create and Cancel.
		case 150: case 151:
			if (type == 150) {
				CreateAlarm(tSelectedTime, MemoData, NewNode);
				NumOfAlarm = AppendNode(&HeadNode, NewNode);
				if (NumOfAlarm > 10) PrintNodePoint++;
				AlarmFileWriter(HeadNode);
				NewNode = NULL;
			}
			else if (type == 151) {
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
	else if (FocusWnd == 2) {

		switch (type) {
			// Hour
		case 220: case 221: case 222: case 223: case 224: case 225: case 226: case 227: case 228: case 229: case 230: case 231:
		case 232: case 233: case 234: case 235: case 236: case 237: case 238: case 239: case 240: case 241: case 242: case 243:
			if (IsFirstH) { PrintSelectedButton(Instance, hWnd, SelectedNode->time.Hou + 219, FocusWnd, false); IsFirstH = false; }
			PrintSelectedButton(Instance, hWnd, OldHourType, FocusWnd, false);
			PrintSelectedButton(Instance, hWnd, type, FocusWnd, true);
			OldHourType = type;
			tSelectedTime->Hou = type;
			break;

			// Minute
		case 250: case 251: case 252: case 253: case 254: case 255: case 256: case 257: case 258: case 259: case 260: case 261:
		case 262: case 263: case 264: case 265: case 266: case 267: case 268: case 269: case 270: case 271: case 272: case 273:
		case 274: case 275: case 276: case 277: case 278: case 279: case 280: case 281: case 282: case 283: case 284: case 285:
		case 286: case 287: case 288: case 289: case 290: case 291: case 292: case 293: case 294: case 295: case 296: case 297:
		case 298: case 299: case 300: case 301: case 302: case 303: case 304: case 305: case 306: case 307: case 308: case 309:
			if (IsFirstM) { PrintSelectedButton(Instance, hWnd, SelectedNode->time.Min + 250, FocusWnd, false); IsFirstM = false; }
			PrintSelectedButton(Instance, hWnd, OldMinuteType, FocusWnd, false);
			PrintSelectedButton(Instance, hWnd, type, FocusWnd, true);
			OldMinuteType = type;
			tSelectedTime->Min = type;
			break;

			// RepeatWeek
		case 320: case 321: case 322: case 323: case 324: case 325: case 326:
			if (tSelectedTime->RepeatWeek[type - 320] == 0) tSelectedTime->RepeatWeek[type - 320] = 1;
			else tSelectedTime->RepeatWeek[type - 320] = 0;
			InvalidateRect(hWnd, NULL, true);
			break;

			// Button for Browse.
		case 7:
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFilter = L"All Files(*.*)\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = 512;

			if (GetOpenFileName(&ofn))
			{
				int i;
				int nlpstrFile = wcslen(ofn.lpstrFile);
				WCHAR tChar = ofn.lpstrFile[nlpstrFile];

				memset(SelectedNode->szSoundFileName, 0, sizeof(SelectedNode->szSoundFileName));
				memset(SelectedNode->szSoundFilePath, 0, sizeof(SelectedNode->szSoundFilePath));
				wcsncpy(SelectedNode->szSoundFilePath, ofn.lpstrFile, nlpstrFile + 1);
				for (i = nlpstrFile; tChar != '\\' && i >= 0; i--) tChar = ofn.lpstrFile[i];
				i += 2;
				wcsncpy(SelectedNode->szSoundFileName, &ofn.lpstrFile[i], nlpstrFile - i);
			}
			InvalidateRect(hWnd, &SoundPathRect_modify, false);
			break;

			// Button for Modify and Cancel.
		case 350: case 351:
			if (type == 350) {
				if (tSelectedTime->Hou != 0) SelectedNode->time.Hou = tSelectedTime->Hou - 219;
				else SelectedNode->time.Hou = 0;
				
				if (tSelectedTime->Min != 0) SelectedNode->time.Min = tSelectedTime->Min - 250;
				else SelectedNode->time.Min = 0;

				for (int i = 0; i < 7; i++) SelectedNode->time.RepeatWeek[i] = tSelectedTime->RepeatWeek[i];
				wcsncpy(SelectedNode->MemoData, MemoData, wcslen(MemoData) + 1);
				AlarmFileWriter(HeadNode);
			}
			else if (type == 351) {
				memset(SelectedNode->szSoundFileName, 0, sizeof(SelectedNode->szSoundFileName));
				memset(SelectedNode->szSoundFilePath, 0, sizeof(SelectedNode->szSoundFilePath));
				wcsncpy(SelectedNode->szSoundFileName, OldFileName, wcslen(OldFileName) + 1);
				wcsncpy(SelectedNode->szSoundFilePath, OldFilePath, wcslen(OldFilePath) + 1);
			}

			IsFirstH = true;
			IsFirstM = true;
			FocusWnd = 0;
			bModifyMenu = false;
			ModifyMenuFirstMotion = true;
			memset(tSelectedTime, 0, sizeof(TIME));
			memset(MemoData, 0, MEMO_MAXBUF * sizeof(wchar_t));
			memset(OldFileName, 0, sizeof(OldFileName));
			memset(OldFilePath, 0, sizeof(OldFilePath));
			MemoDataLen = 0;

			InvalidateRect(hWnd, NULL, true);
			break;
		}
	}
}
