#ifndef SUPER_ENGIN_H
#define SUPER_ENGIN_H
#include <windows.h>

// Por si se te olvida, offBuffer sirve de buffer de fondo, util para cargar un fondo estatico para resetear cada frame.
typedef struct {
    char Char;
    byte Color;
} offBuffer;

enum Colors {
// Dark colors
    BLACK = 0x00,
    DARK_BLUE = 0x01,
    DARK_GREEN = 0x02,
    DARK_RED = 0x04,
    DARK_GRAY = 0x08,
    DARK_CYAN = 0x09,
    PURPLE = 0x05,
    GOLD = 0x06,
// Light colors
    WHITE = 0x0f,
    BLUE = 0x03,
    GREEN = 0x0a,
    RED = 0x0c,
    GRAY = 0x07,
    CYAN = 0x0b,
    VIOLET = 0x0d,
    LIGHT_GOLD = 0x0e,
};

byte joinColor(byte bkg_C, byte frn_C) {
    return bkg_C * 0x10 + frn_C;
}

class AsciiSprite {
public:
    short Width;
    short Height;
    char *AsciiChars;
    byte *Colors;

    AsciiSprite(short sWidth, short sHeight) {
        Width = sWidth;
        Height = sHeight;

        AsciiChars = new char[Width*Height];
        Colors = new byte[Width*Height];

        for(int i = 0; i < Width*Height; i++) {
            AsciiChars[i] = ' ';
            Colors[i] = WHITE;
        }
    }
};

class super_engin {
    private:
        HANDLE hConsole;
        SMALL_RECT rectWindow;
        COORD fontSize;
    public:
        HWND consoleHwnd = GetConsoleWindow();
        CHAR_INFO *wDisplay;
        offBuffer *offScreenDisplay;
        SHORT wWidth, wHeight;

        super_engin(CONST CHAR Title[], byte fWidth, byte fHeight);

        VOID DrawSprite(AsciiSprite Sprite, short X, short Y, BOOL offScreen);
        VOID PutString(const char Str[], short X, short Y, BOOL offScreen);
        VOID OutPut(void) { WriteConsoleOutputA(hConsole, wDisplay, {wWidth, wHeight}, {0, 0}, &rectWindow); }
};

super_engin::super_engin(CONST CHAR Title[], byte fWidth, byte fHeight) {
    wWidth = 120; wHeight = 30;
    wDisplay = new CHAR_INFO[wWidth * wHeight];
    offScreenDisplay = new offBuffer[wWidth * wHeight];

    fontSize.X = fWidth;
    fontSize.Y = fHeight;

    SetConsoleTitleA(Title);

    hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    rectWindow = {0, 0, wWidth, wHeight};

    DWORD prev_mode; HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    GetConsoleMode(hInput, &prev_mode); 
    SetConsoleMode(hInput, ENABLE_EXTENDED_FLAGS | (prev_mode & ~ENABLE_QUICK_EDIT_MODE));
}

VOID super_engin::DrawSprite(AsciiSprite Sprite, short X, short Y, BOOL offScreen) {
    for(int a = 0; a < Sprite.Height; a++) for(int b = 0; b < Sprite.Width; b++) {
        if(offScreen) {
            offScreenDisplay[(a + Y) * wWidth + b + X].Char = Sprite.AsciiChars[a * Sprite.Width + b];
            offScreenDisplay[(a + Y) * wWidth + b + X].Color = Sprite.Colors[a * Sprite.Width + b];
        } else {
            wDisplay[(a + Y) * wWidth + b + X].Char.AsciiChar = Sprite.AsciiChars[a * Sprite.Width + b];
            wDisplay[(a + Y) * wWidth + b + X].Attributes = Sprite.Colors[a * Sprite.Width + b];
        }
    }
}

VOID super_engin::PutString(const char Str[], short X, short Y, BOOL offScreen) {
    if(Y > 29) return;
    for(int i = 0; Str[i] != '\0'; i++) {
        if(X + i > 120) break;
        if(offScreen) offScreenDisplay[Y * wWidth + X + i].Char = Str[i];
        else wDisplay[Y * wWidth + X + i].Char.AsciiChar = Str[i];
    }
}

#endif