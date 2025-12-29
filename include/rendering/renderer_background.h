#ifndef __RENDERER_BACKGROUND_H_
#define __RENDERER_BACKGROUND_H_

#include "../renderer.h"

/* Draw parallax scrolling background */
void Renderer_DrawBackground(Renderer *_Renderer, unsigned int _Tick);

/* Draw semi-transparent black overlay over background */
void Renderer_DrawBackgroundOverlay(Renderer *_Renderer);

/* Draw brightness adjustment overlay (darken or brighten) */
void Renderer_DrawBrightnessOverlay(Renderer *_Renderer, float _Brightness);

#endif
