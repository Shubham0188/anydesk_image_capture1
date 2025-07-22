#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include<future>


bool CaptureScreen(const std::string& filename) {
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemoryDC, hBitmap);

    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader = {};
    BITMAPINFOHEADER bi = {};

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;

    DWORD bmpSize = ((bmp.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmp.bmHeight;
    std::vector<BYTE> bmpBuffer(bmpSize);

    GetDIBits(hMemoryDC, hBitmap, 0, bmp.bmHeight, bmpBuffer.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bmfHeader.bfSize = bmfHeader.bfOffBits + bmpSize;

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file) return false;

    file.write((char*)&bmfHeader, sizeof(bmfHeader));
    file.write((char*)&bi, sizeof(bi));
    file.write((char*)bmpBuffer.data(), bmpSize);
    file.close();

    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    return true;
}

int main() {
    int frame = 0;

    while (frame < 10) {  // Capture 10 frames (for test)
        std::string filename = "frame_" + std::to_string(frame) + ".bmp";
        if (CaptureScreen(filename)) {
            std::cout << "Captured frame: " << filename << "\n";
        } else {
            std::cout << "Failed to capture frame.\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10 fps
        frame++;
    }

    std::cout << "Capture complete.\n";
    return 0;
}

