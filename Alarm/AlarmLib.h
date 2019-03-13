#ifndef _ALARMLIB_H_
#define _ALARMLIB_H_

#include <windows.h>
#include "ButtonPosition.h"


typedef struct tagTime {
	int Sec;
	int Min;
	int Hou;
}TIME;

typedef struct tagAlarm {
	TIME time;
	int RepeatDay[7];
	WCHAR szSoundFilePath[512];
	char Memo[256];

	struct tagAlarm *NextAlarm;
}ALARM;

ALARM* CreateAlarm(int hour, int minute, int second, int nRefeatDay[7], WCHAR *lpSoundFilePath, const char *nMemo);
void PrintMainDisplay(HINSTANCE Instac, HWND hWnd, HDC hdc, PAINTSTRUCT *ps);
void PrintSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int FocusWnd, bool exist);
int CheckingMousePos(int x, int y, int focus);
void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int FocusWnd);
void AppearAddMenu(HINSTANCE Instacne, HWND hWnd, HDC hdc, int *focus);

#endif