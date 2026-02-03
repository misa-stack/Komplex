#include <SDL/SDL.h>
#include "komlex.h"
#include "grafika.h"
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define NUM_THREADS 8 // Zvyšte podle počtu jader vašeho CPU

typedef struct {
    double x, y, zoom;
    int x1, x2, y1, y2;
} task_p;

void* funkce(void* p) {
    task_p* task = (task_p*)p;
    Obrazovka* obrazovka = Obrazovka::instance();
    SDL_PixelFormat* fmt = obrazovka->screen->format;

    // Předvýpočet konstant pro cyklus
    double inv_zoom = 0.01 / task->zoom;

    for (int y = task->y1; y < task->y2; y++) {
        double ym = (y - 300) * inv_zoom + task->y;
        
        for (int x = task->x1; x < task->x2; x++) {
            double xm = (x - 400) * inv_zoom + task->x;
            
            // Optimalizace: Použití double místo objektu Komplex pro vnitřní výpočet
            double zx = 0;
            double zy = 0;
            double zx2 = 0; // zx*zx
            double zy2 = 0; // zy*zy
            
            int s = 0;
            // Limit 200 iterací pro lepší detaily při zoomu
            while (zx2 + zy2 <= 16.0 && s < 200) {
                zy = 2.0 * zx * zy + ym;
                zx = zx2 - zy2 + xm;
                zx2 = zx * zx;
                zy2 = zy * zy;
                s++;
            }

            Uint32 pixel;
            if (s == 200) {
                pixel = SDL_MapRGB(fmt, 0, 0, 0);
            } else {
                // Plynulejší modrý gradient
                Uint8 r = (Uint8)(s * 3 % 256);
                Uint8 g = (Uint8)(s * 7 % 256);
                Uint8 b = (Uint8)(s * 13 % 256);
                pixel = SDL_MapRGB(fmt, r, g, b);
            }
            obrazovka->putpixel(x, y, pixel);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    Obrazovka* obrazovka = Obrazovka::instance();
    obrazovka->inicializuj(800, 600, 0, 0);

    double zoom = 1.0;
    double xs = -0.5;
    double ys = 0.0;

    while (1) {
        obrazovka->smaz();

        pthread_t threads[NUM_THREADS];
        task_p tasks[NUM_THREADS];

        int rows_per_thread = 600 / NUM_THREADS;

        for (int i = 0; i < NUM_THREADS; i++) {
            tasks[i].zoom = zoom;
            tasks[i].x = xs;
            tasks[i].y = ys;
            tasks[i].x1 = 0;
            tasks[i].x2 = 800;
            tasks[i].y1 = i * rows_per_thread;
            tasks[i].y2 = (i + 1) * rows_per_thread;
            pthread_create(&threads[i], NULL, funkce, (void*)(&tasks[i]));
        }

        for (int i = 0; i < NUM_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        obrazovka->aktualizuj();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_Quit();
                return 0;
            }
            if (event.type == SDL_KEYDOWN) {
                double move_step = 0.15 / zoom;
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:  xs += move_step; break;
                    case SDLK_LEFT:   xs -= move_step; break;
                    case SDLK_UP:     ys -= move_step; break;
                    case SDLK_DOWN:   ys += move_step; break;
                    case SDLK_p:      zoom *= 1.3; break; 
                    case SDLK_o:      zoom /= 1.3; break;
                    case SDLK_ESCAPE: SDL_Quit(); return 0;
                    default: break;
                }
            }
        }
    }
    return 0;
}
