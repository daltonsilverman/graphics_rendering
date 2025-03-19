#include <SDL2/SDL.h>
#include <iostream>

const int WIDTH = 1280;
const int HEIGHT = 640;
const int XSCALE = 16;
const int YSCALE = 16;

int transformXCoordinate(int coordinate) {
    return WIDTH / 2 + (coordinate * (WIDTH/XSCALE));
}

int transformYCoordinate(int coordinate) {
    return HEIGHT / 2 + (coordinate * (HEIGHT/YSCALE));
}

void putPixel(int x, int y, uint32_t color, uint32_t* buffer){
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    buffer[y * WIDTH + x] = color;
}

void putCoordinate(int x, int y, uint32_t color, uint32_t* buffer){
    x = transformXCoordinate(x);
    y = transformYCoordinate(y);
    putPixel(x, y, color, buffer);
}

void drawLineByPixel(int x0, int y0, int x1, int y1, uint32_t color, uint32_t* buffer){ //Brenesham's Algorithm
    bool steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = abs(y1 - y0);
    int decisionParam = 2 * dy - dx;
    int yStep = (y0 < y1) ? 1 : -1;
    int y = y0;

    for (int x = x0; x <= x1; ++x) {
        if (steep) {
            putPixel(y, x, color, buffer);
        } else {
            putPixel(x, y, color, buffer);
        }

        if (decisionParam > 0) {
            y += yStep;
            decisionParam -= 2 * dx;
        }
        decisionParam += 2 * dy;
    }
}

void drawLineByCoordinate(int x0coord, int y0coord, int x1coord, int y1coord, uint32_t color, uint32_t* buffer) {
    int x0 = transformXCoordinate(x0coord);
    int x1 = transformXCoordinate(x1coord);
    int y0 = transformYCoordinate(y0coord);
    int y1 = transformYCoordinate(y1coord);
    drawLineByPixel(x0, y0, x1, y1, color, buffer);
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

   //drawGridLines(framebuffer);

    /**for(int y = 0; y < HEIGHT; ++y) {
        putPixel(WIDTH/2, y, 0xFFFF0000, framebuffer);
    }

    for(int x = 0; x < WIDTH; ++x) {
        putPixel(x, HEIGHT/2, 0xFFFF0000, framebuffer);
    } */

    for(int y = -1 * YSCALE; y < YSCALE; ++y) {
        for(int x = -1 * XSCALE; x < XSCALE; ++x) {
            putCoordinate(x, y, 0xFF00FF00, framebuffer);
        }
    }

   drawLineByCoordinate(1, -1, 2, 0, 0xFFFF0000, framebuffer);
   drawLineByCoordinate(3, -1, 2, 0, 0xFFFF0000, framebuffer);
   drawLineByCoordinate(1, -1, 3, -1, 0xFFFF0000, framebuffer);

   


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

