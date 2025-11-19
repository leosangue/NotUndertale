#include <time.h>
#include <stdlib.h>
#include "super-engin.h"
#define input(X) (GetAsyncKeyState(X) & 0x8000)

super_engin Disp("NotUntertale", 9, 21);
AsciiSprite soul(4, 2), spear(3, 5), titleScr(92, 5), overScr(86, 7);
POINT projectiles[7];

void gameLoop(), titleScreen(), gameOver();
bool collides(int, int, int, int, int, int, int, int);
int p_x = 8, p_y = 8, frameCount = 0;
bool showspear[7] = {false, false, false, false, false, false, false};
byte heartColors[7] = {RED, GREEN, GOLD, CYAN, VIOLET, DARK_BLUE, PURPLE};
char *msg[6] = {
    "This is you", // Tutorial
    "Move with WASD",
    "Survive :)",
    "You're supposed to dodge the attacks!", // Death messages
    "You must survive!",
    "Why did you just die :(",
};

int main() {
    // Setup: Background, Sprites, Projectiles
    srand(time(NULL));
    for(int i = 0; i < 30; i++)
        for(int j = 0; j < 120; j++) {
            Disp.offScreenDisplay[i * 120 + j].Char = 176;
            Disp.offScreenDisplay[i * 120 + j].Color = GRAY;
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

    titleScr.AsciiChars =
    "_|    _|  _|      _|  _|_|_|    _|_|_|_|  _|_|_|    _|_|_|_|_|    _|_|    _|        _|_|_|_|"
    "_|    _|  _|_|    _|  _|    _|  _|        _|    _|      _|      _|    _|  _|        _|      "
    "_|    _|  _|  _|  _|  _|    _|  _|_|_|    _|_|_|        _|      _|_|_|_|  _|        _|_|_|  "
    "_|    _|  _|    _|_|  _|    _|  _|        _|    _|      _|      _|    _|  _|        _|      "
    "  _|_|    _|      _|  _|_|_|    _|_|_|_|  _|    _|      _|      _|    _|  _|_|_|_|  _|_|_|_|"
    ;
    for(int i = 0; i < 92*5; i++)
        if(titleScr.AsciiChars[i] != ' ') titleScr.Colors[i] = joinColor(WHITE, BLACK);

    overScr.AsciiChars =
    " #################################################################################### "
    "##      ####      ###  ####  ##        #########      ###  ####  ##        ##       ##"
    "#  ########  ####  ##   ##   ##  ##############  ####  ##  ####  ##  ########  ####  #"
    "#  ###   ##  ####  ##        ##      ##########  ####  ###  ##  ###      ####       ##"
    "#  ####  ##        ##  #  #  ##  ##############  ####  ####    ####  ########  ###  ##"
    "##      ###  ####  ##  ####  ##        #########      ######  #####        ##  ####  #"
    " #################################################################################### "
    ;

    for(int i = 0; i < 86*7; i++)
        overScr.Colors[i] = LIGHT_GOLD;

    titleScreen();
    while(true) gameLoop();
    return 0;
}

bool collides(int x_1, int w_1, int y_1, int h_1, int x_2, int w_2, int y_2, int h_2) {
    if (x_1 + w_1 >= x_2 &&
      x_1 <= x_2 + w_2 && 
      y_1 + h_1 >= y_2 &&   
      y_1 <= y_2 + h_2) {   
        return true;
  }
  return false;
}

void titleScreen() {
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = ' ';
        Disp.wDisplay[i].Attributes = WHITE;
    }
    Disp.DrawSprite(titleScr, 4, 4, false);
    Disp.wDisplay[5*120+49].Char.AsciiChar = 3;
    Disp.wDisplay[5*120+49].Attributes = RED;
    Disp.OutPut();
    Sleep(1000);
    Disp.PutString("Press enter to start!", 6, 10, false);
    while(!input(VK_RETURN)) Disp.OutPut();
}


void gameOver() {
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = ' ';
        Disp.wDisplay[i].Attributes = WHITE;
    }
    Disp.DrawSprite(soul, p_x, p_y, false);
    Disp.OutPut();
    Sleep(1300);
    for(int i = 0; i < 120*30; i++)
        if(Disp.wDisplay[i].Attributes == RED) Disp.wDisplay[i].Char.AsciiChar = '#';
    Disp.OutPut();
    Sleep(200);
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = ' ';
        Disp.wDisplay[i].Attributes = WHITE;
    }
    Disp.DrawSprite(overScr, 4, 4, false);
    Disp.OutPut();
    Sleep(1000);
    Disp.PutString(msg[rand() % 3 + 3], 8, 12, false);
    Disp.wDisplay[12*120+6].Char.AsciiChar = 3;
    Disp.wDisplay[12*120+6].Attributes = heartColors[rand() % 7];
    while(!input(VK_RETURN)) Disp.OutPut();
    exit(0);
}


void gameLoop() {
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
        Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
    }

    Disp.DrawSprite(soul, p_x, p_y, false);
    if(frameCount/50 < 3)
        Disp.PutString(msg[frameCount/50], p_x-3, p_y+3, false);
    else
        for(int i = 0; i < 7; i++) {
            if(showspear[i]) {
                if(collides(p_x, 4, p_y, 2, projectiles[i].x, 3, projectiles[i].y, 5)) gameOver();
                Disp.DrawSprite(spear, projectiles[i].x, projectiles[i].y, false);
            }

            projectiles[i].y = (projectiles[i].y + 1) % 25;
            if(projectiles[i].y == 0) {
                showspear[i] = true;
                projectiles[i].y++;
                projectiles[i].x = (p_x + (rand()%30) - 15) % 113 + 2;
            }
        }
    frameCount++;
    
    if(input('A') || input(VK_LEFT)) if(p_x >= 4) p_x-=2;
    if(input('D') || input(VK_RIGHT)) if(p_x <= 112) p_x+=2;
    if(input('W') || input(VK_UP)) if(p_y >= 2) p_y--;
    if(input('S') || input(VK_DOWN)) if(p_y < 27) p_y++;
    
    Disp.OutPut();
    Sleep(50);
}