#ifndef _ALARMLIB_H_
#define _ALARMLIB_H_

#include <stdio.h>
#include <windows.h>
#include "ButtonPosition.h"

#define MEMO_MAXBUF 18

typedef struct tagTime {
	int Hou;
	int Min;
	int Sec;
	int RepeatWeek;
}TIME;

typedef struct tagAlarm {
	TIME time;
	int RepeatDay[7];
	WCHAR szSoundFilePath[512];
	LPWSTR MemoData = (LPWSTR)calloc(MEMO_MAXBUF, sizeof(wchar_t));
	struct tagAlarm *NextAlarm;
}ALARM;

static bool bPSB = true, bPNSB = false;		// Print Selected Button, Print No Selected Button.
static ALARM *NewNode = NULL;

//ALARM* CreateAlarm(int nRefeatDay[7], WCHAR *lpSoundFilePath, const char *nMemo);
void PrintMainDisplay(HINSTANCE Instac, HWND hWnd, HDC hdc, PAINTSTRUCT *ps);
void PrintSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int *FocusWnd, bool exist);
int CheckingMousePos(int x, int y, int FocusWnd, bool click);
void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int *FocusWnd);
void AppearAddMenu(HINSTANCE Instance, HWND hWnd, HDC hdc, TIME tSelectedTime, LPWSTR MemoData, int *FocusWnd);

#endif