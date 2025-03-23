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
constexpr float Z_NEAR = 0.1f;
constexpr float Z_FAR  = 100.0f;

struct Vertex {
    Eigen::Vector3f position;
};

struct Edge {
    int from;
    int to;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<Edge> edges;
};

struct ScreenPoint {
    int x,y;
};

struct NDCPoint {
    float x, y, z;
};




Eigen::Matrix4f buildPerspectiveProjectionMatrix(float fov, float aspect, float zNear, float zFar) { //Inspired from OpenGL's perspective projection matrix
    float f = 1.0f / tan(fov / 2.0f);
    Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

    m(0, 0) = f / aspect;
    m(1, 1) = f;
    m(2, 2) = (zFar + zNear) / (zNear - zFar);
    m(2, 3) = (2 * zFar * zNear) / (zNear - zFar);
    m(3, 2) = -1.0f;

    return m;
}

int NDCtoScreenX(float ndcX) {
    return static_cast<int>((ndcX + 1.0f) * 0.5f * WIDTH);
}

int NDCtoScreenY(float ndcY){
    return static_cast<int>((1.0f - ndcY) * 0.5f * HEIGHT);
}

Eigen::Vector2i NDCtoScreen(NDCPoint ndc){
    int x = NDCtoScreenX(ndc.x);
    int y = NDCtoScreenY(ndc.y);
    return Eigen::Vector2i(x, y);
}

Eigen::Vector2i NDCtoScreen(float ndcX, float ndcY){
    int x = NDCtoScreenX(ndcX);
    int y = NDCtoScreenY(ndcY);
    return Eigen::Vector2i(x, y);
}



void putPixel(int x, int y, uint32_t color, uint32_t* buffer){
    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    buffer[y * WIDTH + x] = color;
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

void drawLineByPixel(Eigen::Vector2i point1, Eigen::Vector2i point2, uint32_t color, uint32_t* buffer){ //Brenesham's Algorithm

    int x0 = point1[0], y0 = point1[1];
    int x1 = point2[0], y1 = point2[1];

    drawLineByPixel(x0, y0, x1, y1, color, buffer);
}

void drawLineByPixel(ScreenPoint point1, ScreenPoint point2, uint32_t color, uint32_t* buffer){
    int x0 = point1.x, y0 = point1.y;
    int x1 = point2.x, y1 = point2.y;

    drawLineByPixel(x0, y0, x1, y1, color, buffer);
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



    // Define cube vertex positions in model space (centered at origin)
    std::vector<Eigen::Vector3i> cubeVertices = {
        {-1, -1, -1}, {1, -1, -1}, {1,  1, -1}, {-1,  1, -1},
        {-1, -1,  1}, {1, -1,  1}, {1,  1,  1}, {-1,  1,  1}
    };

    // Define which vertex pairs to connect (edges of the cube)
    std::vector<Edge> cubeEdges = {
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

        Eigen::Vector2i screenPoints = NDCtoScreen(projected[0], projected[1]);

        projectedPoints.push_back(screenPoints);
    }

    // Draw cube edges
    for (const auto& edge : cubeEdges) {
        Eigen::Vector2i p0 = projectedPoints[edge.first];
        Eigen::Vector2i p1 = projectedPoints[edge.second];
        drawLineByPixel(p0, p1, 0xFFFFFFFF, framebuffer); // white lines
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

