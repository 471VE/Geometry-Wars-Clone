//
//  DO NOT MODIFY THIS FILE
//

#define WIN32_LEAN_AND_MEAN
#include "Engine.h"
#include <stdlib.h>
#include <windows.h>
#include <fstream>

uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] = { 0 };
std::vector<MessageToRender> messages_to_render;

static HINSTANCE hinst = 0;
static DWORD ticks = 0;
static bool is_active = true;
static POINT cursor_pos;
static bool quited = false;
static LARGE_INTEGER qpc_frequency = { 0 };
static LARGE_INTEGER qpc_ref_time = { 0 };
float dt = 0.f;
FPS fps;

bool is_window_active() { return is_active; }

void clear_buffer() { memset(buffer, 0, sizeof(buffer)); }

bool is_key_pressed(int button_vk_code)
	{ return is_active && (GetAsyncKeyState(button_vk_code) & 0x8000); }

bool is_mouse_button_pressed(int mouse_button_index) {
	if (!is_active)
		return false;

	if (mouse_button_index == 0)
		return GetAsyncKeyState(VK_LBUTTON) != 0;

	if (mouse_button_index == 1)
		return GetAsyncKeyState(VK_RBUTTON) != 0;

	return false;
}

int get_cursor_x() { return cursor_pos.x; }
int get_cursor_y() { return cursor_pos.y; }

void schedule_quit_game() { quited = true; }

void log_error_and_exit(std::string message) {
    std::ofstream log("error_log.txt");
    log << message << std::endl;
    log.close();
    exit(EXIT_FAILURE);
}

LPCSTR szFontFileArcade = "assets/fonts/arcade.ttf";
int nResultsArcade = AddFontResourceEx(szFontFileArcade, FR_PRIVATE, NULL);

LPCSTR szFontFileOutrun = "assets/fonts/outrun.ttf";
int nResultsOutrun = AddFontResourceEx(szFontFileOutrun, FR_PRIVATE, NULL);

void render_messages(HDC hdc) {
    for (const auto& message: messages_to_render) {
		HFONT hFont = CreateFont(message.size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT(const_cast<char*>(message.font.c_str())));
        HFONT hOldFont = (HFONT) SelectObject(hdc, hFont);
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, COLORREF RGB(message.color_r, message.color_g, message.color_b));

        LPSTR lpstr_message = const_cast<char*>(message.message.c_str());
        TextOut(hdc, message.x, message.y, lpstr_message, int(strlen(lpstr_message)));

        SelectObject(hdc, hOldFont);
		DeleteObject(hFont);
    }
}

static void CALLBACK update_proc(HWND hwnd) {
	if (quited)
		return;

	is_active = GetActiveWindow() == hwnd;

	GetCursorPos(&cursor_pos);
	ScreenToClient(hwnd, &cursor_pos);

	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	dt = float((unsigned long)(t.QuadPart - qpc_ref_time.QuadPart)) / qpc_frequency.QuadPart;
	if (dt > 0.1f)
		dt = 0.1f;

	act(dt, fps);

	if (!quited) {
		draw();
		RedrawWindow(hwnd, NULL, 0, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	qpc_ref_time = t;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
		case WM_ERASEBKGND:
        	return (LRESULT)1;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdcMem;
			HBITMAP hbmMem;
			HANDLE hOld;
			HDC hdc = BeginPaint(hwnd, &ps);

			hdcMem = CreateCompatibleDC(hdc);
    		hbmMem = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
			hOld = SelectObject(hdcMem, hbmMem);

			BITMAPINFOHEADER bih;
			bih.biSize = sizeof(bih);
			bih.biWidth = SCREEN_WIDTH;
			bih.biHeight = -SCREEN_HEIGHT;
			bih.biPlanes = 1;
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biSizeImage = 0;
			bih.biXPelsPerMeter = 96;
			bih.biYPelsPerMeter = 96;
			bih.biClrUsed = 0;
			bih.biClrImportant = 0;

			SetDIBitsToDevice(
				hdcMem,
				0, 0,
				SCREEN_WIDTH, SCREEN_HEIGHT,
				0, 0,
				0, SCREEN_HEIGHT,
				buffer,
				(BITMAPINFO*)&bih,
				DIB_RGB_COLORS);
			render_messages(hdcMem);

			BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem, hOld);
			DeleteObject(hbmMem);
    		DeleteDC (hdcMem);
			
			EndPaint(hwnd, &ps);
			}
			break;
		case WM_QUIT:
		case WM_DESTROY:
			quited = true;
			break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
	}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
	{
	SetProcessDPIAware();
	hinst = hInstance;
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WNDCLASSEXA wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = "GameWndClass";
	wcex.hIconSm = 0;
	RegisterClassExA(&wcex);

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = SCREEN_WIDTH;
	rect.bottom = SCREEN_HEIGHT;
	AdjustWindowRectEx(&rect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE, 0);

	HWND hwnd = CreateWindowA(wcex.lpszClassName, "Geometry Wars", WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

	if (!hwnd)
		return 0;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	QueryPerformanceFrequency(&qpc_frequency);
	QueryPerformanceCounter(&qpc_ref_time);

	ticks = GetTickCount();
	initialize();

	MSG msg;
	while (!quited)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}
		update_proc(hwnd);
		Sleep(0);
	}

	finalize();

	RemoveFontResourceEx(szFontFileArcade, FR_PRIVATE, NULL);
	RemoveFontResourceEx(szFontFileOutrun, FR_PRIVATE, NULL);

	return (int)msg.wParam;
	}
