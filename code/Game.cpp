#include <iostream>
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <random>
#include <time.h>
#include "Object.h"
#include "Object.cpp"
#include <SDL2/SDL_ttf.h>

template <typename T>
constexpr T WIDTHSCREEN{1780};

template <typename T>
constexpr T HEIGHTSCREEN{920};

template <typename T>
constexpr T INIT_ASTEROIDS{15};

template <typename T>
constexpr T MIN_ASTEROIDS{10};

// Structure d'une figure
struct figure{ 
    SDL_Texture *pTexture;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
};

// Creation du Backgroud
figure CreationBackground(SDL_Renderer *pRenderer){
    figure fig;
    SDL_Surface *background = SDL_LoadBMP("../source/space.bmp");
    fig.pTexture = SDL_CreateTextureFromSurface(pRenderer, background);
    SDL_FreeSurface(background);
    fig.srcRect = {0, 0, 0, 0};
    SDL_QueryTexture(fig.pTexture, nullptr, nullptr, &fig.srcRect.w, &fig.srcRect.h);
    fig.dstRect = {0, 0, WIDTHSCREEN<int>, HEIGHTSCREEN<int>};
    return fig;
}

// Creation des asteroid sans collision entre eux ni avec la safe zone
std::vector<Asteroid> Create_Asteroids(std::vector<Asteroid> list_Asteroids, Asteroid asteroid, SDL_Rect safeSpawnRectangle, SDL_Renderer *pRenderer, int Nbr_Asteroids){
    // initialize random seed:
    srand(time(NULL));
    int i = list_Asteroids.size();
    int iteration = 0;
    while (i < Nbr_Asteroids){
        // Creation d'asteroid a une position random sur l'écran
        int randWidths = rand() % (WIDTHSCREEN<int> - asteroid.getSrcRect().w / 3);
        int randHeights = rand() % (HEIGHTSCREEN<int> - asteroid.getSrcRect().h / 3);
        SDL_Rect testDstRectAsteroid = {randWidths, randHeights, asteroid.getSrcRect().w / 3, asteroid.getSrcRect().h / 3};
        
        // Condition d'arret si jamais il n'y pas la place de mettre tout les asteroids
        iteration++;
        if (iteration > 1000){
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "not enough space for asteroid");
            std::vector<Asteroid> empty_list;
            return empty_list;
        }
        // verification de l'initalisation des asteroids sans collision avec la safe zone
        if (SDL_HasIntersection(&testDstRectAsteroid, &safeSpawnRectangle)){
            continue;
        }
        if (list_Asteroids.size() > 0){
            bool test = true;
            // verification de l'initalisation des asteroids sans collision avec les autres asteroids
            for (int j = 0; j < i; j++){
                SDL_Rect r = list_Asteroids[j].getDstRect();
                if (SDL_HasIntersection(&testDstRectAsteroid, &r)){
                    test = false;
                    break;
                }
            }
            if (!test){
                continue;
            }
        }
        Asteroid aAsteroid(pRenderer);
        aAsteroid.setDstRect(testDstRectAsteroid);
        aAsteroid.setCenter({aAsteroid.getDstRect().w / 2, aAsteroid.getDstRect().h / 2});
        list_Asteroids.push_back(aAsteroid);
        i++;
    }
    return list_Asteroids;
}

// Crée deux petits asteroids quand on detruit un asteroid
std::vector<Asteroid> Create_TinyAsteroids(std::vector<Asteroid> list_TinyAsteroids, Asteroid asteroidDestroyed, SDL_Renderer *pRenderer, int Nbr_Asteroids){
    for (int i = 0; i < Nbr_Asteroids; i++){
        /* generate random position for asteroid*/
        int randXPosition = asteroidDestroyed.getDstRect().x + rand() % (asteroidDestroyed.getDstRect().w);
        int randYPosition = asteroidDestroyed.getDstRect().y + rand() % (asteroidDestroyed.getDstRect().h);
        SDL_Rect testDstRectAsteroid = {randXPosition, randYPosition, asteroidDestroyed.getSrcRect().w / 6, asteroidDestroyed.getSrcRect().h / 6};
        Asteroid tinyAsteroid(pRenderer);
        tinyAsteroid.setDstRect(testDstRectAsteroid);
        tinyAsteroid.setCenter({tinyAsteroid.getDstRect().w / 2, tinyAsteroid.getDstRect().h / 2});
        list_TinyAsteroids.push_back(tinyAsteroid);
    }
    return list_TinyAsteroids;
}

// Crée un nouveau laser
std::vector<Laser> New_shot(Spaceship spaceship, SDL_Renderer *pRenderer, std::vector<Laser> list_Laser){
    Laser laser(pRenderer, spaceship);
    list_Laser.push_back(laser);
    return list_Laser;
};

// Crée un double shot : tire 2 lasers parallele
std::vector<Laser> New_Double_Shot(Spaceship spaceship, SDL_Renderer *pRenderer, std::vector<Laser> list_Laser){
    Laser laser1(pRenderer, spaceship, 0, 10);
    Laser laser2(pRenderer, spaceship, 0, -10);
    list_Laser.push_back(laser1);
    list_Laser.push_back(laser2);
    return list_Laser;
}

// Crée un triple shot : tire 3 lasers 
std::vector<Laser> New_Triple_Shot(Spaceship spaceship, SDL_Renderer *pRenderer, std::vector<Laser> list_Laser){
    Laser laser1(pRenderer, spaceship, 10, 0);
    Laser laser2(pRenderer, spaceship, -10, 0);
    Laser laser3(pRenderer, spaceship, 0, 0);
    list_Laser.push_back(laser1);
    list_Laser.push_back(laser2);
    list_Laser.push_back(laser3);
    return list_Laser;
}

// Permet aux asteroids d'avancer en permanence
std::vector<Asteroid> Update_Asteroid(std::vector<Asteroid> list_Asteroids, SDL_Rect gameSpawnRectangle){
    for (int a = 0; a < list_Asteroids.size(); a++){
        // Dessin du asteroid
        list_Asteroids[a].advance();
        SDL_Rect r1 = list_Asteroids[a].getDstRect();
        if (!SDL_HasIntersection(&r1, &gameSpawnRectangle))
            list_Asteroids[a].turnover();
    }
    return list_Asteroids;
}

// Permet aux lasers d'avancer en permanence
std::vector<Laser> Update_Laser(std::vector<Laser> list_Laser){
    for (int a = 0; a < list_Laser.size(); a++){
        list_Laser[a].advance();
    }
    return list_Laser;
}

// Si jamais le vaisseau touche un bord il reapparait de l'autre côté 
Spaceship Update_Spaceship(Spaceship spaceship, SDL_Rect gameSpawnRectangle, SDL_Rect hitboxSpaceship){
    if (!SDL_HasIntersection(&hitboxSpaceship, &gameSpawnRectangle)){
        spaceship.advance();
        spaceship.turnover();
    }
    return spaceship;
}

int main(int argc, char *argv[]){
    // verification de la librairie SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    // verification de la librairie SDL TTF
    if (TTF_Init() < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", TTF_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *pWindow{nullptr};
    SDL_Renderer *pRenderer{nullptr};

    if (SDL_CreateWindowAndRenderer(WIDTHSCREEN<unsigned int>, HEIGHTSCREEN<unsigned int>, SDL_WINDOW_SHOWN, &pWindow, &pRenderer) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    figure backgroundStat = CreationBackground(pRenderer);
    Spaceship spaceship(pRenderer);
    Asteroid asteroid(pRenderer);
    Laser laser(pRenderer, spaceship);

    // type of rotation
    SDL_RendererFlip flip = static_cast<SDL_RendererFlip>(SDL_FLIP_NONE);

    // Zone de protection ou les asteroids n'apparaissent pas
    SDL_Rect safeSpawnRectangle{WIDTHSCREEN<int> / 2, HEIGHTSCREEN<int> / 2, 800, 500};
    safeSpawnRectangle.x = WIDTHSCREEN<int> / 2 - safeSpawnRectangle.w / 2;
    safeSpawnRectangle.y = HEIGHTSCREEN<int> / 2 - safeSpawnRectangle.h / 2;

    // Contour de l'ecran
    SDL_Rect gameSpawnRectangle{0, 0, WIDTHSCREEN<int>, HEIGHTSCREEN<int>};

    std::vector<Laser> list_Laser;                  // liste des lasers
    std::vector<Asteroid> list_Asteroids;           // liste des asteroids
    std::vector<Asteroid> list_AsteroidsDestroyed;  // liste des asteroids detruits
    std::vector<Asteroid> list_TinyAsteroids;       // liste des petits asteroids

    // Initialisation des Asteroids de départ
    list_Asteroids = Create_Asteroids(list_Asteroids, asteroid, safeSpawnRectangle, pRenderer, INIT_ASTEROIDS<int>);
    if (list_Asteroids.empty()){
        SDL_Quit();
        return EXIT_FAILURE;
    }

    bool isOpen{true};
    while (isOpen){
        SDL_Event e;
        SDL_Event event;
        while (isOpen && SDL_PollEvent(&event)){
            switch (event.type){
            case SDL_QUIT:
                isOpen = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                    // touche clavier
                case SDLK_SPACE:
                    if (laser.getTripleShot()){
                        list_Laser = New_Triple_Shot(spaceship, pRenderer, list_Laser);
                    }
                    else if (laser.getDoubleShot()){
                        list_Laser = New_Double_Shot(spaceship, pRenderer, list_Laser);
                    }
                    else{
                        list_Laser = New_shot(spaceship, pRenderer, list_Laser);
                    }
                    break;
                case SDLK_UP:
                    spaceship.advance();
                    break;
                case SDLK_LEFT:
                    spaceship.turnLeft();
                    break;
                case SDLK_RIGHT:
                    spaceship.turnRight();
                    break;
                case SDLK_DOWN:
                    break;
                case SDLK_ESCAPE:
                    isOpen = false;
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
        // Nettoyage de l'écran
        SDL_RenderClear(pRenderer);

        // Dessin du Background
        SDL_RenderCopy(pRenderer, backgroundStat.pTexture, &backgroundStat.srcRect, &backgroundStat.dstRect);

        TTF_Font *font = TTF_OpenFont("../source/ULTRASONIK.TTF", 100);

        // Creation du tableau de Points de vie
        char vie[50];
        sprintf(vie, "Vie : %i", spaceship.getHP());
        SDL_Surface *textVie = TTF_RenderText_Blended(font, vie, {255, 255, 255, 255});     
        SDL_Texture *textureVie = SDL_CreateTextureFromSurface(pRenderer, textVie);
        SDL_Rect positionVie;
        SDL_QueryTexture(textureVie, nullptr, nullptr, &positionVie.w, &positionVie.h); 
        positionVie.x = 0; 
        positionVie.y = 0;
        SDL_FreeSurface(textVie);

        // Creation du Score
        char score[50]; 
        sprintf(score, "Score : %i", spaceship.getScore());
        //On crée une surface de notre texte avec la police chargée
        // La couleur de l'écriture est blanche
        SDL_Surface *textScore = TTF_RenderText_Blended(font, score, {255, 255, 255, 255});
        SDL_Texture *textureScore = SDL_CreateTextureFromSurface(pRenderer, textScore);
        SDL_Rect positionScore;
        SDL_QueryTexture(textureScore, nullptr, nullptr, &positionScore.w, &positionScore.h);
        positionScore.x = WIDTHSCREEN<int> / 2;
        positionScore.y = 0;
        SDL_FreeSurface(textScore);

        // Creation de l'affichage de Game Over
        char end[50]; 
        sprintf(end, "GAME OVER !");
        SDL_Surface *textEnd = TTF_RenderText_Blended(font, end, {255, 255, 255, 255});
        SDL_Texture *textureEnd = SDL_CreateTextureFromSurface(pRenderer, textEnd);
        SDL_Rect positionEnd;
        SDL_QueryTexture(textureEnd, nullptr, nullptr, &positionEnd.w, &positionEnd.h);
        positionEnd.x = WIDTHSCREEN<int> / 2 - positionEnd.h*3 ; 
        positionEnd.y = HEIGHTSCREEN<int> / 2 - positionEnd.h;
        SDL_FreeSurface(textEnd);

        // Creation de l'affichage de l'exit game
        char exitKey[50]; 
        sprintf(exitKey, "For EXIT press escape");
        SDL_Surface *textExitKey = TTF_RenderText_Blended(font, exitKey, {255, 255, 255, 255});
        SDL_Texture *textureExitKey = SDL_CreateTextureFromSurface(pRenderer, textExitKey);
        SDL_Rect positionExitKey;
        SDL_QueryTexture(textureExitKey, nullptr, nullptr, &positionExitKey.w, &positionExitKey.h);
        positionExitKey.x = WIDTHSCREEN<int> / 2 - positionExitKey.h*4 ; 
        positionExitKey.y = HEIGHTSCREEN<int> / 2 + positionExitKey.h*2;
        SDL_FreeSurface(textExitKey);

        TTF_CloseFont(font);

        // Creation des asteroids
        list_Asteroids = Create_Asteroids(list_Asteroids, asteroid, safeSpawnRectangle, pRenderer, MIN_ASTEROIDS<int>);
        if (list_Asteroids.empty()){
            SDL_Quit();
            return EXIT_FAILURE;
        }

        // Creation des petits asteroids
        int iter = list_AsteroidsDestroyed.size();
        for (int i = 0 ; i<iter;i++){
            list_TinyAsteroids = Create_TinyAsteroids(list_TinyAsteroids,list_AsteroidsDestroyed.back(),pRenderer,2);
            list_AsteroidsDestroyed.pop_back();
        }

        // Affichage des asteroids
        for (auto &aAsteroid : list_Asteroids){
            SDL_Rect r = aAsteroid.getSrcRect();
            SDL_Rect d = aAsteroid.getDstRect();
            SDL_Point center = aAsteroid.getCenter();
            SDL_RenderCopyEx(pRenderer, asteroid.getTexture(), &r, &d, aAsteroid.getAngle(), &center, flip);
        }

        // Affichage des petits asteroids
        for (auto &tinyAsteroid : list_TinyAsteroids){
            SDL_Rect r = tinyAsteroid.getSrcRect();
            SDL_Rect d = tinyAsteroid.getDstRect();
            SDL_Point center = tinyAsteroid.getCenter();
            SDL_RenderCopyEx(pRenderer, asteroid.getTexture(), &r, &d, tinyAsteroid.getAngle(), &center, flip);
        }

        // Changement du mode de tir
        if (laser.getShotTime() < SDL_GetTicks()){
            laser.setDoubleShot(false);
            laser.setTripleShot(false);
        }

        // Affichage des lasers
        for (int a = 0; a < list_Laser.size(); a++){
            SDL_Rect r = list_Laser[a].getSrcRect();
            SDL_Rect d = list_Laser[a].getDstRect();
            SDL_Point center = list_Laser[a].getCenter();
            SDL_RenderCopyEx(pRenderer, list_Laser[a].getTexture(), &r, &d, list_Laser[a].getAngle(), &center, flip);
            d = list_Laser[a].setHitbox();
        }

        // Affichage du Spaceship
        SDL_Rect r = spaceship.getSrcRect();
        SDL_Rect d = spaceship.getDstRect();
        SDL_Point center = spaceship.getCenter();
        SDL_RenderCopyEx(pRenderer, spaceship.getTexture(), &r, &d, spaceship.getAngle(), &center, flip);

        // Affichage des Points de vie, du Score et de l'ecran de fin si le vaisseau est détruit
        SDL_RenderCopy(pRenderer, textureVie, nullptr, &positionVie);
        SDL_RenderCopy(pRenderer, textureScore, nullptr, &positionScore);
        if(spaceship.getDestroyed()){
            SDL_RenderCopy(pRenderer, textureEnd, nullptr, &positionEnd);
            SDL_RenderCopy(pRenderer, textureExitKey, nullptr, &positionExitKey);
            isOpen = false;
        }

        // Affichage de l'écran
        SDL_RenderPresent(pRenderer);

        if(spaceship.getDestroyed()){
            bool open{true};
            while (open){
                SDL_Event e;
                SDL_Event event;
                while (open && SDL_PollEvent(&event)){
                    switch (event.type){
                    case SDL_QUIT:
                        open = false;
                        break;
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym){
                            // touche clavier
                            case SDLK_ESCAPE:
                                open = false;
                                break;
                            default:
                            break;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        // On met à jour le déplacement des asteroids des lasers et du vaisseau
        list_Asteroids = Update_Asteroid(list_Asteroids, gameSpawnRectangle);
        if (!list_TinyAsteroids.empty()){
            list_TinyAsteroids = Update_Asteroid(list_TinyAsteroids, gameSpawnRectangle);
        }
        list_Laser = Update_Laser(list_Laser);
        spaceship = Update_Spaceship(spaceship, gameSpawnRectangle, spaceship.setHitbox());

        // On verifie si il y collision avec un asteroid 
        for (int a = 0; a < list_Asteroids.size(); a++){
            SDL_Rect r1 = list_Asteroids[a].getDstRect();
            for (int b = 0; b < list_Laser.size(); b++){
                SDL_Rect r2 = list_Laser[b].getHitbox();

                // On détecte la collision entre une météorite et un laser
                if (SDL_HasIntersection(&r1, &r2)){
                    list_Laser[b].~Laser();
                    list_AsteroidsDestroyed.push_back(list_Asteroids[a]);

                    // On active le bonus donné par l'asteroid 
                    if (list_Asteroids[a].getBonusTripleShot()){
                        laser.setTripleShot(true);
                        laser.setDoubleShot(false);
                        laser.setShotTime(SDL_GetTicks() + 5000);
                    }
                    else if (list_Asteroids[a].getBonusDoubleShot()){
                        laser.setDoubleShot(true);
                        laser.setTripleShot(false);
                        laser.setShotTime(SDL_GetTicks() + 5000);
                    }
                    else if (list_Asteroids[a].getBonusExtraLife()){
                        spaceship.setHP(spaceship.getHP() + 1);
                        SDL_DestroyTexture(textureVie);
                    }
                    
                    // On détruit l'asteroid et le laser
                    list_Asteroids[a].~Asteroid(); 
                    list_Asteroids.erase(list_Asteroids.begin() + a);
                    //On met le score à jour
                    spaceship.setScore(spaceship.getScore() + 1); 
                }

                // On détruit le laser s'il touche un bord
                SDL_Rect r4 = gameSpawnRectangle; 
                if (!SDL_HasIntersection(&r2, &r4)){
                    list_Laser[b].~Laser();
                }
            }

            SDL_Rect r3 = spaceship.getHitbox();
            // Cas où une météorite touche le vaisseau
            if (SDL_HasIntersection(&r1, &r3)) {
                list_Asteroids[a].~Asteroid();
                if (!spaceship.getInvincible()){
                    // Si le joueur n'est pas en état d'invincibilité alors il perd une vie
                    spaceship.setHP(spaceship.getHP() - 1);
                    // Cas où le joueur n'a plus de vie
                    if (spaceship.getHP() == 0) {
                        spaceship.setDestroyed(true);
                    }
                    spaceship.setInvincible(true); 
                    spaceship.setInvincibleTime(SDL_GetTicks() + 3000);
                }
            }
        }
        
        // On verifie si il y collision avec un petit asteroid 
        for (int a = 0; a < list_TinyAsteroids.size(); a++){
            SDL_Rect r1 = list_TinyAsteroids[a].getDstRect();
            for (int b = 0; b < list_Laser.size(); b++){

                SDL_Rect r2 = list_Laser[b].getHitbox();
                // On détecte la collision entre un petit asteroid et un laser
                if (SDL_HasIntersection(&r1, &r2)){
                    list_Laser[b].~Laser();
                    list_TinyAsteroids[a].~Asteroid();
                    list_TinyAsteroids.erase(list_TinyAsteroids.begin() + a);
                    // On met le score à jour
                    spaceship.setScore(spaceship.getScore() + 1); 
                }
            }

            // Cas où un petit asteroid touche le vaisseau
            SDL_Rect r3 = spaceship.getHitbox();
            if (SDL_HasIntersection(&r1, &r3)){
                list_TinyAsteroids[a].~Asteroid();
                list_TinyAsteroids.erase(list_TinyAsteroids.begin() + a);
                if (!spaceship.getInvincible()){
                    spaceship.setHP(spaceship.getHP() - 1);
                    SDL_DestroyTexture(textureVie);

                    if (spaceship.getHP() == 0){
                        spaceship.setDestroyed(true);
                    }
                    spaceship.setInvincible(true);
                    spaceship.setInvincibleTime(SDL_GetTicks() + 3000);
                }
            }
        }

        // On vérifie le temps d'invincibilité
        if (spaceship.getInvincibleTime() < SDL_GetTicks()){
            spaceship.setInvincible(false);
        }

        // On detruit les textures du score, de la vie et l'ecran de fin pour les mettres a jour
        SDL_DestroyTexture(textureScore);
        SDL_DestroyTexture(textureVie);
        SDL_DestroyTexture(textureEnd);
    }

    // On détruit toutes les texture avant de quitter
    for (int a = 0; a < list_TinyAsteroids.size(); a++)
    {
        SDL_DestroyTexture(list_TinyAsteroids[a].getTexture());
    }
    for (int a = 0; a < list_Laser.size(); a++)
    {
        SDL_DestroyTexture(list_Laser[a].getTexture());
    }
    for (int a = 0; a < list_Asteroids.size(); a++)
    {
        SDL_DestroyTexture(list_Asteroids[a].getTexture());
    }
    SDL_DestroyTexture(asteroid.getTexture());
    SDL_DestroyTexture(spaceship.getTexture());
    SDL_DestroyTexture(backgroundStat.pTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();

    return EXIT_SUCCESS;
}