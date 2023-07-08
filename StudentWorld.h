#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <iostream>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
  virtual ~StudentWorld();
  Board getBoard() const;
  Player* getPeach() const;
  Player* getYoshi() const;
  Actor* getObj(int x, int y) const;
  Actor* getEnemy(int x, int y) const;
  void addActor(ActObj* actor);
  Actor* isDirSq(int x, int y);
  Actor* checkOverlap(int x, int y);

  virtual int init();
  virtual int move();
  virtual void cleanUp();

  // get # of coins in the bank
  int get_bank_coins() const;

  // add coins to the bank (when player passes over a bank square)
  void deposit_bank_coins(int coins);

  // reset # of coins in the bank to zero
  void reset_bank_coins();

private:
	std::vector<Actor*> m_actors;
	Player* m_peach;
	Player* m_yoshi;
	Board bd;
	int balance;
};

#endif // STUDENTWORLD_H_
