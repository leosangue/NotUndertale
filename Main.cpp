#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <cstdio>
#include <cmath>
#include <stdexcept>
#include "super-engin.h"
#include <mmsystem.h>
#define input(X) (GetAsyncKeyState(X) & 0x8000)

// Track key states for detecting key release
bool prevKeyStates[256] = {false};
bool keyReleased(int key) {
    bool currentState = input(key);
    bool wasPressed = prevKeyStates[key];
    prevKeyStates[key] = currentState;
    return wasPressed && !currentState; // True if key was pressed last frame but not this frame
}

super_engin Disp("NotUntertale", 9, 21);
AsciiSprite soul(4, 2), spear(3, 5), titleScr(92, 5), overScr(86, 7),hlaser(116,3),hfalsolaser(116,3),vlaser(6,28),vfalsolaser(6,28),winScr(73,7);
AsciiSprite buttons[6] = { AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5), AsciiSprite(20,5) };
POINT projectiles[7];
bool inLevel = false;

int titleScreen();
void runLevel(int level);
// Level-specific entrypoints. Implement level behaviour in these functions.
void runLevel1();
void runLevel2();
void runLevel3();
void runLevel4();
void runLevel5();
void runLevel6();
void gameOver();

// Draw the lives hearts at the top-right of the screen
void DrawLives(int lives) {
    int startX = 120 - (3 * 2) - 1; // three hearts spaced by 2
    for (int li = 0; li < 3; ++li) {
        int hx = startX + li * 2;
        Disp.wDisplay[0*120 + hx].Char.AsciiChar = 3; // heart symbol
        Disp.wDisplay[0*120 + hx].Attributes = (li < lives) ? joinColor(WHITE, RED) : joinColor(WHITE, DARK_GRAY);
    }
}
void gameWin();
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

    {
        const char *s =
        "_|    _|  _|      _|  _|_|_|    _|_|_|_|  _|_|_|    _|_|_|_|_|    _|_|    _|        _|_|_|_|"
        "_|    _|  _|_|    _|  _|    _|  _|        _|    _|      _|      _|    _|  _|        _|      "
        "_|    _|  _|  _|  _|  _|    _|  _|_|_|    _|_|_|        _|      _|_|_|_|  _|        _|_|_|  "
        "_|    _|  _|    _|_|  _|    _|  _|        _|    _|      _|      _|    _|  _|        _|      "
        "  _|_|    _|      _|  _|_|_|    _|_|_|_|  _|    _|      _|      _|    _|  _|_|_|_|  _|_|_|_|";
        size_t len = strlen(s);
        if (len > (size_t)(titleScr.Width * titleScr.Height)) len = titleScr.Width * titleScr.Height;
        memcpy(titleScr.AsciiChars, s, len);
    }
    for(int i = 0; i < 92*5; i++)
        if(titleScr.AsciiChars[i] != ' ') titleScr.Colors[i] = joinColor(WHITE, BLACK);

    {
        const char *s =
        " #################################################################################### "
        "##      ####      ###  ####  ##        #########      ###  ####  ##        ##       ##"
        "#  ########  ####  ##   ##   ##  ##############  ####  ##  ####  ##  ########  ####  #"
        "#  ###   ##  ####  ##        ##      ##########  ####  ###  ##  ###      ####       ##"
        "#  ####  ##        ##  #  #  ##  ##############  ####  ####    ####  ########  ###  ##"
        "##      ###  ####  ##  ####  ##        #########      ######  #####        ##  ####  #"
        " #################################################################################### ";
        size_t len = strlen(s);
        if (len > (size_t)(overScr.Width * overScr.Height)) len = overScr.Width * overScr.Height;
        memcpy(overScr.AsciiChars, s, len);
    }
    {
        const char *s =
        " _______  _______  __    _  _______  ______    _______  _______  _______ "
        "|       ||       ||  |  | ||       ||    _ |  |   _   ||       ||       |"
        "|       ||   _   ||   |_| ||    ___||   | ||  |  |_|  ||_     _||  _____|"
        "|       ||  | |  ||       ||   | __ |   |_||_ |       |  |   |  | |_____ "
        "|      _||  |_|  ||  _    ||   ||  ||    __  ||       |  |   |  |_____  |"
        "|     |_ |       || | |   ||   |_| ||   |  | ||   _   |  |   |   _____| |"
        "|_______||_______||_|  |__||_______||___|  |_||__| |__|  |___|  |_______|";
        size_t len = strlen(s);
        if (len > (size_t)(winScr.Width * winScr.Height)) len = winScr.Width * winScr.Height;
        memcpy(winScr.AsciiChars, s, len);
    }
                                                        
                                                        

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
        // Determine per-button background and foreground colors
        byte bgColor = DARK_GRAY; // default background
        byte fgColor = BLACK;     // default text color
        switch (b) {
            case 0: bgColor = RED;    fgColor = WHITE; break; // first button red
            case 1: bgColor = BLUE;   fgColor = WHITE; break; // second button blue
            case 2: bgColor = GREEN;  fgColor = WHITE; break; // third button green
            case 3: bgColor = PURPLE; fgColor = WHITE; break; // fourth button purple
            case 4: bgColor = GOLD;   fgColor = BLACK; break;  // fifth button yellow (GOLD)
            case 5: /* leave defaults for sixth */ break;
        }
        for (int y = 0; y < bh; ++y) {
            for (int x = 0; x < bw; ++x) {
                int idx = y * bw + x;
                if (y == 0 || y == bh-1 || x == 0 || x == bw-1) {
                    btn.AsciiChars[idx] = border;
                    // Apply colored border if a custom bg was chosen; otherwise keep the default
                    if (b < 5) btn.Colors[idx] = joinColor(bgColor, fgColor);
                    else btn.Colors[idx] = joinColor(WHITE, DARK_GRAY);
                }
                else {
                    // Interior cell gets the button bg/fg if custom, else default background
                    if (b < 5) btn.Colors[idx] = joinColor(bgColor, fgColor);
                    // else default already assigned earlier as joinColor(WHITE, DARK_GRAY)
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
            // Keep label foreground contrast with the chosen background, use message colors for first 5
            if (b < 5) btn.Colors[idx] = joinColor((byte)bgColor, (byte)fgColor);
            else btn.Colors[idx] = joinColor(WHITE, BLACK);
        }
        int leftIdx = labelRow * bw + (startX - 1);
        int rightIdx = labelRow * bw + (startX + llen);
        if (leftIdx >= 0 && leftIdx < bw*bh) { btn.AsciiChars[leftIdx] = '<'; btn.Colors[leftIdx] = joinColor(WHITE, BLACK); }
        if (rightIdx >= 0 && rightIdx < bw*bh) { btn.AsciiChars[rightIdx] = '>'; btn.Colors[rightIdx] = joinColor(WHITE, BLACK); }
    }  

    for(int i=0; i<120*3; i++){
        if(i<116){
            hlaser.AsciiChars[i]=(char)223;
            hlaser.Colors[i]=joinColor(WHITE, CYAN);
        } else if(i<116*2){
            hlaser.AsciiChars[i]=(char)219;
            hlaser.Colors[i]=WHITE;
        } else{
            hlaser.AsciiChars[i]=(char)220;
            hlaser.Colors[i]=joinColor(WHITE, CYAN);
        }
    }
        for(int i=0; i<120*3; i++){
            if(i<116){
                hfalsolaser.AsciiChars[i]=(char)223;
                hfalsolaser.Colors[i]=joinColor(GRAY, DARK_GRAY);
            } else if(i<116*2){
                hfalsolaser.AsciiChars[i]=(char)219;
                hfalsolaser.Colors[i]=GRAY;
            } else{
                hfalsolaser.AsciiChars[i]=(char)220;
                hfalsolaser.Colors[i]=joinColor(GRAY, DARK_GRAY);
            }
        }
        for(int i=0; i<6*28; i++){
            if(i%6==0){
                vlaser.AsciiChars[i]=(char)219;
                vlaser.Colors[i]=joinColor(WHITE, CYAN);
            }
            if(i%6==5){
                vlaser.AsciiChars[i]=(char)219;
                vlaser.Colors[i]=WHITE;
            } else{
                vlaser.AsciiChars[i]=(char)219;
                vlaser.Colors[i]=joinColor(WHITE, CYAN);
            }
        }
        for(int i=0; i<6*28; i++){
            if(i%6==0){
                vfalsolaser.AsciiChars[i]=(char)219;
                vfalsolaser.Colors[i]=joinColor(GRAY, DARK_GRAY);
            }
            if(i%6==5){
                vfalsolaser.AsciiChars[i]=(char)219;
                vfalsolaser.Colors[i]=GRAY;
            } else{
                vfalsolaser.AsciiChars[i]=(char)219;
                vfalsolaser.Colors[i]=joinColor(GRAY, DARK_GRAY);
            }
        }

    
    
    

    // Attempt to play title music if a file exists in assets/title.wav
    if (GetFileAttributesA("assets/title.wav") != INVALID_FILE_ATTRIBUTES) {
        PlaySoundA("assets/title.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }
    while (true) {
        int sel = titleScreen();
        // Reset key state tracking before entering a level
        memset(prevKeyStates, false, sizeof(prevKeyStates));
        runLevel(sel);
        // After returning from the level, (re)play the title music if present
        if (GetFileAttributesA("assets/title.wav") != INVALID_FILE_ATTRIBUTES) {
            PlaySoundA("assets/title.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        }
    }
    
    // Cleanup before exit
    PlaySoundA(NULL, NULL, 0);
    system("cls");
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
            // Start the currently selected level (0..5)
            return selected;
        }
        if (input(VK_ESCAPE)) {
            exit(0);
        }
        Disp.OutPut();  
    }
}


void gameOver() {
    // stop any level music
    PlaySoundA(NULL, NULL, 0);
    // play death sound once if present
    if (GetFileAttributesA("assets/death.wav") != INVALID_FILE_ATTRIBUTES) {
        PlaySoundA("assets/death.wav", NULL, SND_FILENAME | SND_ASYNC);
    }
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

void gameWin() {
    // Duplicate of gameOver but using winScr and victory messages
    PlaySoundA(NULL, NULL, 0);
    if (GetFileAttributesA("assets/victory.wav") != INVALID_FILE_ATTRIBUTES) {
        PlaySoundA("assets/victory.wav", NULL, SND_FILENAME | SND_ASYNC);
    }
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
    Disp.DrawSprite(winScr, 4, 4, false);
    Disp.OutPut();
    Sleep(1000);
    // Use the first three messages as victory messages (rotated at random)
    Disp.PutString("Congratulations, you beat the level", 8, 12, false);
    Disp.wDisplay[12*120+6].Char.AsciiChar = 3;
    int a = rand() % 7;
    Disp.wDisplay[12*120+6].Attributes = joinColor(BLACK, GOLD);
    Disp.wDisplay[14*120+6].Char.AsciiChar = 3;
    Disp.wDisplay[14*120+6].Attributes = joinColor(BLACK, GOLD);
    Disp.PutString("Press enter to continue", 8, 14, false);
    while(!input(VK_RETURN)) Disp.OutPut();
    inLevel = false;
}


// Dispatcher: calls the implementation for the selected level
void runLevel(int level) {
    // stop title music
    PlaySoundA(NULL, NULL, 0);

    // attempt to play level-specific music file: assets/level{n}.wav
    char _levelMusicPath[64];
    snprintf(_levelMusicPath, sizeof(_levelMusicPath), "assets/level%d.wav", level+1);
    if (GetFileAttributesA(_levelMusicPath) != INVALID_FILE_ATTRIBUTES) {
        PlaySoundA(_levelMusicPath, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    }

    switch(level) {
        case 0: runLevel1(); break;
        case 1: runLevel2(); break;
        case 2: runLevel3(); break;
        case 3: runLevel4(); break;
        case 4: runLevel5(); break;
        case 5: runLevel6(); break;
        default: runLevel1(); break;
    }
    // When returning to the title screen, stop level music. Title music will be restarted in main().
    PlaySoundA(NULL, NULL, 0);
}

// --- Level 1 (current gameplay): moved from previous runLevel implementation
void runLevel1() {
    inLevel = true;
    frameCount = 0;
    p_x = 8; p_y = 8;
    int lives = 3;
    int invTimer = 0;
    const int INV_FRAMES = 20;
    for (int i = 0; i < 7; ++i) {
        projectiles[i].x = (rand() % 38) * 3 + 3;
        projectiles[i].y = rand() % 25;
        showspear[i] = false;
    }
    int laser_y= rand()%25+1;
    int laser_x = rand() % 112 + 2; // x position for vertical laser (keep inside screen)
    int laserPrepStart = -1;
    int laserCycleStart = -1; // start of the first laser cycle after prep
    int laserActiveStart = -1;
    while (inLevel) {
        for(int i = 0; i < 120*30; i++) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        // Draw lives at top-right
        DrawLives(lives);
        if (invTimer > 0) invTimer--; 

        Disp.DrawSprite(soul, p_x, p_y, false);
        if(frameCount/50 < 3)
            Disp.PutString(msg[frameCount/50], p_x-3, p_y+3, false);
        else if(frameCount>=550){
            // Begin prep for lasers: show a message for 30 frames, then start laser cycles
            if(laserPrepStart == -1) laserPrepStart = frameCount;
            int prep = frameCount - laserPrepStart;
            if(prep < 30) {
                Disp.PutString("Dodge the lasers", p_x-3, p_y+3, false);
            } else {
                // mark the start of the first laser cycle so cycles always begin with the 'fake' phase
                if(laserCycleStart == -1) laserCycleStart = frameCount;
                int cycle = frameCount - laserCycleStart; // cycle 0..39

                // choose new laser positions at the start of each 40-frame cycle
                if(cycle % 40 == 0) {
                    laser_y = rand()%25+1;
                    laser_x = rand()%112 + 2;
                }

                // draw fading/false horizontal and vertical indicators for the first 15 frames of cycle
                if(cycle % 40 < 15){
                    Disp.DrawSprite(hfalsolaser, 2, laser_y, false);
                    Disp.DrawSprite(vfalsolaser, laser_x, 1, false);
                }

                // after the fake indicator period, draw real lasers and perform collisions
                if(cycle % 40 >= 10){
                    Disp.DrawSprite(hlaser, 2, laser_y, false);
                    Disp.DrawSprite(vlaser, laser_x, 1, false);
                    // mark laser-active start
                    if(laserActiveStart == -1) laserActiveStart = frameCount;
                    // horizontal collision
                    if(collides(p_x,4,p_y,2,2,116,laser_y,3)){
                        if (invTimer == 0) {
                            lives--;
                            invTimer = INV_FRAMES;
                            if (lives <= 0) gameOver();
                        }
                    }
                    // vertical collision (vlaser width 6, height 28 starting at y=1)
                    if(collides(p_x,4,p_y,2,laser_x,6,1,28)){
                        if (invTimer == 0) {
                            lives--;
                            invTimer = INV_FRAMES;
                            if (lives <= 0) gameOver();
                        }
                    }
                    // if lasers have been active for 20 seconds -> player wins
                    if(laserActiveStart != -1 && frameCount - laserActiveStart >= 20 * 20) { // 20s * 20fps
                        gameWin();
                    }
                }
            }

        } else{

            for(int i = 0; i < 7; i++) {
                if(showspear[i]) {
                    if(collides(p_x, 4, p_y, 2, projectiles[i].x, 3, projectiles[i].y, 5)) {
                        if (invTimer == 0) {
                            lives--;
                            invTimer = INV_FRAMES;
                            if (lives <= 0) gameOver();
                        }
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
        }
        frameCount++;

        if(input('A') || input(VK_LEFT)) if(p_x >= 4) p_x-=2;
        if(input('D') || input(VK_RIGHT)) if(p_x <= 112) p_x+=2;
        if(input('W') || input(VK_UP)) if(p_y >= 2) p_y--;
        if(input('S') || input(VK_DOWN)) if(p_y < 27) p_y++;
        if(input('K')) gameWin();
        if(input('J')) gameOver();

        Disp.OutPut();
        Sleep(50);
    }
}

// --- Placeholders for levels 2..6
void runLevel2() {
    // Level 2: Blue-heart physics (Papyrus-like). Blue heart can move left/right and jump due to gravity.
    inLevel = true;
    frameCount = 0;
    // Use the same initial position as the other levels
    p_x = 30; p_y = 24; // start near bottom-center

    int lives = 3;
    int invTimer = 0;
    const int INV_FRAMES = 20;


    byte savedSoulColors[256]; // plenty of room (soul is small)
    for (int i = 0; i < 5*3; ++i) savedSoulColors[i] = soul.Colors[i];


    // Make the soul blue for this level (Blue heart mode)
    for (int i = 0; i < 5*3; ++i) soul.Colors[i] = BLUE;

    // Gravity / jump variables
    float posX = (float)p_x;
    float posY = (float)p_y;
    float velY = 0.0f;
    // Lower and slower jump: weaker gravity and smaller jump velocity
    const float GRAVITY = 0.4f;    // lower gravity to make jumps slower
    const float JUMP_V = -4.0f;   // reduced jump impulse -> lower jump height
    const float MOVE_SPEED = 2.5f; // horizontal speed in units per frame
    const float MAX_FALL_SPEED = 12.0f;
    const int LEFT_LIMIT = 2;
    const int RIGHT_LIMIT = 113;
    const int GROUND_Y = 25; // ground y coordinate (soul.Height==2, so this keeps bottom of sprite within view)

    bool onGround = false;
    bool prevL = false; // previous state of 'L' key (debounce for minigame skip)

    // --- PRE-LEVEL MESSAGE SEQUENCE (3 messages x 2s each)
    const char *preMsgs[3] = { "Now there's gravity", "Use W to jump", "Avoid the Obstacles" };
    const int MSG_FRAMES = 40; // 40 frames = 2 seconds at 20 FPS
    // Run a single pre-message loop that allows movement and jumping while showing messages
    const int PRE_TOTAL_FRAMES = 3 * MSG_FRAMES;
    for (int pf = 0; pf < PRE_TOTAL_FRAMES; ++pf) {
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }

        // Draw lives at top-right
        DrawLives(lives);
        if (invTimer > 0) invTimer--;

        // (hearts drawn once above)

        // Apply physics while messages show (allow movement and jumping)
        velY += GRAVITY;
        if (velY > MAX_FALL_SPEED) velY = MAX_FALL_SPEED;
        posY += velY;
        if (posY >= GROUND_Y) { posY = (float)GROUND_Y; velY = 0.0f; onGround = true; }
        else onGround = false;

        if (input('A') || input(VK_LEFT)) posX -= MOVE_SPEED;
        if (input('D') || input(VK_RIGHT)) posX += MOVE_SPEED;
        if (posX < LEFT_LIMIT) posX = (float)LEFT_LIMIT;
        if (posX > RIGHT_LIMIT) posX = (float)RIGHT_LIMIT;
        if ((input('W') || input(VK_SPACE) || input(VK_UP)) && onGround) { velY = JUMP_V; onGround = false; }

        // Draw the soul at current position
        p_x = (int)posX; p_y = (int)posY; Disp.DrawSprite(soul, p_x, p_y, false);

        // Choose which message to display based on frame
        int m = pf / MSG_FRAMES; if (m < 0) m = 0; if (m > 2) m = 2;
        Disp.PutString(preMsgs[m], p_x-5, p_y+2, false);

        // Allow exiting while the messages show
        if (input(VK_ESCAPE)) exit(0);

        frameCount++;
        Disp.OutPut();
        Sleep(50);
    }

    // --- MINIGAME 1 (Dino-style run): spawn obstacles from the right and move left for 15 seconds
    const int MINIGAME_FRAMES = 15 * 20; // 15 seconds at 20 FPS
    const int MAX_OBSTACLES = 12;
    struct Obstacle { int x; int y; int w; int h; bool active; int dir; float speed; };
    Obstacle obstaclesArr[MAX_OBSTACLES];
    for (int i = 0; i < MAX_OBSTACLES; ++i) { obstaclesArr[i].active = false; obstaclesArr[i].dir = 0; obstaclesArr[i].speed = 0.0f; }
    int spawnCounter = 0;
    const int SPAWN_MIN = 18; // spawn interval minimum (frames)
    const int SPAWN_MAX = 36; // spawn interval maximum (frames)
    const float OBST_SPEED = 1.1f; // base obstacle speed per frame (slower)
    int minigameCounter = 0;

    while (inLevel) {
        // minigame loop continues while minigameCounter < MINIGAME_FRAMES
            if (minigameCounter >= MINIGAME_FRAMES) {
            // minigame passed successfully
            // Clear all minigame 1 obstacles and stop drawing them
            for (int i = 0; i < MAX_OBSTACLES; ++i) obstaclesArr[i].active = false;
            // No completion messages—just begin next minigame
            break;
        }
        // Reset the screen to the off-screen buffer
        for (int i = 0; i < 120 * 30; i++) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        // Draw lives at top-right
        DrawLives(lives);
        if (invTimer > 0) invTimer--;


        // Draw the (now-blue) soul at its current coordinates
        p_x = (int)posX;
        p_y = (int)posY;
        Disp.DrawSprite(soul, p_x, p_y, false);

        // Physics: Apply gravity
        velY += GRAVITY;
        if (velY > MAX_FALL_SPEED) velY = MAX_FALL_SPEED;
        posY += velY;

        // Check ground collision
        if (posY >= GROUND_Y) {
            posY = (float)GROUND_Y;
            velY = 0.0f;
            onGround = true;
        } else {
            onGround = false;
        }

        // Input handling: left/right movement
        if (input('A') || input(VK_LEFT)) {
            posX -= MOVE_SPEED;
        }
        if (input('D') || input(VK_RIGHT)) {
            posX += MOVE_SPEED;
        }

        // Clamp horizontal position
        if (posX < LEFT_LIMIT) posX = (float)LEFT_LIMIT;
        if (posX > RIGHT_LIMIT) posX = (float)RIGHT_LIMIT;

        // Jumping - allowed only when on ground
        if ((input('W') || input(VK_SPACE) || input(VK_UP)) && onGround) {
            velY = JUMP_V;
            onGround = false;
        }

        // Minigame obstacle handling
        // Spawn logic: we spawn a wave that creates obstacles on three lanes equally spaced along the Y axis.
        if (spawnCounter <= 0) {
            // lanes: 3 equally spaced Y positions between a top margin and the ground
            int lanes[3];
            int topMargin = 6; // keep some space from top for HUD
            int bottomY = GROUND_Y; // bottom lane
            int topY = topMargin;   // top lane
            lanes[0] = bottomY;
            lanes[2] = topY;
            lanes[1] = (lanes[0] + lanes[2]) / 2; // middle lane
            // Clamp lanes into visible Y range (1..29)
            for (int li = 0; li < 3; ++li) {
                if (lanes[li] < 1) lanes[li] = 1;
                if (lanes[li] > 29) lanes[li] = 29;
            }

            // Plan waves: spawn 4 obstacles per wave (2 from left->right, 2 from right->left), distributed to lanes.
            // We'll assign each obstacle to a lane so all 3 lanes are used and one lane gets a second obstacle.
            int dirs[4] = { -1, -1, 1, 1 }; // -1 = right->left, 1 = left->right

            // shuffle dirs slightly to vary waves
            for (int s = 0; s < 4; ++s) {
                int tgt = rand() % 4;
                int tmp = dirs[s]; dirs[s] = dirs[tgt]; dirs[tgt] = tmp;
            }

            // assign lanes to each obstacle in order: bottom, middle, top, middle (so 3 lanes used)
            int laneAssignment[4] = { 0, 1, 2, 1 };
            for (int s = 0; s < 4; ++s) {
                // find a free slot
                int slot = -1;
                for (int i = 0; i < MAX_OBSTACLES; ++i) if (!obstaclesArr[i].active) { slot = i; break; }
                if (slot == -1) break; // no more free slots
                int laneIndex = laneAssignment[s];
                int dir = dirs[s];
                obstaclesArr[slot].active = true;
                obstaclesArr[slot].w = 3;
                obstaclesArr[slot].h = 2;
                obstaclesArr[slot].y = lanes[laneIndex];
                obstaclesArr[slot].dir = dir;
                obstaclesArr[slot].speed = OBST_SPEED + ((rand() % 10) / 10.0f); // slight speed variation
                if (dir < 0) {
                    obstaclesArr[slot].x = 119; // spawn at right edge
                } else {
                    obstaclesArr[slot].x = -obstaclesArr[slot].w; // spawn off left edge
                }
            }

            spawnCounter = SPAWN_MIN + (rand() % (SPAWN_MAX - SPAWN_MIN + 1));
        } else {
            spawnCounter--;
        }

        // Move obstacles and draw them
        for (int i = 0; i < MAX_OBSTACLES; ++i) {
            if (!obstaclesArr[i].active) continue;
            // Move according to direction
            obstaclesArr[i].x = (int)(obstaclesArr[i].x + obstaclesArr[i].dir * obstaclesArr[i].speed);
            // Draw simple obstacle as a 2x2 block using '#'
            for (int oy = 0; oy < obstaclesArr[i].h; ++oy) {
                for (int ox = 0; ox < obstaclesArr[i].w; ++ox) {
                    int drawx = obstaclesArr[i].x + ox;
                    int drawy = obstaclesArr[i].y - oy; // place above ground
                    if (drawx >= 0 && drawx < 120 && drawy >= 0 && drawy < 30) {
                        Disp.wDisplay[drawy*120 + drawx].Char.AsciiChar = '#';
                        Disp.wDisplay[drawy*120 + drawx].Attributes = joinColor(WHITE, DARK_GRAY);
                    }
                }
            }
            // Collision: check against player
            if (collides(p_x, 4, p_y, 2, obstaclesArr[i].x, obstaclesArr[i].w, obstaclesArr[i].y - (obstaclesArr[i].h - 1), obstaclesArr[i].h)) {
                if (invTimer == 0) {
                    lives--;
                    invTimer = INV_FRAMES;
                    if (lives <= 0) gameOver();
                }
            }
            // Deactivate if off-screen
            if (obstaclesArr[i].x + obstaclesArr[i].w < 0 || obstaclesArr[i].x > 119) obstaclesArr[i].active = false;
        }

        // Quick debug cheats (same as other levels)
        if (input('K')) gameWin();
        if (input('J')) gameOver();
        bool curL = input('L');
        if (curL && !prevL) { // Skip to next minigame on rising edge
            for (int i = 0; i < MAX_OBSTACLES; ++i) obstaclesArr[i].active = false; // clear obstacles
            break; // exit minigame1 loop to start minigame2
        }
        prevL = curL;

        // Allow exit to OS with escape (consistent with title)
        if (input(VK_ESCAPE)) exit(0);

        frameCount++;
        minigameCounter++;
        Disp.OutPut();
        Sleep(50);
    }

    // If player died inside minigame1, don't continue
    if (!inLevel) {
        for (int i = 0; i < 5*3; ++i) soul.Colors[i] = savedSoulColors[i];
        return;
    }

    // --- MINIGAME 2 (Flappy-style columns): pass through gaps, columns travel right->left
    const int MINIGAME2_FRAMES = 15 * 20; // 15 seconds
    struct Column { int x; int w; int gapY; int gapSize; bool active; float speed; };
    const int MAX_COLUMNS = 8;
    Column columns[MAX_COLUMNS];
    for (int i = 0; i < MAX_COLUMNS; ++i) { columns[i].active = false; columns[i].speed = 0.0f; }
    const int SPAWN_MIN2 = 26; // 1.3s minimal interval
    const int SPAWN_MAX2 = 42; // 2.1s maximal interval
    const float COL_SPEED = 1.2f; // column speed per frame (relatively hard)
    const int COL_WIDTH = 6;
    const int GAP_SIZE = 6; // smaller gap to make it relatively hard
    // Use a fixed max jump height (in rows) instead of computing from physics
    const int FIXED_MAX_JUMP_HEIGHT = 20; // tune this value to adjust how high player can jump
    int maxJumpHeight = FIXED_MAX_JUMP_HEIGHT;
    // Choose the column gap so top of gap is around the maximum jump height from ground
    const int DISPLAY_BOTTOM = 29;
    int fixedGapTop = GROUND_Y - maxJumpHeight;
    if (fixedGapTop < 1) fixedGapTop = 1;
    if (fixedGapTop > DISPLAY_BOTTOM - GAP_SIZE - 1) fixedGapTop = DISPLAY_BOTTOM - GAP_SIZE - 1; // ensure gap below mid point
    int spawnCounter2 = 0;
    int minigame2Counter = 0;
    const int SAFE_TOP_MARGIN = 2;

    // Before starting, do not change player position — preserve player's posX/posY from previous minigame
    while (inLevel) {
        if (minigame2Counter >= MINIGAME2_FRAMES) {
            // Minigame 2 success -> proceed to Minigame 3 (final part of Level 2)
            for (int a = 0; a < 20; ++a) { for (int i = 0; i < 120 * 30; ++i) { Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char; Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color; }  Disp.OutPut(); Sleep(50);}            
            // Clear columns used in minigame2
            for (int i = 0; i < MAX_COLUMNS; ++i) columns[i].active = false;
            break; // exit minigame2 loop and continue to minigame3
        }

        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }

        // Draw lives at top-right and advance invincibility timer
        DrawLives(lives);
        if (invTimer > 0) invTimer--;

        // Physics and input
        velY += GRAVITY; if (velY > MAX_FALL_SPEED) velY = MAX_FALL_SPEED; posY += velY;
        if (posY >= GROUND_Y) { posY = (float)GROUND_Y; velY = 0.0f; onGround = true; } else onGround = false;
        if (input('A') || input(VK_LEFT)) posX -= MOVE_SPEED;
        if (input('D') || input(VK_RIGHT)) posX += MOVE_SPEED;
        if (posX < LEFT_LIMIT) posX = (float)LEFT_LIMIT; if (posX > RIGHT_LIMIT) posX = (float)RIGHT_LIMIT;
        if ((input('W') || input(VK_SPACE) || input(VK_UP)) && onGround) { velY = JUMP_V; onGround = false; }

        p_x = (int)posX; p_y = (int)posY;
        Disp.DrawSprite(soul, p_x, p_y, false);


        // Spawn columns
        if (spawnCounter2 <= 0) {
            // pick a free column slot
            int slot = -1; for (int i = 0; i < MAX_COLUMNS; ++i) if (!columns[i].active) { slot = i; break; }
            if (slot != -1) {
                columns[slot].active = true;
                columns[slot].w = COL_WIDTH;
                columns[slot].speed = COL_SPEED + ((rand() % 20) / 10.0f); // random speed up to +2.0
                columns[slot].x = 119;
                // set a fixed gapY for all columns: fixedGapTop (computed to match player's max jump)
                columns[slot].gapY = fixedGapTop;
                columns[slot].gapSize = GAP_SIZE;
            }
            spawnCounter2 = SPAWN_MIN2 + (rand() % (SPAWN_MAX2 - SPAWN_MIN2 + 1));
        } else spawnCounter2--;

        // Move and draw columns, check collision
        for (int i = 0; i < MAX_COLUMNS; ++i) {
            if (!columns[i].active) continue;
            columns[i].x = (int)(columns[i].x - columns[i].speed);
            // draw top rectangle from y=0 .. gapY-1
            for (int y = 0; y < columns[i].gapY; ++y) {
                for (int x = 0; x < columns[i].w; ++x) {
                    int drawx = columns[i].x + x;
                    if (drawx >= 0 && drawx < 120) {
                        int drawy = y;
                        Disp.wDisplay[drawy * 120 + drawx].Char.AsciiChar = '#';
                        Disp.wDisplay[drawy * 120 + drawx].Attributes = joinColor(WHITE, DARK_GRAY);
                    }
                }
            }
            // draw bottom rectangle from gapY + gapSize .. DISPLAY_BOTTOM
            for (int y = columns[i].gapY + columns[i].gapSize; y <= DISPLAY_BOTTOM; ++y) {
                for (int x = 0; x < columns[i].w; ++x) {
                    int drawx = columns[i].x + x;
                    if (drawx >= 0 && drawx < 120 && y >= 0 && y < 30) {
                        Disp.wDisplay[y * 120 + drawx].Char.AsciiChar = '#';
                        Disp.wDisplay[y * 120 + drawx].Attributes = joinColor(WHITE, DARK_GRAY);
                    }
                }
            }
            // collision check against top rect and bottom rect
            if (columns[i].x + columns[i].w >= 0 && columns[i].x < 120) {
                // top rect: x, w, y1=0..gapY-1
                if (collides(p_x, 4, p_y, 2, columns[i].x, columns[i].w, 0, columns[i].gapY)) {
                    if (invTimer == 0) {
                        lives--;
                        invTimer = INV_FRAMES;
                        if (lives <= 0) gameOver();
                    }
                }
                // bottom rect: x, w, y2 = gapY+gapSize .. bottom
                int bottomH = DISPLAY_BOTTOM - (columns[i].gapY + columns[i].gapSize) + 1;
                if (bottomH > 0 && collides(p_x, 4, p_y, 2, columns[i].x, columns[i].w, columns[i].gapY + columns[i].gapSize, bottomH)) {
                    if (invTimer == 0) {
                        lives--;
                        invTimer = INV_FRAMES;
                        if (lives <= 0) gameOver();
                    }
                }
            }
            if (columns[i].x + columns[i].w < 0) columns[i].active = false; // off screen
        }

        if (input('K')) gameWin();
        if (input('J')) gameOver();
        bool curL = input('L');
        if (curL && !prevL) { // on rising edge skip
            minigame2Counter = MINIGAME2_FRAMES; // force success and exit
            // do not set prevL=false here; let next loop update it
        }
        prevL = curL;
        if (input(VK_ESCAPE)) exit(0);

        frameCount++;
        
        minigame2Counter++;
        Disp.OutPut(); 
        Sleep(50);
    }

    // After Minigame 2 we will progress to Minigame 3 (final challenge)
    // clear any prev columns and reuse variables below
    for (int i = 0; i < MAX_COLUMNS; ++i) columns[i].active = false;

    // --- MINIGAME 3 (Dual-direction columns with opening/closing gaps)
    const int MINIGAME3_FRAMES = 20 * 20; // 20 seconds
    struct Column3 { int x; int w; int baseGapY; int gapSize; bool active; float speed; int dir; int blinkTimer; int blinkOffset; bool isOpen; };
    const int MAX_COLUMNS3 = 3; // reduce columns so the final minigame isn't too hard
    Column3 columns3[MAX_COLUMNS3];
    for (int i = 0; i < MAX_COLUMNS3; ++i) { columns3[i].active = false; columns3[i].speed = 0.0f; columns3[i].blinkTimer = 0; columns3[i].isOpen = true; columns3[i].blinkOffset = 0; }
    const int SPAWN_MIN3 = 34; // increase minimal interval to reduce density
    const int SPAWN_MAX3 = 60; // increase maximal interval to reduce density
    const int COL_WIDTH3 = 6;
    const int GAP_SIZE3 = GAP_SIZE; // same gap size for lower difficulty
    const int COL_SPEED3 = 1; // fixed speed for all columns
    int spawnCounter3 = 0;
    int minigame3Counter = 0;

    // Keep player's pos (don't reset) - only change background
    while (inLevel) {
        if (minigame3Counter >= MINIGAME3_FRAMES) {
            // success -> level complete
            for (int a = 0; a < 20; ++a) { for (int i = 0; i < 120 * 30; ++i) { Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char; Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color; }  Disp.OutPut(); }            
            // restore soul colors and victory
            for (int i = 0; i < 5*3; ++i) soul.Colors[i] = savedSoulColors[i];
            gameWin();
            return;
        }

        // Basic screen reset
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }

        // Draw lives and progress invincibility timer
        DrawLives(lives);
        if (invTimer > 0) invTimer--;

        // Spawn columns3 from either left or right
        if (spawnCounter3 <= 0) {
            int slot = -1; for (int i = 0; i < MAX_COLUMNS3; ++i) if (!columns3[i].active) { slot = i; break; }
            if (slot != -1) {
                columns3[slot].active = true;
                columns3[slot].w = COL_WIDTH3;
                columns3[slot].speed = (float)COL_SPEED3;
                columns3[slot].dir = (rand() % 2 == 0) ? -1 : 1; // -1: right->left, 1: left->right
                columns3[slot].baseGapY = fixedGapTop; // same base gap as minigame 2
                columns3[slot].gapSize = GAP_SIZE3;
                columns3[slot].blinkOffset = rand() % 20; // start offset for blinking
                columns3[slot].isOpen = (rand() % 2 == 0);
                // set start x depending on direction
                if (columns3[slot].dir == -1) columns3[slot].x = 119; else columns3[slot].x = -columns3[slot].w;
            }
            spawnCounter3 = SPAWN_MIN3 + (rand() % (SPAWN_MAX3 - SPAWN_MIN3 + 1));
        } else spawnCounter3--;

        // Update player physics and draw player
        velY += GRAVITY; if (velY > MAX_FALL_SPEED) velY = MAX_FALL_SPEED; posY += velY;
        if (posY >= GROUND_Y) { posY = (float)GROUND_Y; velY = 0.0f; onGround = true; } else onGround = false;
        if (input('A') || input(VK_LEFT)) posX -= MOVE_SPEED; if (input('D') || input(VK_RIGHT)) posX += MOVE_SPEED;
        if (posX < LEFT_LIMIT) posX = (float)LEFT_LIMIT; if (posX > RIGHT_LIMIT) posX = (float)RIGHT_LIMIT;
        if ((input('W') || input(VK_SPACE) || input(VK_UP)) && onGround) { velY = JUMP_V; onGround = false; }
        p_x = (int)posX; p_y = (int)posY; Disp.DrawSprite(soul, p_x, p_y, false);

        // Move and draw columns3 and check collisions
        for (int i = 0; i < MAX_COLUMNS3; ++i) {
            if (!columns3[i].active) continue;
            columns3[i].x = (int)(columns3[i].x + columns3[i].dir * columns3[i].speed);
            // update blink state
            const int BLINK_FRAMES = 40; // toggle every ~1 second (20 fps)
            columns3[i].blinkTimer = (columns3[i].blinkTimer + 1) % (BLINK_FRAMES);
            // offset the blink timing per column so they don't all perfectly sync
            int effectiveTimer = (columns3[i].blinkTimer + columns3[i].blinkOffset) % BLINK_FRAMES;
            bool blinkOpen = (effectiveTimer < (BLINK_FRAMES / 2));
            columns3[i].isOpen = blinkOpen;
            int dynGapY = columns3[i].baseGapY;
            if (dynGapY < 1) dynGapY = 1;
            if (dynGapY > DISPLAY_BOTTOM - columns3[i].gapSize - 1) dynGapY = DISPLAY_BOTTOM - columns3[i].gapSize - 1;
            // draw top rect (and bottom) if open, otherwise draw a solid column while closed
            if (columns3[i].isOpen) {
            for (int y = 0; y < dynGapY; ++y) {
                for (int x = 0; x < columns3[i].w; ++x) {
                    int drawx = columns3[i].x + x;
                    if (drawx >= 0 && drawx < 120) {
                        int drawy = y;
                        Disp.wDisplay[drawy * 120 + drawx].Char.AsciiChar = '#';
                        Disp.wDisplay[drawy * 120 + drawx].Attributes = joinColor(WHITE, DARK_GRAY);
                    }
                }
            }
            // draw bottom rect
            for (int y = dynGapY + columns3[i].gapSize; y <= DISPLAY_BOTTOM; ++y) {
                for (int x = 0; x < columns3[i].w; ++x) {
                    int drawx = columns3[i].x + x;
                    if (drawx >= 0 && drawx < 120 && y >= 0 && y < 30) {
                        Disp.wDisplay[y * 120 + drawx].Char.AsciiChar = '#';
                        Disp.wDisplay[y * 120 + drawx].Attributes = joinColor(WHITE, DARK_GRAY);
                    }
                }
            }
            } else {
                // closed: draw solid column covering top..bottom
                for (int y = 0; y <= DISPLAY_BOTTOM; ++y) {
                    for (int x = 0; x < columns3[i].w; ++x) {
                        int drawx = columns3[i].x + x;
                        if (drawx >= 0 && drawx < 120 && y >= 0 && y < 30) {
                            Disp.wDisplay[y * 120 + drawx].Char.AsciiChar = '#';
                            Disp.wDisplay[y * 120 + drawx].Attributes = joinColor(WHITE, WHITE);
                        }
                    }
                }
            }
            // collision check using dynamic gap
            if (columns3[i].x + columns3[i].w >= 0 && columns3[i].x < 120) {
                if (!columns3[i].isOpen) {
                    // closed: treat as full column (no gap)
                    if (collides(p_x, 4, p_y, 2, columns3[i].x, columns3[i].w, 0, DISPLAY_BOTTOM+1)) {
                        if (invTimer == 0) {
                            lives--; invTimer = INV_FRAMES; if (lives <= 0) { for (int a = 0; a < 5*3; ++a) soul.Colors[a] = savedSoulColors[a]; gameOver(); }
                        }
                    }
                } else {
                    // open: normal gap collisions
                    if (collides(p_x, 4, p_y, 2, columns3[i].x, columns3[i].w, 0, dynGapY)) {
                    if (invTimer == 0) {
                        lives--; invTimer = INV_FRAMES; if (lives <= 0) { for (int a = 0; a < 5*3; ++a) soul.Colors[a] = savedSoulColors[a]; gameOver(); }
                    }
                }
                    int bottomH = DISPLAY_BOTTOM - (dynGapY + columns3[i].gapSize) + 1;
                    if (bottomH > 0 && collides(p_x, 4, p_y, 2, columns3[i].x, columns3[i].w, dynGapY + columns3[i].gapSize, bottomH)) {
                    if (invTimer == 0) {
                        lives--; invTimer = INV_FRAMES; if (lives <= 0) { for (int a = 0; a < 5*3; ++a) soul.Colors[a] = savedSoulColors[a]; gameOver(); }
                    }
                    }
                }
            }
            if (columns3[i].dir == -1 && columns3[i].x + columns3[i].w < 0) columns3[i].active = false;
            if (columns3[i].dir == 1 && columns3[i].x > 119) columns3[i].active = false;
        }

        // Allow debug K/J and skip L, and exit on ESC
        if (input('K')) gameWin();
        if (input('J')) gameOver();
        bool curL = input('L');
        if (curL && !prevL) { // on rising edge skip
            minigame3Counter = MINIGAME3_FRAMES; // force success
        }
        prevL = curL;
        if (input(VK_ESCAPE)) exit(0);

        frameCount++; minigame3Counter++; Disp.OutPut(); Sleep(50);
    }

    // End of runLevel2 - restore colors if not already restored
    for (int i = 0; i < 5*3; ++i) soul.Colors[i] = savedSoulColors[i];
    return;
}

void runLevel3() {
    inLevel = true;
    frameCount = 0;
    
    // Player position: fixed at center
    p_x = 58; p_y = 14;
    
    int lives = 3;
    int invTimer = 0;
    const int INV_FRAMES = 20;
    
    // Create green soul sprite for this level
    AsciiSprite greenSoul = soul;
    byte savedSoulColors[256];
    for (int i = 0; i < 4*2; ++i) {
        savedSoulColors[i] = soul.Colors[i];
        greenSoul.Colors[i] = GREEN;
    }
    
    // Messages for Level 3 minigame 1
    const char *level3Msgs[2] = {
        "Use WASD to move the Shield",
        "Protect yourself from arrows"
    };
    
    // --- MINIGAME 1 (Shield defense): 15 seconds ---
    const int MINIGAME1_FRAMES = 15 * 20; // 15 seconds at 20 FPS
    const int MAX_ARROWS = 24;
    struct Arrow { float x, y; int dir; float speed; bool active; };
    Arrow arrows[MAX_ARROWS];
    for (int i = 0; i < MAX_ARROWS; ++i) { arrows[i].active = false; arrows[i].speed = 0.0f; }
    
    int spawnCounter = 0;
    const int SPAWN_MIN = 8;   // spawn every 0.4s minimum
    const int SPAWN_MAX = 18;  // spawn every 0.9s maximum
    const float VERTICAL_MIN_SPEED = 0.6f;
    const float VERTICAL_MAX_SPEED = 0.8f;
    const float HORIZONTAL_MIN_SPEED = 1.2f;
    const float HORIZONTAL_MAX_SPEED = 1.6f;
    
    // Shield state: 0=none, 1=up, 2=down, 3=left, 4=right
    int shieldDir = 0;
    int shieldX = p_x, shieldY = p_y;
    
    int minigame1Counter = 0;
    
    // --- PRE-MINIGAME MESSAGE SEQUENCE (3 messages x 2s each = 6 seconds total)
    const int MSG_FRAMES = 40; // 40 frames = 2 seconds at 20 FPS
    const int PRE_TOTAL_FRAMES = 3 * MSG_FRAMES;
    shieldDir = 0; // Initialize shield direction
    for (int pf = 0; pf < PRE_TOTAL_FRAMES; ++pf) {
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        // Draw lives at top-right
        DrawLives(lives);
        
        // Draw green soul at fixed center position
        Disp.DrawSprite(greenSoul, p_x, p_y, false);
        
        // Handle shield input (WASD) during messages
        shieldDir = 0; // reset each frame
        if (input('W') || input(VK_UP)) shieldDir = 1;      // up
        else if (input('S') || input(VK_DOWN)) shieldDir = 2; // down
        else if (input('A') || input(VK_LEFT)) shieldDir = 3; // left
        else if (input('D') || input(VK_RIGHT)) shieldDir = 4; // right
        
        // Draw shield if active (accounting for player dimensions: 4 wide, 2 tall)
        if (shieldDir > 0) {
            if (shieldDir == 1) {
                // Up: horizontal line above player (centered at player x, above player y)
                // Player is 4 wide, so shield extends from p_x to p_x+3, with 1 char separation
                for (int sx = 0; sx < 4; ++sx) {
                    int drawX = p_x + sx;
                    int drawY = p_y - 2; // 1 char separation above player
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '-';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir == 2) {
                // Down: horizontal line below player (player is 2 tall, so shield is at p_y+2 with 1 char separation)
                for (int sx = 0; sx < 4; ++sx) {
                    int drawX = p_x + sx;
                    int drawY = p_y + 3; // 1 char separation below player
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '-';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir == 3) {
                // Left: vertical line to the left of player (player is 2 tall, with 1 char separation)
                for (int sy = 0; sy < 2; ++sy) {
                    int drawX = p_x - 2; // 1 char separation to the left
                    int drawY = p_y + sy;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '|';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir == 4) {
                // Right: vertical line to the right of player (player is 2 tall, 4 wide, with 1 char separation)
                for (int sy = 0; sy < 2; ++sy) {
                    int drawX = p_x + 5; // 1 char separation to the right
                    int drawY = p_y + sy;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '|';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            }
        }
        
        // Choose which message to display based on frame
        int m = pf / MSG_FRAMES; if (m < 0) m = 0; if (m > 1) m = 1;
        Disp.PutString(level3Msgs[m], p_x - 7, p_y + 2, false);
        
        // Allow exiting while the messages show
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        Disp.OutPut();
        Sleep(50);
    }
    
    while (inLevel) {
        if (minigame1Counter >= MINIGAME1_FRAMES) {
            // Minigame 1 completed
            break;
        }
        
        // Reset screen
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        // Draw lives
        DrawLives(lives);
        
        if (invTimer > 0) invTimer--;
        
        // Draw green soul at fixed center position
        Disp.DrawSprite(greenSoul, p_x, p_y, false);
        
        // Handle shield input (WASD)
        shieldDir = 0; // reset each frame
        if (input('W') || input(VK_UP)) shieldDir = 1;      // up
        else if (input('S') || input(VK_DOWN)) shieldDir = 2; // down
        else if (input('A') || input(VK_LEFT)) shieldDir = 3; // left
        else if (input('D') || input(VK_RIGHT)) shieldDir = 4; // right
        
        // Draw shield if active (accounting for player dimensions: 4 wide, 2 tall)
        if (shieldDir > 0) {
            if (shieldDir == 1) {
                // Up: horizontal line above player (centered at player x, above player y)
                // Player is 4 wide, so shield extends from p_x to p_x+3, with 1 char separation
                for (int sx = 0; sx < 4; ++sx) {
                    int drawX = p_x + sx;
                    int drawY = p_y - 2; // 1 char separation above player
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '-';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir == 2) {
                // Down: horizontal line below player (player is 2 tall, so shield is at p_y+2 with 1 char separation)
                for (int sx = 0; sx < 4; ++sx) {
                    int drawX = p_x + sx;
                    int drawY = p_y + 3; // 1 char separation below player
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '-';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir == 3) {
                // Left: vertical line to the left of player (player is 2 tall, with 1 char separation)
                for (int sy = 0; sy < 2; ++sy) {
                    int drawX = p_x - 2; // 1 char separation to the left
                    int drawY = p_y + sy;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '|';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir == 4) {
                // Right: vertical line to the right of player (player is 2 tall, 4 wide, with 1 char separation)
                for (int sy = 0; sy < 2; ++sy) {
                    int drawX = p_x + 5; // 1 char separation to the right
                    int drawY = p_y + sy;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '|';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            }
        }
        
        // Spawn arrows randomly
        if (spawnCounter <= 0) {
            // Find free arrow slot
            int slot = -1;
            for (int i = 0; i < MAX_ARROWS; ++i) {
                if (!arrows[i].active) { slot = i; break; }
            }
            
            if (slot != -1) {
                arrows[slot].active = true;
                
                // Choose random direction: 0=from top, 1=from bottom, 2=from left, 3=from right
                int spawnDir = rand() % 4;
                arrows[slot].dir = spawnDir;
                
                // Assign speed based on direction (vertical: 0,1 or horizontal: 2,3)
                if (spawnDir == 0 || spawnDir == 1) {
                    // Vertical arrows (from top or bottom)
                    arrows[slot].speed = VERTICAL_MIN_SPEED + ((rand() % 100) / 100.0f) * (VERTICAL_MAX_SPEED - VERTICAL_MIN_SPEED);
                } else {
                    // Horizontal arrows (from left or right)
                    arrows[slot].speed = HORIZONTAL_MIN_SPEED + ((rand() % 100) / 100.0f) * (HORIZONTAL_MAX_SPEED - HORIZONTAL_MIN_SPEED);
                }
                
                if (spawnDir == 0) {
                    // From top at (x/2, 0) moving down - spawn at center-x, top
                    arrows[slot].x = 60.0f; // 120 / 2
                    arrows[slot].y = -1.0f;
                } else if (spawnDir == 1) {
                    // From bottom at (x/2, y) moving up - spawn at center-x, bottom
                    arrows[slot].x = 60.0f; // 120 / 2
                    arrows[slot].y = 30.0f;
                } else if (spawnDir == 2) {
                    // From left at (0, y/2) moving right - spawn at left, center-y
                    arrows[slot].x = -1.0f;
                    arrows[slot].y = 15.0f; // 30 / 2
                } else {
                    // From right at (x, y/2) moving left - spawn at right, center-y
                    arrows[slot].x = 121.0f;
                    arrows[slot].y = 15.0f; // 30 / 2
                }
            }
            
            spawnCounter = SPAWN_MIN + (rand() % (SPAWN_MAX - SPAWN_MIN + 1));
        } else {
            spawnCounter--;
        }
        
        // Move arrows and check collisions
        for (int i = 0; i < MAX_ARROWS; ++i) {
            if (!arrows[i].active) continue;
            
            // Move arrow
            if (arrows[i].dir == 0) {
                // From top, moving down
                arrows[i].y += arrows[i].speed;
            } else if (arrows[i].dir == 1) {
                // From bottom, moving up
                arrows[i].y -= arrows[i].speed;
            } else if (arrows[i].dir == 2) {
                // From left, moving right
                arrows[i].x += arrows[i].speed;
            } else {
                // From right, moving left
                arrows[i].x -= arrows[i].speed;
            }
            
            // Check shield collision (if shield is active) - using float positions for accuracy
            bool hitShield = false;
            if (shieldDir == 1) {
                // Shield up: horizontal line at (p_x..p_x+3, p_y-2)
                // Generous collision to catch 2-wide arrows
                if (arrows[i].y >= p_y - 2.7f && arrows[i].y <= p_y - 1.3f &&
                    arrows[i].x >= p_x - 1.0f && arrows[i].x <= p_x + 4.0f) {
                    hitShield = true;
                }
            } else if (shieldDir == 2) {
                // Shield down: horizontal line at (p_x..p_x+3, p_y+3)
                if (arrows[i].y >= p_y + 2.3f && arrows[i].y <= p_y + 3.7f &&
                    arrows[i].x >= p_x - 1.0f && arrows[i].x <= p_x + 4.0f) {
                    hitShield = true;
                }
            } else if (shieldDir == 3) {
                // Shield left: vertical line at (p_x-2, p_y..p_y+1)
                // Generous collision to catch 2-tall arrows
                if (arrows[i].x >= p_x - 2.7f && arrows[i].x <= p_x - 1.3f &&
                    arrows[i].y >= p_y - 1.0f && arrows[i].y <= p_y + 2.0f) {
                    hitShield = true;
                }
            } else if (shieldDir == 4) {
                // Shield right: vertical line at (p_x+5, p_y..p_y+1)
                // Generous collision to catch 2-tall arrows
                if (arrows[i].x >= p_x + 4.3f && arrows[i].x <= p_x + 5.7f &&
                    arrows[i].y >= p_y - 1.0f && arrows[i].y <= p_y + 2.0f) {
                    hitShield = true;
                }
            }
            
            // Check player collision (if arrow wasn't blocked by shield)
            // Player dimensions: 4 wide, 2 tall at (p_x, p_y)
            // Arrow dimensions: 2x1 for horizontal, 1x2 for vertical
            // Use float-based collision for accuracy
            bool hitPlayer = false;
            
            if (arrows[i].dir == 0 || arrows[i].dir == 1) {
                // Vertical arrow: 1 wide, 2 tall (spans from x to x+0.9, y to y+1.9)
                if (arrows[i].x + 0.9f >= p_x && arrows[i].x <= p_x + 3.9f &&
                    arrows[i].y + 1.9f >= p_y && arrows[i].y <= p_y + 1.9f) {
                    hitPlayer = true;
                }
            } else {
                // Horizontal arrow: 2 wide, 1 tall (spans from x to x+1.9, y to y+0.9)
                if (arrows[i].x + 1.9f >= p_x && arrows[i].x <= p_x + 3.9f &&
                    arrows[i].y + 0.9f >= p_y && arrows[i].y <= p_y + 1.9f) {
                    hitPlayer = true;
                }
            }
            
            if (!hitShield && hitPlayer) {
                if (invTimer == 0) {
                    lives--;
                    invTimer = INV_FRAMES;
                    if (lives <= 0) {
                        for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
                        gameOver();
                        return;
                    }
                }
                hitShield = true; // treat hit as blocked
            }
            
            // Remove arrow if hit or off-screen
            if (hitShield) {
                arrows[i].active = false;
                continue; // Skip drawing this arrow
            } else if (arrows[i].x < -5 || arrows[i].x > 125 || arrows[i].y < -5 || arrows[i].y > 35) {
                arrows[i].active = false;
                continue; // Skip drawing this arrow
            }
            
            // Draw arrow ONLY if it wasn't removed (represented as 2 characters - 2x1 for horizontal, 1x2 for vertical)
            int arrowX = (int)arrows[i].x;
            int arrowY = (int)arrows[i].y;
            char arrowChar = '>';
            if (arrows[i].dir == 0) arrowChar = 'v'; // down
            else if (arrows[i].dir == 1) arrowChar = '^'; // up
            else if (arrows[i].dir == 2) arrowChar = '>';  // right
            else arrowChar = '<'; // left
            
            // Draw arrow with size based on direction
            if (arrows[i].dir == 0 || arrows[i].dir == 1) {
                // Vertical arrows: draw 1x2 (top and bottom)
                if (arrowX >= 0 && arrowX < 120 && arrowY >= 0 && arrowY < 30) {
                    Disp.wDisplay[arrowY * 120 + arrowX].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[arrowY * 120 + arrowX].Attributes = joinColor(BLACK, ORANGE);
                }
                if (arrowX >= 0 && arrowX < 120 && arrowY + 1 >= 0 && arrowY + 1 < 30) {
                    Disp.wDisplay[(arrowY + 1) * 120 + arrowX].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[(arrowY + 1) * 120 + arrowX].Attributes = joinColor(BLACK, ORANGE);
                }
            } else {
                // Horizontal arrows: draw 2x1 (left and right)
                if (arrowX >= 0 && arrowX < 120 && arrowY >= 0 && arrowY < 30) {
                    Disp.wDisplay[arrowY * 120 + arrowX].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[arrowY * 120 + arrowX].Attributes = joinColor(BLACK, ORANGE);
                }
                if (arrowX + 1 >= 0 && arrowX + 1 < 120 && arrowY >= 0 && arrowY < 30) {
                    Disp.wDisplay[arrowY * 120 + arrowX + 1].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[arrowY * 120 + arrowX + 1].Attributes = joinColor(BLACK, ORANGE);
                }
            }
        }
        
        // Debug keys
        if (input('K')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameWin();
            return;
        }
        if (input('J')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameOver();
            return;
        }
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        minigame1Counter++;
        Disp.OutPut();
        Sleep(50);
    }
    
    // Restore soul colors
    for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
    
    // If player died during minigame 1, don't continue
    if (!inLevel) return;
    
    // --- MINIGAME 2 (Moving dodge with multi-directional arrows): 15 seconds ---
    const int MINIGAME2_FRAMES = 15 * 20; // 15 seconds at 20 FPS
    const int MAX_ARROWS2 = 36;
    struct Arrow2 { float x, y; int dir; float speed; bool active; };
    Arrow2 arrows2[MAX_ARROWS2];
    for (int i = 0; i < MAX_ARROWS2; ++i) { arrows2[i].active = false; arrows2[i].speed = 0.0f; }
    
    // Change soul back to green and allow movement
    for (int i = 0; i < 4*2; ++i) greenSoul.Colors[i] = GREEN;
    
    // Player position for minigame 2 (Level 1 style movement - no gravity)
    p_x = 58; p_y = 14;
    const int MOVE_LIMIT_LEFT = 4, MOVE_LIMIT_RIGHT = 112, MOVE_LIMIT_UP = 2, MOVE_LIMIT_DOWN = 27;
    
    int spawn2Counter = 0;
    const int SPAWN_MIN2 = 6, SPAWN_MAX2 = 12;
    const float ARROW_SPEED2_MIN = 1.5f, ARROW_SPEED2_MAX = 2.5f;
    int minigame2Counter = 0;
    
    while (inLevel) {
        if (minigame2Counter >= MINIGAME2_FRAMES) {
            break;
        }
        
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        DrawLives(lives);
        if (invTimer > 0) invTimer--;
        
        // Level 1 style movement (simple WASD, no gravity)
        if (input('A') || input(VK_LEFT)) if (p_x >= MOVE_LIMIT_LEFT) p_x -= 2;
        if (input('D') || input(VK_RIGHT)) if (p_x <= MOVE_LIMIT_RIGHT) p_x += 2;
        if (input('W') || input(VK_UP)) if (p_y >= MOVE_LIMIT_UP) p_y--;
        if (input('S') || input(VK_DOWN)) if (p_y < MOVE_LIMIT_DOWN) p_y++;
        
        Disp.DrawSprite(greenSoul, p_x, p_y, false);
        
        // Spawn arrows for minigame 2
        if (spawn2Counter <= 0) {
            int slot = -1;
            for (int i = 0; i < MAX_ARROWS2; ++i) if (!arrows2[i].active) { slot = i; break; }
            
            if (slot != -1) {
                arrows2[slot].active = true;
                arrows2[slot].speed = ARROW_SPEED2_MIN + ((rand() % 100) / 100.0f) * (ARROW_SPEED2_MAX - ARROW_SPEED2_MIN);
                int spawnDir = rand() % 4;
                arrows2[slot].dir = spawnDir;
                
                if (spawnDir == 0) {
                    arrows2[slot].x = (float)(2 + rand() % 116);
                    arrows2[slot].y = -2.0f;
                } else if (spawnDir == 1) {
                    arrows2[slot].x = (float)(2 + rand() % 116);
                    arrows2[slot].y = 30.0f;
                } else if (spawnDir == 2) {
                    arrows2[slot].x = -2.0f;
                    arrows2[slot].y = (float)(2 + rand() % 26);
                } else {
                    arrows2[slot].x = 122.0f;
                    arrows2[slot].y = (float)(2 + rand() % 26);
                }
            }
            spawn2Counter = SPAWN_MIN2 + (rand() % (SPAWN_MAX2 - SPAWN_MIN2 + 1));
        } else {
            spawn2Counter--;
        }
        
        // Move and draw arrows for minigame 2
        for (int i = 0; i < MAX_ARROWS2; ++i) {
            if (!arrows2[i].active) continue;
            
            if (arrows2[i].dir == 0) arrows2[i].y += arrows2[i].speed;
            else if (arrows2[i].dir == 1) arrows2[i].y -= arrows2[i].speed;
            else if (arrows2[i].dir == 2) arrows2[i].x += arrows2[i].speed;
            else arrows2[i].x -= arrows2[i].speed;
            
            int arrowX = (int)arrows2[i].x;
            int arrowY = (int)arrows2[i].y;
            char arrowChar = '>';
            if (arrows2[i].dir == 0) arrowChar = 'v';
            else if (arrows2[i].dir == 1) arrowChar = '^';
            else if (arrows2[i].dir == 2) arrowChar = '>';
            else arrowChar = '<';
            
            // Draw 1x3 or 3x1 arrows
            if (arrows2[i].dir == 0 || arrows2[i].dir == 1) {
                // Vertical: 1x3
                for (int dy = 0; dy < 3; ++dy) {
                    if (arrowX >= 0 && arrowX < 120 && arrowY + dy >= 0 && arrowY + dy < 30) {
                        Disp.wDisplay[(arrowY + dy) * 120 + arrowX].Char.AsciiChar = arrowChar;
                        Disp.wDisplay[(arrowY + dy) * 120 + arrowX].Attributes = joinColor(BLACK, ORANGE);
                    }
                }
            } else {
                // Horizontal: 3x1
                for (int dx = 0; dx < 3; ++dx) {
                    if (arrowX + dx >= 0 && arrowX + dx < 120 && arrowY >= 0 && arrowY < 30) {
                        Disp.wDisplay[arrowY * 120 + arrowX + dx].Char.AsciiChar = arrowChar;
                        Disp.wDisplay[arrowY * 120 + arrowX + dx].Attributes = joinColor(BLACK, ORANGE);
                    }
                }
            }
            
            // Collision check
            bool hitArrow = false;
            if (arrows2[i].dir == 0 || arrows2[i].dir == 1) {
                if (arrows2[i].x + 0.9f >= p_x && arrows2[i].x <= p_x + 3.9f &&
                    arrows2[i].y + 2.9f >= p_y && arrows2[i].y <= p_y + 1.9f) {
                    hitArrow = true;
                }
            } else {
                if (arrows2[i].x + 2.9f >= p_x && arrows2[i].x <= p_x + 3.9f &&
                    arrows2[i].y + 0.9f >= p_y && arrows2[i].y <= p_y + 1.9f) {
                    hitArrow = true;
                }
            }
            
            if (hitArrow) {
                if (invTimer == 0) {
                    lives--;
                    invTimer = INV_FRAMES;
                    if (lives <= 0) {
                        for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
                        gameOver();
                        return;
                    }
                }
                arrows2[i].active = false;
            } else if (arrowX < -5 || arrowX > 125 || arrowY < -5 || arrowY > 35) {
                arrows2[i].active = false;
            }
        }
        
        if (input('K')) { for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a]; gameWin(); return; }
        if (input('J')) { for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a]; gameOver(); return; }
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        minigame2Counter++;
        Disp.OutPut();
        Sleep(50);
    }
    
    if (!inLevel) return;
    
    // --- MINIGAME 3 (Shield with inverted controls): 10 seconds ---
    const int MINIGAME3_FRAMES = 10 * 20; // 10 seconds at 20 FPS
    const int MAX_ARROWS3 = 18;
    struct Arrow3 { float x, y; int dir; float speed; bool active; };
    Arrow3 arrows3[MAX_ARROWS3];
    for (int i = 0; i < MAX_ARROWS3; ++i) { arrows3[i].active = false; arrows3[i].speed = 0.0f; }
    
    p_x = 58; p_y = 14;
    
    // Show "Be careful" message
    for (int i = 0; i < 120 * 30; ++i) {
        Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
        Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
    }
    DrawLives(lives);
    Disp.DrawSprite(greenSoul, p_x, p_y, false);
    Disp.PutString("Be careful", p_x - 3, p_y + 3, false);
    Disp.OutPut();
    Sleep(1500);
    
    int shieldDir3 = 0;
    int spawn3Counter = 0;
    const int SPAWN_MIN3 = 8, SPAWN_MAX3 = 18;
    int minigame3Counter = 0;
    
    while (inLevel) {
        if (minigame3Counter >= MINIGAME3_FRAMES) {
            break;
        }
        
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        DrawLives(lives);
        if (invTimer > 0) invTimer--;
        
        Disp.DrawSprite(greenSoul, p_x, p_y, false);
        
        // Handle inverted shield input
        shieldDir3 = 0;
        if (input('W') || input(VK_UP)) shieldDir3 = 2;      // W = down (inverted)
        else if (input('S') || input(VK_DOWN)) shieldDir3 = 1; // S = up (inverted)
        else if (input('A') || input(VK_LEFT)) shieldDir3 = 4; // A = right (inverted)
        else if (input('D') || input(VK_RIGHT)) shieldDir3 = 3; // D = left (inverted)
        
        // Draw inverted shield
        if (shieldDir3 > 0) {
            if (shieldDir3 == 1) {
                for (int sx = 0; sx < 4; ++sx) {
                    int drawX = p_x + sx;
                    int drawY = p_y - 2;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '-';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir3 == 2) {
                for (int sx = 0; sx < 4; ++sx) {
                    int drawX = p_x + sx;
                    int drawY = p_y + 3;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '-';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir3 == 3) {
                for (int sy = 0; sy < 2; ++sy) {
                    int drawX = p_x - 2;
                    int drawY = p_y + sy;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '|';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            } else if (shieldDir3 == 4) {
                for (int sy = 0; sy < 2; ++sy) {
                    int drawX = p_x + 5;
                    int drawY = p_y + sy;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '|';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(BLACK, GOLD);
                    }
                }
            }
        }
        
        // Spawn arrows for minigame 3
        if (spawn3Counter <= 0) {
            int slot = -1;
            for (int i = 0; i < MAX_ARROWS3; ++i) if (!arrows3[i].active) { slot = i; break; }
            
            if (slot != -1) {
                arrows3[slot].active = true;
                int spawnDir = rand() % 4;
                arrows3[slot].dir = spawnDir;
                
                if (spawnDir == 0 || spawnDir == 1) {
                    arrows3[slot].speed = VERTICAL_MIN_SPEED + ((rand() % 100) / 100.0f) * (VERTICAL_MAX_SPEED - VERTICAL_MIN_SPEED);
                } else {
                    arrows3[slot].speed = HORIZONTAL_MIN_SPEED + ((rand() % 100) / 100.0f) * (HORIZONTAL_MAX_SPEED - HORIZONTAL_MIN_SPEED);
                }
                
                if (spawnDir == 0) {
                    arrows3[slot].x = 60.0f;
                    arrows3[slot].y = -1.0f;
                } else if (spawnDir == 1) {
                    arrows3[slot].x = 60.0f;
                    arrows3[slot].y = 30.0f;
                } else if (spawnDir == 2) {
                    arrows3[slot].x = -1.0f;
                    arrows3[slot].y = 15.0f;
                } else {
                    arrows3[slot].x = 121.0f;
                    arrows3[slot].y = 15.0f;
                }
            }
            spawn3Counter = SPAWN_MIN3 + (rand() % (SPAWN_MAX3 - SPAWN_MIN3 + 1));
        } else {
            spawn3Counter--;
        }
        
        // Move and draw arrows for minigame 3
        for (int i = 0; i < MAX_ARROWS3; ++i) {
            if (!arrows3[i].active) continue;
            
            if (arrows3[i].dir == 0) arrows3[i].y += arrows3[i].speed;
            else if (arrows3[i].dir == 1) arrows3[i].y -= arrows3[i].speed;
            else if (arrows3[i].dir == 2) arrows3[i].x += arrows3[i].speed;
            else arrows3[i].x -= arrows3[i].speed;
            
            bool hitShield3 = false;
            if (shieldDir3 == 1) {
                if (arrows3[i].y >= p_y - 2.7f && arrows3[i].y <= p_y - 1.3f &&
                    arrows3[i].x >= p_x - 1.0f && arrows3[i].x <= p_x + 4.0f) {
                    hitShield3 = true;
                }
            } else if (shieldDir3 == 2) {
                if (arrows3[i].y >= p_y + 2.3f && arrows3[i].y <= p_y + 3.7f &&
                    arrows3[i].x >= p_x - 1.0f && arrows3[i].x <= p_x + 4.0f) {
                    hitShield3 = true;
                }
            } else if (shieldDir3 == 3) {
                if (arrows3[i].x >= p_x - 2.7f && arrows3[i].x <= p_x - 1.3f &&
                    arrows3[i].y >= p_y - 1.0f && arrows3[i].y <= p_y + 2.0f) {
                    hitShield3 = true;
                }
            } else if (shieldDir3 == 4) {
                if (arrows3[i].x >= p_x + 4.3f && arrows3[i].x <= p_x + 5.7f &&
                    arrows3[i].y >= p_y - 1.0f && arrows3[i].y <= p_y + 2.0f) {
                    hitShield3 = true;
                }
            }
            
            bool hitPlayer3 = false;
            if (arrows3[i].dir == 0 || arrows3[i].dir == 1) {
                if (arrows3[i].x + 0.9f >= p_x && arrows3[i].x <= p_x + 3.9f &&
                    arrows3[i].y + 1.9f >= p_y && arrows3[i].y <= p_y + 1.9f) {
                    hitPlayer3 = true;
                }
            } else {
                if (arrows3[i].x + 1.9f >= p_x && arrows3[i].x <= p_x + 3.9f &&
                    arrows3[i].y + 0.9f >= p_y && arrows3[i].y <= p_y + 1.9f) {
                    hitPlayer3 = true;
                }
            }
            
            if (!hitShield3 && hitPlayer3) {
                if (invTimer == 0) {
                    lives--;
                    invTimer = INV_FRAMES;
                    if (lives <= 0) {
                        for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
                        gameOver();
                        return;
                    }
                }
                hitShield3 = true;
            }
            
            if (hitShield3) {
                arrows3[i].active = false;
                continue;
            } else if (arrows3[i].x < -5 || arrows3[i].x > 125 || arrows3[i].y < -5 || arrows3[i].y > 35) {
                arrows3[i].active = false;
                continue;
            }
            
            int arrowX = (int)arrows3[i].x;
            int arrowY = (int)arrows3[i].y;
            char arrowChar = '>';
            if (arrows3[i].dir == 0) arrowChar = 'v';
            else if (arrows3[i].dir == 1) arrowChar = '^';
            else if (arrows3[i].dir == 2) arrowChar = '>';
            else arrowChar = '<';
            
            if (arrows3[i].dir == 0 || arrows3[i].dir == 1) {
                if (arrowX >= 0 && arrowX < 120 && arrowY >= 0 && arrowY < 30) {
                    Disp.wDisplay[arrowY * 120 + arrowX].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[arrowY * 120 + arrowX].Attributes = joinColor(BLACK, ORANGE);
                }
                if (arrowX >= 0 && arrowX < 120 && arrowY + 1 >= 0 && arrowY + 1 < 30) {
                    Disp.wDisplay[(arrowY + 1) * 120 + arrowX].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[(arrowY + 1) * 120 + arrowX].Attributes = joinColor(BLACK, ORANGE);
                }
            } else {
                if (arrowX >= 0 && arrowX < 120 && arrowY >= 0 && arrowY < 30) {
                    Disp.wDisplay[arrowY * 120 + arrowX].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[arrowY * 120 + arrowX].Attributes = joinColor(BLACK, ORANGE);
                }
                if (arrowX + 1 >= 0 && arrowX + 1 < 120 && arrowY >= 0 && arrowY < 30) {
                    Disp.wDisplay[arrowY * 120 + arrowX + 1].Char.AsciiChar = arrowChar;
                    Disp.wDisplay[arrowY * 120 + arrowX + 1].Attributes = joinColor(BLACK, ORANGE);
                }
            }
        }
        
        if (input('K')) { for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a]; gameWin(); return; }
        if (input('J')) { for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a]; gameOver(); return; }
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        minigame3Counter++;
        Disp.OutPut();
        Sleep(50);
    }
    
    // All minigames completed - victory!
    for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
    gameWin();
}

void runLevel4() {
    inLevel = true;
    frameCount = 0;
    
    int lives = 3;
    int invTimer = 0;
    const int INV_FRAMES = 20;
    
    // Create purple soul sprite for this level
    AsciiSprite purpleSoul = soul;
    byte savedSoulColors[256];
    for (int i = 0; i < 4*2; ++i) {
        savedSoulColors[i] = soul.Colors[i];
        purpleSoul.Colors[i] = PURPLE;
    }
    
    // Messages for Level 4
    const char *level4Msgs[2] = {
        "Use AD to displace horizontally",
        "Use WS to change of line"
    };
    
    // Three purple horizontal lines (y positions)
    const int LINE_Y[3] = { 8, 14, 20 };
    int currentLine = 1; // Start at center line (index 1)
    int playerX = 58;    // Start at center x
    
    // --- PRE-MINIGAME MESSAGE SEQUENCE (2 messages x 2s each = 4 seconds total)
    const int MSG_FRAMES = 40; // 40 frames = 2 seconds at 20 FPS
    const int PRE_TOTAL_FRAMES = 2 * MSG_FRAMES;
    for (int pf = 0; pf < PRE_TOTAL_FRAMES; ++pf) {
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        DrawLives(lives);
        
        // Draw the three purple lines
        for (int line = 0; line < 3; ++line) {
            for (int x = 2; x < 118; ++x) {
                Disp.wDisplay[LINE_Y[line] * 120 + x].Char.AsciiChar = (char)196;
                Disp.wDisplay[LINE_Y[line] * 120 + x].Attributes = joinColor(BLACK, PURPLE);
            }
        }
        
        // Handle player input for line change and horizontal movement during messages
        // Use key release detection: player must release and press again to switch lines
        if (keyReleased('W') || keyReleased(VK_UP)) { if (currentLine > 0) currentLine--; }
        if (keyReleased('S') || keyReleased(VK_DOWN)) { if (currentLine < 2) currentLine++; }
        if (input('A') || input(VK_LEFT)) if (playerX >= 4) playerX -= 2;
        if (input('D') || input(VK_RIGHT)) if (playerX <= 112) playerX += 2;
        
        // Draw purple soul
        Disp.DrawSprite(purpleSoul, playerX, LINE_Y[currentLine], false);
        
        // Choose which message to display based on frame
        int m = pf / MSG_FRAMES; if (m < 0) m = 0; if (m > 1) m = 1;
        Disp.PutString(level4Msgs[m], playerX - 10, LINE_Y[currentLine] + 3, false);
        
        // Allow exiting while the messages show
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        Disp.OutPut();
        Sleep(50);
    }
    
    // --- MINIGAME 1 (Line-based obstacle dodge): 15 seconds ---
    const int MINIGAME1_FRAMES = 15 * 20; // 15 seconds at 20 FPS
    const int MAX_OBSTACLES = 20;
    struct Obstacle { int x, y; int speed; int dir; bool active; };
    Obstacle obstacles[MAX_OBSTACLES];
    for (int i = 0; i < MAX_OBSTACLES; ++i) obstacles[i].active = false;
    
    int spawnCounter = 0;
    const int SPAWN_MIN = 8, SPAWN_MAX = 16;
    int minigame1Counter = 0;
    
    while (inLevel) {
        if (minigame1Counter >= MINIGAME1_FRAMES) {
            break;
        }
        
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        DrawLives(lives);
        if (invTimer > 0) invTimer--;
        
        // Draw the three purple lines
        for (int line = 0; line < 3; ++line) {
            for (int x = 2; x < 118; ++x) {
                Disp.wDisplay[LINE_Y[line] * 120 + x].Char.AsciiChar = (char)196;
                Disp.wDisplay[LINE_Y[line] * 120 + x].Attributes = joinColor(BLACK, PURPLE);
            }
        }
        
        // Handle player input for line change and horizontal movement
        // Use key release detection: player must release and press again to switch lines
        if (keyReleased('W') || keyReleased(VK_UP)) { if (currentLine > 0) currentLine--; }
        if (keyReleased('S') || keyReleased(VK_DOWN)) { if (currentLine < 2) currentLine++; }
        if (input('A') || input(VK_LEFT)) if (playerX >= 4) playerX -= 2;
        if (input('D') || input(VK_RIGHT)) if (playerX <= 112) playerX += 2;
        
        // Draw purple soul on current line
        Disp.DrawSprite(purpleSoul, playerX, LINE_Y[currentLine], false);
        
        // Spawn obstacles
        if (spawnCounter <= 0) {
            int slot = -1;
            for (int i = 0; i < MAX_OBSTACLES; ++i) if (!obstacles[i].active) { slot = i; break; }
            
            if (slot != -1) {
                obstacles[slot].active = true;
                obstacles[slot].speed = 2 + (rand() % 3); // Speed 2, 3, or 4 (faster)
                obstacles[slot].dir = (rand() % 2 == 0) ? -1 : 1; // -1 = right to left, 1 = left to right
                
                // Choose 2 out of 3 lines to block
                int blockedLines[2];
                int firstLine = rand() % 3;
                int secondLine = rand() % 3;
                while (secondLine == firstLine) secondLine = rand() % 3;
                blockedLines[0] = firstLine;
                blockedLines[1] = secondLine;
                
                // Set obstacle position on one of the blocked lines (will be the first)
                obstacles[slot].y = LINE_Y[blockedLines[0]];
                if (obstacles[slot].dir == -1) {
                    obstacles[slot].x = 118; // Start from right
                } else {
                    obstacles[slot].x = 2; // Start from left
                }
            }
            spawnCounter = SPAWN_MIN + (rand() % (SPAWN_MAX - SPAWN_MIN + 1));
        } else {
            spawnCounter--;
        }
        
        // Move and draw obstacles (3x3 white rectangles)
        for (int i = 0; i < MAX_OBSTACLES; ++i) {
            if (!obstacles[i].active) continue;
            
            obstacles[i].x += obstacles[i].dir * obstacles[i].speed;
            
            // Draw 3x3 obstacle
            for (int oy = 0; oy < 3; ++oy) {
                for (int ox = 0; ox < 3; ++ox) {
                    int drawX = obstacles[i].x + ox;
                    int drawY = obstacles[i].y + oy - 1;
                    if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                        Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '#';
                        Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(WHITE, WHITE);
                    }
                }
            }
            
            // Collision check (3x3 obstacle vs 4x2 soul)
            if (collides(obstacles[i].x, 3, obstacles[i].y - 1, 3, playerX, 4, LINE_Y[currentLine], 2)) {
                if (invTimer == 0) {
                    lives--;
                    invTimer = INV_FRAMES;
                    if (lives <= 0) {
                        for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
                        gameOver();
                        return;
                    }
                }
            }
            
            // Remove if off-screen
            if (obstacles[i].x + 3 < 0 || obstacles[i].x > 119) {
                obstacles[i].active = false;
            }
        }
        
        // Debug keys
        if (input('K')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameWin();
            return;
        }
        if (input('J')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameOver();
            return;
        }
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        minigame1Counter++;
        Disp.OutPut();
        Sleep(50);
    }
    
    if (!inLevel) return;
    
    // --- MINIGAME 2 (Laser avoidance): 10.5 seconds ---
    const int MINIGAME2_FRAMES = 10.5 * 20; // 10.5 seconds at 20 FPS (210 frames)
    const int LASER_CYCLE = 40; // 2 seconds = 40 frames (0.5s normal + 1s red + 0.5s filled)
    const int LASER_NORMAL_FRAMES = 10; // 0.5 seconds = 10 frames (normal purple)
    const int LASER_WARNING_FRAMES = 30; // 1.5 seconds total for warning (1s red + 0.5s filled)
    const int LASER_FILLED_START = 20; // Filled laser starts at frame 20 (after 1s red)
    
    int minigame2Counter = 0;
    
    while (inLevel) {
        if (minigame2Counter >= MINIGAME2_FRAMES) {
            break;
        }
        
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        DrawLives(lives);
        if (invTimer > 0) invTimer--;
        
        // Determine position in laser cycle
        int cyclePos = minigame2Counter % LASER_CYCLE;
        bool laserFilled = (cyclePos >= LASER_FILLED_START); // 0.5s into cycle
        bool laserWarning = (cyclePos >= LASER_NORMAL_FRAMES); // After 0.5s normal phase
        
        // Determine which 2 lines will have lasers (rotate through combinations)
        // Cycle through: lines 0&1, lines 1&2, lines 0&2
        int cycleNum = minigame2Counter / LASER_CYCLE;
        int laserLine1 = 0, laserLine2 = 0;
        switch (cycleNum % 3) {
            case 0: laserLine1 = 0; laserLine2 = 1; break;
            case 1: laserLine1 = 1; laserLine2 = 2; break;
            case 2: laserLine1 = 0; laserLine2 = 2; break;
        }
        
        // Draw the three lines
        for (int line = 0; line < 3; ++line) {
            bool isLaserLine = (line == laserLine1 || line == laserLine2);
            byte lineColor = PURPLE;
            char lineChar = (char)196; // Normal line character
            
            if (laserWarning && isLaserLine) {
                lineColor = RED; // Lines turn red during red phase and filled phase
                if (laserFilled) {
                    lineChar = (char)219; // Solid block (laser beam)
                }
            }
            
            for (int x = 2; x < 118; ++x) {
                Disp.wDisplay[LINE_Y[line] * 120 + x].Char.AsciiChar = lineChar;
                Disp.wDisplay[LINE_Y[line] * 120 + x].Attributes = joinColor(BLACK, lineColor);
            }
        }
        
        // Handle player input for line change and horizontal movement
        // Use key release detection: player must release and press again to switch lines
        if (keyReleased('W') || keyReleased(VK_UP)) { if (currentLine > 0) currentLine--; }
        if (keyReleased('S') || keyReleased(VK_DOWN)) { if (currentLine < 2) currentLine++; }
        if (input('A') || input(VK_LEFT)) if (playerX >= 4) playerX -= 2;
        if (input('D') || input(VK_RIGHT)) if (playerX <= 112) playerX += 2;
        
        // Draw purple soul on current line
        Disp.DrawSprite(purpleSoul, playerX, LINE_Y[currentLine], false);
        
        // Check if player is on a laser line during filled laser phase (0.5s of the cycle)
        bool onLaserLine = (currentLine == laserLine1 || currentLine == laserLine2);
        if (laserFilled && onLaserLine) {
            if (invTimer == 0) {
                lives--;
                invTimer = INV_FRAMES;
                if (lives <= 0) {
                    for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
                    gameOver();
                    return;
                }
            }
        }
        
        // Debug keys
        if (input('K')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameWin();
            return;
        }
        if (input('J')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameOver();
            return;
        }
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        minigame2Counter++;
        Disp.OutPut();
        Sleep(50);
    }
    
    if (!inLevel) return;
    
    // --- MINIGAME 3 (Paired obstacle dodge): 15 seconds ---
    const int MINIGAME3_FRAMES = 15 * 20; // 15 seconds at 20 FPS
    const int MAX_PAIRS = 10; // Maximum 10 pairs of obstacles
    struct ObstaclePair { int x, speed; int dir; bool active; int line1, line2; };
    ObstaclePair pairs[MAX_PAIRS];
    for (int i = 0; i < MAX_PAIRS; ++i) pairs[i].active = false;
    
    int spawn3Counter = 0;
    const int SPAWN3_MIN = 8, SPAWN3_MAX = 16;
    int minigame3Counter = 0;
    
    while (inLevel) {
        if (minigame3Counter >= MINIGAME3_FRAMES) {
            break;
        }
        
        for (int i = 0; i < 120 * 30; ++i) {
            Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
            Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
        }
        
        DrawLives(lives);
        if (invTimer > 0) invTimer--;
        
        // Draw the three purple lines
        for (int line = 0; line < 3; ++line) {
            for (int x = 2; x < 118; ++x) {
                Disp.wDisplay[LINE_Y[line] * 120 + x].Char.AsciiChar = (char)196;
                Disp.wDisplay[LINE_Y[line] * 120 + x].Attributes = joinColor(BLACK, PURPLE);
            }
        }
        
        // Handle player input for line change and horizontal movement
        // Use key release detection: player must release and press again to switch lines
        if (keyReleased('W') || keyReleased(VK_UP)) { if (currentLine > 0) currentLine--; }
        if (keyReleased('S') || keyReleased(VK_DOWN)) { if (currentLine < 2) currentLine++; }
        if (input('A') || input(VK_LEFT)) if (playerX >= 4) playerX -= 2;
        if (input('D') || input(VK_RIGHT)) if (playerX <= 112) playerX += 2;
        
        // Draw purple soul on current line
        Disp.DrawSprite(purpleSoul, playerX, LINE_Y[currentLine], false);
        
        // Spawn pairs of obstacles
        if (spawn3Counter <= 0) {
            int slot = -1;
            for (int i = 0; i < MAX_PAIRS; ++i) if (!pairs[i].active) { slot = i; break; }
            
            if (slot != -1) {
                pairs[slot].active = true;
                pairs[slot].speed = 2 + (rand() % 3); // Speed 2, 3, or 4
                pairs[slot].dir = (rand() % 2 == 0) ? -1 : 1; // -1 = right to left, 1 = left to right
                
                // Choose 2 different lines for the pair
                int firstLine = rand() % 3;
                int secondLine = rand() % 3;
                while (secondLine == firstLine) secondLine = rand() % 3;
                pairs[slot].line1 = firstLine;
                pairs[slot].line2 = secondLine;
                
                // Set obstacle position
                if (pairs[slot].dir == -1) {
                    pairs[slot].x = 118; // Start from right
                } else {
                    pairs[slot].x = 2; // Start from left
                }
            }
            spawn3Counter = SPAWN3_MIN + (rand() % (SPAWN3_MAX - SPAWN3_MIN + 1));
        } else {
            spawn3Counter--;
        }
        
        // Move and draw paired obstacles (3x3 white rectangles on two different lines)
        for (int i = 0; i < MAX_PAIRS; ++i) {
            if (!pairs[i].active) continue;
            
            pairs[i].x += pairs[i].dir * pairs[i].speed;
            
            // Draw pair of obstacles (one on line1, one on line2)
            for (int pairIdx = 0; pairIdx < 2; ++pairIdx) {
                int obstacleY = (pairIdx == 0) ? pairs[i].line1 : pairs[i].line2;
                
                // Draw 3x3 obstacle
                for (int oy = 0; oy < 3; ++oy) {
                    for (int ox = 0; ox < 3; ++ox) {
                        int drawX = pairs[i].x + ox;
                        int drawY = LINE_Y[obstacleY] + oy - 1;
                        if (drawX >= 0 && drawX < 120 && drawY >= 0 && drawY < 30) {
                            Disp.wDisplay[drawY * 120 + drawX].Char.AsciiChar = '#';
                            Disp.wDisplay[drawY * 120 + drawX].Attributes = joinColor(WHITE, WHITE);
                        }
                    }
                }
                
                // Collision check (3x3 obstacle vs 4x2 soul)
                if (collides(pairs[i].x, 3, LINE_Y[obstacleY] - 1, 3, playerX, 4, LINE_Y[currentLine], 2)) {
                    if (invTimer == 0) {
                        lives--;
                        invTimer = INV_FRAMES;
                        if (lives <= 0) {
                            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
                            gameOver();
                            return;
                        }
                    }
                }
            }
            
            // Remove if off-screen
            if (pairs[i].x + 3 < 0 || pairs[i].x > 119) {
                pairs[i].active = false;
            }
        }
        
        // Debug keys
        if (input('K')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameWin();
            return;
        }
        if (input('J')) {
            for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
            gameOver();
            return;
        }
        if (input(VK_ESCAPE)) exit(0);
        
        frameCount++;
        minigame3Counter++;
        Disp.OutPut();
        Sleep(50);
    }
    
    if (!inLevel) return;
    
    // Victory
    for (int a = 0; a < 4*2; ++a) soul.Colors[a] = savedSoulColors[a];
    gameWin();
}

void runLevel5() {
    // TODO: Implement Level 5 logic here. See runLevel1 for reference.
}

void runLevel6() {
    // TODO: Implement Level 6 logic here. See runLevel1 for reference.
}