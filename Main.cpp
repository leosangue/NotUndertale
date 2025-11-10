#include <time.h>
#include <stdlib.h>
#include "super-engin.h"
#define input(X) if(GetAsyncKeyState(X) & 0x8000)

super_engin Disp("NotUntertale", 9, 21);
AsciiSprite soul(4, 2), spear(3, 5);

struct Projectile {
    int x, y;
    int dx, dy; // dirección
    bool show;
};

Projectile projectiles[10];

void gameLoop();

int main() {
    srand(time(NULL));

    // Fondo
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

    // Sprite del jugador
    for(int i = 0; i < 4*2; i++) soul.Colors[i] = RED;
    soul.AsciiChars[0] = 219; soul.AsciiChars[1] = 219; soul.AsciiChars[2] = 219; soul.AsciiChars[3] = 219;
    soul.AsciiChars[4] = 223; soul.AsciiChars[5] = 219; soul.AsciiChars[6] = 219; soul.AsciiChars[7] = 223;

    // Sprite de la lanza
    for(int i = 0; i < 3*5; i++) spear.Colors[i] = CYAN;
    spear.AsciiChars[1] = 186; spear.AsciiChars[4] = 186; spear.AsciiChars[7] = 186;
    spear.AsciiChars[9] = '\\'; spear.AsciiChars[10] = 186; spear.AsciiChars[11] = '/';
    spear.AsciiChars[13] = 'v';

    // Inicializar proyectiles
    for(int i = 0; i < 10; i++) {
        int side = rand() % 4; // 0=arriba, 1=abajo, 2=izquierda, 3=derecha
        projectiles[i].show = true;
        if(side == 0) { // arriba
            projectiles[i].x = rand() % 115 + 2;
            projectiles[i].y = 0;
            projectiles[i].dx = 0;
            projectiles[i].dy = 1;
        } else if(side == 1) { // abajo
            projectiles[i].x = rand() % 115 + 2;
            projectiles[i].y = 29;
            projectiles[i].dx = 0;
            projectiles[i].dy = -1;
        } else if(side == 2) { // izquierda
            projectiles[i].x = 0;
            projectiles[i].y = rand() % 28 + 1;
            projectiles[i].dx = 1;
            projectiles[i].dy = 0;
        } else { // derecha
            projectiles[i].x = 119;
            projectiles[i].y = rand() % 28 + 1;
            projectiles[i].dx = -1;
            projectiles[i].dy = 0;
        }
    }

    while(true) gameLoop();
    return 0;
}

int x = 60, y = 15, c = 0;
char *msg[3] = {"This is you", "Move with WASD", "Survive all directions!"};

void resetProjectile(Projectile &p) {
    int side = rand() % 4;
    if(side == 0) { // arriba
        p.x = rand() % 115 + 2;
        p.y = 0;
        p.dx = 0;
        p.dy = 1;
    } else if(side == 1) { // abajo
        p.x = rand() % 115 + 2;
        p.y = 29;
        p.dx = 0;
        p.dy = -1;
    } else if(side == 2) { // izquierda
        p.x = 0;
        p.y = rand() % 28 + 1;
        p.dx = 1;
        p.dy = 0;
    } else { // derecha
        p.x = 119;
        p.y = rand() % 28 + 1;
        p.dx = -1;
        p.dy = 0;
    }
}

void gameLoop() {
    for(int i = 0; i < 120*30; i++) {
        Disp.wDisplay[i].Char.AsciiChar = Disp.offScreenDisplay[i].Char;
        Disp.wDisplay[i].Attributes = Disp.offScreenDisplay[i].Color;
    }

    Disp.DrawSprite(soul, x, y, false);

    if(c/50 < 3)
        Disp.PutString(msg[c/50], x-5, y+3, false);
    else {
        for(int i = 0; i < 10; i++) {
            if(projectiles[i].show)
                Disp.DrawSprite(spear, projectiles[i].x, projectiles[i].y, false);

            // Mover lanza
            projectiles[i].x += projectiles[i].dx;
            projectiles[i].y += projectiles[i].dy;

            // Si sale del área visible, reaparece en un borde nuevo
            if(projectiles[i].x < 0 || projectiles[i].x > 119 || projectiles[i].y < 0 || projectiles[i].y > 29)
                resetProjectile(projectiles[i]);
        }
    }
    c++;

    // Movimiento del jugador
    input('A') if(x >= 4) x-=2;
    input('D') if(x <= 112) x+=2;
    input('W') if(y >= 2) y--;
    input('S') if(y < 27) y++;

    Disp.OutPut();
    Sleep(50);
}
