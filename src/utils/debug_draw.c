#include "../../include/utils/debug_draw.h"
#include "../../include/config.h"
#include <SDL2/SDL.h>
#include <stdio.h>

static unsigned int lastFrameTime = 0;
static int frameCount = 0;
static float currentFPS = 0.0f;

void DebugDraw_Init(DebugDrawConfig *_Config)
{
    if (!_Config)
        return;

    _Config->showGrid = false;
    _Config->showHitboxes = false;
    _Config->showFPS = false;
    _Config->showPlayerInfo = false;
    _Config->showEventStats = false;
    _Config->showNetworkStats = false;
    _Config->enabled = false;
}

void DebugDraw_ToggleGrid(DebugDrawConfig *_Config)
{
    if (_Config)
        _Config->showGrid = !_Config->showGrid;
}

void DebugDraw_ToggleHitboxes(DebugDrawConfig *_Config)
{
    if (_Config)
        _Config->showHitboxes = !_Config->showHitboxes;
}

void DebugDraw_ToggleFPS(DebugDrawConfig *_Config)
{
    if (_Config)
        _Config->showFPS = !_Config->showFPS;
}

void DebugDraw_ToggleAll(DebugDrawConfig *_Config)
{
    if (!_Config)
        return;

    _Config->enabled = !_Config->enabled;
    if (_Config->enabled)
    {
        _Config->showGrid = true;
        _Config->showFPS = true;
        _Config->showPlayerInfo = true;
        printf("[DebugDraw] Debug visualization ENABLED\n");
    }
    else
    {
        _Config->showGrid = false;
        _Config->showHitboxes = false;
        _Config->showFPS = false;
        _Config->showPlayerInfo = false;
        _Config->showEventStats = false;
        _Config->showNetworkStats = false;
        printf("[DebugDraw] Debug visualization DISABLED\n");
    }
}

void DebugDraw_Grid(Renderer *_Renderer, DebugDrawConfig *_Config)
{
    if (!_Renderer || !_Config || !_Config->showGrid)
        return;

    SDL_Renderer *sdlRenderer = (SDL_Renderer*)_Renderer->sdlRenderer;

    // Draw vertical grid lines
    SDL_SetRenderDrawColor(sdlRenderer, 255, 255, 0, 100);  // Yellow, semi-transparent
    for (int x = 0; x <= GRID_WIDTH; x++)
    {
        SDL_RenderDrawLine(sdlRenderer, x * CELL_SIZE, 0, x * CELL_SIZE, WINDOW_HEIGHT);
    }

    // Draw horizontal grid lines
    for (int y = 0; y <= GRID_HEIGHT; y++)
    {
        SDL_RenderDrawLine(sdlRenderer, 0, y * CELL_SIZE, WINDOW_WIDTH, y * CELL_SIZE);
    }
}

void DebugDraw_Hitboxes(Renderer *_Renderer, Game *_Game, DebugDrawConfig *_Config)
{
    if (!_Renderer || !_Game || !_Config || !_Config->showHitboxes)
        return;

    SDL_Renderer *sdlRenderer = (SDL_Renderer*)_Renderer->sdlRenderer;

    // Draw snake hitboxes
    SDL_SetRenderDrawColor(sdlRenderer, 0, 255, 0, 150);  // Green
    for (int i = 0; i < _Game->snake.length; i++)
    {
        SDL_Rect rect = {
            _Game->snake.segments[i].x * CELL_SIZE,
            _Game->snake.segments[i].y * CELL_SIZE,
            CELL_SIZE,
            CELL_SIZE
        };
        SDL_RenderDrawRect(sdlRenderer, &rect);
    }

    // Draw food hitbox
    SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 150);  // Red
    SDL_Rect foodRect = {
        _Game->food.x * CELL_SIZE,
        _Game->food.y * CELL_SIZE,
        CELL_SIZE,
        CELL_SIZE
    };
    SDL_RenderDrawRect(sdlRenderer, &foodRect);
}

void DebugDraw_FPS(Renderer *_Renderer, DebugDrawConfig *_Config, unsigned int _CurrentTime)
{
    if (!_Renderer || !_Config || !_Config->showFPS)
        return;

    // Update FPS counter every second
    frameCount++;
    if (_CurrentTime - lastFrameTime >= 1000)
    {
        currentFPS = frameCount / ((_CurrentTime - lastFrameTime) / 1000.0f);
        frameCount = 0;
        lastFrameTime = _CurrentTime;
    }

    // FPS counter is printed to console instead of rendered
    // (We don't have text rendering implemented yet)
    static int printCounter = 0;
    if (++printCounter >= 60)  // Print every second (at 60 FPS)
    {
        printf("[Debug] FPS: %.1f\n", currentFPS);
        printCounter = 0;
    }
}

void DebugDraw_PlayerInfo(Renderer *_Renderer, Game *_Game, DebugDrawConfig *_Config)
{
    if (!_Renderer || !_Game || !_Config || !_Config->showPlayerInfo)
        return;

    // Print player info to console
    static int printCounter = 0;
    if (++printCounter >= 180)  // Print every 3 seconds
    {
        printf("[Debug] Player: %s, Score: %d, Length: %d, Speed: %u\n",
               _Game->playerName, _Game->snake.score, _Game->snake.length, _Game->currentSpeed);
        printCounter = 0;
    }
}

void DebugDraw_EventStats(Renderer *_Renderer, EventDispatcher *_Dispatcher, DebugDrawConfig *_Config)
{
    if (!_Renderer || !_Dispatcher || !_Config || !_Config->showEventStats)
        return;

    static int printCounter = 0;
    if (++printCounter >= 300)  // Print every 5 seconds
    {
        printf("[Debug] Events: Processed=%u, Queued=%u\n",
               _Dispatcher->eventsProcessed, _Dispatcher->eventsQueued);
        printCounter = 0;
    }
}

void DebugDraw_NetworkStats(Renderer *_Renderer, MultiplayerContext *_MpCtx, DebugDrawConfig *_Config)
{
    if (!_Renderer || !_MpCtx || !_Config || !_Config->showNetworkStats)
        return;

    static int printCounter = 0;
    if (++printCounter >= 120)  // Print every 2 seconds
    {
        printf("[Debug] Network: State=%d, PlayerIndex=%d, IsHost=%d\n",
               _MpCtx->state, _MpCtx->localPlayerIndex, _MpCtx->isHost);
        printCounter = 0;
    }
}

void DebugDraw_Render(Renderer *_Renderer, Game *_Game, MultiplayerContext *_MpCtx,
                      DebugDrawConfig *_Config, unsigned int _CurrentTime)
{
    if (!_Config || !_Config->enabled)
        return;

    DebugDraw_Grid(_Renderer, _Config);
    DebugDraw_Hitboxes(_Renderer, _Game, _Config);
    DebugDraw_FPS(_Renderer, _Config, _CurrentTime);
    DebugDraw_PlayerInfo(_Renderer, _Game, _Config);

    if (_Game && _Game->eventDispatcher)
        DebugDraw_EventStats(_Renderer, _Game->eventDispatcher, _Config);

    if (_MpCtx)
        DebugDraw_NetworkStats(_Renderer, _MpCtx, _Config);
}
