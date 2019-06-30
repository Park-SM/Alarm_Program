#define _CRT_SECURE_NO_WARNINGS
#include <locale.h>
#include "AlarmLib.h"
#include "resource.h"

void CreateAlarm(TIME *nSelectedTime, LPWSTR nMemoData, ALARM *NewNode) {
	for (int i = 0; i < 7; i++) NewNode->time.RepeatWeek[i] = nSelectedTime->RepeatWeek[i];
	memset(NewNode->MemoData, 0, MEMO_MAXBUF);
	wcsncpy(NewNode->MemoData, nMemoData, wcslen(nMemoData) + 1);
	NewNode->NextAlarm = NULL;
	NewNode->OnOff = true;
	NewNode->Selected = false;
}

// The difference between C's feof() and Pascal's eof().
// Pascal's function returns true if the next read will fail because of end of file. C's function returns true if the last function failed
void AlarmFileReader(ALARM **HeadNode, int *uNumOfAlarm) {
	ALARM *NewNode = NULL;
	FILE *hDataFile = fopen("D:\\AlarmData.txt", "rt");
	_wsetlocale(LC_ALL, L"korean");
	if (hDataFile != NULL) {
		while (1) {
			NewNode = (ALARM*)calloc(1, sizeof(ALARM));
			fwscanf(hDataFile, L"\n%d %d %d %d %d %d %d %d %d %d %ws %ws %ws %d %d", &NewNode->time.Hou, &NewNode->time.Min, &NewNode->time.Sec, &NewNode->time.RepeatWeek[0], &NewNode->time.RepeatWeek[1], &NewNode->time.RepeatWeek[2], &NewNode->time.RepeatWeek[3], &NewNode->time.RepeatWeek[4], &NewNode->time.RepeatWeek[5], &NewNode->time.RepeatWeek[6], NewNode->szSoundFileName, NewNode->szSoundFilePath, NewNode->MemoData, &NewNode->OnOff, &NewNode->Selected);
			if (feof(hDataFile)) { free(NewNode); break; }
			if (wcsncmp(NewNode->szSoundFileName, L"$", wcslen(L"$")) == 0) memset(NewNode->szSoundFileName, 0, 1);
			if (wcsncmp(NewNode->szSoundFilePath, L"$", wcslen(L"$")) == 0) memset(NewNode->szSoundFilePath, 0, 1);
			if (wcsncmp(NewNode->MemoData, L"$", wcslen(L"$")) == 0) memset(NewNode->MemoData, 0, 1);
			*uNumOfAlarm = AppendNode(HeadNode, NewNode);
		}
		fclose(hDataFile);
	}
}

void AlarmFileWriter(ALARM *HeadNode) {
	FILE *hDataFile = fopen("D:\\AlarmData.txt", "wt");
	_wsetlocale(LC_ALL, L"korean");
	if (hDataFile != NULL) {
		while (HeadNode != NULL) {
			if (HeadNode->szSoundFileName[0] == NULL) wcsncpy(HeadNode->szSoundFileName, L"$", wcslen(L"$"));
			if (HeadNode->szSoundFilePath[0] == NULL) wcsncpy(HeadNode->szSoundFilePath, L"$", wcslen(L"$"));
			if (HeadNode->MemoData[0] == NULL) wcsncpy(HeadNode->MemoData, L"$", wcslen(L"$"));
			fwprintf(hDataFile, L"%d %d %d %d %d %d %d %d %d %d %ws %ws %ws %d %d\n", HeadNode->time.Hou, HeadNode->time.Min, HeadNode->time.Sec, HeadNode->time.RepeatWeek[0], HeadNode->time.RepeatWeek[1], HeadNode->time.RepeatWeek[2], HeadNode->time.RepeatWeek[3], HeadNode->time.RepeatWeek[4], HeadNode->time.RepeatWeek[5], HeadNode->time.RepeatWeek[6], HeadNode->szSoundFileName, HeadNode->szSoundFilePath, HeadNode->MemoData, HeadNode->OnOff, HeadNode->Selected);
			if (wcsncmp(HeadNode->szSoundFileName, L"$", wcslen(L"$")) == 0) memset(HeadNode->szSoundFileName, 0x00, 1);
			if (wcsncmp(HeadNode->szSoundFilePath, L"$", wcslen(L"$")) == 0) memset(HeadNode->szSoundFilePath, 0x00, 1);
			if (wcsncmp(HeadNode->MemoData, L"$", wcslen(L"$")) == 0) memset(HeadNode->MemoData, 0x00, 1);
			HeadNode = HeadNode->NextAlarm;
		}
		fclose(hDataFile);
	}
}

void DeleteAlarm(ALARM **HeadNode, int *NumOfAlarm) {
	if (*HeadNode != NULL) {
		if ((*HeadNode)->Selected) {
			ALARM *DeleteAlarm = *HeadNode;
			*HeadNode = (*HeadNode)->NextAlarm;
			free(DeleteAlarm);
		} else {
			ALARM *Current = *HeadNode;
			while (Current->NextAlarm != NULL && !Current->NextAlarm->Selected) Current = Current->NextAlarm;
			if (Current->NextAlarm != NULL) {
				ALARM *DeleteAlarm = Current->NextAlarm;
				Current->NextAlarm = Current->NextAlarm->NextAlarm;
				free(DeleteAlarm);
			}
		}
		(*NumOfAlarm)--;
	}
}

int AppendNode(ALARM **HeadNode, ALARM *NewNode) {		// Return value is the number of Node.
	int count = 0;
	if (*HeadNode == NULL) {
		*HeadNode = NewNode;
	} else {
		ALARM *TailNode = *HeadNode;
		while (TailNode->NextAlarm != NULL) {
			TailNode = TailNode->NextAlarm;
			count++;
		}
		count++; // Counting for last node.
		TailNode->NextAlarm = NewNode;
	}
	return ++count;	// ++ is the NewNode.
}

void PrintAlarmList(ALARM *HeadNode, HINSTANCE Instance, HDC hdc, int PrintNodePoint) {
	if (HeadNode != NULL) {
		int SingleTimeDistanceH = 0;
		int SingleTimeDistanceM = -1;
		int PrintAlarm_y = 142;
		int PrintAlarmBorder_y = 130;
		LPWSTR Buf_Hour = (LPWSTR)calloc(3, sizeof(wchar_t));
		LPWSTR Buf_Minu = (LPWSTR)calloc(3, sizeof(wchar_t));
		HDC MemDC;
		HBITMAP UpBit, DownBit, OldBit;
		HPEN BorderPen, OldPen;
		HFONT TimeFont, MemoFont, RepeatFont, OldFont;
		HBRUSH SelectedBrush, OldBrush;

		BorderPen = CreatePen(PS_SOLID, 1, RGB(37, 177, 245));
		TimeFont = CreateFont(25, 0, 1, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²");
		MemoFont = CreateFont(16, 0, 1, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²");
		RepeatFont = CreateFont(11, 0, 1, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²");
		SelectedBrush = CreateSolidBrush(RGB(150, 200, 255));

		OldPen = (HPEN)SelectObject(hdc, BorderPen);
		OldFont = (HFONT)SelectObject(hdc, TimeFont);

		SetTextColor(hdc, RGB(37, 177, 245));
		for (int i = 0; i < PrintNodePoint; i++) HeadNode = HeadNode->NextAlarm;
		for (int i = PrintNodePoint; i < PrintNodePoint + 10 && HeadNode != NULL; i++) {	// Printing alarm object.
			OldBrush = (HBRUSH)SelectObject(hdc, SelectedBrush);
			if (!(HeadNode->Selected)) {
				SelectObject(hdc, OldBrush);
				SetBkColor(hdc, RGB(255, 255, 255));
			} else SetBkColor(hdc, RGB(150, 200, 255));
			Rectangle(hdc, 40, PrintAlarmBorder_y, 442, PrintAlarmBorder_y + 50);
			
			
			if (HeadNode->time.Hou < 10) {
				SingleTimeDistanceH = 6;
				TextOut(hdc, 76, PrintAlarm_y, L"0", wcslen(L"0"));
			}
			if (HeadNode->time.Min < 10) {
				SingleTimeDistanceM = 6;
				TextOut(hdc, 114, PrintAlarm_y, L"0", wcslen(L"0"));
			}
			_itow(HeadNode->time.Hou, Buf_Hour, 10);
			_itow(HeadNode->time.Min, Buf_Minu, 10);
			SetTextAlign(hdc, TA_CENTER);
			SelectObject(hdc, TimeFont);
			TextOut(hdc, 91 + SingleTimeDistanceH, PrintAlarm_y, Buf_Hour, wcslen(Buf_Hour));
			TextOut(hdc, 109, PrintAlarm_y, L":", wcslen(L":"));
			TextOut(hdc, 129 + SingleTimeDistanceM, PrintAlarm_y, Buf_Minu, wcslen(Buf_Minu));
			SingleTimeDistanceH = 0;
			SingleTimeDistanceM = -1;

			SetTextAlign(hdc, TA_LEFT);
			SelectObject(hdc, MemoFont);
			TextOut(hdc, 155, PrintAlarm_y, HeadNode->MemoData, wcslen(HeadNode->MemoData));
			SelectObject(hdc, RepeatFont);
			int count = 0;
			for (int i = 0; i < 7; i++) {
				if (HeadNode->time.RepeatWeek[i]) {
					count++;
					switch (i) {
					case 0: TextOut(hdc, 141 + (count * 15), PrintAlarm_y + 22, L"ÀÏ", wcslen(L"ÀÏ")); break;
					case 1: TextOut(hdc, 141 + (count * 15), PrintAlarm_y + 22, L"¿ù", wcslen(L"¿ù")); break;
					case 2: TextOut(hdc, 141 + (count * 15), PrintAlarm_y + 22, L"È­", wcslen(L"È­")); break;
					case 3: TextOut(hdc, 141 + (count * 15), PrintAlarm_y + 22, L"¼ö", wcslen(L"¼ö")); break;
					case 4: TextOut(hdc, 141 + (count * 15), PrintAlarm_y + 22, L"¸ñ", wcslen(L"¸ñ")); break;
					case 5: TextOut(hdc, 141 + (count * 15), PrintAlarm_y + 22, L"±Ý", wcslen(L"±Ý")); break;
					case 6: TextOut(hdc, 141 + (count * 15), PrintAlarm_y + 22, L"Åä", wcslen(L"Åä")); break;
					}
				}
			}

			SelectObject(hdc, TimeFont);
			PrintAlarm_y += 49;
			PrintAlarmBorder_y += 49;

			if (HeadNode->Selected) {
				SelectObject(hdc, OldBrush);
				SetBkColor(hdc, RGB(255, 255, 255));
			}
			HeadNode = HeadNode->NextAlarm;
		}

		SelectObject(hdc, OldFont);
		SelectObject(hdc, OldPen);

		MemDC = CreateCompatibleDC(hdc);
		UpBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP14));
		DownBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP15));

		OldBit = (HBITMAP)SelectObject(MemDC, UpBit);
		BitBlt(hdc, 442, 130, 19, 17, MemDC, 0, 0, SRCCOPY);

		SelectObject(MemDC, DownBit);
		BitBlt(hdc, 442, 603, 19, 17, MemDC, 0, 0, SRCCOPY);

		SelectObject(hdc, OldBit);
		DeleteObject(UpBit);
		DeleteObject(DownBit);

		DeleteObject(TimeFont);
		DeleteObject(MemoFont);
		DeleteObject(BorderPen);
		DeleteObject(SelectedBrush);
	}
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
	BitBlt(hdc, 68, 0, 356, 79, MemDC, 0, 0, SRCCOPY);		// Print Tilte string

	SelectObject(MemDC, BarBit);
	BitBlt(hdc, 40, 73, 402, 17, MemDC, 0, 0, SRCCOPY);		// Print Bar

	SelectObject(MemDC, AddBit);
	BitBlt(hdc, 115, 90, 51, 29, MemDC, 0, 0, SRCCOPY);		// Print Add Button

	SelectObject(MemDC, ModifyBit);
	BitBlt(hdc, 168, 90, 75, 29, MemDC, 0, 0, SRCCOPY);		// Print Modify Button

	SelectObject(MemDC, DeleteBit);
	BitBlt(hdc, 245, 90, 71, 29, MemDC, 0, 0, SRCCOPY);		// Print Delete Button

	SelectObject(MemDC, CopyBit);
	BitBlt(hdc, 318, 90, 59, 29, MemDC, 0, 0, SRCCOPY);		// Print Copy Button

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
	if (type != 0) {
		HDC hdc;
		HBRUSH RectBrush, OldBrush;
		HPEN RectPen, OldPen;
		hdc = GetDC(hWnd);

		RectBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		OldBrush = (HBRUSH)SelectObject(hdc, RectBrush);

		if (exist) {
			RectPen = CreatePen(PS_SOLID, 1, RGB(37, 177, 245));
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
			case 5: Rectangle(hdc, LISTCTRL_UP_LEFT, LISTCTRL_UP_TOP, LISTCTRL_UP_RIGHT, LISTCTRL_UP_BOTTOM); break;
			case 6: Rectangle(hdc, LISTCTRL_DOWN_LEFT, LISTCTRL_DOWN_TOP, LISTCTRL_DOWN_RIGHT, LISTCTRL_DOWN_BOTTOM); break;
			}
		} else if (FocusWnd == 1) {
			switch (type) {
			case 1: Rectangle(hdc, HB_LEFT, HB_TOP, HB_RIGHT, HB_BOTTOM); break;
			case 2: Rectangle(hdc, MB_LEFT, MB_TOP, MB_RIGHT, MB_BOTTOM); break;
			case 3: Rectangle(hdc, RB_LEFT, RB_TOP, RB_RIGHT, RB_BOTTOM); break;
			case 4: Rectangle(hdc, MEB_LEFT, MEB_TOP, MEB_RIGHT, MEB_BOTTOM); break;
			case 5: Rectangle(hdc, CT_LEFT, CT_TOP, CT_RIGHT, CT_BOTTOM); break;
			case 6: Rectangle(hdc, CL_LEFT, CL_TOP, CL_RIGHT, CL_BOTTOM); break;
			case 7: Rectangle(hdc, BR_LEFT, BR_TOP, BR_RIGHT, BR_BOTTOM); break;

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
		} else if (FocusWnd == 2) {
			switch (type) {
			case 1: Rectangle(hdc, HB_LEFT + 150, HB_TOP, HB_RIGHT + 150, HB_BOTTOM); break;
			case 2: Rectangle(hdc, MB_LEFT + 150, MB_TOP, MB_RIGHT + 150, MB_BOTTOM); break;
			case 3: Rectangle(hdc, RB_LEFT + 150, RB_TOP, RB_RIGHT + 150, RB_BOTTOM); break;
			case 4: Rectangle(hdc, MEB_LEFT + 150, MEB_TOP, MEB_RIGHT + 150, MEB_BOTTOM); break;
			case 5: Rectangle(hdc, CT_LEFT + 150, CT_TOP, CT_RIGHT + 150, CT_BOTTOM); break;
			case 6: Rectangle(hdc, CL_LEFT + 150, CL_TOP, CL_RIGHT + 150, CL_BOTTOM); break;
			case 7: Rectangle(hdc, BR_LEFT + 150, BR_TOP, BR_RIGHT + 150, BR_BOTTOM); break;

			case 220: Rectangle(hdc, X0 + 150, Y0, X1 + 150, Y1); break;
			case 221: Rectangle(hdc, X1 + 150, Y0, X2 + 150, Y1); break;
			case 222: Rectangle(hdc, X2 + 150, Y0, X3 + 150, Y1); break;
			case 223: Rectangle(hdc, X3 + 150, Y0, X4 + 150, Y1); break;
			case 224: Rectangle(hdc, X4 + 150, Y0, X5 + 150, Y1); break;
			case 225: Rectangle(hdc, X5 + 150, Y0, X6 + 150, Y1); break;
			case 226: Rectangle(hdc, X6 + 150, Y0, X7 + 150, Y1); break;
			case 227: Rectangle(hdc, X7 + 150, Y0, X8 + 150, Y1); break;
			case 228: Rectangle(hdc, X8 + 150, Y0, X9 + 150, Y1); break;
			case 229: Rectangle(hdc, X9 + 150, Y0, X10 + 150, Y1); break;
			case 230: Rectangle(hdc, X10 + 150, Y0, X11 + 150, Y1); break;
			case 231: Rectangle(hdc, X11 + 150, Y0, X12 + 150, Y1); break;

			case 232: Rectangle(hdc, X0 + 150, Y1, X1 + 150, Y2); break;
			case 233: Rectangle(hdc, X1 + 150, Y1, X2 + 150, Y2); break;
			case 234: Rectangle(hdc, X2 + 150, Y1, X3 + 150, Y2); break;
			case 235: Rectangle(hdc, X3 + 150, Y1, X4 + 150, Y2); break;
			case 236: Rectangle(hdc, X4 + 150, Y1, X5 + 150, Y2); break;
			case 237: Rectangle(hdc, X5 + 150, Y1, X6 + 150, Y2); break;
			case 238: Rectangle(hdc, X6 + 150, Y1, X7 + 150, Y2); break;
			case 239: Rectangle(hdc, X7 + 150, Y1, X8 + 150, Y2); break;
			case 240: Rectangle(hdc, X8 + 150, Y1, X9 + 150, Y2); break;
			case 241: Rectangle(hdc, X9 + 150, Y1, X10 + 150, Y2); break;
			case 242: Rectangle(hdc, X10 + 150, Y1, X11 + 150, Y2); break;
			case 243: Rectangle(hdc, X11 + 150, Y1, X12 + 150, Y2); break;

			case 250: Rectangle(hdc, X0 + 150, Y3, X1 + 150, Y4); break;
			case 251: Rectangle(hdc, X1 + 150, Y3, X2 + 150, Y4); break;
			case 252: Rectangle(hdc, X2 + 150, Y3, X3 + 150, Y4); break;
			case 253: Rectangle(hdc, X3 + 150, Y3, X4 + 150, Y4); break;
			case 254: Rectangle(hdc, X4 + 150, Y3, X5 + 150, Y4); break;
			case 255: Rectangle(hdc, X5 + 150, Y3, X6 + 150, Y4); break;
			case 256: Rectangle(hdc, X6 + 150, Y3, X7 + 150, Y4); break;
			case 257: Rectangle(hdc, X7 + 150, Y3, X8 + 150, Y4); break;
			case 258: Rectangle(hdc, X8 + 150, Y3, X9 + 150, Y4); break;
			case 259: Rectangle(hdc, X9 + 150, Y3, X10 + 150, Y4); break;
			case 260: Rectangle(hdc, X10 + 150, Y3, X11 + 150, Y4); break;
			case 261: Rectangle(hdc, X11 + 150, Y3, X12 + 150, Y4); break;

			case 262: Rectangle(hdc, X0 + 150, Y4, X1 + 150, Y5); break;
			case 263: Rectangle(hdc, X1 + 150, Y4, X2 + 150, Y5); break;
			case 264: Rectangle(hdc, X2 + 150, Y4, X3 + 150, Y5); break;
			case 265: Rectangle(hdc, X3 + 150, Y4, X4 + 150, Y5); break;
			case 266: Rectangle(hdc, X4 + 150, Y4, X5 + 150, Y5); break;
			case 267: Rectangle(hdc, X5 + 150, Y4, X6 + 150, Y5); break;
			case 268: Rectangle(hdc, X6 + 150, Y4, X7 + 150, Y5); break;
			case 269: Rectangle(hdc, X7 + 150, Y4, X8 + 150, Y5); break;
			case 270: Rectangle(hdc, X8 + 150, Y4, X9 + 150, Y5); break;
			case 271: Rectangle(hdc, X9 + 150, Y4, X10 + 150, Y5); break;
			case 272: Rectangle(hdc, X10 + 150, Y4, X11 + 150, Y5); break;
			case 273: Rectangle(hdc, X11 + 150, Y4, X12 + 150, Y5); break;

			case 274: Rectangle(hdc, X0 + 150, Y5, X1 + 150, Y6); break;
			case 275: Rectangle(hdc, X1 + 150, Y5, X2 + 150, Y6); break;
			case 276: Rectangle(hdc, X2 + 150, Y5, X3 + 150, Y6); break;
			case 277: Rectangle(hdc, X3 + 150, Y5, X4 + 150, Y6); break;
			case 278: Rectangle(hdc, X4 + 150, Y5, X5 + 150, Y6); break;
			case 279: Rectangle(hdc, X5 + 150, Y5, X6 + 150, Y6); break;
			case 280: Rectangle(hdc, X6 + 150, Y5, X7 + 150, Y6); break;
			case 281: Rectangle(hdc, X7 + 150, Y5, X8 + 150, Y6); break;
			case 282: Rectangle(hdc, X8 + 150, Y5, X9 + 150, Y6); break;
			case 283: Rectangle(hdc, X9 + 150, Y5, X10 + 150, Y6); break;
			case 284: Rectangle(hdc, X10 + 150, Y5, X11 + 150, Y6); break;
			case 285: Rectangle(hdc, X11 + 150, Y5, X12 + 150, Y6); break;

			case 286: Rectangle(hdc, X0 + 150, Y6, X1 + 150, Y7); break;
			case 287: Rectangle(hdc, X1 + 150, Y6, X2 + 150, Y7); break;
			case 288: Rectangle(hdc, X2 + 150, Y6, X3 + 150, Y7); break;
			case 289: Rectangle(hdc, X3 + 150, Y6, X4 + 150, Y7); break;
			case 290: Rectangle(hdc, X4 + 150, Y6, X5 + 150, Y7); break;
			case 291: Rectangle(hdc, X5 + 150, Y6, X6 + 150, Y7); break;
			case 292: Rectangle(hdc, X6 + 150, Y6, X7 + 150, Y7); break;
			case 293: Rectangle(hdc, X7 + 150, Y6, X8 + 150, Y7); break;
			case 294: Rectangle(hdc, X8 + 150, Y6, X9 + 150, Y7); break;
			case 295: Rectangle(hdc, X9 + 150, Y6, X10 + 150, Y7); break;
			case 296: Rectangle(hdc, X10 + 150, Y6, X11 + 150, Y7); break;
			case 297: Rectangle(hdc, X11 + 150, Y6, X12 + 150, Y7); break;

			case 298: Rectangle(hdc, X0 + 150, Y7, X1 + 150, Y8); break;
			case 299: Rectangle(hdc, X1 + 150, Y7, X2 + 150, Y8); break;
			case 300: Rectangle(hdc, X2 + 150, Y7, X3 + 150, Y8); break;
			case 301: Rectangle(hdc, X3 + 150, Y7, X4 + 150, Y8); break;
			case 302: Rectangle(hdc, X4 + 150, Y7, X5 + 150, Y8); break;
			case 303: Rectangle(hdc, X5 + 150, Y7, X6 + 150, Y8); break;
			case 304: Rectangle(hdc, X6 + 150, Y7, X7 + 150, Y8); break;
			case 305: Rectangle(hdc, X7 + 150, Y7, X8 + 150, Y8); break;
			case 306: Rectangle(hdc, X8 + 150, Y7, X9 + 150, Y8); break;
			case 307: Rectangle(hdc, X9 + 150, Y7, X10 + 150, Y8); break;
			case 308: Rectangle(hdc, X10 + 150, Y7, X11 + 150, Y8); break;
			case 309: Rectangle(hdc, X11 + 150, Y7, X12 + 150, Y8); break;

			case 320: Rectangle(hdc, R_X0 + 150, Y9, R_X1 + 150, Y10); break;
			case 321: Rectangle(hdc, R_X2 + 150, Y9, R_X3 + 150, Y10); break;
			case 322: Rectangle(hdc, R_X4 + 150, Y9, R_X5 + 150, Y10); break;
			case 323: Rectangle(hdc, R_X6 + 150, Y9, R_X7 + 150, Y10); break;
			case 324: Rectangle(hdc, R_X8 + 150, Y9, R_X9 + 150, Y10); break;
			case 325: Rectangle(hdc, R_X10 + 150, Y9, R_X11 + 150, Y10); break;
			case 326: Rectangle(hdc, R_X12 + 150, Y9, R_X13 + 150, Y10); break;

			}
		}

		SelectObject(hdc, OldBrush);
		SelectObject(hdc, OldPen);
		DeleteObject(RectBrush);
		ReleaseDC(hWnd, hdc);
	}
}

////
int CheckingMousePos(ALARM *TargetNode, TIME *tSelectedTime, int x, int y, int FocusWnd, bool click) {
	if (FocusWnd == 0) {
		if (x > ADD_LEFT && x < ADD_RIGHT && y > ADD_TOP && y < ADD_BOTTOM) return 1;
		else if (x > MODIFY_LEFT && x < MODIFY_RIGHT && y > MODIFY_TOP && y < MODIFY_BOTTOM) return 2;
		else if (x > DELETE_LEFT && x < DELETE_RIGHT && y > DELETE_TOP && y < DELETE_BOTTOM) return 3;
		else if (x > COPY_LEFT && x < COPY_RIGHT && y > COPY_TOP && y < COPY_BOTTOM) return 4;
		else if (x > LISTCTRL_UP_LEFT && x < LISTCTRL_UP_RIGHT && y > LISTCTRL_UP_TOP && y < LISTCTRL_UP_BOTTOM) return 5;
		else if (x > LISTCTRL_DOWN_LEFT && x < LISTCTRL_DOWN_RIGHT && y > LISTCTRL_DOWN_TOP && y < LISTCTRL_DOWN_BOTTOM) return 6;
		else if (x > ALARMTABLE_LEFT && x < ALARMTABLE_RIGHT && y > ALARMTABLE_TOP && y < ALARMTABLE_BOTTOM) return 7;
		else return 0;
	} else if (FocusWnd == 1) {
		if (x > HB_LEFT && x < HB_RIGHT && y > HB_TOP && y < HB_BOTTOM) {
			if (click && x > X0 && x < X1 && y > Y0 && y < Y1) { TargetNode->time.Hou = 1; return 20; }
			else if (click && x > X1 && x < X2 && y > Y0 && y < Y1) { TargetNode->time.Hou = 2;  return 21; }
			else if (click && x > X2 && x < X3 && y > Y0 && y < Y1) { TargetNode->time.Hou = 3;  return 22;	}
			else if (click && x > X3 && x < X4 && y > Y0 && y < Y1) { TargetNode->time.Hou = 4;  return 23; }
			else if (click && x > X4 && x < X5 && y > Y0 && y < Y1) { TargetNode->time.Hou = 5;  return 24; }
			else if (click && x > X5 && x < X6 && y > Y0 && y < Y1) { TargetNode->time.Hou = 6;  return 25; }
			else if (click && x > X6 && x < X7 && y > Y0 && y < Y1) { TargetNode->time.Hou = 7;  return 26; }
			else if (click && x > X7 && x < X8 && y > Y0 && y < Y1) { TargetNode->time.Hou = 8;  return 27; }
			else if (click && x > X8 && x < X9 && y > Y0 && y < Y1) { TargetNode->time.Hou = 9;  return 28; }
			else if (click && x > X9 && x < X10 && y > Y0 && y < Y1) { TargetNode->time.Hou = 10;  return 29; }
			else if (click && x > X10 && x < X11 && y > Y0 && y < Y1) { TargetNode->time.Hou = 11;  return 30; }
			else if (click && x > X11 && x < X12 && y > Y0 && y < Y1) { TargetNode->time.Hou = 12;  return 31; }

			else if (click && x > X0 && x < X1 && y > Y1 && y < Y2) { TargetNode->time.Hou = 13;  return 32; }
			else if (click && x > X1 && x < X2 && y > Y1 && y < Y2) { TargetNode->time.Hou = 14;  return 33; }
			else if (click && x > X2 && x < X3 && y > Y1 && y < Y2) { TargetNode->time.Hou = 15;  return 34; }
			else if (click && x > X3 && x < X4 && y > Y1 && y < Y2) { TargetNode->time.Hou = 16;  return 35; }
			else if (click && x > X4 && x < X5 && y > Y1 && y < Y2) { TargetNode->time.Hou = 17;  return 36; }
			else if (click && x > X5 && x < X6 && y > Y1 && y < Y2) { TargetNode->time.Hou = 18;  return 37; }
			else if (click && x > X6 && x < X7 && y > Y1 && y < Y2) { TargetNode->time.Hou = 19;  return 38; }
			else if (click && x > X7 && x < X8 && y > Y1 && y < Y2) { TargetNode->time.Hou = 20;  return 39; }
			else if (click && x > X8 && x < X9 && y > Y1 && y < Y2) { TargetNode->time.Hou = 21;  return 40; }
			else if (click && x > X9 && x < X10 && y > Y1 && y < Y2) { TargetNode->time.Hou = 22;  return 41; }
			else if (click && x > X10 && x < X11 && y > Y1 && y < Y2) { TargetNode->time.Hou = 23;  return 42; }
			else if (click && x > X11 && x < X12 && y > Y1 && y < Y2) { TargetNode->time.Hou = 00;  return 43; }
			else return 1; }
		else if (x > MB_LEFT && x < MB_RIGHT && y > MB_TOP && y < MB_BOTTOM) {
			if (click && x > X0 && x < X1 && y > Y3 && y < Y4) { TargetNode->time.Min = 0; return 50; }
			else if (click && x > X1 && x < X2 && y > Y3 && y < Y4) { TargetNode->time.Min = 1; return 51; }
			else if (click && x > X2 && x < X3 && y > Y3 && y < Y4) { TargetNode->time.Min = 2; return 52; }
			else if (click && x > X3 && x < X4 && y > Y3 && y < Y4) { TargetNode->time.Min = 3; return 53; }
			else if (click && x > X4 && x < X5 && y > Y3 && y < Y4) { TargetNode->time.Min = 4; return 54; }
			else if (click && x > X5 && x < X6 && y > Y3 && y < Y4) { TargetNode->time.Min = 5; return 55; }
			else if (click && x > X6 && x < X7 && y > Y3 && y < Y4) { TargetNode->time.Min = 6; return 56; }
			else if (click && x > X7 && x < X8 && y > Y3 && y < Y4) { TargetNode->time.Min = 7; return 57; }
			else if (click && x > X8 && x < X9 && y > Y3 && y < Y4) { TargetNode->time.Min = 8; return 58; }
			else if (click && x > X9 && x < X10 && y > Y3 && y < Y4) { TargetNode->time.Min = 9; return 59; }
			else if (click && x > X10 && x < X11 && y > Y3 && y < Y4) { TargetNode->time.Min = 10; return 60; }
			else if (click && x > X11 && x < X12 && y > Y3 && y < Y4) { TargetNode->time.Min = 11; return 61; }

			else if (click && x > X0 && x < X1 && y > Y4 && y < Y5) { TargetNode->time.Min = 12; return 62; }
			else if (click && x > X1 && x < X2 && y > Y4 && y < Y5) { TargetNode->time.Min = 13; return 63; }
			else if (click && x > X2 && x < X3 && y > Y4 && y < Y5) { TargetNode->time.Min = 14; return 64; }
			else if (click && x > X3 && x < X4 && y > Y4 && y < Y5) { TargetNode->time.Min = 15; return 65; }
			else if (click && x > X4 && x < X5 && y > Y4 && y < Y5) { TargetNode->time.Min = 16; return 66; }
			else if (click && x > X5 && x < X6 && y > Y4 && y < Y5) { TargetNode->time.Min = 17; return 67; }
			else if (click && x > X6 && x < X7 && y > Y4 && y < Y5) { TargetNode->time.Min = 18; return 68; }
			else if (click && x > X7 && x < X8 && y > Y4 && y < Y5) { TargetNode->time.Min = 19; return 69; }
			else if (click && x > X8 && x < X9 && y > Y4 && y < Y5) { TargetNode->time.Min = 20; return 70; }
			else if (click && x > X9 && x < X10 && y > Y4 && y < Y5) { TargetNode->time.Min = 21; return 71; }
			else if (click && x > X10 && x < X11 && y > Y4 && y < Y5) { TargetNode->time.Min = 22; return 72; }
			else if (click && x > X11 && x < X12 && y > Y4 && y < Y5) { TargetNode->time.Min = 23; return 73; }

			else if (click && x > X0 && x < X1 && y > Y5 && y < Y6) { TargetNode->time.Min = 24; return 74; }
			else if (click && x > X1 && x < X2 && y > Y5 && y < Y6) { TargetNode->time.Min = 25; return 75; }
			else if (click && x > X2 && x < X3 && y > Y5 && y < Y6) { TargetNode->time.Min = 26; return 76; }
			else if (click && x > X3 && x < X4 && y > Y5 && y < Y6) { TargetNode->time.Min = 27; return 77; }
			else if (click && x > X4 && x < X5 && y > Y5 && y < Y6) { TargetNode->time.Min = 28; return 78; }
			else if (click && x > X5 && x < X6 && y > Y5 && y < Y6) { TargetNode->time.Min = 29; return 79; }
			else if (click && x > X6 && x < X7 && y > Y5 && y < Y6) { TargetNode->time.Min = 30; return 80; }
			else if (click && x > X7 && x < X8 && y > Y5 && y < Y6) { TargetNode->time.Min = 31; return 81; }
			else if (click && x > X8 && x < X9 && y > Y5 && y < Y6) { TargetNode->time.Min = 32; return 82; }
			else if (click && x > X9 && x < X10 && y > Y5 && y < Y6) { TargetNode->time.Min = 33; return 83; }
			else if (click && x > X10 && x < X11 && y > Y5 && y < Y6) { TargetNode->time.Min = 34; return 84; }
			else if (click && x > X11 && x < X12 && y > Y5 && y < Y6) { TargetNode->time.Min = 35; return 85; }

			else if (click && x > X0 && x < X1 && y > Y6 && y < Y7) { TargetNode->time.Min = 36; return 86; }
			else if (click && x > X1 && x < X2 && y > Y6 && y < Y7) { TargetNode->time.Min = 37; return 87; }
			else if (click && x > X2 && x < X3 && y > Y6 && y < Y7) { TargetNode->time.Min = 38; return 88; }
			else if (click && x > X3 && x < X4 && y > Y6 && y < Y7) { TargetNode->time.Min = 39; return 89; }
			else if (click && x > X4 && x < X5 && y > Y6 && y < Y7) { TargetNode->time.Min = 40; return 90; }
			else if (click && x > X5 && x < X6 && y > Y6 && y < Y7) { TargetNode->time.Min = 41; return 91; }
			else if (click && x > X6 && x < X7 && y > Y6 && y < Y7) { TargetNode->time.Min = 42; return 92; }
			else if (click && x > X7 && x < X8 && y > Y6 && y < Y7) { TargetNode->time.Min = 43; return 93; }
			else if (click && x > X8 && x < X9 && y > Y6 && y < Y7) { TargetNode->time.Min = 44; return 94; }
			else if (click && x > X9 && x < X10 && y > Y6 && y < Y7) { TargetNode->time.Min = 45; return 95; }
			else if (click && x > X10 && x < X11 && y > Y6 && y < Y7) { TargetNode->time.Min = 46; return 96; }
			else if (click && x > X11 && x < X12 && y > Y6 && y < Y7) { TargetNode->time.Min = 47; return 97; }

			else if (click && x > X0 && x < X1 && y > Y7 && y < Y8) { TargetNode->time.Min = 48; return 98; }
			else if (click && x > X1 && x < X2 && y > Y7 && y < Y8) { TargetNode->time.Min = 49; return 99; }
			else if (click && x > X2 && x < X3 && y > Y7 && y < Y8) { TargetNode->time.Min = 50; return 100; }
			else if (click && x > X3 && x < X4 && y > Y7 && y < Y8) { TargetNode->time.Min = 51; return 101; }
			else if (click && x > X4 && x < X5 && y > Y7 && y < Y8) { TargetNode->time.Min = 52; return 102; }
			else if (click && x > X5 && x < X6 && y > Y7 && y < Y8) { TargetNode->time.Min = 53; return 103; }
			else if (click && x > X6 && x < X7 && y > Y7 && y < Y8) { TargetNode->time.Min = 54; return 104; }
			else if (click && x > X7 && x < X8 && y > Y7 && y < Y8) { TargetNode->time.Min = 55; return 105; }
			else if (click && x > X8 && x < X9 && y > Y7 && y < Y8) { TargetNode->time.Min = 56; return 106; }
			else if (click && x > X9 && x < X10 && y > Y7 && y < Y8) { TargetNode->time.Min = 57; return 107; }
			else if (click && x > X10 && x < X11 && y > Y7 && y < Y8) { TargetNode->time.Min = 58; return 108; }
			else if (click && x > X11 && x < X12 && y > Y7 && y < Y8) { TargetNode->time.Min = 59; return 109; }
			else return 2;
		}
		else if (x > RB_LEFT && x < RB_RIGHT && y > RB_TOP && y < RB_BOTTOM) {
			if (click && x > R_X0 && x < R_X1 && y > Y9 && y < Y10) return 120;
			else if (click && x > R_X2 && x < R_X3 && y > Y9 && y < Y10) return 121;
			else if (click && x > R_X4 && x < R_X5 && y > Y9 && y < Y10) return 122;
			else if (click && x > R_X6 && x < R_X7 && y > Y9 && y < Y10) return 123;
			else if (click && x > R_X8 && x < R_X9 && y > Y9 && y < Y10) return 124;
			else if (click && x > R_X10 && x < R_X11 && y > Y9 && y < Y10) return 125;
			else if (click && x > R_X12 && x < R_X13 && y > Y9 && y < Y10) return 126;
			else return 3; }
		else if (x > MEB_LEFT && x < MEB_RIGHT && y > MEB_TOP && y < MEB_BOTTOM) return 4;
		else if (x > BR_LEFT && x < BR_RIGHT && y > BR_TOP && y < BR_BOTTOM) return 7;
		else if (x > CT_LEFT && x < CT_RIGHT && y > CT_TOP && y < CT_BOTTOM) {
			if (click && x > CT_LEFT && x < CT_RIGHT && y > CT_TOP && y < CT_BOTTOM) return 150;
			else return 5; }
		else if (x > CL_LEFT && x < CL_RIGHT && y > CL_TOP && y < CL_BOTTOM) {
			if (click && x > CL_LEFT && x < CL_RIGHT && y > CL_TOP && y < CL_BOTTOM) return 151;
			else return 6; }
		else return 0;
	} else if (FocusWnd == 2) {
		if (x > HB_LEFT + 150 && x < HB_RIGHT + 150 && y > HB_TOP && y < HB_BOTTOM) {
			if (click && x > X0 + 150 && x < X1 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 1; return 220; }
			else if (click && x > X1 + 150 && x < X2 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 2;  return 221; }
			else if (click && x > X2 + 150 && x < X3 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 3;  return 222; }
			else if (click && x > X3 + 150 && x < X4 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 4;  return 223; }
			else if (click && x > X4 + 150 && x < X5 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 5;  return 224; }
			else if (click && x > X5 + 150 && x < X6 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 6;  return 225; }
			else if (click && x > X6 + 150 && x < X7 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 7;  return 226; }
			else if (click && x > X7 + 150 && x < X8 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 8;  return 227; }
			else if (click && x > X8 + 150 && x < X9 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 9;  return 228; }
			else if (click && x > X9 + 150 && x < X10 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 10;  return 229; }
			else if (click && x > X10 + 150 && x < X11 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 11;  return 230; }
			else if (click && x > X11 + 150 && x < X12 + 150 && y > Y0 && y < Y1) { tSelectedTime->Hou = 12;  return 231; }

			else if (click && x > X0 + 150 && x < X1 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 13;  return 232; }
			else if (click && x > X1 + 150 && x < X2 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 14;  return 233; }
			else if (click && x > X2 + 150 && x < X3 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 15;  return 234; }
			else if (click && x > X3 + 150 && x < X4 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 16;  return 235; }
			else if (click && x > X4 + 150 && x < X5 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 17;  return 236; }
			else if (click && x > X5 + 150 && x < X6 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 18;  return 237; }
			else if (click && x > X6 + 150 && x < X7 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 19;  return 238; }
			else if (click && x > X7 + 150 && x < X8 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 20;  return 239; }
			else if (click && x > X8 + 150 && x < X9 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 21;  return 240; }
			else if (click && x > X9 + 150 && x < X10 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 22;  return 241; }
			else if (click && x > X10 + 150 && x < X11 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 23;  return 242; }
			else if (click && x > X11 + 150 && x < X12 + 150 && y > Y1 && y < Y2) { tSelectedTime->Hou = 00;  return 243; }
			else return 1;
		}
		else if (x > MB_LEFT + 150 && x < MB_RIGHT + 150 && y > MB_TOP && y < MB_BOTTOM) {
			if (click && x > X0 + 150 && x < X1 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 0; return 250; }
			else if (click && x > X1 + 150 && x < X2 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 1; return 251; }
			else if (click && x > X2 + 150 && x < X3 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 2; return 252; }
			else if (click && x > X3 + 150 && x < X4 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 3; return 253; }
			else if (click && x > X4 + 150 && x < X5 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 4; return 254; }
			else if (click && x > X5 + 150 && x < X6 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 5; return 255; }
			else if (click && x > X6 + 150 && x < X7 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 6; return 256; }
			else if (click && x > X7 + 150 && x < X8 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 7; return 257; }
			else if (click && x > X8 + 150 && x < X9 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 8; return 258; }
			else if (click && x > X9 + 150 && x < X10 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 9; return 259; }
			else if (click && x > X10 + 150 && x < X11 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 10; return 260; }
			else if (click && x > X11 + 150 && x < X12 + 150 && y > Y3 && y < Y4) { tSelectedTime->Min = 11; return 261; }

			else if (click && x > X0 + 150 && x < X1 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 12; return 262; }
			else if (click && x > X1 + 150 && x < X2 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 13; return 263; }
			else if (click && x > X2 + 150 && x < X3 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 14; return 264; }
			else if (click && x > X3 + 150 && x < X4 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 15; return 265; }
			else if (click && x > X4 + 150 && x < X5 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 16; return 266; }
			else if (click && x > X5 + 150 && x < X6 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 17; return 267; }
			else if (click && x > X6 + 150 && x < X7 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 18; return 268; }
			else if (click && x > X7 + 150 && x < X8 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 19; return 269; }
			else if (click && x > X8 + 150 && x < X9 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 20; return 270; }
			else if (click && x > X9 + 150 && x < X10 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 21; return 271; }
			else if (click && x > X10 + 150 && x < X11 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 22; return 272; }
			else if (click && x > X11 + 150 && x < X12 + 150 && y > Y4 && y < Y5) { tSelectedTime->Min = 23; return 273; }

			else if (click && x > X0 + 150 && x < X1 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 24; return 274; }
			else if (click && x > X1 + 150 && x < X2 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 25; return 275; }
			else if (click && x > X2 + 150 && x < X3 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 26; return 276; }
			else if (click && x > X3 + 150 && x < X4 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 27; return 277; }
			else if (click && x > X4 + 150 && x < X5 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 28; return 278; }
			else if (click && x > X5 + 150 && x < X6 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 29; return 279; }
			else if (click && x > X6 + 150 && x < X7 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 30; return 280; }
			else if (click && x > X7 + 150 && x < X8 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 31; return 281; }
			else if (click && x > X8 + 150 && x < X9 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 32; return 282; }
			else if (click && x > X9 + 150 && x < X10 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 33; return 283; }
			else if (click && x > X10 + 150 && x < X11 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 34; return 284; }
			else if (click && x > X11 + 150 && x < X12 + 150 && y > Y5 && y < Y6) { tSelectedTime->Min = 35; return 285; }

			else if (click && x > X0 + 150 && x < X1 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 36; return 286; }
			else if (click && x > X1 + 150 && x < X2 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 37; return 287; }
			else if (click && x > X2 + 150 && x < X3 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 38; return 288; }
			else if (click && x > X3 + 150 && x < X4 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 39; return 289; }
			else if (click && x > X4 + 150 && x < X5 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 40; return 290; }
			else if (click && x > X5 + 150 && x < X6 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 41; return 291; }
			else if (click && x > X6 + 150 && x < X7 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 42; return 292; }
			else if (click && x > X7 + 150 && x < X8 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 43; return 293; }
			else if (click && x > X8 + 150 && x < X9 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 44; return 294; }
			else if (click && x > X9 + 150 && x < X10 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 45; return 295; }
			else if (click && x > X10 + 150 && x < X11 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 46; return 296; }
			else if (click && x > X11 + 150 && x < X12 + 150 && y > Y6 && y < Y7) { tSelectedTime->Min = 47; return 297; }

			else if (click && x > X0 + 150 && x < X1 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 48; return 298; }
			else if (click && x > X1 + 150 && x < X2 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 49; return 299; }
			else if (click && x > X2 + 150 && x < X3 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 50; return 300; }
			else if (click && x > X3 + 150 && x < X4 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 51; return 301; }
			else if (click && x > X4 + 150 && x < X5 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 52; return 302; }
			else if (click && x > X5 + 150 && x < X6 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 53; return 303; }
			else if (click && x > X6 + 150 && x < X7 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 54; return 304; }
			else if (click && x > X7 + 150 && x < X8 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 55; return 305; }
			else if (click && x > X8 + 150 && x < X9 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 56; return 306; }
			else if (click && x > X9 + 150 && x < X10 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 57; return 307; }
			else if (click && x > X10 + 150 && x < X11 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 58; return 308; }
			else if (click && x > X11 + 150 && x < X12 + 150 && y > Y7 && y < Y8) { tSelectedTime->Min = 59; return 309; }
			else return 2;
		} else if (x > RB_LEFT + 150 && x < RB_RIGHT + 150 && y > RB_TOP && y < RB_BOTTOM) {
			if (click && x > R_X0 + 150 && x < R_X1 + 150 && y > Y9 && y < Y10) return 320;
			else if (click && x > R_X2 + 150 && x < R_X3 + 150 && y > Y9 && y < Y10) return 321;
			else if (click && x > R_X4 + 150 && x < R_X5 + 150 && y > Y9 && y < Y10) return 322;
			else if (click && x > R_X6 + 150 && x < R_X7 + 150 && y > Y9 && y < Y10) return 323;
			else if (click && x > R_X8 + 150 && x < R_X9 + 150 && y > Y9 && y < Y10) return 324;
			else if (click && x > R_X10 + 150 && x < R_X11 + 150 && y > Y9 && y < Y10) return 325;
			else if (click && x > R_X12 + 150 && x < R_X13 + 150 && y > Y9 && y < Y10) return 326;
			else return 3;
		} else if (x > MEB_LEFT + 150 && x < MEB_RIGHT + 150 && y > MEB_TOP && y < MEB_BOTTOM) return 4;
		else if (x > BR_LEFT + 150 && x < BR_RIGHT + 150 && y > BR_TOP && y < BR_BOTTOM) return 7;
		  else if (x > CT_LEFT + 150 && x < CT_RIGHT + 150 && y > CT_TOP && y < CT_BOTTOM) {
			if (click && x > CT_LEFT + 150 && x < CT_RIGHT + 150 && y > CT_TOP && y < CT_BOTTOM) return 350;
			else return 5;
		}
		else if (x > CL_LEFT + 150 && x < CL_RIGHT + 150 && y > CL_TOP && y < CL_BOTTOM) {
			if (click && x > CL_LEFT + 150 && x < CL_RIGHT + 150 && y > CL_TOP && y < CL_BOTTOM) return 351;
			else return 6;
		}
		else return 0;
	}

	return 0;
}

////
void UpdateSelectedButton(HINSTANCE Instance, HWND hWnd, int type, int FocusWnd) {
	static int onButtonType = 0;

	if (FocusWnd == 0) {
		switch (type) {
		case 1: if (bPSB) { PrintSelectedButton(Instance, hWnd, 1, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 1; } return;
		case 2: if (bPSB) { PrintSelectedButton(Instance, hWnd, 2, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 2; } return;
		case 3: if (bPSB) { PrintSelectedButton(Instance, hWnd, 3, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 3; } return;
		case 4: if (bPSB) { PrintSelectedButton(Instance, hWnd, 4, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 4; } return;
		case 5: if (bPSB) { PrintSelectedButton(Instance, hWnd, 5, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 5; } return;
		case 6: if (bPSB) { PrintSelectedButton(Instance, hWnd, 6, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 6; } return;
		case 0: if (bPNSB) { PrintSelectedButton(Instance, hWnd, onButtonType, FocusWnd, false); bPSB = true; bPNSB = false; } return;
		}
	} else if (FocusWnd == 1) {
		switch (type) {
		case 1: if (bPSB) { PrintSelectedButton(Instance, hWnd, 1, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 1; } return;
		case 2: if (bPSB) { PrintSelectedButton(Instance, hWnd, 2, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 2; } return;
		case 3: if (bPSB) { PrintSelectedButton(Instance, hWnd, 3, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 3; } return;
		case 4: if (bPSB) { PrintSelectedButton(Instance, hWnd, 4, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 4; } return;
		case 5: if (bPSB) { PrintSelectedButton(Instance, hWnd, 5, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 5; } return;
		case 6: if (bPSB) { PrintSelectedButton(Instance, hWnd, 6, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 6; } return;
		case 7: if (bPSB) { PrintSelectedButton(Instance, hWnd, 7, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 7; } return;
		case 0: if (bPNSB) { PrintSelectedButton(Instance, hWnd, onButtonType, FocusWnd, false); bPSB = true; bPNSB = false; } return;
		}
	} else if (FocusWnd == 2) {
		switch (type) {
		case 1: if (bPSB) { PrintSelectedButton(Instance, hWnd, 1, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 1; } return;
		case 2: if (bPSB) { PrintSelectedButton(Instance, hWnd, 2, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 2; } return;
		case 3: if (bPSB) { PrintSelectedButton(Instance, hWnd, 3, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 3; } return;
		case 4: if (bPSB) { PrintSelectedButton(Instance, hWnd, 4, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 4; } return;
		case 5: if (bPSB) { PrintSelectedButton(Instance, hWnd, 5, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 5; } return;
		case 6: if (bPSB) { PrintSelectedButton(Instance, hWnd, 6, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 6; } return;
		case 7: if (bPSB) { PrintSelectedButton(Instance, hWnd, 7, FocusWnd, true); bPSB = false; bPNSB = true; onButtonType = 7; } return;
		case 0: if (bPNSB) { PrintSelectedButton(Instance, hWnd, onButtonType, FocusWnd, false); bPSB = true; bPNSB = false; } return;
		}
	}
}

void AppearAddMenu(HINSTANCE Instance, HWND hWnd, HDC hdc, TIME tSelectedTime, LPWSTR MemoData, ALARM **NewNode, bool *AddMenuFirstMotion, int *FocusWnd) {
	HDC MemDC;
	HPEN BorderPen, OldPen;
	HFONT MemoFont, SoundPathFont, OldFont;
	HBITMAP TitleBit, HourBit, MinuteBit, RepeatWeekBit, MemoBit, CreateBit, CancelBit, SoundPathBit, OldBit;

	if (*NewNode == NULL) *NewNode = (ALARM*)calloc(1, sizeof(ALARM));

	BorderPen = CreatePen(PS_SOLID, 1, RGB(37, 177, 245));
	OldPen = (HPEN)SelectObject(hdc, BorderPen);
	if (*AddMenuFirstMotion) {
		for (int StayX = 1; StayX <= 35; StayX++) {
			for (int Frame = StayX; Frame > 0; Frame--)
				Rectangle(hdc, 0, 50, StayX * 10, 450);
			Sleep(15);
		}
		*FocusWnd = 1;
		*AddMenuFirstMotion = false;
	} else Rectangle(hdc, 0, 50, 350, 450);
	SelectObject(hdc, OldPen);
	DeleteObject(BorderPen);

	MemDC = CreateCompatibleDC(hdc);
	TitleBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP11));
	HourBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP7));
	MinuteBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP8));
	RepeatWeekBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP9));
	MemoBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP10));
	SoundPathBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP18));
	CreateBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP12));
	CancelBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP13));

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

	SelectObject(MemDC, SoundPathBit);
	BitBlt(hdc, 10, 370, 350, 45, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, CreateBit);
	BitBlt(hdc, 165, 410, 77, 31, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, CancelBit);
	BitBlt(hdc, 250, 410, 77, 31, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBit);
	DeleteObject(TitleBit);
	DeleteObject(HourBit);
	DeleteObject(MinuteBit);
	DeleteObject(RepeatWeekBit);
	DeleteObject(MemoBit);
	DeleteObject(SoundPathBit);
	DeleteObject(CreateBit);
	DeleteObject(CancelBit);
	DeleteDC(MemDC);

	PrintSelectedButton(Instance, hWnd, tSelectedTime.Hou, *FocusWnd, true);
	PrintSelectedButton(Instance, hWnd, tSelectedTime.Min, *FocusWnd, true);
	for (int i = 0; i < 7; i++) {
		if (tSelectedTime.RepeatWeek[i] == 1) PrintSelectedButton(Instance, hWnd, i + 120, *FocusWnd, true);
	}

	MemoFont = CreateFont(13, 0, 1, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²Ã¼");
	SoundPathFont = CreateFont(13, 0, 1, 0, 600, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²Ã¼");
	OldFont = (HFONT)SelectObject(hdc, MemoFont);
	SetTextColor(hdc, RGB(37, 177, 245));
	TextOut(hdc, MEB_LEFT + 3, MEB_TOP, MemoData, wcslen(MemoData));
	SelectObject(hdc, SoundPathFont);
	TextOut(hdc, BR_RIGHT + 40, BR_TOP + 10, (*NewNode)->szSoundFileName, wcslen((*NewNode)->szSoundFileName));
	SelectObject(hdc, OldFont);
	DeleteObject(MemoFont);
}

void AppearModifyMenu(HINSTANCE Instance, HWND hWnd, HDC hdc, TIME *tSelectedTime, LPWSTR MemoData, ALARM *SelectedNode, bool *ModifyMenuFistMotion, int *FocusWnd) {
	HDC MemDC;
	HPEN BorderPen, OldPen;
	HFONT MemoFont, SoundPathFont, OldFont;
	HBITMAP TitleBit, HourBit, MinuteBit, RepeatWeekBit, MemoBit, ModifyBit, CancelBit, SoundPathBit, OldBit;

	BorderPen = CreatePen(PS_SOLID, 1, RGB(37, 177, 245));
	OldPen = (HPEN)SelectObject(hdc, BorderPen);
	if (*ModifyMenuFistMotion) {
		for (int StayX = 1; StayX <= 35; StayX++) {
			for (int Frame = StayX; Frame > 0; Frame--)
				Rectangle(hdc, 500 - (StayX * 10), 50, 500, 450);
			Sleep(15);
		}
		*FocusWnd = 2;
		*ModifyMenuFistMotion = false;
	}
	else Rectangle(hdc, 150, 50, 500, 450);
	SelectObject(hdc, OldPen);
	DeleteObject(BorderPen);

	MemDC = CreateCompatibleDC(hdc);
	TitleBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP16));
	HourBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP7));
	MinuteBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP8));
	RepeatWeekBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP9));
	MemoBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP10));
	SoundPathBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP18));
	ModifyBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP17));
	CancelBit = LoadBitmap(Instance, MAKEINTRESOURCE(IDB_BITMAP13));

	OldBit = (HBITMAP)SelectObject(MemDC, TitleBit);
	BitBlt(hdc, 160, 55, 350, 55, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, HourBit);
	BitBlt(hdc, 160, 105, 350, 55, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, MinuteBit);
	BitBlt(hdc, 160, 170, 350, 110, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, RepeatWeekBit);
	BitBlt(hdc, 160, 280, 350, 45, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, MemoBit);
	BitBlt(hdc, 160, 325, 350, 45, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, SoundPathBit);
	BitBlt(hdc, 160, 370, 350, 45, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, ModifyBit);
	BitBlt(hdc, 312, 411, 77, 32, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, CancelBit);
	BitBlt(hdc, 400, 410, 77, 31, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBit);
	DeleteObject(TitleBit);
	DeleteObject(HourBit);
	DeleteObject(MinuteBit);
	DeleteObject(RepeatWeekBit);
	DeleteObject(MemoBit);
	DeleteObject(SoundPathBit);
	DeleteObject(ModifyBit);
	DeleteObject(CancelBit);
	DeleteDC(MemDC);

	PrintSelectedButton(Instance, hWnd, tSelectedTime->Hou, *FocusWnd, true);
	PrintSelectedButton(Instance, hWnd, tSelectedTime->Min, *FocusWnd, true);
	for (int i = 0; i < 7; i++) {
		if (tSelectedTime->RepeatWeek[i] == 1) PrintSelectedButton(Instance, hWnd, i + 320, *FocusWnd, true);
	}

	MemoFont = CreateFont(13, 0, 1, 0, 0, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²Ã¼");
	SoundPathFont = CreateFont(13, 0, 1, 0, 600, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0, 0, L"±¼¸²Ã¼");
	OldFont = (HFONT)SelectObject(hdc, MemoFont);
	SetTextColor(hdc, RGB(37, 177, 245));
	TextOut(hdc, MEB_LEFT + 3 + 150, MEB_TOP, MemoData, wcslen(MemoData));
	SelectObject(hdc, SoundPathFont);
	TextOut(hdc, BR_RIGHT + 40 + 150, BR_TOP + 10, SelectedNode->szSoundFileName, wcslen(SelectedNode->szSoundFileName));
	SelectObject(hdc, OldFont);
	DeleteObject(MemoFont);
}