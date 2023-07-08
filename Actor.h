#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Player;
class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sw, int imageID, int x, int y, int dir, int depth, double size);
	virtual void doSomething() = 0;
	virtual bool canBeHit() = 0;
	virtual void changeHit(bool hit) = 0;
	virtual bool changesDir() = 0;
	StudentWorld* world();
	void changeDir(int d);
	int moveDir() const;
	Player* Peach() const;
	Player* Yoshi() const;
	bool alive() const;
	void dead(bool dead);
	int depth() const;
private:
	StudentWorld* m_world;
	int m_moveDir;
	bool m_alive;
	int m_depth;
};

class Player : public Actor
{
public: 
	Player(StudentWorld* sw, int imageID, int x, int y, int num);
	bool canBeHit();
	void changeHit(bool hit);
	bool changesDir();
	void doSomething();
	int coins(int n);
	void changeCoins(int coin);
	void swapCoins(Player* p);
	void swapStars(Player* p);
	int stars(int s);
	void changeStars(int star);
	void hasVortex(bool vor);
	bool vor() const;
	std::string printVor();
	bool waiting() const;
	void changeState(bool waiting);
	int roll() const;
	void changeRoll(int roll);
	bool fresh() const;
	void changeNew(bool n);
	bool fork(int x, int y);
	void changeFork(bool fork);
	int ticks();
	void changeTicks(int ticks);
	void swap(Player* p);
	void move(int x, int y, int dir, int moveDir);
	void valid(bool valid);
	void update();
private: 
	bool m_hit;
	bool m_fork;
	bool m_new;
	int m_roll;
	int m_num;
	int ticks_to_move;
	bool m_waiting;
	int m_coins;
	int m_stars;
	bool m_vor; 
	bool m_valid;
};

class ActObj : public Actor 
{
public:
	ActObj(StudentWorld* sw, int imageID, int x, int y, int dir, int depth, bool hit, bool changesDir);
	virtual void doSomething();
	virtual void action() = 0;
	bool canBeHit();
	void changeHit(bool hit);
	bool changesDir();
	Player* landsOn(int x, int y);
	Player* movesOn(int x, int y);
	Player* enemyLands(int x, int y);
	bool landed();
	void land(bool l);
private:
	bool m_changesDir;
	bool m_hit;
	bool m_landed;
};

class CoinSquare : public ActObj
{
public:
	CoinSquare(StudentWorld* sw, int imageID, int x, int y, bool good);
	void action();
	bool good();
private:
	bool m_good;
};

class StarSquare : public ActObj
{
public:
	StarSquare(StudentWorld* sw, int imageID, int x, int y);
	void action();

private:
};

class dirSquare : public ActObj
{
public:
	dirSquare(StudentWorld* sw, int imageID, int x, int y, int dir);
	void action();
private:
	int forceDir;
};

class bankSquare : public ActObj
{
public:
	bankSquare(StudentWorld* sw, int imageID, int x, int y);
	void action();
private:
};

class eventSquare : public ActObj
{
public: 
	eventSquare(StudentWorld* sw, int imageID, int x, int y);
	void action();

private:
};

class dropSquare : public ActObj
{
public:
	dropSquare(StudentWorld* sw, int imageID, int x, int y);
	void action();
private:
	
};

class vortex : public ActObj
{
public:
	vortex(StudentWorld* sw, int imageID, int x, int y, int fireDir);
	void doSomething();
	void action() {}
private:
	int fireDir;
};

class enemy : public ActObj
{
public:
	enemy(StudentWorld* sw, int imageID, int x, int y, int depth);
	void doSomething();
	virtual void action() = 0;
	virtual void drop();
	bool walking() const;
	void changeWalking(bool walk);
	int counter() const;
	void changeCounter(int c);
	bool changeDirection(int x, int y);
	void move(int dir);
	bool fork(int x, int y);
	virtual int getRoll() = 0;
private:
	bool m_fork;
	bool m_walking;
	int m_counter;
	int ticks_to_move;
};

class bowser : public enemy
{
public:
	bowser(StudentWorld* sw, int imageID, int x, int y);
	void action();
	void drop();
	int getRoll();
private:

};

class boo : public enemy
{
public:
	boo(StudentWorld* sw, int imageID, int x, int y);
	void action();
	int getRoll();
private:
};


#endif // ACTOR_H_
