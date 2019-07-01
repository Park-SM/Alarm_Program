#ifndef _ALARMLIB_H_
#define _ALARMLIB_H_

#include <stdio.h>
#include <windows.h>
#include "ButtonPosition.h"

#define MEMO_MAXBUF 17

typedef struct tagPoint {
	int x;
	int y;
}uPOINT;

typedef struct tagTime {
	int Hou;
	int Min;
	int Sec;
	int RepeatWeek[7];
}TIME;

typedef struct tagAlarm {
	TIME time;
	WCHAR szSoundFilePath[512];
	WCHAR szSoundFileName[128];
	WCHAR MemoData[MEMO_MAXBUF + 1];
	bool OnOff;
	bool Selected;
	struct tagAlarm *NextAlarm;
}ALARM;

static bool bPSB = true, bPNSB = false;		// Print Selected Button, Print No Selected Button.

void CreateAlarm(TIME *nSelectedTime, LPWSTR nMemoData, ALARM *NewNode);
void AlarmFileReader(ALARM **HeadNode, int *uNumOfAlarm);
void AlarmFileWriter(ALARM *HeadNode);
void DestroyList(ALARM **HeadNode, int *NumOfAlarm);
void DeleteAlarm(ALARM **HeadNode, int *NumOfAlarm);
int AppendNode(ALARM **HeadNode, ALARM *NewNode);
void PrintAlarmList(ALARM *HeadNode, HINSTANCE Instance, HDC hdc, int PrintNodePoint);
void PrintMainDisplay(HINSTANCE Instac, HWND hWnd, HDC hdc, PAINTSTRUCT *ps);
void PrintSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int FocusWnd, bool exist);
int CheckingMousePos(ALARM *TargetNode, TIME *tSelectedTime, int x, int y, int FocusWnd, bool click);
void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int FocusWnd);
void AppearAddMenu(HINSTANCE Instance, HWND hWnd, HDC hdc, TIME tSelectedTime, LPWSTR MemoData, ALARM **NewNode, bool *AddMenuFirstMotion, int *FocusWnd);
void AppearModifyMenu(HINSTANCE Instance, HWND hWnd, HDC hdc, TIME *tSelectedTime, LPWSTR MemoData, ALARM *SelectedNode, bool *ModifyMenuFistMotion, int *FocusWnd);

#endif