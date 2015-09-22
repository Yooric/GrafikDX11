#pragma once
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "InputClass.h"
#include "GraphicsClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
	
private:
	bool Frame();
	void InitializeWindows(int& p_screenWidth, int& p_screenHeight);
	void ShutdownWindows();
private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	InputClass* m_Input;
	GraphicsClass* m_Graphics;

};


////Function prottyper säger dem..
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

///Globals ska detta va.
static SystemClass* ApplicationHandle = 0;


#endif