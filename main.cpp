#include <iostream>
#include <Windows.h>
#include <vector>
#include <mutex>
#include <thread>

#define CHANGE_HOTKEY 1
#define START_APP 2

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
HWND hotkeyText;
HWND hkSetChar;
HWND startButton;
bool not_running = true;
bool loot_loop = true;
std::mutex mu;

void AddControls(HWND hWnd);


void loot(const std::vector<std::pair<int, int>>& coordiates){
    for (auto &el : coordiates){
        int random_sleep = (std::rand() % 30);
        int random_sleep_release = (std::rand() % 10);
        Sleep(random_sleep);
        int x = el.first;
        int y = el.second;
        INPUT  input = {0};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
        SetCursorPos(x, y);
        SendInput(1, &input, sizeof(input));
        Sleep(random_sleep_release);
        ZeroMemory(&input, sizeof(input));
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
        SendInput(1, &input, sizeof(input));

    }
}
int main_loop(bool &run, char selected_hk){
    std::vector<std::pair<int, int>> coordinates;
    coordinates.emplace_back(794, 406);
    coordinates.emplace_back(866, 406);
    coordinates.emplace_back(938, 406);
    coordinates.emplace_back(794, 478);
    coordinates.emplace_back(938, 478);
    coordinates.emplace_back(794, 550);
    coordinates.emplace_back(866, 406);
    coordinates.emplace_back(938, 406);
    HKL kl = LoadKeyboardLayoutA("00000409", KLF_ACTIVATE);
    SHORT vkey_code = VkKeyScanExA(selected_hk, kl);

    while (run){
        Sleep(10);
        if (GetAsyncKeyState(vkey_code)){
            std::cout<<"loot pressed"<< std::endl;
            loot(coordinates);
        }

    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow){
    WNDCLASSW wc = {0};
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpszClassName = L"myWindowClass";
    wc.lpfnWndProc = WindowProcedure;

    if(!RegisterClassW(&wc))
        return -1;

    CreateWindowW(L"myWindowClass", L"Tibia Looter", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 400, 175,
                 NULL, NULL, NULL, NULL);
    MSG msg = {0};

    while(GetMessage(&msg, NULL, NULL, NULL)){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
    switch (msg){
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_CREATE:
            AddControls(hWnd);
        case WM_COMMAND:
            switch(wp){
                case CHANGE_HOTKEY:
                    wchar_t text[2];
                    GetWindowTextW(hotkeyText, text, 2);
                    SetWindowTextW(hkSetChar, text);
                    break;
                case START_APP:
                    if (not_running){
                        LPCWSTR stop_text = L"Stop";
                        SetWindowTextW(startButton, stop_text);
                        wchar_t hk[2];
                        GetWindowTextW(hkSetChar, hk, 2);
                        char selected_hk = hk[0];
                        not_running = false;
                        loot_loop = true;
                        std::thread t1(&main_loop, std::ref(loot_loop), selected_hk);
                        t1.detach();
                        break;
                    } else {

                        LPCWSTR const start_text_1 = L"Start";
                        SetWindowTextW(startButton, start_text_1);
                        not_running = true;
                        loot_loop = false;
                        break;
                    }
            }
        default:
            return DefWindowProcW(hWnd, msg, wp, lp);
    }
}
 void AddControls(HWND hWnd){
     CreateWindowW(L"Static", L"Quick loot hotkey:", WS_VISIBLE | WS_CHILD, 25, 25,
                   120, 20, hWnd, NULL, NULL, NULL);
     hotkeyText = CreateWindowW(L"Edit", L"v", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER, 150, 25,
                   35, 20, hWnd, NULL, NULL, NULL);
     CreateWindowW(L"Static", L"Current hotkey:", WS_VISIBLE | WS_CHILD, 25, 50,
                                100, 20, hWnd, NULL, NULL, NULL);
     hkSetChar = CreateWindowW(L"Static", L"v", WS_VISIBLE | WS_CHILD | SS_CENTER, 150, 50,
                   35, 20, hWnd, NULL, NULL, NULL);
     CreateWindowW(L"Button", L"Set Hotkey", WS_VISIBLE | WS_CHILD, 187, 25,
                   80, 20, hWnd, (HMENU) CHANGE_HOTKEY, NULL, NULL);
     startButton = CreateWindowW(L"Button", L"Start", WS_VISIBLE | WS_CHILD, 25, 75,
                   80, 20, hWnd, (HMENU) START_APP, NULL, NULL);

}