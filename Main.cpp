#include <time.h>
#include <stdlib.h>
#include "super-engin.h"
#define input(X) (GetAsyncKeyState(X) & 0x8000)

super_engin Disp("NotUntertale", 9, 21);
AsciiSprite soul(4, 2), spear(3, 5);
POINT projectiles[7];

void gameLoop();

int main() {
    // Setup: Background, Sprites, Projectiles
    srand(time(NULL));
    for(int i = 0; i < 30; i++)
        for(int j = 0; j < 120; j++) {
            Disp.offScreenDisplay[i * 120 + j].Char = 176;
            Disp.offScreenDisplay[i * 120 + j].Color = DARK_GRAY;
        }
    
    for(int i = 1; i+1 < 30; i++)
        for(int j = 2; j+2 < 120; j++) {
            Disp.offScreenDisplay[i * 120 + j].Char = ' ';
            Disp.offScreenDisplay[i * 120 + j].Color = WHITE;
        }
    
    for(int i = 0; i < 4*2; i++) soul.Colors[i] = RED;
    soul.AsciiChars[0] = 219; soul.AsciiChars[1] = 219; soul.AsciiChars[2] = 219; soul.AsciiChars[3] = 219;
    soul.AsciiChars[4] = 223; soul.AsciiChars[5] = 219; soul.AsciiChars[6] = 219; soul.AsciiChars[7] = 223;

    for(int i = 0; i < 3*5; i++) spear.Colors[i] = CYAN;
    spear.AsciiChars[1] = 186; spear.AsciiChars[4] = 186; spear.AsciiChars[7] = 186;
    spear.AsciiChars[9] = '\\'; spear.AsciiChars[10] = 186; spear.AsciiChars[11] = '/';
    spear.AsciiChars[13] = 'v';
    
    for(int i = 0; i < 7; i++) {
        projectiles[i].x = (rand() % 38) * 3 + 3;
        projectiles[i].y = rand() % 25;
    }

    while(true) gameLoop();
    return 0;
}

int x = 8, y = 8, c = 0;
char *msg[3] = {"This is you", "Move with WASD", "Survive :)"};
// Shut up and appreciate my absolute garbage code
bool showspear[7] = {false, false, false, false, false, false, false};

void gameLoop() {
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
        Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
    }

    Disp.DrawSprite(soul, x, y, false);
    if(c/50 < 3) Disp.PutString(msg[c/50], x-3, y+3, false);
    else for(int i = 0; i < 7; i++) {
        if(showspear[i]) Disp.DrawSprite(spear, projectiles[i].x, projectiles[i].y, false);
        projectiles[i].y = (projectiles[i].y + 1) % 25;
        if(projectiles[i].y == 0) {
            showspear[i] = true;
            projectiles[i].y++;
            projectiles[i].x = (x + (rand()%30) - 15) % 113 + 2;
        }
    }
    c++;
    
    if(input('A')) if(x >= 4) x-=2;
    if(input('D')) if(x <= 112) x+=2;
    if(input('W')) if(y >= 2) y--;
    if(input('S')) if(y < 27) y++;
    
    Disp.OutPut();
    Sleep(50);
}