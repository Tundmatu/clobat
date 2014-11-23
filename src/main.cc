#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <tiny_obj_loader.h>
#include <iostream>

#include "globals.h"
#include "texture.h"

/* singletons definition */

CVarSystem *gCVarSystem;
AssetSystem *gAssetSystem;

const int kWidth = 640;
const int kHeight = 480;

int main(int argc, const char *argv[]) {

  // Initialize SDL2
  if (int err = SDL_Init(SDL_INIT_VIDEO) != 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization returned error code %i", err);
    return 1;
  }

  // Create window
  SDL_Window *window = SDL_CreateWindow(
      "An SDL2 window",
      SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED,
      kWidth,
      kHeight,
      SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
  );

  if (!window) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL window creation returned error: ", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  // Create gl context
  SDL_GLContext cxt = SDL_GL_CreateContext(window);
  if (!cxt) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "OpenGL context creation returned error: ", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  gCVarSystem = new CVarSystem();
  gAssetSystem = new AssetSystem();
  gAssetSystem->registerDescriptor("texture", new TextureDescriptor());
  // gAssetSystem->registerDescriptor("sprite", new SpriteDescriptor());
  gAssetSystem->addCatalogue("./assets.xml");

  {

    std::shared_ptr<Texture> tex = gAssetSystem->getAsset<Texture>("character");

    std::cout << tex->path() << std::endl;

  }

  std::shared_ptr<Texture> tex2 = gAssetSystem->getAsset<Texture>("character");

  std::cout << tex2->path() << std::endl;

  //std::cout << gAssetSystem->getAsset<Texture>("test")->path() << std::endl;

  SDL_Event e;
  while (true) {
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
        case SDL_QUIT:
          goto cleanup;

        default:
          break;
      }
    }



    SDL_GL_SwapWindow(window);
  }

cleanup:

  delete gAssetSystem;
  delete gCVarSystem;

  SDL_GL_DeleteContext(cxt);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
