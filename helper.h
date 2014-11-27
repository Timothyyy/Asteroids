#include "models.h"
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#ifndef HELPER_H
#define HELPER_H

SDL_Texture* LoadTexture(char* path, SDL_Renderer* renderer);
void InitShip(Ship* ship);
void UpdateShip(Ship* ship);
void InitMissile(Missile* missile, Ship ship);
void UpdateMissile(Missile* missile);
void InitAsteroid(Asteroid* asteroid);
void UpdateAsteroid(Asteroid* asteroid);
void InitExplosion(Explosion* explosion, int x, int y, SDL_Renderer* renderer);
void UpdateExplosion(Explosion* explosion);
void ProcessExplosions(Explosion explosions[10], SDL_Renderer* renderer);
Text LoadFromRenderedText(char* text, TTF_Font* font, SDL_Renderer* renderer);
void ProcessMissiles(Missile missiles[8], SDL_Renderer* renderer);
void ProcessAsteroids(Asteroid asteroids[12], SDL_Renderer* renderer);
void RespawnAsteroids(Asteroid asteroids[12], SDL_Renderer* renderer);
bool CollideTwoItems(double x1, double y1, double x2, double y2, double min);
void GameOver(Asteroid asteroids[12], Missile missiles[8], Ship* ship);

#endif
