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
	DeleteObject(BarBit);
	DeleteObject(AddBit);
	DeleteObject(ModifyBit);
	DeleteObject(DeleteBit);
	DeleteObject(CopyBit);
	DeleteDC(MemDC);
}

////
void PrintSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int FocusWnd, bool exist) {
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

	if (FocusWnd == 0) {
		switch (type) {
		case 1: Rectangle(hdc, ADD_LEFT, ADD_TOP, ADD_RIGHT, ADD_BOTTOM); break;
		case 2: Rectangle(hdc, MODIFY_LEFT, MODIFY_TOP, MODIFY_RIGHT, MODIFY_BOTTOM); break;
		case 3: Rectangle(hdc, DELETE_LEFT, DELETE_TOP, DELETE_RIGHT, DELETE_BOTTOM); break;
		case 4: Rectangle(hdc, COPY_LEFT, COPY_TOP, COPY_RIGHT, COPY_BOTTOM); break;
		}
	} else if (FocusWnd == 1) {
		switch (type) {
		case 1: Rectangle(hdc, HB_LEFT, HB_TOP, HB_RIGHT, HB_BOTTOM); break;
		case 2: Rectangle(hdc, MB_LEFT, MB_TOP, MB_RIGHT, MB_BOTTOM); break;
		}
	}

	SelectObject(hdc, OldBrush);
	SelectObject(hdc, OldPen);
	DeleteObject(RectBrush);
	ReleaseDC(hWnd, hdc);
}

////
int CheckingMousePos(int x, int y, int focus) {
	if (focus == 0) {
		if (x > ADD_LEFT && x < ADD_RIGHT && y > ADD_TOP && y < ADD_BOTTOM) return 1;
		else if (x > MODIFY_LEFT && x < MODIFY_RIGHT && y > MODIFY_TOP && y < MODIFY_BOTTOM) return 2;
		else if (x > DELETE_LEFT && x < DELETE_RIGHT && y > DELETE_TOP && y < DELETE_BOTTOM) return 3;
		else if (x > COPY_LEFT && x < COPY_RIGHT && y > COPY_TOP && y < COPY_BOTTOM) return 4;
		else return 0;
	} else if (focus == 1) {
		if (x > HB_LEFT && x < HB_RIGHT && y > HB_TOP && y < HB_BOTTOM) return 1;
		else if (x > MB_LEFT && x < MB_RIGHT && y > MB_TOP && y < MB_BOTTOM) return 2;
		else return 0;
	}
	else return 0;
}

////
void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int FocusWnd) {
	static int onButtonType = 0;
	static bool bPSB = true, bPNSB = false;

	if (FocusWnd == 0) {
		switch (type) {
		case 1: if (bPSB) { PrintSelectedButton(Instance, hWnd, 1, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 1; } return;
		case 2: if (bPSB) { PrintSelectedButton(Instance, hWnd, 2, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 2; } return;
		case 3: if (bPSB) { PrintSelectedButton(Instance, hWnd, 3, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 3; } return;
		case 4: if (bPSB) { PrintSelectedButton(Instance, hWnd, 4, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 4; } return;
		case 0: if (bPNSB) { PrintSelectedButton(Instance, hWnd, onButtonType, FocusWnd, false); bPSB = true; bPNSB = false; } return;
		}
	} else if (FocusWnd == 1) {
		switch (type) {
		case 1: if (bPSB) { PrintSelectedButton(Instance, hWnd, 1, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 1; } return;
		case 2: if (bPSB) { PrintSelectedButton(Instance, hWnd, 2, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 2; } return;
		case 0: if (bPNSB) { PrintSelectedButton(Instance, hWnd, onButtonType, FocusWnd, false); bPSB = true; bPNSB = false; } return;
		}
	}
}

void AppearAddMenu(HINSTANCE Instance, HWND hWnd, HDC hdc, int *focus) {
	HDC MemDC;
	int StayX = 1;
	int Frame = StayX;
	HPEN BorderPen, OldPen;
	HBITMAP TitleBit, HourBit, MinuteBit, RepeatWeekBit, MemoBit, OldBit;
	static bool FirstMotion = true;

	if (FirstMotion) {
		BorderPen = CreatePen(PS_SOLID, 1, RGB(61, 183, 204));
		OldPen = (HPEN)SelectObject(hdc, BorderPen);
		while (true) {
			if (StayX != 35 && --Frame == 0) {
				Rectangle(hdc, 0, 50, StayX * 10, 450);
				Frame = StayX++;
			}
			else if (StayX == 35) break;
			Sleep(1);
		}
		SelectObject(hdc, OldPen);
		DeleteObject(BorderPen);

		*focus = 1;
		FirstMotion = false;
	}
	else Rectangle(hdc, 0, 50, 350, 450);

	MemDC = CreateCompatibleDC(hdc);
	TitleBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP11));
	HourBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP7));
	MinuteBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP8));
	RepeatWeekBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP9));
	MemoBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP10));

	OldBit = (HBITMAP)SelectObject(MemDC, TitleBit);
	BitBlt(hdc, 10, 55, 350, 55, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, HourBit);
	BitBlt(hdc, 10, 105, 350, 55, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, MinuteBit);
	BitBlt(hdc, 10, 170, 350, 110, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, RepeatWeekBit);
	BitBlt(hdc, 10, 280, 350, 45, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, MemoBit);
	BitBlt(hdc, 10, 325, 350, 45, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBit);
	DeleteObject(TitleBit);
	DeleteObject(HourBit);
	DeleteObject(MinuteBit);
	DeleteObject(RepeatWeekBit);
	DeleteObject(MemoBit);
	DeleteDC(MemDC);

}