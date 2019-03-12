#ifndef _ALARMLIB_H_
#define _ALARMLIB_H_

#include <windows.h>

#define ADD_LEFT 121
#define ADD_TOP 93
#define ADD_RIGHT 160
#define ADD_BOTTOM 115

#define MODIFY_LEFT 174
#define MODIFY_TOP 93
#define MODIFY_RIGHT 239
#define MODIFY_BOTTOM 115 

#define DELETE_LEFT 249
#define DELETE_TOP 93
#define DELETE_RIGHT 311
#define DELETE_BOTTOM 115

#define COPY_LEFT 322
#define COPY_TOP 93
#define COPY_RIGHT 370
#define COPY_BOTTOM 115

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
void PrintSelectedButton(HINSTANCE Instance, HWND hWnd, const char type, bool exist);
int CheckingMousePos(int x, int y);
void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type);
void OnClickListener(HWND hWnd, int type);

#endif