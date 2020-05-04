#pragma once
#include <iostream>
#include <SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <random>
#include <time.h>
#include <cstdlib>    

//Classe mère
class Object
{
public:
	Object();
	~Object();
	SDL_Texture* getTexture();
	SDL_Rect  getSrcRect();
	SDL_Rect getDstRect();
	SDL_Point getCenter();
	void setCenter(SDL_Point center);
	void setDstRect(SDL_Rect dstRect);

	int getAngle();
	void setAngle(int angle);
	double getSpeed();
	void setSpeed(double speed);
	bool getDestroyed();
	void setDestroyed(bool destroyed);
	int getHP();
	void setHP(int HP);
	int getAngleSpeed();
	void setAngleSpeed(int angleSpeed);
	SDL_Rect getHitbox();

	void turnRight(void);
	void turnLeft(void);
	void advance(void);
	void turnover(void);

protected:
	int HP;
	int angle;
	double speed;
	int angleSpeed;
	bool destroyed;
	SDL_Texture* pTexture;
	SDL_Rect srcRect;
	SDL_Rect dstRect;
	SDL_Point center;
	SDL_Rect hitbox;

};


class Spaceship : public Object
{
public :
	Spaceship(SDL_Renderer* pRenderer);
	~Spaceship();
	bool getInvincible();
	int getInvincibleTime();
	void setInvincibleTime(int time);
	void setInvincible(bool invincible);
	void setScore(int score);
	int getScore();
	SDL_Rect setHitbox();
private :
	bool invincible;
	int invincibleTime;
	int score;

};



class Laser : public Object
{
public:
	Laser(SDL_Renderer* pRenderer, Spaceship spaceship);
	Laser(SDL_Renderer* pRenderer, Spaceship spaceship, int adAngle, int adEcart);
	Laser();
	~Laser();
	SDL_Rect setHitbox();
	bool getDoubleShot();
	void setDoubleShot(bool open);
	bool getTripleShot();
	void setTripleShot(bool open);
	int getShotTime();
	void setShotTime(int time);

private:
	int shotTime;
	bool doubleShot;
	bool tripleShot;
};

class Asteroid : public Object
{
public:
	Asteroid(SDL_Renderer* pRenderer);
	Asteroid();
	~Asteroid();
	bool getBonusDoubleShot();
	bool getBonusTripleShot();
	bool getBonusExtraLife();
	void setBonusDoubleShot(bool bonusDoubleShot);
	void setBonusTripleShot(bool bonusTripleShot);
	void setBonusExtraLife(bool bonusExtraLife);

private:
	bool bonusDoubleShot;
	bool bonusTripleShot;
	bool bonusExtraLife;
};	


