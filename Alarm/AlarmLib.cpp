#define _CRT_SECURE_NO_WARNINGS
#include "AlarmLib.h"
#include "resource.h"

/*
ALARM* CreateAlarm(int nRepeatDay[7], WCHAR *lpSoundFilePath, const char *nMemo) {
	ALARM *NewAlarm = (ALARM*)malloc(sizeof(ALARM));

	for (int i = 0; i < 7; i++) NewAlarm->RepeatDay[i] = nRepeatDay[i];
	wcsncpy(NewAlarm->szSoundFilePath, lpSoundFilePath, wcslen(lpSoundFilePath) + 1);
	strncpy(NewAlarm->Memo, nMemo, strlen(nMemo) + 1);
	NewAlarm->NextAlarm = NULL;

	return NewAlarm;
}
*/

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
void PrintSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int *FocusWnd, bool exist) {
	if (type != 0) {
		HDC hdc;
		HBRUSH RectBrush, OldBrush;
		HPEN RectPen, OldPen;
		hdc = GetDC(hWnd);

		RectBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		OldBrush = (HBRUSH)SelectObject(hdc, RectBrush);

		if (exist) {
			RectPen = CreatePen(PS_SOLID, 1, RGB(61, 183, 204));
			OldPen = (HPEN)SelectObject(hdc, RectPen);
		}
		else {
			RectPen = (HPEN)GetStockObject(WHITE_PEN);
			OldPen = (HPEN)SelectObject(hdc, RectPen);
		}

		if (*FocusWnd == 0) {
			switch (type) {
			case 1: Rectangle(hdc, ADD_LEFT, ADD_TOP, ADD_RIGHT, ADD_BOTTOM); break;
			case 2: Rectangle(hdc, MODIFY_LEFT, MODIFY_TOP, MODIFY_RIGHT, MODIFY_BOTTOM); break;
			case 3: Rectangle(hdc, DELETE_LEFT, DELETE_TOP, DELETE_RIGHT, DELETE_BOTTOM); break;
			case 4: Rectangle(hdc, COPY_LEFT, COPY_TOP, COPY_RIGHT, COPY_BOTTOM); break;
			}
		}
		else if (*FocusWnd == 1) {
			switch (type) {
			case 1: Rectangle(hdc, HB_LEFT, HB_TOP, HB_RIGHT, HB_BOTTOM); break;
			case 2: Rectangle(hdc, MB_LEFT, MB_TOP, MB_RIGHT, MB_BOTTOM); break;
			case 3: Rectangle(hdc, RB_LEFT, RB_TOP, RB_RIGHT, RB_BOTTOM); break;
			case 4: Rectangle(hdc, MEB_LEFT, MEB_TOP, MEB_RIGHT, MEB_BOTTOM); break;

			case 20: Rectangle(hdc, X0, Y0, X1, Y1); break;
			case 21: Rectangle(hdc, X1, Y0, X2, Y1); break;
			case 22: Rectangle(hdc, X2, Y0, X3, Y1); break;
			case 23: Rectangle(hdc, X3, Y0, X4, Y1); break;
			case 24: Rectangle(hdc, X4, Y0, X5, Y1); break;
			case 25: Rectangle(hdc, X5, Y0, X6, Y1); break;
			case 26: Rectangle(hdc, X6, Y0, X7, Y1); break;
			case 27: Rectangle(hdc, X7, Y0, X8, Y1); break;
			case 28: Rectangle(hdc, X8, Y0, X9, Y1); break;
			case 29: Rectangle(hdc, X9, Y0, X10, Y1); break;
			case 30: Rectangle(hdc, X10, Y0, X11, Y1); break;
			case 31: Rectangle(hdc, X11, Y0, X12, Y1); break;

			case 32: Rectangle(hdc, X0, Y1, X1, Y2); break;
			case 33: Rectangle(hdc, X1, Y1, X2, Y2); break;
			case 34: Rectangle(hdc, X2, Y1, X3, Y2); break;
			case 35: Rectangle(hdc, X3, Y1, X4, Y2); break;
			case 36: Rectangle(hdc, X4, Y1, X5, Y2); break;
			case 37: Rectangle(hdc, X5, Y1, X6, Y2); break;
			case 38: Rectangle(hdc, X6, Y1, X7, Y2); break;
			case 39: Rectangle(hdc, X7, Y1, X8, Y2); break;
			case 40: Rectangle(hdc, X8, Y1, X9, Y2); break;
			case 41: Rectangle(hdc, X9, Y1, X10, Y2); break;
			case 42: Rectangle(hdc, X10, Y1, X11, Y2); break;
			case 43: Rectangle(hdc, X11, Y1, X12, Y2); break;

			case 50: Rectangle(hdc, X0, Y3, X1, Y4); break;
			case 51: Rectangle(hdc, X1, Y3, X2, Y4); break;
			case 52: Rectangle(hdc, X2, Y3, X3, Y4); break;
			case 53: Rectangle(hdc, X3, Y3, X4, Y4); break;
			case 54: Rectangle(hdc, X4, Y3, X5, Y4); break;
			case 55: Rectangle(hdc, X5, Y3, X6, Y4); break;
			case 56: Rectangle(hdc, X6, Y3, X7, Y4); break;
			case 57: Rectangle(hdc, X7, Y3, X8, Y4); break;
			case 58: Rectangle(hdc, X8, Y3, X9, Y4); break;
			case 59: Rectangle(hdc, X9, Y3, X10, Y4); break;
			case 60: Rectangle(hdc, X10, Y3, X11, Y4); break;
			case 61: Rectangle(hdc, X11, Y3, X12, Y4); break;

			case 62: Rectangle(hdc, X0, Y4, X1, Y5); break;
			case 63: Rectangle(hdc, X1, Y4, X2, Y5); break;
			case 64: Rectangle(hdc, X2, Y4, X3, Y5); break;
			case 65: Rectangle(hdc, X3, Y4, X4, Y5); break;
			case 66: Rectangle(hdc, X4, Y4, X5, Y5); break;
			case 67: Rectangle(hdc, X5, Y4, X6, Y5); break;
			case 68: Rectangle(hdc, X6, Y4, X7, Y5); break;
			case 69: Rectangle(hdc, X7, Y4, X8, Y5); break;
			case 70: Rectangle(hdc, X8, Y4, X9, Y5); break;
			case 71: Rectangle(hdc, X9, Y4, X10, Y5); break;
			case 72: Rectangle(hdc, X10, Y4, X11, Y5); break;
			case 73: Rectangle(hdc, X11, Y4, X12, Y5); break;

			case 74: Rectangle(hdc, X0, Y5, X1, Y6); break;
			case 75: Rectangle(hdc, X1, Y5, X2, Y6); break;
			case 76: Rectangle(hdc, X2, Y5, X3, Y6); break;
			case 77: Rectangle(hdc, X3, Y5, X4, Y6); break;
			case 78: Rectangle(hdc, X4, Y5, X5, Y6); break;
			case 79: Rectangle(hdc, X5, Y5, X6, Y6); break;
			case 80: Rectangle(hdc, X6, Y5, X7, Y6); break;
			case 81: Rectangle(hdc, X7, Y5, X8, Y6); break;
			case 82: Rectangle(hdc, X8, Y5, X9, Y6); break;
			case 83: Rectangle(hdc, X9, Y5, X10, Y6); break;
			case 84: Rectangle(hdc, X10, Y5, X11, Y6); break;
			case 85: Rectangle(hdc, X11, Y5, X12, Y6); break;

			case 86: Rectangle(hdc, X0, Y6, X1, Y7); break;
			case 87: Rectangle(hdc, X1, Y6, X2, Y7); break;
			case 88: Rectangle(hdc, X2, Y6, X3, Y7); break;
			case 89: Rectangle(hdc, X3, Y6, X4, Y7); break;
			case 90: Rectangle(hdc, X4, Y6, X5, Y7); break;
			case 91: Rectangle(hdc, X5, Y6, X6, Y7); break;
			case 92: Rectangle(hdc, X6, Y6, X7, Y7); break;
			case 93: Rectangle(hdc, X7, Y6, X8, Y7); break;
			case 94: Rectangle(hdc, X8, Y6, X9, Y7); break;
			case 95: Rectangle(hdc, X9, Y6, X10, Y7); break;
			case 96: Rectangle(hdc, X10, Y6, X11, Y7); break;
			case 97: Rectangle(hdc, X11, Y6, X12, Y7); break;

			case 98: Rectangle(hdc, X0, Y7, X1, Y8); break;
			case 99: Rectangle(hdc, X1, Y7, X2, Y8); break;
			case 100: Rectangle(hdc, X2, Y7, X3, Y8); break;
			case 101: Rectangle(hdc, X3, Y7, X4, Y8); break;
			case 102: Rectangle(hdc, X4, Y7, X5, Y8); break;
			case 103: Rectangle(hdc, X5, Y7, X6, Y8); break;
			case 104: Rectangle(hdc, X6, Y7, X7, Y8); break;
			case 105: Rectangle(hdc, X7, Y7, X8, Y8); break;
			case 106: Rectangle(hdc, X8, Y7, X9, Y8); break;
			case 107: Rectangle(hdc, X9, Y7, X10, Y8); break;
			case 108: Rectangle(hdc, X10, Y7, X11, Y8); break;
			case 109: Rectangle(hdc, X11, Y7, X12, Y8); break;

			case 120: Rectangle(hdc, R_X0, Y9, R_X1, Y10); break;
			case 121: Rectangle(hdc, R_X2, Y9, R_X3, Y10); break;
			case 122: Rectangle(hdc, R_X4, Y9, R_X5, Y10); break;
			case 123: Rectangle(hdc, R_X6, Y9, R_X7, Y10); break;
			case 124: Rectangle(hdc, R_X8, Y9, R_X9, Y10); break;
			case 125: Rectangle(hdc, R_X10, Y9, R_X11, Y10); break;
			case 126: Rectangle(hdc, R_X12, Y9, R_X13, Y10); break;

			}
		}

		SelectObject(hdc, OldBrush);
		SelectObject(hdc, OldPen);
		DeleteObject(RectBrush);
		ReleaseDC(hWnd, hdc);
	}
}

////
int CheckingMousePos(int x, int y, int FocusWnd, bool click) {
	if (FocusWnd == 0) {
		if (x > ADD_LEFT && x < ADD_RIGHT && y > ADD_TOP && y < ADD_BOTTOM) return 1;
		else if (x > MODIFY_LEFT && x < MODIFY_RIGHT && y > MODIFY_TOP && y < MODIFY_BOTTOM) return 2;
		else if (x > DELETE_LEFT && x < DELETE_RIGHT && y > DELETE_TOP && y < DELETE_BOTTOM) return 3;
		else if (x > COPY_LEFT && x < COPY_RIGHT && y > COPY_TOP && y < COPY_BOTTOM) return 4;
		else return 0;
	}
	else if (FocusWnd == 1) {
		if (x > HB_LEFT && x < HB_RIGHT && y > HB_TOP && y < HB_BOTTOM) {
			if (click && x > X0 && x < X1 && y > Y0 && y < Y1) { NewNode->time.Hou = 1; return 20; }
			if (click && x > X1 && x < X2 && y > Y0 && y < Y1) { NewNode->time.Hou = 2;  return 21; }
			if (click && x > X2 && x < X3 && y > Y0 && y < Y1) { NewNode->time.Hou = 3;  return 22;	}
			if (click && x > X3 && x < X4 && y > Y0 && y < Y1) { NewNode->time.Hou = 4;  return 23; }
			if (click && x > X4 && x < X5 && y > Y0 && y < Y1) { NewNode->time.Hou = 5;  return 24; }
			if (click && x > X5 && x < X6 && y > Y0 && y < Y1) { NewNode->time.Hou = 6;  return 25; }
			if (click && x > X6 && x < X7 && y > Y0 && y < Y1) { NewNode->time.Hou = 7;  return 26; }
			if (click && x > X7 && x < X8 && y > Y0 && y < Y1) { NewNode->time.Hou = 8;  return 27; }
			if (click && x > X8 && x < X9 && y > Y0 && y < Y1) { NewNode->time.Hou = 9;  return 28; }
			if (click && x > X9 && x < X10 && y > Y0 && y < Y1) { NewNode->time.Hou = 10;  return 29; }
			if (click && x > X10 && x < X11 && y > Y0 && y < Y1) { NewNode->time.Hou = 11;  return 30; }
			if (click && x > X11 && x < X12 && y > Y0 && y < Y1) { NewNode->time.Hou = 12;  return 31; }

			if (click && x > X0 && x < X1 && y > Y1 && y < Y2) { NewNode->time.Hou = 13;  return 32; }
			if (click && x > X1 && x < X2 && y > Y1 && y < Y2) { NewNode->time.Hou = 14;  return 33; }
			if (click && x > X2 && x < X3 && y > Y1 && y < Y2) { NewNode->time.Hou = 15;  return 34; }
			if (click && x > X3 && x < X4 && y > Y1 && y < Y2) { NewNode->time.Hou = 16;  return 35; }
			if (click && x > X4 && x < X5 && y > Y1 && y < Y2) { NewNode->time.Hou = 17;  return 36; }
			if (click && x > X5 && x < X6 && y > Y1 && y < Y2) { NewNode->time.Hou = 18;  return 37; }
			if (click && x > X6 && x < X7 && y > Y1 && y < Y2) { NewNode->time.Hou = 19;  return 38; }
			if (click && x > X7 && x < X8 && y > Y1 && y < Y2) { NewNode->time.Hou = 20;  return 39; }
			if (click && x > X8 && x < X9 && y > Y1 && y < Y2) { NewNode->time.Hou = 21;  return 40; }
			if (click && x > X9 && x < X10 && y > Y1 && y < Y2) { NewNode->time.Hou = 22;  return 41; }
			if (click && x > X10 && x < X11 && y > Y1 && y < Y2) { NewNode->time.Hou = 23;  return 42; }
			if (click && x > X11 && x < X12 && y > Y1 && y < Y2) { NewNode->time.Hou = 00;  return 43; }
			return 1;
		}
		if (x > MB_LEFT && x < MB_RIGHT && y > MB_TOP && y < MB_BOTTOM) {
			if (click && x > X0 && x < X1 && y > Y3 && y < Y4) { NewNode->time.Min = 0; return 50; }
			if (click && x > X1 && x < X2 && y > Y3 && y < Y4) { NewNode->time.Min = 1; return 51; }
			if (click && x > X2 && x < X3 && y > Y3 && y < Y4) { NewNode->time.Min = 2; return 52; }
			if (click && x > X3 && x < X4 && y > Y3 && y < Y4) { NewNode->time.Min = 3; return 53; }
			if (click && x > X4 && x < X5 && y > Y3 && y < Y4) { NewNode->time.Min = 4; return 54; }
			if (click && x > X5 && x < X6 && y > Y3 && y < Y4) { NewNode->time.Min = 5; return 55; }
			if (click && x > X6 && x < X7 && y > Y3 && y < Y4) { NewNode->time.Min = 6; return 56; }
			if (click && x > X7 && x < X8 && y > Y3 && y < Y4) { NewNode->time.Min = 7; return 57; }
			if (click && x > X8 && x < X9 && y > Y3 && y < Y4) { NewNode->time.Min = 8; return 58; }
			if (click && x > X9 && x < X10 && y > Y3 && y < Y4) { NewNode->time.Min = 9; return 59; }
			if (click && x > X10 && x < X11 && y > Y3 && y < Y4) { NewNode->time.Min = 10; return 60; }
			if (click && x > X11 && x < X12 && y > Y3 && y < Y4) { NewNode->time.Min = 11; return 61; }

			if (click && x > X0 && x < X1 && y > Y4 && y < Y5) { NewNode->time.Min = 12; return 62; }
			if (click && x > X1 && x < X2 && y > Y4 && y < Y5) { NewNode->time.Min = 13; return 63; }
			if (click && x > X2 && x < X3 && y > Y4 && y < Y5) { NewNode->time.Min = 14; return 64; }
			if (click && x > X3 && x < X4 && y > Y4 && y < Y5) { NewNode->time.Min = 15; return 65; }
			if (click && x > X4 && x < X5 && y > Y4 && y < Y5) { NewNode->time.Min = 16; return 66; }
			if (click && x > X5 && x < X6 && y > Y4 && y < Y5) { NewNode->time.Min = 17; return 67; }
			if (click && x > X6 && x < X7 && y > Y4 && y < Y5) { NewNode->time.Min = 18; return 68; }
			if (click && x > X7 && x < X8 && y > Y4 && y < Y5) { NewNode->time.Min = 19; return 69; }
			if (click && x > X8 && x < X9 && y > Y4 && y < Y5) { NewNode->time.Min = 20; return 70; }
			if (click && x > X9 && x < X10 && y > Y4 && y < Y5) { NewNode->time.Min = 21; return 71; }
			if (click && x > X10 && x < X11 && y > Y4 && y < Y5) { NewNode->time.Min = 22; return 72; }
			if (click && x > X11 && x < X12 && y > Y4 && y < Y5) { NewNode->time.Min = 23; return 73; }

			if (click && x > X0 && x < X1 && y > Y5 && y < Y6) { NewNode->time.Min = 24; return 74; }
			if (click && x > X1 && x < X2 && y > Y5 && y < Y6) { NewNode->time.Min = 25; return 75; }
			if (click && x > X2 && x < X3 && y > Y5 && y < Y6) { NewNode->time.Min = 26; return 76; }
			if (click && x > X3 && x < X4 && y > Y5 && y < Y6) { NewNode->time.Min = 27; return 77; }
			if (click && x > X4 && x < X5 && y > Y5 && y < Y6) { NewNode->time.Min = 28; return 78; }
			if (click && x > X5 && x < X6 && y > Y5 && y < Y6) { NewNode->time.Min = 29; return 79; }
			if (click && x > X6 && x < X7 && y > Y5 && y < Y6) { NewNode->time.Min = 30; return 80; }
			if (click && x > X7 && x < X8 && y > Y5 && y < Y6) { NewNode->time.Min = 31; return 81; }
			if (click && x > X8 && x < X9 && y > Y5 && y < Y6) { NewNode->time.Min = 32; return 82; }
			if (click && x > X9 && x < X10 && y > Y5 && y < Y6) { NewNode->time.Min = 33; return 83; }
			if (click && x > X10 && x < X11 && y > Y5 && y < Y6) { NewNode->time.Min = 34; return 84; }
			if (click && x > X11 && x < X12 && y > Y5 && y < Y6) { NewNode->time.Min = 35; return 85; }

			if (click && x > X0 && x < X1 && y > Y6 && y < Y7) { NewNode->time.Min = 36; return 86; }
			if (click && x > X1 && x < X2 && y > Y6 && y < Y7) { NewNode->time.Min = 37; return 87; }
			if (click && x > X2 && x < X3 && y > Y6 && y < Y7) { NewNode->time.Min = 38; return 88; }
			if (click && x > X3 && x < X4 && y > Y6 && y < Y7) { NewNode->time.Min = 39; return 89; }
			if (click && x > X4 && x < X5 && y > Y6 && y < Y7) { NewNode->time.Min = 40; return 90; }
			if (click && x > X5 && x < X6 && y > Y6 && y < Y7) { NewNode->time.Min = 41; return 91; }
			if (click && x > X6 && x < X7 && y > Y6 && y < Y7) { NewNode->time.Min = 42; return 92; }
			if (click && x > X7 && x < X8 && y > Y6 && y < Y7) { NewNode->time.Min = 43; return 93; }
			if (click && x > X8 && x < X9 && y > Y6 && y < Y7) { NewNode->time.Min = 44; return 94; }
			if (click && x > X9 && x < X10 && y > Y6 && y < Y7) { NewNode->time.Min = 45; return 95; }
			if (click && x > X10 && x < X11 && y > Y6 && y < Y7) { NewNode->time.Min = 46; return 96; }
			if (click && x > X11 && x < X12 && y > Y6 && y < Y7) { NewNode->time.Min = 47; return 97; }

			if (click && x > X0 && x < X1 && y > Y7 && y < Y8) { NewNode->time.Min = 48; return 98; }
			if (click && x > X1 && x < X2 && y > Y7 && y < Y8) { NewNode->time.Min = 49; return 99; }
			if (click && x > X2 && x < X3 && y > Y7 && y < Y8) { NewNode->time.Min = 50; return 100; }
			if (click && x > X3 && x < X4 && y > Y7 && y < Y8) { NewNode->time.Min = 51; return 101; }
			if (click && x > X4 && x < X5 && y > Y7 && y < Y8) { NewNode->time.Min = 52; return 102; }
			if (click && x > X5 && x < X6 && y > Y7 && y < Y8) { NewNode->time.Min = 53; return 103; }
			if (click && x > X6 && x < X7 && y > Y7 && y < Y8) { NewNode->time.Min = 54; return 104; }
			if (click && x > X7 && x < X8 && y > Y7 && y < Y8) { NewNode->time.Min = 55; return 105; }
			if (click && x > X8 && x < X9 && y > Y7 && y < Y8) { NewNode->time.Min = 56; return 106; }
			if (click && x > X9 && x < X10 && y > Y7 && y < Y8) { NewNode->time.Min = 57; return 107; }
			if (click && x > X10 && x < X11 && y > Y7 && y < Y8) { NewNode->time.Min = 58; return 108; }
			if (click && x > X11 && x < X12 && y > Y7 && y < Y8) { NewNode->time.Min = 59; return 109; }

			return 2;
		}
		if (x > RB_LEFT && x < RB_RIGHT && y > RB_TOP && y < RB_BOTTOM) {
			if (click && x > R_X0 && x < R_X1 && y > Y9 && y < Y10) return 120;
			if (click && x > R_X2 && x < R_X3 && y > Y9 && y < Y10) return 121;
			if (click && x > R_X4 && x < R_X5 && y > Y9 && y < Y10) return 122;
			if (click && x > R_X6 && x < R_X7 && y > Y9 && y < Y10) return 123;
			if (click && x > R_X8 && x < R_X9 && y > Y9 && y < Y10) return 124;
			if (click && x > R_X10 && x < R_X11 && y > Y9 && y < Y10) return 125;
			if (click && x > R_X12 && x < R_X13 && y > Y9 && y < Y10) return 126;
			
			return 3; }
		if (x > MEB_LEFT && x < MEB_RIGHT && y > MEB_TOP && y < MEB_BOTTOM) return 4;

		return 0;
	}
	return 0;
}

////
void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int *FocusWnd) {
	static int onButtonType = 0;

	if (*FocusWnd == 0) {
		switch (type) {
		case 1: if (bPSB) { PrintSelectedButton(Instance, hWnd, 1, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 1; } return;
		case 2: if (bPSB) { PrintSelectedButton(Instance, hWnd, 2, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 2; } return;
		case 3: if (bPSB) { PrintSelectedButton(Instance, hWnd, 3, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 3; } return;
		case 4: if (bPSB) { PrintSelectedButton(Instance, hWnd, 4, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 4; } return;
		case 0: if (bPNSB) { PrintSelectedButton(Instance, hWnd, onButtonType, FocusWnd, false); bPSB = true; bPNSB = false; } return;
		}
	} else if (*FocusWnd == 1) {
		switch (type) {
		case 1: if (bPSB) { PrintSelectedButton(Instance, hWnd, 1, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 1; } return;
		case 2: if (bPSB) { PrintSelectedButton(Instance, hWnd, 2, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 2; } return;
		case 3: if (bPSB) { PrintSelectedButton(Instance, hWnd, 3, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 3; } return;
		case 4: if (bPSB) { PrintSelectedButton(Instance, hWnd, 4, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 4; } return;
		case 0: if (bPNSB) { PrintSelectedButton(Instance, hWnd, onButtonType, FocusWnd, false); bPSB = true; bPNSB = false; } return;
		}
	}
}

void AppearAddMenu(HINSTANCE Instance, HWND hWnd, HDC hdc, TIME tSelectedTime, LPWSTR MemoData, int *FocusWnd) {
	HDC MemDC;
	HPEN BorderPen, OldPen;
	HFONT Font, OldFont;
	HBITMAP TitleBit, HourBit, MinuteBit, RepeatWeekBit, MemoBit, OldBit;
	static bool FirstMotion = true;

	if (NewNode == NULL) NewNode = (ALARM*)malloc(sizeof(ALARM));

	BorderPen = CreatePen(PS_SOLID, 1, RGB(61, 183, 204));
	OldPen = (HPEN)SelectObject(hdc, BorderPen);
	if (FirstMotion) {
		for (int StayX = 1; StayX <= 35; StayX++) {
			for (int Frame = StayX; Frame > 0; Frame--)
				Rectangle(hdc, 0, 50, StayX * 10, 450);
			Sleep(15);
		}
		*FocusWnd = 1;
		FirstMotion = false;
	} else Rectangle(hdc, 0, 50, 350, 450);
	SelectObject(hdc, OldPen);
	DeleteObject(BorderPen);

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

	PrintSelectedButton(Instance, hWnd, tSelectedTime.Hou, FocusWnd, true);
	PrintSelectedButton(Instance, hWnd, tSelectedTime.Min, FocusWnd, true);

	Font = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²Ã¼");
	OldFont = (HFONT)SelectObject(hdc, Font);
	SetTextColor(hdc, RGB(61, 183, 204));
	TextOut(hdc, MEB_LEFT + 3, MEB_TOP, MemoData, wcslen(MemoData));
	SelectObject(hdc, OldFont);
	DeleteObject(Font);
}