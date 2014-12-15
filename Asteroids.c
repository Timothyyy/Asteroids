#include "helper.h"
#include <string.h>
#include <SDL2/SDL_mixer.h>

GameSettings settings = { 800, 600, 3, 0, false, true };

//The window we'll be rendering to
SDL_Window* window = NULL;
//The surface contained by the window
SDL_Renderer* renderer = NULL;
//Background texture
SDL_Texture* background = NULL;
//Debris texture
SDL_Texture* debris = NULL;
//
SDL_Texture* splash = NULL;
//Font
TTF_Font* font = NULL;
//Ship texture
Ship ship;
//Lives texture
Text lives;
//Score texture
Text score;
//Missile textures array
Missile missiles[8];
//Asteroid textures array
Asteroid asteroids[12];
//Explosion textures array
Explosion explosions[10];
//Engine sound
Mix_Chunk* engine = NULL;
//Explosion sound
Mix_Chunk* explosion = NULL;
//Missile sound
Mix_Chunk* missile = NULL;
//Soundtrack
Mix_Music* soundtrack = NULL;

bool init()
{
    //Initialization flag
    bool success = true;
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC/*SCALE_QUALITY*/, "1"))
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }
        //Create window
        window = SDL_CreateWindow("Asteroids", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings.SCREEN_WIDTH, settings.SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create renderer for window
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (!renderer)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
                //Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
                //Initialize SDL_mixer
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
                { 
                    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
                    success = false;
                }
            }
        }
    }
    return success;
}

bool LoadMedia()
{
    bool success = true;
    background = LoadTexture("img/nebula.png", renderer);
    debris = LoadTexture("img/debris.png", renderer);
    ship.image = LoadTexture("img/ship.png", renderer);
    splash = LoadTexture("img/splash.png", renderer);
    InitShip(&ship);
    if (!background || !debris || !ship.image || !splash)
    {
        printf("Failed to load texture image!\n");
        success = false;
    }
    engine = Mix_LoadWAV("sound/engine.wav");
    explosion = Mix_LoadWAV("sound/explosion.wav");
    missile = Mix_LoadWAV("sound/missile.wav");
    soundtrack = Mix_LoadMUS("sound/soundtrack.wav");
    Mix_Volume(-1, 20);
    Mix_VolumeMusic(20);
    if (!engine || !explosion || !missile || !soundtrack)
    {
        printf("Failed to load sounds!\n");
        success = false;
    }
    font = TTF_OpenFont("img/font.ttf", 28);
    if (!font)
    {
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    return success;
}

bool EventNotQuit()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            return false;
        }
        else if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_SPACE:
                if (settings.canShoot)
                {
                    settings.canShoot = false;
                    int i = 0;
                    for (; i < 8; ++i)
                    {
                        if (!missiles[i].image)
                        {
                            missiles[i].image = LoadTexture("img/missile.png", renderer);
                            InitMissile(&missiles[i], ship);
                            Mix_PlayChannelTimed(-1, missile, 0, 500);
                            break;
                        }
                    }
                }
                break;

                default: break;
            }
        }
        else if (e.type == SDL_KEYUP)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_SPACE:
                    settings.canShoot = true;
                break;

                default: break;
            }
        }
        else if (e.type == SDL_MOUSEBUTTONUP && !settings.started)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
            if ((x >= 200 && x <= 600) && (y >= 150 && y <= 450))
            {
                settings.started = true;
                settings.lives = 3;
                settings.score = 0;
                Mix_PlayMusic(soundtrack, -1);
            }
        }
    }
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    if (currentKeyStates[SDL_SCANCODE_A])
        ship.degrees -= 3;
    if (currentKeyStates[SDL_SCANCODE_D])
        ship.degrees += 3;
    if (currentKeyStates[SDL_SCANCODE_W])
        ship.trust = true;
    else
        ship.trust = false;
    return true;
}

void DrawText()
{
    SDL_DestroyTexture(lives.image);
    lives.image = NULL;
    SDL_DestroyTexture(score.image);
    score.image = NULL;

    char str[15];
    sprintf(&str, "Lives: %d", settings.lives);
    lives = LoadFromRenderedText(str, font, renderer);
    SDL_Rect textRect = { 10, 10, lives.w, lives.h };
    SDL_RenderCopy(renderer, lives.image, NULL, &textRect);

    sprintf(&str, "Score: %d", settings.score);
    score = LoadFromRenderedText(str, font, renderer);
    textRect.x = 660;
    textRect.w = score.w;
    SDL_RenderCopy(renderer, score.image, NULL, &textRect);
}

void CreateExplosion(double x, double y, int num)
{
    int i = 0;
    while (explosions[i].image)
        i = (i + 1) % 10;
    SDL_DestroyTexture(asteroids[num].image);
    asteroids[num].image = NULL;
    InitExplosion(&explosions[i], x - 19, y - 19, renderer);
}

void DetectCollisions()
{
    int i = 0;
    for (; i < 12; i++)
        if (asteroids[i].image && CollideTwoItems(ship.x + 45, ship.y + 45, asteroids[i].x + 45, asteroids[i].y + 45, 75))
        {
            CreateExplosion(ship.x, ship.y, i);
            settings.lives--;
            Mix_PlayChannel(-1, explosion, 0);
        }
    i = 0;
    for (; i < 8; i++)
        if (missiles[i].image)
            CheckCollision(&missiles[i]);
}

void CheckCollision(Missile* missile)
{
    int i = 0;
    for (; i < 12; i++)
        if (asteroids[i].image && CollideTwoItems(missile->x + 5, missile->y + 5, asteroids[i].x + 45, asteroids[i].y + 45, 43))
        {
            SDL_DestroyTexture(missile->image);
            missile->image = NULL;
            CreateExplosion(asteroids[i].x, asteroids[i].y, i);
            settings.score += 10;
            Mix_PlayChannel(-1, explosion, 0);
            return;
        }
}

void Quit()
{
    SDL_DestroyRenderer(renderer);
    renderer = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void DrawBackground(int* times)
{
    SDL_RenderCopy(renderer, background, NULL, NULL);
    SDL_Rect debr = { 0, 0, settings.SCREEN_WIDTH, settings.SCREEN_HEIGHT };
    *times += 1;
    debr.x = (*times / 4) % settings.SCREEN_WIDTH - settings.SCREEN_WIDTH;
    SDL_RenderCopy(renderer, debris, NULL, &debr);
    debr.x = (*times / 4) % settings.SCREEN_WIDTH;
    SDL_RenderCopy(renderer, debris, NULL, &debr);
}

int main(int argc, char* args[])
{
    srand(time(0));
    if (init())
    {
        if (LoadMedia())
        {
            int fps = 0;
            int sec = 0;
            int times = 0;
            int channel = -1;
            SDL_Event e;
            while (EventNotQuit())
            {
                //Clear screen
                SDL_RenderClear(renderer);

                DrawBackground(&times);

                if (settings.started)
                {
                    UpdateShip(&ship);
                    SDL_Rect quad = { ship.x, ship.y, ship.w, ship.h };
                    int x = 0;
                    if (ship.trust)
                    {
                        x = 90;
                        if (channel == -1)
                        {
                            channel = Mix_PlayChannel(-1, engine, 0);
                            printf("%d\n", channel);
                        }
                        //printf("%d\n", channel);
                        /*if (Mix_PlayingMusic() == 0)
                            Mix_PlayMusic(engine, -1);*/
                    }
                    else 
                    {
                        if (channel != -1)
                        {
                            Mix_HaltChannel(channel);
                            channel = -1;
                        }
                    }
                    SDL_Rect sprite = { x, 0, ship.w, ship.h };
                    SDL_RenderCopyEx(renderer, ship.image, &sprite, &quad, ship.degrees, NULL, SDL_FLIP_NONE);
                
                    ProcessAsteroids(asteroids, renderer);

                    ProcessMissiles(missiles, renderer);

                    if ((SDL_GetTicks() / 1000) > sec)
                    {
                        printf("FPS: %d\n", fps);
                        fps = 0;
                        RespawnAsteroids(asteroids, renderer);
                    }
                    fps++;

                    DetectCollisions();

                    if (settings.lives <= 0)
                    {
                        settings.started = false;
                        GameOver(asteroids, missiles, &ship);
                        Mix_HaltMusic();
                        Mix_HaltChannel(channel);
                    }
                }
                else
                {
                    SDL_Rect spl = { 200, 150, 400, 300 };
                    SDL_RenderCopy(renderer, splash, NULL, &spl);
                    if (settings.lives <= 0)
                    {
                        Text gameOver = LoadFromRenderedText("GAME OVER!", font, renderer);
                        SDL_Rect textRect = { 290, 150, gameOver.w+100, gameOver.h+20 };
                        SDL_RenderCopy(renderer, gameOver.image, NULL, &textRect);
                    }
                }

                DrawText();

                ProcessExplosions(explosions, renderer);
                sec = SDL_GetTicks() / 1000;
                SDL_RenderPresent(renderer);
                SDL_Delay(16.5);
            }
        }
        else
            printf("Failed to load media!\n");
    }
    else
        printf("Failed to initialize!\n");
    Quit();
    return 0;
}
