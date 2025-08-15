// photoApplication.hpp
#ifndef PHOTO_APPLICATION_HPP
#define PHOTO_APPLICATION_HPP

#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <print>

class Application {
private:
    SDL_Window* window;
    SDL_Surface* surface;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    std::vector<std::string> photoFiles;
    int currentIndex;
    bool hasPhotos;

public:
    Application() : window(nullptr), surface(nullptr), texture(nullptr), renderer(nullptr), currentIndex(0), hasPhotos(false) {}

    bool init() {
        std::print("Starting SDL...\n");
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::print("SDL Error: {}\n", SDL_GetError());
            return false;
        }
        std::print("SDL ready\n");

        window = SDL_CreateWindow("Photo Displayer - Press 'f' to load folder",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            800, 600, SDL_WINDOW_SHOWN);

        if (!window) {
            std::print("Window error: {}\n", SDL_GetError());
            SDL_Quit();
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if (!renderer) {
            std::print("Renderer error: {}\n", SDL_GetError());
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        int imgFlags = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
        if (imgFlags == 0) {
            std::print("SDL_image error: {}\n", IMG_GetError());
        }
        std::print("App ready\n");
        return true;
    }

    bool isImageFile(const std::string& filename) {
        std::string ext = filename.substr(filename.find_last_of(".") + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext == "jpg" || ext == "jpeg" || ext == "png";
    }

    bool loadImagesFromDirectory(const std::string& directory) {
        photoFiles.clear();
        currentIndex = 0;
        hasPhotos = false;
        std::print("Looking in: {}\n", directory);
        try {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().string();
                    if (isImageFile(filename)) {
                        photoFiles.push_back(filename);
                        std::print("Found: {}\n", filename);
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& ex) {
            std::print("Directory error: {}\n", ex.what());
            return false;
        }
        if (!photoFiles.empty()) {
            std::print("Loaded {} images\n", photoFiles.size());
            hasPhotos = true;
            loadCurrentPhoto();
            return true;
        }
        std::print("No images found\n");
        return false;
    }

    void loadCurrentPhoto() {
        if (!hasPhotos || photoFiles.empty()) return;
        if (surface) {
            SDL_FreeSurface(surface);
            surface = nullptr;
        }
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        std::string filename = photoFiles[currentIndex];
        std::print("Loading: {}\n", filename);
        surface = IMG_Load(filename.c_str());
        if (surface) {
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                std::print("Texture error: {}\n", SDL_GetError());
            } else {
                std::print("Loaded\n");
            }
        } else {
            std::print("Image error: {}\n", IMG_GetError());
        }
    }

    void renderPhoto() {
        if (!texture) return;
        int textureWidth, textureHeight;
        SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        float scaleX = (float)(windowWidth - 100) / textureWidth;
        float scaleY = (float)(windowHeight - 100) / textureHeight;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        int scaledWidth = (int)(textureWidth * scale);
        int scaledHeight = (int)(textureHeight * scale);
        int x = (windowWidth - scaledWidth) / 2;
        int y = (windowHeight - scaledHeight) / 2;
        SDL_Rect destRect = {x, y, scaledWidth, scaledHeight};
        SDL_RenderCopy(renderer, texture, NULL, &destRect);
    }

    void renderUI() {
        int windowWidth, windowHeight;
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);
        if (hasPhotos && !photoFiles.empty()) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            SDL_Rect prevButton = {20, windowHeight - 80, 100, 40};
            SDL_RenderFillRect(renderer, &prevButton);
            SDL_Rect nextButton = {windowWidth - 120, windowHeight - 80, 100, 40};
            SDL_RenderFillRect(renderer, &nextButton);
        }
        SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        SDL_Rect instructionBox = {windowWidth/2 - 200, 10, 400, 40};
        SDL_RenderFillRect(renderer, &instructionBox);
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);
        if (hasPhotos && texture) {
            renderPhoto();
        } else {
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            int windowWidth, windowHeight;
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            SDL_Rect messageBox = {windowWidth/2 - 150, windowHeight/2 - 25, 300, 50};
            SDL_RenderFillRect(renderer, &messageBox);
        }
        renderUI();
        SDL_RenderPresent(renderer);
    }

    void nextPhoto() {
        if (!hasPhotos || photoFiles.empty()) return;
        currentIndex = (currentIndex + 1) % photoFiles.size();
        loadCurrentPhoto();
        std::print("Next: {}/{}\n", currentIndex + 1, photoFiles.size());
    }

    void prevPhoto() {
        if (!hasPhotos || photoFiles.empty()) return;
        currentIndex = (currentIndex - 1 + photoFiles.size()) % photoFiles.size();
        loadCurrentPhoto();
        std::print("Previous: {}/{}\n", currentIndex + 1, photoFiles.size());
    }

    bool destroy() {
        std::print("Shutting down...\n");
        if (surface) SDL_FreeSurface(surface);
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        std::print("Done\n");
        return true;
    }
};


#endif // PHOTO_APPLICATION_HPP
