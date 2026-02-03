#include <windows.h>
#include <shellapi.h>

#include <hidapi.h>


#define MAX_STR 255

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001
#define ID_TRAY_FNLOCK 1002
#define ID_TRAY_FNUNLOCK 1003

HBITMAP g_hIconFnlock = nullptr;
HBITMAP g_hIconFnunlock = nullptr;

HINSTANCE g_hInst;
NOTIFYICONDATA nid = {};

static const int seq_len = 7;
const unsigned char k400p_seq_fkeys_on[] = { 0x10, 0x01, 0x09, 0x19, 0x00, 0x00, 0x00 };
const unsigned char k400p_seq_fkeys_off[] = { 0x10, 0x01, 0x09, 0x18, 0x01, 0x00, 0x00 };
static const int k400p_vid = 0x46d;
static const int k400p_pid = 0xc52b;
static const int TARGET_USAGE = 1;
static const int TARGET_USAGE_PAGE = 65280;


void ChangeTrayIcon(int iconId)
{
    HICON hIcon = LoadIconW(g_hInst, MAKEINTRESOURCE(iconId));

    nid.uFlags = NIF_ICON;
    nid.hIcon = hIcon;

    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

HBITMAP IconToBitmap(HICON hIcon, int width = 16, int height = 16)
{
    // Create memory DC
    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    // Create a bitmap
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    // Fill background transparent (optional)
    PatBlt(hdcMem, 0, 0, width, height, WHITENESS);

    // Draw icon onto bitmap
    DrawIconEx(hdcMem, 0, 0, hIcon, width, height, 0, nullptr, DI_NORMAL);

    // Cleanup
    SelectObject(hdcMem, hOld);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);

    return hBitmap; // Remember to DeleteObject(hBitmap) when done
}

int FnLock(bool lock, HWND hwnd)
{
    int res;
    int result = -1;
    unsigned char buf[65];
    wchar_t wstr[MAX_STR];
    hid_device* handle;
    struct hid_device_info* devs, * cur_dev;

    res = hid_init();

    devs = hid_enumerate(k400p_vid, k400p_pid);
    cur_dev = devs;
    while (cur_dev)
    {
        if (cur_dev->usage == TARGET_USAGE && cur_dev->usage_page == TARGET_USAGE_PAGE)
        {
            handle = hid_open_path(cur_dev->path);
            if (lock)
            {
                res = hid_write(handle, k400p_seq_fkeys_on, seq_len);
            }
            else
            {
                res = hid_write(handle, k400p_seq_fkeys_off, seq_len);
            }

            if (res != seq_len)
            {
                MessageBox(hwnd, hid_error(handle), L"Fn Error", MB_OK);
            }
            hid_close(handle);
            if (res < 0)
            {
                result = -1;
                break;
            }
            else
            {
                result = 0;
                break;
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    hid_exit();

    return result;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP)
        {
            POINT pt;
            GetCursorPos(&pt);

            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, ID_TRAY_FNLOCK, L"Fn Lock");
            AppendMenu(hMenu, MF_STRING, ID_TRAY_FNUNLOCK, L"Fn UnLock");
            AppendMenu(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");

            // assign icons
            SetMenuItemBitmaps(hMenu, ID_TRAY_FNLOCK, MF_BYCOMMAND, g_hIconFnlock, g_hIconFnlock);
            SetMenuItemBitmaps(hMenu, ID_TRAY_FNUNLOCK, MF_BYCOMMAND, g_hIconFnunlock, g_hIconFnunlock);

            SetForegroundWindow(hwnd);

            TrackPopupMenu(
                hMenu,
                TPM_RIGHTBUTTON,
                pt.x,
                pt.y,
                0,
                hwnd,
                nullptr
            );

            DestroyMenu(hMenu);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_TRAY_FNLOCK:
            if(FnLock(true, hwnd)==0) ChangeTrayIcon(100);
            break;
        case ID_TRAY_FNUNLOCK:
            if(FnLock(false, hwnd)==0) ChangeTrayIcon(101);
            break;

        case ID_TRAY_EXIT:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
    g_hInst = hInstance;

    const wchar_t CLASS_NAME[] = L"K400pFnTrayAppClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"K400p FnTray App",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    // Hide the window
    ShowWindow(hwnd, SW_HIDE);

    // Setup tray icon
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(100));
    wcscpy_s(nid.szTip, L"K400p FnTray App");

    Shell_NotifyIcon(NIM_ADD, &nid);

    g_hIconFnlock = IconToBitmap(LoadIconW(g_hInst, MAKEINTRESOURCE(100))); // custom icon from rc
    g_hIconFnunlock = IconToBitmap(LoadIconW(g_hInst, MAKEINTRESOURCE(101)));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(g_hIconFnlock);
    DeleteObject(g_hIconFnunlock);

    return 0;
}