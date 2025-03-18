#include <SDL2/SDL.h>
#include <iostream>

const int WIDTH = 1280;
const int HEIGHT = 640;
const int XSCALE = 16;
const int YSCALE = 16;

void putPixel(int x, int y, uint32_t color, uint32_t* buffer){
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    buffer[y * WIDTH + x] = color;
}

void drawGridLines(uint32_t* buffer){
    for (int x = 0; x < WIDTH; x += WIDTH / XSCALE){
        for (int y = 0; y < HEIGHT; ++y) {
            putPixel(x, y, 0xFF00FF00, buffer);
        }
    }

    for (int y = 0; y < HEIGHT; y += HEIGHT / YSCALE){
        for (int x = 0; x < WIDTH; ++x){
            putPixel(x, y, 0xFF00FF00, buffer);
        }
    }
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Software Renderer",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            WIDTH, HEIGHT,
                                            SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL Window Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        std::cerr << "SDL Renderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Texture* texture = SDL_CreateTexture(renderer, 
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WIDTH, HEIGHT);

    uint32_t* framebuffer = new uint32_t[WIDTH * HEIGHT];

    drawGridLines(framebuffer);

    for(int y = 0; y < HEIGHT; ++y) {
        putPixel(WIDTH/2, y, 0xFFFF0000, framebuffer);
    }

    for(int x = 0; x < WIDTH; ++x) {
        putPixel(x, HEIGHT/2, 0xFFFF0000, framebuffer);
    }


    bool running = true;
    Uint32 startTime = SDL_GetTicks();
    SDL_Event event;
    while (running && (SDL_GetTicks() - startTime < 10000)) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
            }

            SDL_UpdateTexture(texture, nullptr, framebuffer, WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            SDL_RenderPresent(renderer);
            SDL_Delay(16); //60 FPS idle loop to prevent pegging
        }


    delete[] framebuffer;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

