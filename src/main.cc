#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <tiny_obj_loader.h>
#include <iostream>
#include <stitch.h>

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

  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

  // Create window
  SDL_Window *window = SDL_CreateWindow(
      "clobat",
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
  gAssetSystem->addCatalogue("./assets.xml");

  std::shared_ptr<Texture> a = gAssetSystem->getAsset<Texture>("character");
  Sprite sprite("character", 0, 0, 292, 324);


  GLint size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);

  std::cout << "Max size: " << size << std::endl;

  Stitcher stitcher(16384, 16384);

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


    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glColor4f(1.f, 1.f, 1.f, 1.f);

    glEnable(GL_TEXTURE_2D);
    glLoadIdentity();
    glOrtho(0, kWidth, kHeight, 0, -1, 1);

    stitcher.begin();
    stitcher.draw(sprite, 50, 50);
    stitcher.end();

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
