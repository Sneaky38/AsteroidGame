#include "Object.h"


template<typename T>
constexpr T WIDTHSCREEN{ 1780 };

template<typename T>
constexpr T HEIGHTSCREEN{ 920 };


Object::Object()
{}

Object::~Object()
{
    HP = 0;
    angle = 0;
    speed = 0;
    angleSpeed = 0;
    dstRect = {0,0,0,0};
}

SDL_Texture* Object::getTexture()
{
	return this->pTexture;
}

SDL_Rect  Object::getSrcRect()
{
	return this->srcRect;
}

SDL_Rect  Object::getDstRect()
{
    return this->dstRect;
}

void Object::setDstRect(SDL_Rect dstRect)
{
    this->dstRect = dstRect;
}
 
int Object::getAngle(void) {
    return angle;
}

SDL_Point Object::getCenter(void) {
    return center;
}

void Object::setCenter(SDL_Point center)
{
    this->center = center;
}

void Object::setAngle(int angle)
{
    this->angle = angle;
}

double Object::getSpeed()
{
    return speed;
}

void Object::setSpeed(double speed)
{
    this->speed = speed;
}


int Object::getHP()
{
    return HP;
}

void Object::setHP(int HP)
{
    this->HP = HP;
}

int Object::getAngleSpeed()
{
    return angleSpeed;
}

void Object::setAngleSpeed(int angleSpeed)
{
    this->angleSpeed = angleSpeed;
}

SDL_Rect Object::getHitbox()
{
    return this->hitbox;
}


bool Object::getDestroyed()
{
    return destroyed;
}

void Object::setDestroyed(bool destroyed)
{
    this->destroyed = destroyed;
}

void Object::turnRight(void) {
    this->angle = this->angle + angleSpeed;
    if (this->angle > 360) {
        this->angle = 0;
    }

}

void Object::turnLeft(void) {
    this->angle = this->angle - angleSpeed;
    if (this->angle < 0) {
        this->angle = 360;
    }

}

void Object::advance(void) {
    float pi = 3.14159;
    float angle2 = this->angle * pi / 180;
    this->dstRect.x = this->dstRect.x + (int)(cos(angle2) * this->speed);
    this->dstRect.y = this->dstRect.y + (int)(sin(angle2) * this->speed);

}
/*Cette fonction permet à un objet touchant un bord d'avancer de l'autre côté de l'écran*/
void Object::turnover(void)
{
    float pi = 3.14159;
    float angle2 = this->angle * pi / 180;
    if (this->dstRect.x < 0 ) 
    {
        this->dstRect.x = this->dstRect.x + WIDTHSCREEN<int> + (int)(cos(angle2) * this->speed);
        if (this->dstRect.y < 0)
        {
            this->dstRect.y = this->dstRect.y + HEIGHTSCREEN<int> + (int)(sin(angle2) * this->speed);
            return;
        }
        if (this->dstRect.y > HEIGHTSCREEN<int>)
        {
            this->dstRect.y = this->dstRect.y - HEIGHTSCREEN<int> + (int)(sin(angle2) * this->speed);
            return;
        }
    }

    if (this->dstRect.x > WIDTHSCREEN<int>)
    {
        this->dstRect.x = this->dstRect.x - WIDTHSCREEN<int> + (int)(cos(angle2) * this->speed);
        if (this->dstRect.y < 0)
        {
            this->dstRect.y = HEIGHTSCREEN<int> +this->dstRect.y + (int)(sin(angle2) * this->speed);
            return;
        }
        if (this->dstRect.y > HEIGHTSCREEN<int>)
        {
            this->dstRect.y = this->dstRect.y -  HEIGHTSCREEN<int> + (int)(sin(angle2) * this->speed);
            return;
        }
    }

    if (this->dstRect.y > HEIGHTSCREEN<int>)
    {
        this->dstRect.y = this->dstRect.y - HEIGHTSCREEN<int> + (int)(cos(angle2) * this->speed);
        if (this->dstRect.x < 0)
        {
            this->dstRect.x = WIDTHSCREEN<int> + this->dstRect.x + (int)(sin(angle2) * this->speed);
            return;
        }
        if (this->dstRect.x > WIDTHSCREEN<int>)
        {
            this->dstRect.x = this->dstRect.x - WIDTHSCREEN<int> + (int)(sin(angle2) * this->speed);
            return;
        }
    }
    
    if (this->dstRect.y < 0)
    {
        this->dstRect.y = HEIGHTSCREEN<int> + this->dstRect.y + (int)(cos(angle2) * this->speed);
        if (this->dstRect.x < 0)
        {
            this->dstRect.x = this->dstRect.x + WIDTHSCREEN<int> + (int)(sin(angle2) * this->speed);
            return;
        }
        if (this->dstRect.x > WIDTHSCREEN<int>)
        {
            this->dstRect.x = this->dstRect.x - WIDTHSCREEN<int> + (int)(sin(angle2) * this->speed);
            return;
        }
    }
    return;
}



/////////////////////////////////////////////////////////////////////////////////////////

Spaceship::Spaceship(SDL_Renderer* pRenderer): Object()
{
    SDL_Surface* spaceship = SDL_LoadBMP("../source/Spaceship.bmp");
    this->pTexture = SDL_CreateTextureFromSurface(pRenderer, spaceship);
    SDL_FreeSurface(spaceship);
    this->srcRect = { 0, 0, 0, 0 };
    SDL_QueryTexture(this->pTexture, nullptr, nullptr, &this->srcRect.w, &this->srcRect.h);
    this->dstRect = { WIDTHSCREEN<int> / 2 - (this->srcRect.w / 9) / 2,HEIGHTSCREEN<int> / 2 - (this->srcRect.h / 9) / 2 , this->srcRect.w / 9, this->srcRect.h / 9 };
    this->angleSpeed = 10;
    this->speed = 15;
    this->angle = 0;
    this->invincible = false;
    this->center = { this->dstRect.w / 2,this->dstRect.h / 2 };
    this->HP = 3;
    this->score = 0;
}

Spaceship::~Spaceship() {
}

bool Spaceship::getInvincible() {
    return invincible;
}

void Spaceship::setInvincibleTime(int time){
    this->invincibleTime = time;
}

int Spaceship::getInvincibleTime(){
    return invincibleTime;
}

void Spaceship::setInvincible(bool invincible)
{
    this->invincible = invincible;
}

void Spaceship::setScore(int score)
{
    this->score = score;
}
int Spaceship::getScore(){
    return score;
}
//Fonction qui permet de mettre à jour la hitbox car RenderCopyEx ne met pas à jour la hitbox lors d'une rotation
SDL_Rect Spaceship::setHitbox()
{
    float pi = 3.14159;
    float angle2 = this->angle * pi / 180;
    this->hitbox.x = this->dstRect.x + abs((int)((this->dstRect.w - this->dstRect.h)*0.5*(pow(sin(angle2),3))));
    this->hitbox.y = this->dstRect.y + 2.5 - abs((int)((this->dstRect.w - this->dstRect.h)*0.5*(pow(sin(angle2),2))));
    this->hitbox.w = this->dstRect.h - 2.5 + abs((int)((this->dstRect.w - this->dstRect.h)*(pow(cos(angle2),2))));
    this->hitbox.h = this->dstRect.w - 2.5 - abs((int)((this->dstRect.w - this->dstRect.h)*(pow(cos(angle2),2))));
    return this->hitbox;
}

/////////////////////////////////////////////////////////////////////////////////////////

Laser::Laser(SDL_Renderer* pRenderer, Spaceship spaceship) : Object()
{
    //Laser    
    SDL_Surface* laser = SDL_LoadBMP("../source/laser.bmp");
    this->pTexture = SDL_CreateTextureFromSurface(pRenderer, laser);
    SDL_FreeSurface(laser);
    this->srcRect = { 0, 0, 0, 0 };
    SDL_QueryTexture(this->pTexture, nullptr, nullptr, &this->srcRect.w, &this->srcRect.h);
    this->speed = 10;
    this->angle = spaceship.getAngle();
    float pi = 3.14159;
    float angle2 = this->angle * pi / 180;
    this->dstRect = { spaceship.getCenter().x + spaceship.getDstRect().x + (int)(cos(angle2) * spaceship.getDstRect().h) - this->srcRect.w/6,
                      spaceship.getCenter().y + (int)(sin(angle2) * spaceship.getDstRect().h)+ spaceship.getDstRect().y - this->srcRect.h/6,
                      this->srcRect.w / 3,
                      this->srcRect.h / 3 };
    this->center = {this->dstRect.w / 2,this->dstRect.h / 2};
}

Laser::Laser(SDL_Renderer* pRenderer, Spaceship spaceship, int adAngle, int adEcart ) : Object()
{
    //Laser    
    SDL_Surface* laser = SDL_LoadBMP("../source/laser.bmp");
    this->pTexture = SDL_CreateTextureFromSurface(pRenderer, laser);
    SDL_FreeSurface(laser);
    this->srcRect = { 0, 0, 0, 0 };
    SDL_QueryTexture(this->pTexture, nullptr, nullptr, &this->srcRect.w, &this->srcRect.h);
    this->speed = 20;
    this->angle = spaceship.getAngle() + adAngle;
    float pi = 3.14159;
    float angle2 = this->angle * pi / 180;
    if(adEcart >=0)
    {
        this->dstRect = { spaceship.getCenter().x + spaceship.getDstRect().x + (int)(cos(angle2) * (spaceship.getDstRect().h )) - this->srcRect.w/6 + (int)(abs((sin(angle2) + cos(angle2)))*adEcart),
                          spaceship.getCenter().y + (int)(sin(angle2) * (spaceship.getDstRect().h)) + spaceship.getDstRect().y - this->srcRect.h/6 + (int)(abs((cos(angle2) + sin(angle2)))*adEcart),
                          this->srcRect.w / 3,
                          this->srcRect.h / 3 };
    }
    else
    {
        this->dstRect = { spaceship.getCenter().x + spaceship.getDstRect().x + (int)(cos(angle2) * (spaceship.getDstRect().h )) - this->srcRect.w/6 + (int)(abs((sin(angle2) + cos(angle2)))*adEcart),
                          spaceship.getCenter().y + (int)(sin(angle2) * (spaceship.getDstRect().h)) + spaceship.getDstRect().y - this->srcRect.h/6 + (int)(abs((-cos(angle2) + sin(angle2)))*adEcart),
                          this->srcRect.w / 3,
                          this->srcRect.h / 3 };
    }
    this->doubleShot = false;
    this->tripleShot = false;
    this->center = {this->dstRect.w / 2,this->dstRect.h / 2};
}

SDL_Rect Laser::setHitbox()
{
    float pi = 3.14159;
    float angle2 = this->angle * pi / 180;
    this->hitbox.x = this->dstRect.x +this->center.x - 3 + abs(this->center.x*(pow(cos(angle2),12)));
    this->hitbox.y = this->dstRect.y + this->center.y -3 + abs(this->center.y*(pow(cos(angle2),12)));
    this->hitbox.w = this->dstRect.h + abs((int)(this->dstRect.h)*(sin(angle2)));
    this->hitbox.h = this->dstRect.h + abs((int)( this->dstRect.h)*(sin(angle2)));
    return this->hitbox;
}

bool Laser::getDoubleShot()
{
    return this->doubleShot;
}

void Laser::setDoubleShot(bool doubleShot)
{
    this->doubleShot = doubleShot;
}

bool Laser::getTripleShot()
{
    return this->tripleShot;
}

void Laser::setTripleShot(bool tripleShot)
{
    this->tripleShot = tripleShot;
}

Laser::Laser()
{
}

Laser::~Laser()
{
}

void Laser::setShotTime(int time){
    shotTime = time;
}

int Laser::getShotTime()
{
    return shotTime;
}
/////////////////////////////////////////////////////////////////////////////////////////

Asteroid::Asteroid(SDL_Renderer* pRenderer) : Object()
{
    //Asteroid
    SDL_Surface* asteroid = SDL_LoadBMP("../source/meteorite.bmp");
    this->pTexture = SDL_CreateTextureFromSurface(pRenderer, asteroid);
    SDL_FreeSurface(asteroid);
    this->srcRect = { 0, 0, 0, 0 };
    SDL_QueryTexture(this->pTexture, nullptr, nullptr, &this->srcRect.w, &this->srcRect.h);
    this->center = { this->srcRect.w / 2 ,this->srcRect.h / 2 };
    this->angle = rand() % 360;
    this->speed = 2;
    int randomBonus = rand() % 100;
    if(randomBonus < 64 && randomBonus > 59 )
    {
        this->bonusExtraLife = true;
    }
    else if(randomBonus > 92 && randomBonus <= 95)
    {
        this->bonusTripleShot = true;
    }
    else if (randomBonus < 34 && randomBonus <= 30)
    {
        this->bonusDoubleShot = true;
    }
    else
    {
        this->bonusDoubleShot = false;
	    this->bonusTripleShot = false;
	    this->bonusExtraLife = false;
    }
    

}


Asteroid::Asteroid() : Object()
{
}


bool Asteroid::getBonusDoubleShot()
{
    return this->bonusDoubleShot;
}

bool Asteroid::getBonusTripleShot()
{
    return this->bonusTripleShot;
}

bool Asteroid::getBonusExtraLife()
{
    return this->bonusExtraLife;
}

void Asteroid::setBonusDoubleShot(bool bonusDoubleShot)
{
    this->bonusDoubleShot = bonusDoubleShot;
}

void Asteroid::setBonusTripleShot(bool bonusTripleShot)
{
    this->bonusTripleShot = bonusTripleShot;
}
	
void Asteroid::setBonusExtraLife(bool bonusExtraLife)
{
    this->bonusExtraLife = bonusExtraLife;
}

Asteroid::~Asteroid()
{
}