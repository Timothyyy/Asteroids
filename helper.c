#include "helper.h"

SDL_Texture* LoadTexture(char* path, SDL_Renderer* renderer)
{
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (!loadedSurface)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
    }
    else
    {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (!newTexture)
        {
            printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
        }
        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void InitShip(Ship* ship)
{
    ship->w = 90;
    ship->h = 90;
    ship->x = 355;
    ship->y = 255;
    ship->xvel = 0;
    ship->yvel = 0;
    ship->trust = false;
    ship->degrees = 0;
}

void UpdateShip(Ship* ship)
{
    ship->xvel *= 0.975;
    ship->yvel *= 0.975;
    if (ship->trust)
    {
        ship->xvel += 0.2 * cos(M_PI * ship->degrees / 180);
        ship->yvel += 0.2 * sin(M_PI * ship->degrees / 180);
    }
    ship->x += ship->xvel;
    ship->y += ship->yvel;                
    if (ship->x < -45)
        ship->x = 755;
    else if (ship->x > 755)
        ship->x = -45;
    if (ship->y < -45)
        ship->y = 555;
    else if (ship->y > 555)
        ship->y = -45;
}

void InitMissile(Missile* missile, Ship ship)
{
    missile->life = 0;
    missile->w = 10;
    missile->h = 10;
    missile->x = ship.x + 45 * cos(M_PI * ship.degrees / 180) + 40;
    missile->y = ship.y + 45 * sin(M_PI * ship.degrees / 180) + 40;
    missile->xvel = 5 * cos(M_PI * ship.degrees / 180) + ship.xvel;
    missile->yvel = 5 * sin(M_PI * ship.degrees / 180) + ship.yvel;
}

void UpdateMissile(Missile* missile)
{
    missile->x += missile->xvel;
    missile->y += missile->yvel;
    if (missile->x > 795)
        missile->x = -5;
    else if (missile->x < -5)
        missile->x = 795;
    if (missile->y > 595)
        missile->y = -5;
    else if (missile->y < -5)
        missile->y = 595;
    missile->life++;
    if (missile->life == 60)
    {
        SDL_DestroyTexture(missile->image);
        missile->image = NULL;
    }
}

void InitAsteroid(Asteroid* asteroid)
{
    asteroid->w = 90;
    asteroid->h = 90;
    int angle = M_PI * (rand() % 361) /180;
    asteroid->x = (rand() % 2) * 710;
    asteroid->y = (rand() % 2) * 510;
    asteroid->xvel = cos(angle);
    asteroid->yvel = sin(angle);
    asteroid->direction = (1 - (rand() % 2) * 2) * (rand() % 5);
    asteroid->degrees = 0;
}

void UpdateAsteroid(Asteroid* asteroid)
{
    asteroid->x += asteroid->xvel;
    asteroid->y += asteroid->yvel;
    if (asteroid->x < -45)
        asteroid->x = 755;
    else if (asteroid->x > 755)
        asteroid->x = -45;
    if (asteroid->y < -45)
        asteroid->y = 555;
    else if (asteroid->y > 555)
        asteroid->y = -45;
    asteroid->degrees += asteroid->direction;
}

void InitExplosion(Explosion* explosion, int x, int y, SDL_Renderer* renderer)
{
    explosion->image = LoadTexture("img/explosion.png", renderer);
    explosion->x = x;
    explosion->y = y;
    explosion->w = 128;
    explosion->h = 128;
    explosion->life = 0;
}

void UpdateExplosions(Explosion* explosion)
{
    explosion->life++;
    if (explosion->life == 24)
    {
        SDL_DestroyTexture(explosion->image);
        explosion->image = NULL;
    }
}

void ProcessMissiles(Missile missiles[8], SDL_Renderer* renderer)
{
    int i = 0;
    for (; i < 8; i++)
        if (missiles[i].image)
        {
            SDL_Rect miss = { missiles[i].x, missiles[i].y, 10, 10 };
            SDL_RenderCopy(renderer, missiles[i].image, NULL, &miss);
            UpdateMissile(&missiles[i]);            
        }
}

void ProcessAsteroids(Asteroid asteroids[12], SDL_Renderer* renderer)
{
    int i = 0;
    for (; i < 12; i++)
        if (asteroids[i].image)
        {
            SDL_Rect ast = { asteroids[i].x, asteroids[i].y, asteroids[i].w, asteroids[i].h };
            SDL_RenderCopyEx(renderer, asteroids[i].image, NULL, &ast, asteroids[i].degrees, NULL, SDL_FLIP_NONE);
            UpdateAsteroid(&asteroids[i]);
        }
}

void ProcessExplosions(Explosion explosions[10], SDL_Renderer* renderer)
{
    int i = 0;
    for (; i < 10; i++)
        if (explosions[i].image)
        {
            SDL_Rect quad = { explosions[i].x, explosions[i].y, explosions[i].w, explosions[i].h };
            SDL_Rect sprite = { explosions[i].w * explosions[i].life, 0, explosions[i].w, explosions[i].h };
            SDL_RenderCopyEx(renderer, explosions[i].image, &sprite, &quad, 0, NULL, SDL_FLIP_NONE);
            UpdateExplosions(&explosions[i]);
        }
}


Text LoadFromRenderedText(char* text, TTF_Font* font, SDL_Renderer* renderer)
{
    SDL_Color textColor = { 150, 150, 150 };
    Text texture;
    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, textColor);
    if (!textSurface)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        //Create texture from surface pixels
        texture.image = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (!texture.image)
        {
            printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get image dimensions
            texture.w = textSurface->w;
            texture.h = textSurface->h;
        }
        //Get rid of old surface
        SDL_FreeSurface(textSurface);
    }
    //Return success
    return texture;
}

void RespawnAsteroids(Asteroid asteroids[12], SDL_Renderer* renderer)
{
    int i = 0;
    for (; i < 12; i++)
    {
        if (!asteroids[i].image)
        {
            asteroids[i].image = LoadTexture("img/asteroid.png", renderer);
            InitAsteroid(&asteroids[i]);
            break;
        }
    }
}

bool CollideTwoItems(double x1, double y1, double x2, double y2, double min)
{
    double distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
    return min > distance;
}

void GameOver(Asteroid asteroids[12], Missile missiles[8], Ship* ship)
{
    int i = 0;
    for (; i < 12; i++)
    {
        SDL_DestroyTexture(asteroids[i].image);
        asteroids[i].image = NULL;
    }
    i = 0;
    for (; i < 8; i++)
    {
        SDL_DestroyTexture(missiles[i].image);
        asteroids[i].image = NULL;
    }
    InitShip(ship);
}
