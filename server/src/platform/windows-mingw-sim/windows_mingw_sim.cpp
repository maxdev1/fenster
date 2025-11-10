// fenster — lightweight window server and UI toolkit
// Copyright (c) 2025 Max Schlüssel
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifdef _WIN32

#include "windows_mingw_sim.hpp"

#include <server.hpp>
#include <components/cursor.hpp>
#include <libfenster/input/key_info.hpp>

#include <windows.h>
#include <cairo/cairo.h>
#include <cairo/cairo-win32.h>
#include <dirent.h>

static const int WIDTH = 1600, HEIGHT = 1200;
static unsigned int pixels[WIDTH * HEIGHT];
static BITMAPINFO bmi;
static HWND hwnd;
static bool running = true;

namespace fensterserver
{
	Server* server = nullptr;
}

void CenterWindow(HWND hwnd)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);

	int winWidth = rect.right - rect.left;
	int winHeight = rect.bottom - rect.top;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int posX = (screenWidth - winWidth) / 2;
	int posY = (screenHeight - winHeight) / 2;

	SetWindowPos(hwnd, HWND_TOP, posX, posY, 0, 0,
	             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

bool ctrl = false;
bool shift = false;
bool alt = false;

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l)
{
	switch(m)
	{
		case WM_CREATE:
			ShowCursor(FALSE);
			return 0;
		case WM_DESTROY:
			ShowCursor(TRUE);
			running = false;
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
		{
			if(w == VK_ESCAPE)
			{
				running = false;
				DestroyWindow(h);
			}

			auto key = fensterserver::platformKeyForVKey(w);
			if(key == "KEY_SHIFT_L")
			{
				shift = true;
			}
			else if(key == "KEY_CTRL_L")
			{
				ctrl = true;
			}
			else if(key == "KEY_ALT_L")
			{
				alt = true;
			}

			if(ctrl && key == "KEY_Q" )
			{
				fensterserver::Server::setDebug(!fensterserver::Server::isDebug());
				return 0;
			}

			fenster::KeyInfo info;
			info.pressed = true;
			info.key = key;
			info.ctrl = ctrl;
			info.shift = shift;
			info.alt = alt;
			info.pressed = true;
			fensterserver::Server::instance()->eventProcessor->bufferKeyEvent(info);
			fensterserver::Server::instance()->requestUpdateImmediately();
			return 0;
		}
		case WM_KEYUP:
		{
			if(w == VK_ESCAPE)
			{
				running = false;
				DestroyWindow(h);
			}

			auto key = fensterserver::platformKeyForVKey(w);
			if(key == "KEY_SHIFT_L")
			{
				shift = false;
			}
			else if(key == "KEY_CTRL_L")
			{
				ctrl = false;
			}
			else if(key == "KEY_ALT_L")
			{
				alt = false;
			}

			fenster::KeyInfo info;
			info.scancode = w;
			info.key = key;
			info.ctrl = ctrl;
			info.shift = shift;
			info.alt = alt;
			info.pressed = false;
			fensterserver::Server::instance()->eventProcessor->bufferKeyEvent(info);
			fensterserver::Server::instance()->requestUpdateImmediately();
			return 0;
		}
		case WM_MOUSEMOVE:
		{
			int mouseX = GET_X_LPARAM(l);
			int mouseY = GET_Y_LPARAM(l);

			if(mouseY < 10)
				while(ShowCursor(TRUE) < 0)
				{
				}
			else
				while(ShowCursor(FALSE) >= 0)
				{
				}

			fensterserver::Cursor::nextPosition.x = mouseX;
			fensterserver::Cursor::nextPosition.y = mouseY;

			if(fensterserver::Cursor::nextPosition.x < 0)
			{
				fensterserver::Cursor::nextPosition.x = 0;
			}
			if(fensterserver::Cursor::nextPosition.x > WIDTH - 2)
			{
				fensterserver::Cursor::nextPosition.x = WIDTH - 2;
			}
			if(fensterserver::Cursor::nextPosition.y < 0)
			{
				fensterserver::Cursor::nextPosition.y = 0;
			}
			if(fensterserver::Cursor::nextPosition.y > HEIGHT - 2)
			{
				fensterserver::Cursor::nextPosition.y = HEIGHT - 2;
			}
			fensterserver::Server::instance()->requestUpdateImmediately();

			return 0;
		}
		case WM_LBUTTONDOWN:
			fensterserver::Cursor::nextPressedButtons |= fenster::MouseButton::Button1;
			fensterserver::Server::instance()->requestUpdateImmediately();
			return 0;
		case WM_LBUTTONUP:
			fensterserver::Cursor::nextPressedButtons &= ~fenster::MouseButton::Button1;
			fensterserver::Server::instance()->requestUpdateImmediately();
			return 0;
		case WM_RBUTTONDOWN:
			fensterserver::Cursor::nextPressedButtons |= fenster::MouseButton::Button2;
			fensterserver::Server::instance()->requestUpdateImmediately();
			return 0;
		case WM_RBUTTONUP:
			fensterserver::Cursor::nextPressedButtons &= ~fenster::MouseButton::Button2;
			fensterserver::Server::instance()->requestUpdateImmediately();
			return 0;
		case WM_MOUSEWHEEL:
		{
			int delta = GET_WHEEL_DELTA_WPARAM(w);
			int steps = delta / WHEEL_DELTA;

			fensterserver::Cursor::nextScroll += -steps;
			fensterserver::Server::instance()->requestUpdateImmediately();
			return 0;
		}
	}
	return DefWindowProc(h, m, w, l);
}


int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int)
{
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = inst;
	wc.lpszClassName = "GhostWindowServer";
	RegisterClass(&wc);

	hwnd = CreateWindowEx(0, wc.lpszClassName, "fenster", WS_OVERLAPPEDWINDOW,
	                      CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT, 0, 0, inst, 0);
	CenterWindow(hwnd);
	ShowWindow(hwnd, SW_SHOW);

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = WIDTH;
	bmi.bmiHeader.biHeight = -HEIGHT;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC dc = GetDC(hwnd);

	cairo_surface_t* surface = cairo_image_surface_create_for_data(
			(unsigned char*) pixels, CAIRO_FORMAT_RGB24, WIDTH, HEIGHT, WIDTH * 4);
	cairo_t* cr = cairo_create(surface);

	fensterserver::server = new fensterserver::Server();
	fenster::platformCreateThread((void*) &fensterserver::platformServerThread);

	MSG msg;
	while(running)
	{
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		StretchDIBits(dc, 0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT,
		              pixels, &bmi, DIB_RGB_COLORS, SRCCOPY);
		Sleep(16);
	}

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	ReleaseDC(hwnd, dc);
	return 0;
}

namespace fensterserver
{
	class windows_video_output_t : public VideoOutput
	{
	public:
		explicit windows_video_output_t() = default;

		bool initialize() override
		{
			return true;
		}

		void blit(fenster::Rectangle invalid, fenster::Rectangle sourceSize, fenster::ColorArgb* source) override
		{
			for(int y = 0; y < invalid.height; ++y)
			{
				fenster::ColorArgb* srcRow = source + (invalid.y + y) * sourceSize.width + invalid.x;
				fenster::ColorArgb* dstRow = pixels + (invalid.y + y) * WIDTH + invalid.x;
				memcpy(dstRow, srcRow, invalid.width * sizeof(fenster::ColorArgb));
			}
		}

		fenster::Dimension getResolution() override
		{
			return fenster::Dimension(WIDTH, HEIGHT);
		}
	};

	void platformServerThread()
	{
		server->launch();
	}

	void platformStartInput()
	{
	}

	VideoOutput* platformCreateVideoOutput()
	{
		return new windows_video_output_t();
	}

	bool platformInitializeKeyboardLayout(std::string layout)
	{
		return true;
	}

	void platformLoadCursors()
	{
		const char* path = "platform/sim/cursor";
		DIR* dir = opendir(path);
		if(!dir)
		{
			return;
		}
		struct dirent* entry;
		while((entry = readdir(dir)) != NULL)
		{
			std::string path = std::string("platform/sim/cursor") + "/" + entry->d_name;
			Cursor::load(path);
		}
		Cursor::set("default");
		closedir(dir);
	}
}


#endif
