#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "super-engin.h"
#define input(X) (GetAsyncKeyState(X) & 0x8000)

super_engin Disp("NotUntertale", 9, 21);
AsciiSprite soul(4, 2), spear(3, 5), titleScr(92, 5), overScr(86, 7);
AsciiSprite buttons[6] = { AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5) };
POINT projectiles[7];
bool inLevel = false;

int titleScreen();
void runLevel(int level);
void gameOver();
bool collides(int, int, int, int, int, int, int, int);
int p_x = 8, p_y = 8, frameCount = 0;
bool showspear[7] = {false, false, false, false, false, false, false};
byte heartColors[7] = {RED, GREEN, GOLD, CYAN, VIOLET, DARK_BLUE, PURPLE};
const char *msg[6] = {
    "This is you",
    "Move with WASD",
    "Dodge the attacks :)",
    "You're supposed to dodge the attacks!",
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

    const int bw = 20, bh = 5, spacing = 4;
    const int numButtons = 6;
    const char border = (char)219;
    const char *labels[numButtons] = { " LEVEL 1 ", " LEVEL 2 ", " LEVEL 3 ", " LEVEL 4 ", " LEVEL 5 ", " LEVEL 6 " };
    for (int b = 0; b < numButtons; ++b) {
        AsciiSprite &btn = buttons[b];
        for (int i = 0; i < bw * bh; ++i) {
            btn.AsciiChars[i] = ' ';
            btn.Colors[i] = joinColor(WHITE, DARK_GRAY);
        }
        for (int y = 0; y < bh; ++y) {
            for (int x = 0; x < bw; ++x) {
                int idx = y * bw + x;
                if (y == 0 || y == bh-1 || x == 0 || x == bw-1) {
                    btn.AsciiChars[idx] = border;
                    btn.Colors[idx] = joinColor(WHITE, DARK_GRAY);
                }
            }
        }
        const char *label = labels[b];
        int llen = (int)strlen(label);
        int labelRow = bh/2;
        int startX = (bw - llen) / 2;
        if (startX < 1) startX = 1;
        for (int i = 0; i < llen && startX + i < bw-1; ++i) {
            int idx = labelRow * bw + (startX + i);
            btn.AsciiChars[idx] = label[i];
            btn.Colors[idx] = joinColor(WHITE, BLACK);
        }
        int leftIdx = labelRow * bw + (startX - 1);
        int rightIdx = labelRow * bw + (startX + llen);
        if (leftIdx >= 0 && leftIdx < bw*bh) { btn.AsciiChars[leftIdx] = '<'; btn.Colors[leftIdx] = joinColor(WHITE, BLACK); }
        if (rightIdx >= 0 && rightIdx < bw*bh) { btn.AsciiChars[rightIdx] = '>'; btn.Colors[rightIdx] = joinColor(WHITE, BLACK); }
    }
    
    
    

    while (true) {
        int sel = titleScreen();
        runLevel(sel);
    }
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
 
int titleScreen() {
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = ' ';
        Disp.wDisplay[i].Attributes = WHITE;
    }
    Disp.DrawSprite(titleScr, 4, 4, false);
    const int bw = 20, bh = 5, spacing = 4;
    Disp.wDisplay[5*120+49].Char.AsciiChar = 3;
    Disp.wDisplay[5*120+49].Attributes = RED;
    Disp.OutPut();
    Sleep(1000);

    int selected = 0;
    const int totalW = 3 * bw + 2 * spacing;
    const int startX = (120 - totalW) / 2;
    const int rowY1 = 12;
    const int rowY2 = rowY1 + bh + 2;

    while (true) {
        for(int i = 0; i < 120*30; i++) {
            Disp.wDisplay[i].Char.AsciiChar = ' ';
            Disp.wDisplay[i].Attributes = WHITE;
        }
        Disp.DrawSprite(titleScr, 12, 4, false);

        for (int b = 0; b < 6; ++b) {
            AsciiSprite &btn = buttons[b];
            for (int y = 0; y < bh; ++y) {
                for (int x = 0; x < bw; ++x) {
                    int idx = y * bw + x;
                    if (y == 0 || y == bh-1 || x == 0 || x == bw-1) {
                        btn.Colors[idx] = (b == selected) ? RED : joinColor(WHITE, DARK_GRAY);
                    }
                }
            }
            int drawX = startX + (b % 3) * (bw + spacing);
            int drawY = (b < 3) ? rowY1 : rowY2;
            Disp.DrawSprite(btn, drawX, drawY, false);
        }

        Disp.wDisplay[5*120+57].Char.AsciiChar = 3;
        Disp.wDisplay[5*120+57].Attributes = RED;
        Disp.PutString("Press ESC to exit", 0, 29, false);
        Disp.OutPut();
        Sleep(50);

        if (input('A')) { if (selected % 3 > 0) selected--; while(input('A')) Disp.OutPut(); }
        if (input('D')) { if (selected % 3 < 2) selected++; while(input('D')) Disp.OutPut(); }
        if (input('W')) { if (selected >= 3) selected -= 3; while(input('W')) Disp.OutPut(); }
        if (input('S')) { if (selected < 3) selected += 3; while(input('S')) Disp.OutPut(); }

        if (input(VK_RETURN)) {
            if (selected == 0) return selected; // start level 1
            while(input(VK_RETURN)) Disp.OutPut(); // otherwise ignore until released
        }
        if (input(VK_ESCAPE)) {
            exit(0);
        }
        Disp.OutPut();  
    }
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
    int a=rand() % 7;
    Disp.wDisplay[12*120+6].Attributes = heartColors[a];
    Disp.wDisplay[14*120+6].Char.AsciiChar = 3;
    Disp.wDisplay[14*120+6].Attributes = heartColors[a];
    Disp.PutString("Press enter to continue", 8, 14, false);
    while(!input(VK_RETURN)) Disp.OutPut();
    inLevel = false;
}


void runLevel(int level) {
    inLevel = true;
    frameCount = 0;
    p_x = 8; p_y = 8;
    for (int i = 0; i < 7; ++i) {
        projectiles[i].x = (rand() % 38) * 3 + 3;
        projectiles[i].y = rand() % 25;
        showspear[i] = false;
    }

    while (inLevel) {
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
                    if(collides(p_x, 4, p_y, 2, projectiles[i].x, 3, projectiles[i].y, 5)) {
                        gameOver();
                    }
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
}