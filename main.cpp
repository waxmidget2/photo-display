#include "photoApplication.hpp"
#include <print>
#include <iostream>
#include <string>

int main() {
    std::print("Photo Displayer Starting\n");
    
    Application photoApp;
    
    if (!photoApp.init()) {
        std::print("Failed to initialize application\n");
        return 1;
    }
    
    std::print("Window is now visible\n");
    std::print("Instructions:\n");
    std::print("- Press 'l' to load an image folder\n");
    std::print("- Use arrow keys to browse photos\n");
    std::print("- Press ESC to exit\n");
    
    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    std::print("Quit event\n");
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            std::print("Escape pressed\n");
                            running = false;
                            break;
                        case SDLK_l: {
                            std::print("\nLoad Image Folder\n");
                            std::print("Enter folder path: ");
                            std::string folderPath;
                            std::getline(std::cin, folderPath);
                            if (!folderPath.empty()) {
                                std::print("Scanning folder for images...\n");
                                if (photoApp.loadImagesFromDirectory(folderPath)) {
                                    std::print("Images loaded\n");
                                } else {
                                    std::print("Failed to load images from: {}\n", folderPath);
                                }
                            }
                            break;
                        }
                        case SDLK_RIGHT:
                        case SDLK_SPACE:
                            photoApp.nextPhoto();
                            break;
                        case SDLK_LEFT:
                            photoApp.prevPhoto();
                            break;
                        case SDLK_r:
                            std::print("Reload feature not implemented\n");
                            break;
                    }
                    break;
            }
        }
        photoApp.render();
        SDL_Delay(16);
    }
    
    std::print("Cleaning up\n");
    photoApp.destroy();
    std::print("Photo Displayer Finished\n");
    return 0;
}
