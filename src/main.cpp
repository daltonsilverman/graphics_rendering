#include <SDL2/SDL.h>
#include <iostream>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Software Renderer",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            800, 600,
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

    bool running = true;
    Uint32 startTime = SDL_GetTicks();
    SDL_Event event;
    while (running && (SDL_GetTicks() - startTime < 2000)) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = false;
                }
            }
            SDL_Delay(16); //60 FPS idle loop to prevent pegging
        }



    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

