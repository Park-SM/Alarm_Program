#define _CRT_SECURE_NO_WARNINGS
#include "AlarmLib.h"
#include "resource.h"

ALARM* CreateAlarm(int hour, int minute, int second, int nRepeatDay[7], WCHAR *lpSoundFilePath, const char *nMemo) {
	ALARM *NewAlarm = (ALARM*)malloc(sizeof(ALARM));
	NewAlarm->time.Hou = hour;
	NewAlarm->time.Min = minute;
	NewAlarm->time.Sec = second;
	for (int i = 0; i < 7; i++) NewAlarm->RepeatDay[i] = nRepeatDay[i];
	wcsncpy(NewAlarm->szSoundFilePath, lpSoundFilePath, wcslen(lpSoundFilePath) + 1);
	strncpy(NewAlarm->Memo, nMemo, strlen(nMemo) + 1);
	NewAlarm->NextAlarm = NULL;

	return NewAlarm;
}

void PrintMainDisplay(HINSTANCE Instance, HWND hWnd, HDC hdc, PAINTSTRUCT *ps) {
	HDC MemDC;
	HBITMAP TitleBit, BarBit, AddBit, ModifyBit, DeleteBit, CopyBit, OldBit;

	MemDC = CreateCompatibleDC(hdc);
	TitleBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP1));
	BarBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP2));
	AddBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP3));
	ModifyBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP4));
	DeleteBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP5));
	CopyBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP6));

	OldBit = (HBITMAP)SelectObject(MemDC, TitleBit);
	BitBlt(hdc, 68, 0, 500, 100, MemDC, 0, 0, SRCCOPY);		// Print Tilte string

	SelectObject(MemDC, BarBit);
	BitBlt(hdc, 40, 73, 500, 100, MemDC, 0, 0, SRCCOPY);	// Print Bar

	SelectObject(MemDC, AddBit);
	BitBlt(hdc, 115, 90, 45, 23, MemDC, 0, 0, SRCCOPY);		// Print Add Button

	SelectObject(MemDC, ModifyBit);
	BitBlt(hdc, 168, 90, 70, 23, MemDC, 0, 0, SRCCOPY);		// Print Modify Button

	SelectObject(MemDC, DeleteBit);
	BitBlt(hdc, 245, 90, 70, 23, MemDC, 0, 0, SRCCOPY);		// Print Delete Button

	SelectObject(MemDC, CopyBit);
	BitBlt(hdc, 318, 90, 70, 23, MemDC, 0, 0, SRCCOPY);		// Print Copy Button

	SelectObject(MemDC, OldBit);
	DeleteObject(TitleBit);
	DeleteDC(MemDC);
}

void PrintSelectedButton(HINSTANCE Instance, HWND hWnd, const char type, bool exist) {
	HDC hdc;
	HBRUSH RectBrush, OldBrush;
	HPEN RectPen, OldPen;
	hdc = GetDC(hWnd);
	
	RectBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	OldBrush = (HBRUSH)SelectObject(hdc, RectBrush);

	if (exist) {
		RectPen = CreatePen(PS_SOLID, 1, RGB(61, 183, 204));
		OldPen = (HPEN)SelectObject(hdc, RectPen);
	} else {
		RectPen = (HPEN)GetStockObject(WHITE_PEN);
		OldPen = (HPEN)SelectObject(hdc, RectPen);
	}

	switch (type) {
	case 'a':
		Rectangle(hdc, ADD_LEFT, ADD_TOP, ADD_RIGHT, ADD_BOTTOM);
		break;
	case 'm':	
		Rectangle(hdc, MODIFY_LEFT, MODIFY_TOP, MODIFY_RIGHT, MODIFY_BOTTOM);
		break;
	case 'd':
		Rectangle(hdc, DELETE_LEFT, DELETE_TOP, DELETE_RIGHT, DELETE_BOTTOM);
		break;
	case 'c':
		Rectangle(hdc, COPY_LEFT, COPY_TOP, COPY_RIGHT, COPY_BOTTOM);
		break;
	}
	SelectObject(hdc, OldBrush);
	SelectObject(hdc, OldPen);
	DeleteObject(RectBrush);
	ReleaseDC(hWnd, hdc);
}

int CheckingMousePos(int x, int y) {
	if (x > ADD_LEFT && x < ADD_RIGHT && y > ADD_TOP && y < ADD_BOTTOM) return 1;
	else if (x > MODIFY_LEFT && x < MODIFY_RIGHT && y > MODIFY_TOP && y < MODIFY_BOTTOM) return 2;
	else if (x > DELETE_LEFT && x < DELETE_RIGHT && y > DELETE_TOP && y < DELETE_BOTTOM) return 3;
	else if (x > COPY_LEFT && x < COPY_RIGHT && y > COPY_TOP && y < COPY_BOTTOM) return 4;
	else return 0;
}

void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type) {
	static char ButtonType = NULL;
	static bool bPSB = true, bPNSB = false;

	switch (type) {
	case 1:
		if (bPSB) {
			PrintSelectedButton(Instance, hWnd, 'a', true);
			bPSB = false;
			bPNSB = true;
			ButtonType = 'a';
		}
		return;
		
	case 2:
		if (bPSB) {
			PrintSelectedButton(Instance, hWnd, 'm', true);
			bPSB = false;
			bPNSB = true;
			ButtonType = 'm';
		}
		return;

	case 3:
		if (bPSB) {
			PrintSelectedButton(Instance, hWnd, 'd', true);
			bPSB = false;
			bPNSB = true;
			ButtonType = 'd';
		}
		return;

	case 4:
		if (bPSB) {
			PrintSelectedButton(Instance, hWnd, 'c', true);
			bPSB = false;
			bPNSB = true;
			ButtonType = 'c';
		}
		return;

	case 0:
		if (bPNSB) {
			PrintSelectedButton(Instance, hWnd, ButtonType, false);
			bPSB = true;
			bPNSB = false;
		}
		return;
	}	
}

void OnClickListener(HWND hWnd, int type) {
	switch (type) {
	case 1:
		MessageBox(hWnd, "Click Add button!!", "From. Park Alarm", MB_OK);
		break;
		
	case 2:
		MessageBox(hWnd, "Click Modify button!!", "From. Park Alarm", MB_OK);
		break;

	case 3:
		MessageBox(hWnd, "Click Delete button!!", "From. Park Alarm", MB_OK);
		break;

	case 4:
		MessageBox(hWnd, "Click Copy button!!", "From. Park Alarm", MB_OK);
		break;
	}
}