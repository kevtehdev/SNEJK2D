#ifndef __RENDERER_CORE_H_
#define __RENDERER_CORE_H_

#include "../renderer.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

/* Helper function to load textures (used by other rendering modules) */
SDL_Texture *Renderer_LoadTexture(SDL_Renderer *_Renderer, const char *_Path);

/* Initialize renderer - sets up SDL, window, fonts, and loads all assets */
bool Renderer_Init(Renderer *_Renderer);

/* Cleanup renderer - frees all resources */
void Renderer_Cleanup(Renderer *_Renderer);

/* Clear screen to black */
void Renderer_Clear(Renderer *_Renderer);

/* Present the rendered frame to the screen */
void Renderer_Present(Renderer *_Renderer);

#endif
