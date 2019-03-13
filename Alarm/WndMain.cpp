#include "AlarmLib.h"

HINSTANCE gInstance;
const char *lpszClassN = "Park Alarm";
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnClickListener(HINSTANCE Instance, HWND hWnd, int type, int *FocusWnd);
int FocusWnd = 0;
bool bAddMenu = false;
bool bDeleteMenu = false;

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

	hWnd = CreateWindow(lpszClassN, "", WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Msg, 0, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	int x, y;
	PAINTSTRUCT ps;

	switch (iMessage) {
	case WM_CREATE:
		MoveWindow(hWnd, 0, 0, 300, 150, TRUE);
		return 0;

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 700;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 700;
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		PrintMainDisplay(gInstance, hWnd, hdc, &ps);
		if (bAddMenu) AppearAddMenu(gInstance, hWnd, hdc, &FocusWnd);
		if (bDeleteMenu);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_MOUSEMOVE:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		UpdateSelectedButton(gInstance, hWnd, CheckingMousePos(x, y, FocusWnd), FocusWnd);
		return 0;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		OnClickListener(gInstance, hWnd, CheckingMousePos(x, y, FocusWnd), &FocusWnd);
		SendMessage(hWnd, WM_MOUSEMOVE, 0, 0);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

void OnClickListener(HINSTANCE Instance, HWND hWnd, int type, int *FocusWnd) {
	
	if (*FocusWnd == 0) {
		switch (type) {
		case 1:
			bAddMenu = true;
			InvalidateRect(hWnd, NULL, false);
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
	} else if (*FocusWnd == 1) {
		switch (type) {
		case 1:

			break;
		}
	}
}