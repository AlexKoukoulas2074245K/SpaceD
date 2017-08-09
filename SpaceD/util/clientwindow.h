/********************************************************************/
/** clientwindow.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Remote Headers
#include <Windows.h>
#include <string>

// Local Headers

class ClientWindow final
{
public:
	ClientWindow(HINSTANCE instance, WNDPROC wndProc, const std::string& title, const int width, const int height);
	~ClientWindow();

	int GetWidth() const;
	int GetHeight() const;
	HWND GetHandle() const;
	const std::string& GetTitle() const;

	void UpdateOnResize(const int newWidth, const int newHeight);
	void UpdateCaption(const std::string& caption);

private:	
	const std::string _title;

	int _width;
	int _height;

	HWND _handle;
};