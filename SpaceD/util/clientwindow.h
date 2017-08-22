/********************************************************************/
/** clientwindow.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <Windows.h>
#include <string>

class ClientWindow final
{
public:
	ClientWindow(HINSTANCE instance, WNDPROC wndProc, const std::string& title, const int width, const int height);
	~ClientWindow();

	INT GetWidth() const;
	INT GetHeight() const;
	FLOAT GetAspectRatio() const;
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