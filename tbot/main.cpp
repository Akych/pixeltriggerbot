#include <Windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#pragma comment(lib, "winmm.lib")
using namespace std;

std::wstring soundFilePath;
int resolution = 45;

int shot_button = VK_NUMPAD1;
int sound_button = VK_NUMPAD2;

double colorDistance(int r1, int g1, int b1, int r2, int g2, int b2) {
    int deltaR = r2 - r1;
    int deltaG = g2 - g1;
    int deltaB = b2 - b1;
    return sqrt(deltaR * deltaR + deltaG * deltaG + deltaB * deltaB);
}

void capturePixelColor() {

    HWND hWnd = GetDesktopWindow();

    HDC hdc = GetDC(hWnd);

    COLORREF oldcolor = RGB(255, 0, 0);

    if (hdc == NULL) {
        cerr << "Failed to get device context." << endl;
        return;
    }

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int centerX = (screenWidth) / 2 - 1;
    int centerY = (screenHeight) / 2 - 1;

    while (true) {
        RECT screenRect;
        GetClientRect(hWnd, &screenRect);
        COLORREF centerColor = GetPixel(hdc, centerX, centerY);
        if (GetAsyncKeyState(sound_button) & 0x8000) {
            if (oldcolor != centerColor) {
                int red = GetRValue(centerColor);
                int green = GetGValue(centerColor);
                int blue = GetBValue(centerColor);
                int old_red = GetRValue(oldcolor);
                int old_green = GetGValue(oldcolor);
                int old_blue = GetBValue(oldcolor);
                if (colorDistance(red, green, blue, old_red, old_green, old_blue) > resolution) {
                    PlaySound(soundFilePath.c_str(), NULL, SND_FILENAME);
                    //wcout << soundFilePath.c_str() << endl;
                }
                oldcolor = centerColor;
            }
            this_thread::sleep_for(chrono::milliseconds(100));
        }

        if (GetAsyncKeyState(shot_button) & 0x8000) {
            if (oldcolor != centerColor) {

                int red = GetRValue(centerColor);
                int green = GetGValue(centerColor);
                int blue = GetBValue(centerColor);
                int old_red = GetRValue(oldcolor);
                int old_green = GetGValue(oldcolor);
                int old_blue = GetBValue(oldcolor);

                if (colorDistance(red, green, blue, old_red, old_green, old_blue) > resolution) {
                    // cout << "DIST (" << colorDistance(red, green, blue, old_red, old_green, old_blue) << "): " << endl;
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    this_thread::sleep_for(chrono::milliseconds(20));
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                    this_thread::sleep_for(chrono::milliseconds(400));
                }
                oldcolor = centerColor;
            }
        }
        else {
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        oldcolor = centerColor;
    }
    ReleaseDC(hWnd, hdc);
}

int main() {
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);
    soundFilePath = szPath;
    size_t lastSlashPos = soundFilePath.find_last_of(L"\\/");
    if (lastSlashPos != std::wstring::npos) {
        soundFilePath = soundFilePath.substr(0, lastSlashPos)+ L"\\tick.wav";;
    }
    else {
        cerr << "Failed to get executable path." << endl;
        return 1;
    }
    wcout << soundFilePath << endl;
    capturePixelColor();
    return 0;
}