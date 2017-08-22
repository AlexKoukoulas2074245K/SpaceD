/*********************************************************************/
/** clientwindow.cpp by Alex Koukoulas (C) 2017 All Rights Reserved **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "clientwindow.h"

// Remote Headers

ClientWindow::ClientWindow(HINSTANCE instance, WNDPROC wndProc, const std::string& title, const int width, const int height)
	: _title(title)
	, _width(width)
	, _height(height)
	, _handle(0)
{
	// Window Registration
	WNDCLASS wc      = {};
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hbrBackground = 0;
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hInstance     = instance;
	wc.lpfnWndProc   = wndProc;
	wc.lpszClassName = _title.c_str();
	wc.lpszMenuName  = 0;
	wc.style         = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, "Window class registration failed", 0, MB_ICONERROR);
		PostQuitMessage(-1);
	}

	// Adjust window to fit client rect 
	RECT rect = { 0, 0, width, height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

	_handle = CreateWindow(_title.c_str(), _title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, w, h, 0, 0, instance, 0);

	if (!_handle)
	{
		MessageBox(0, "Window creation failed", "Failure", 0);
		PostQuitMessage(-1);
	}

	// Show and update window
	ShowWindow(_handle, SW_SHOW);
	UpdateWindow(_handle);
}

ClientWindow::~ClientWindow(){}

INT ClientWindow::GetWidth() const
{
	return _width;
}

INT ClientWindow::GetHeight() const
{
	return _height;
}

FLOAT ClientWindow::GetAspectRatio() const
{
	return static_cast<float>(_width)/_height;
}

const std::string& ClientWindow::GetTitle() const
{
	return _title;
}

HWND ClientWindow::GetHandle() const
{
	return _handle;
}

void ClientWindow::UpdateOnResize(const int newWidth, const int newHeight)
{
	_width = newWidth;
	_height = newHeight;
}

void ClientWindow::UpdateCaption(const std::string& updatedCaption)
{
	SetWindowText(_handle, (_title + " -- " + updatedCaption).c_str());
}