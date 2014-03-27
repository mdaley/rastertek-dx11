#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <memory>
#include "inputclass.h"
#include "graphicsclass.h"

using namespace std;

class SystemClass
{
public:
    SystemClass();
    ~SystemClass();

    void Initialize();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame();
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_applicationName;
    HINSTANCE m_hinstance;
    HWND m_hwnd;

    unique_ptr<InputClass> m_Input;
    unique_ptr<GraphicsClass> m_Graphics;
};

// FUNCTION PROTOTYPES 
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// GLOBALS
static SystemClass* ApplicationHandle = 0;
