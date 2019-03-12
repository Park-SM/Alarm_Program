#include "AlarmLib.h"

HINSTANCE gInstance;
const char *lpszClassN = "Park Alarm";
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
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
		EndPaint(hWnd, &ps);
		return 0;

	case WM_MOUSEMOVE:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		CheckingMousePosAndPrintSelectedButton(gInstance, hWnd, x, y);
		return 0;

	case WM_LBUTTONDOWN:

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}

