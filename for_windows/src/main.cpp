#pragma once

#include "misc.h"

#include <windows.h>
#include <shlobj.h>
#include <sstream>
#include <string>
#include <list>
#include <shellapi.h>

#include "resource.h"

#include "../../utils_win.h"
#include "../../logger.h"
#include "../../aps_meter.h"

#define MAX_LOADSTRING 255
const unsigned int ID_LOGGER = 3939;
const unsigned int IDI_SYSTRAY = 1000;

const unsigned int WM_USER_SHELLICON = 2000;

const unsigned int IDM_EXIT = 3000;
const unsigned int IDM_SHOW_WINDOW = 3001;
const unsigned int IDM_HIDE_WINDOW = 3002;

const std::wstring APP_NAME_STRING =L"ACTIVITY_TRACKER_FOR_GUI";

// Global Variables:
HINSTANCE g_hInst = NULL;   // current instance
HWND g_hWnd = NULL;   // current HWND
HWND g_hWndEdit = NULL;
HMENU g_hPopup = NULL;
HANDLE g_hEvent = NULL;
TCHAR szTitle[MAX_LOADSTRING] = L"";  // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING] = L"";  // the main window class name

tracker::aps::ActionPerSecondMeter g_aps;
Logger g_logger;
NOTIFYICONDATA g_nidApp;
BOOL g_ShowStatus;

// Forward declarations of functions included in this code module.
ATOM RegisterMainClass(HINSTANCE hInstance);
BOOL CreateMainWindow(HINSTANCE, int);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

int Initialize() {
  g_hEvent = ::CreateEvent(NULL, FALSE, FALSE, APP_NAME_STRING.c_str());

  if (g_hEvent == NULL || GetLastError() == ERROR_ALREADY_EXISTS ) {
    ::CloseHandle(g_hEvent);
    HANDLE hOpen = ::OpenEvent(EVENT_ALL_ACCESS | EVENT_MODIFY_STATE, FALSE, APP_NAME_STRING.c_str());
    ::SetEvent(hOpen);
    ::CloseHandle(hOpen);
    return 1;
  }
  return 0;
}

int RunMain(HINSTANCE hInstance, int nCmdShow) {
  // Initialize global strings
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_WNDCLASS, szWindowClass, MAX_LOADSTRING);
  RegisterMainClass(hInstance);

  // Perform application initialization
  if (!CreateMainWindow(hInstance, nCmdShow))
    return FALSE;

  g_aps.start();

  FluentdUdpOutput fluentd_udp_output;
  g_logger.route(L"activity", &fluentd_udp_output);

  EditBoxOutput editbox_output(g_hWndEdit);
  CounterFilter count_filter(0);
  editbox_output.withFilter(&count_filter);
  g_logger.route(L"log", &editbox_output);

  g_logger.send(InternalLog(L"INFO", L"starting activity-tracker"));
  count_filter.set_max(60);

  ::SetTimer(g_hWnd, ID_LOGGER, 1000, NULL);

  // Main message loop:
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  Shell_NotifyIcon(NIM_DELETE, &g_nidApp);

  return (int) msg.wParam;
}

// Register the main window class.
ATOM RegisterMainClass(HINSTANCE hInstance) {
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = MainWndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = NULL;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APP_ICON));

  return RegisterClassEx(&wcex);
}

// Create and show the main window.
BOOL CreateMainWindow(HINSTANCE hInstance, int nCmdShow) {
  UNREFERENCED_PARAMETER(nCmdShow);
  g_hInst = hInstance;  // Store instance handle in our global variable

  g_hWnd = CreateWindow(szWindowClass, szTitle,
                        WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, 0,
                        700, 500, NULL, NULL, hInstance, NULL);

  g_hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"Edit", L"",
          WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL ,
          0, 0, 685, 463, g_hWnd, NULL, NULL, NULL);
  ::SendMessage(g_hWndEdit, EM_SETREADONLY, 1,0);

  if (!g_hWnd)
    return FALSE;

  g_nidApp.cbSize = sizeof(NOTIFYICONDATA);
  g_nidApp.hWnd = (HWND)g_hWnd;
  g_nidApp.uID = IDI_SYSTRAY;
  g_nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
  g_nidApp.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
  g_nidApp.uCallbackMessage = WM_USER_SHELLICON;
  LoadString(hInstance, IDS_APPTOOLTIP, g_nidApp.szTip, MAX_LOADSTRING);
  Shell_NotifyIcon(NIM_ADD, &g_nidApp);

  g_ShowStatus = false;
  ShowWindow(g_hWnd, SW_HIDE);
  UpdateWindow(g_hWnd);

  return TRUE;
}

void PopupMenu() {
  POINT point;
  UINT uFlag = MF_BYPOSITION | MF_STRING;
  GetCursorPos(&point);
  g_hPopup = CreatePopupMenu();
  if (g_ShowStatus)
    InsertMenu(g_hPopup, 0xFFFFFFFF, uFlag, IDM_HIDE_WINDOW, L"Hide Log");
  else
    InsertMenu(g_hPopup, 0xFFFFFFFF, uFlag, IDM_SHOW_WINDOW, L"Show Log");

  InsertMenu(g_hPopup, 0xFFFFFFFF, uFlag, IDM_EXIT, L"Exit");

  SetForegroundWindow(g_hWnd);
  TrackPopupMenu(g_hPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN,
    point.x, point.y, 0, g_hWnd, NULL);
}

void CheckEvent() {
  if (::WaitForSingleObject(g_hEvent, 0) == WAIT_OBJECT_0) {
    g_ShowStatus = true;
    ShowWindow(g_hWnd, SW_SHOW);
    UpdateWindow(g_hWnd);
  }
}

LRESULT OnTimer(UINT timerId) {
  using namespace tracker::utils;
  switch (timerId) {
    case ID_LOGGER: {
      CheckEvent();
      HWND activeWindow = GetForegroundWindow();
      DWORD activePID;
      GetWindowThreadProcessId(activeWindow, &activePID);

      std::wstring activeWindowText = getActiveWindowText(activeWindow);
      std::wstring activeFilename = getAcitveFilename(activePID);
      std::wstring activeUsername = getActiveUsername();
      std::wstring status = isIdle() ? L"idle" : L"active";
      int actionPerSecond = g_aps.getActionPerSecond();

      g_logger.send(ActivityLog(status, activeUsername, activeWindowText, activeFilename, actionPerSecond));
      g_logger.send(InternalLog(L"INFO", L"activity-tracker is running..."));
      break;
    }
    default:
    break;
  }
  return TRUE;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  HDC hdc;

  switch (message) {
  case WM_PAINT: {
    hdc = BeginPaint(hWnd, &ps);
    // TODO: Add any drawing code here...
    EndPaint(hWnd, &ps);
    return 0;
  }
  case WM_TIMER:
    return OnTimer((UINT)wParam);
  case WM_DESTROY: {
    Shell_NotifyIcon(NIM_DELETE, &g_nidApp);
    PostQuitMessage(0);
    return 0;
  }
  case WM_CLOSE: {
    g_ShowStatus = false;
    ShowWindow(g_hWnd, SW_HIDE);
    UpdateWindow(g_hWnd);
    return 0;
  }
  case WM_USER_SHELLICON: {
    // systray msg callback
    switch (LOWORD(lParam))
    {
    case WM_RBUTTONDOWN:
      PopupMenu();
      return TRUE;
    }
  }
  case WM_COMMAND: {
    int wmID = LOWORD(wParam);
    int wmEvent = HIWORD(wParam);
    switch (wmID)
    {
    case IDM_HIDE_WINDOW:
      g_ShowStatus = false;
      ShowWindow(g_hWnd, SW_HIDE);
      UpdateWindow(g_hWnd);
      break;
    case IDM_SHOW_WINDOW:
      g_ShowStatus = true;
      ShowWindow(g_hWnd, SW_SHOW);
      UpdateWindow(g_hWnd);
      break;
    case IDM_EXIT:
      ::MessageBox(NULL, L"You can EXECUTE any time you like, but you can never close!", L"NOT ALLOW", MB_ICONWARNING | MB_OK);
      break;
    }
  }
    return TRUE;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

// Program entry point function.
int APIENTRY wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR    lpCmdLine,
                      int       nCmdShow) {

  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  int exit_code;

  try {
    do {
      if ((exit_code = Initialize()) != 0) break;
      if ((exit_code = RunMain(hInstance, nCmdShow)) != 0) break;
    } while (0);
  } catch (...) {
    exit_code = -1;

    MessageBoxW(NULL, L"ERROR!!!", L"Title", MB_OK | MB_ICONINFORMATION | MB_SYSTEMMODAL);

    //UNREFERENCED_PARAMETER(error);
  }
  return exit_code;
}