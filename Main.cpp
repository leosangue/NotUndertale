#include <time.h>
#include <stdlib.h>
#include "super-engin.h"
#define input(X) if(GetAsyncKeyState(X) & 0x8000)

super_engin Disp("NotUntertale", 9, 21);
AsciiSprite soul(4, 2), spearsouth(3, 5), spearnorth(3, 5);
POINT projectiles[7];
int projectileDir[7];

void gameLoop();

int main() {
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

    for(int i = 0; i < 3*5; i++) spearsouth.Colors[i] = CYAN;
    spearsouth.AsciiChars[1] = 186; spearsouth.AsciiChars[4] = 186; spearsouth.AsciiChars[7] = 186;
    spearsouth.AsciiChars[9] = '\\'; spearsouth.AsciiChars[10] = 186; spearsouth.AsciiChars[11] = '/';
    spearsouth.AsciiChars[13] = 'v';
    for(int i = 0; i < 3*5; i++) spearnorth.Colors[i] = CYAN;
    spearnorth.AsciiChars[1] = '^'; spearnorth.AsciiChars[3] = '/'; spearnorth.AsciiChars[4] = 186;
    spearnorth.AsciiChars[5] = '\\'; spearnorth.AsciiChars[7] = 186; spearnorth.AsciiChars[10] = 186;
    spearnorth.AsciiChars[13] = 186;
    
    for(int i = 0; i < 7; i++) {
        projectiles[i].x = (rand() % 38) * 3 + 3;
        projectiles[i].y = rand() % 25;
        projectileDir[i] = (rand() % 2) ? 1 : -1;
    }

    while(true) gameLoop();
    return 0;
}

int x = 8, y = 8, c = 0;
char *msg[3] = {"This is you", "Move with WASD", "Survive :)"};
bool showspearsouth[7] = {false, false, false, false, false, false, false};
bool showspearnorth[7] = {false, false, false, false, false, false, false};

void gameLoop() {
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
        Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
    }

    Disp.DrawSprite(soul, x, y, false);
    if(c/50 < 3) Disp.PutString(msg[c/50], x-3, y+3, false);
    else for(int i = 0; i < 7; i++) {
        if(projectileDir[i] == 1) {
            if(showspearsouth[i]) Disp.DrawSprite(spearsouth, projectiles[i].x, projectiles[i].y, false);
            projectiles[i].y = (projectiles[i].y + 1) % 25;
            if(projectiles[i].y == 0) {
                showspearsouth[i] = true;
                showspearnorth[i] = false;
                projectiles[i].y = 1;
                projectiles[i].x = ((x + (rand()%30) - 15 + 113) % 113) + 2;
            }
        }
        else {
            if(showspearnorth[i]) Disp.DrawSprite(spearnorth, projectiles[i].x, projectiles[i].y, false);
            projectiles[i].y = (projectiles[i].y - 1 + 25) % 25;
            if(projectiles[i].y == 24) {
                showspearnorth[i] = true;
                showspearsouth[i] = false;
                projectiles[i].y = 23;
                projectiles[i].x = ((x + (rand()%30) - 15 + 113) % 113) + 2;
            }
        }
    }
    c++;
    
    input('A') if(x >= 4) x-=2;
    input('D') if(x <= 112) x+=2;
    input('W') if(y >= 2) y--;
    input('S') if(y < 27) y++;
    
    Disp.OutPut();
    Sleep(50);
}