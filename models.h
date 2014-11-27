#include <stdbool.h>
#include <SDL2/SDL_image.h>

#ifndef MODELS_H
#define MODELS_H

typedef struct
{
    SDL_Texture* image;
    int w;
    int h;
    double x;
    double y;
    double xvel;
    double yvel;
    bool trust;
    int degrees;
} Ship;

typedef struct
{
    SDL_Texture* image;
    int w;
    int h;
    double x;
    double y;
    double xvel;
    double yvel;
    double degrees;
    int life;
} Missile;

typedef struct
{
    SDL_Texture* image;
    int w;
    int h;
    double x;
    double y;
    double xvel;
    double yvel;
    int direction;
    int degrees;
} Asteroid;

typedef struct
{
    SDL_Texture* image;
    int w;
    int h;
    double x;
    double y;
    double life;
} Explosion;

typedef struct
{
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;
    int lives;
    int score;
    bool started;
    bool canShoot;
} GameSettings;

typedef struct
{
    SDL_Texture* image;
    int w;
    int h;
} Text;

#endif
