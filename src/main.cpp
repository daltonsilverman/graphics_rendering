#include <SDL2/SDL.h>
#include <Eigen/Dense>
#include <iostream>
#include <cmath>

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 640;
constexpr int XSCALE = 16;
constexpr int YSCALE = 16;
constexpr float ASPECT_RATIO = static_cast<float>(WIDTH) / HEIGHT;
constexpr float PI = 3.14159265358979323846f;
constexpr float FIELD_OF_VIEW = PI / 3;
const float FIELD_OF_VIEW_SCALING = 1 / (tan(FIELD_OF_VIEW / 2));
constexpr float Z_NEAR = 0.1f;
constexpr float Z_FAR  = 100.0f;


Eigen::Matrix4f buildPerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar) {
    float f = 1.0f / tan(fov / 2.0f);
    Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

    m(0, 0) = f / aspect;
    m(1, 1) = f;
    m(2, 2) = (zFar + zNear) / (zNear - zFar);
    m(2, 3) = (2 * zFar * zNear) / (zNear - zFar);
    m(3, 2) = -1.0f;

    return m;
}



int transformXCoordinate(int coordinate) {
    return WIDTH / 2 + (coordinate * (WIDTH/XSCALE));
}

int transformYCoordinate(int coordinate) {
    return HEIGHT / 2 + (coordinate * (HEIGHT/YSCALE));
}

Eigen::Vector2i transformCoordinate(Eigen::Vector2i coordinate){
    int x = transformXCoordinate(coordinate[0]);
    int y = transformYCoordinate(coordinate[1]);
    Eigen::Vector2i result(x, y);
    return result;
}

Eigen::Vector2i transformCoordinate(int coordX, int coordY){
    int x = transformXCoordinate(coordX);
    int y = transformYCoordinate(coordY);
    Eigen::Vector2i result(x, y);
    return result;
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

void drawTriangleFromCoordinates(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color, uint32_t* buffer){
    drawLineByCoordinate(x0, y0, x1, y1, color, buffer);
    drawLineByCoordinate(x1, y1, x2, y2, color, buffer);
    drawLineByCoordinate(x0, y0, x2, y2, color, buffer);
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

   //drawLineByCoordinate(1, -1, 2, 0, 0xFFFF0000, framebuffer);
   //drawLineByCoordinate(3, -1, 2, 0, 0xFFFF0000, framebuffer);
   //drawLineByCoordinate(1, -1, 3, -1, 0xFFFF0000, framebuffer);

// Define cube vertex positions in model space (centered at origin)
std::vector<Eigen::Vector3f> cubeVertices = {
    {-1, -1, -1}, {1, -1, -1}, {1,  1, -1}, {-1,  1, -1},
    {-1, -1,  1}, {1, -1,  1}, {1,  1,  1}, {-1,  1,  1}
};

// Define which vertex pairs to connect (edges of the cube)
std::vector<std::pair<int, int>> cubeEdges = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // back face
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // front face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // sides
};

// Create perspective projection matrix
Eigen::Matrix4f projection = buildPerspectiveProjectionMatrix(FIELD_OF_VIEW, ASPECT_RATIO, Z_NEAR, Z_FAR);

// Model transform: push the cube back along z-axis
Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
model(2, 3) = -5.0f; // Move cube -5 units in z

// Project vertices into screen space
std::vector<Eigen::Vector2i> projectedPoints;
for (const auto& v : cubeVertices) {
    Eigen::Vector4f v4(v[0], v[1], v[2], 1.0f);
    Eigen::Vector4f projected = projection * model * v4;

    // Perspective divide
    projected /= projected[3];

    // Map NDC (-1 to 1) to screen coordinates
    int screenX = static_cast<int>((projected[0] + 1.0f) * 0.5f * WIDTH);
    int screenY = static_cast<int>((1.0f - projected[1]) * 0.5f * HEIGHT); // flip Y

    projectedPoints.push_back(Eigen::Vector2i(screenX, screenY));
}

// Draw cube edges
for (const auto& edge : cubeEdges) {
    Eigen::Vector2i p0 = projectedPoints[edge.first];
    Eigen::Vector2i p1 = projectedPoints[edge.second];
    drawLineByPixel(p0[0], p0[1], p1[0], p1[1], 0xFFFFFFFF, framebuffer); // white lines
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

