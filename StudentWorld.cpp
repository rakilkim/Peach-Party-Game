#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), balance(0)
{
    m_peach = nullptr;
    m_yoshi = nullptr;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

void StudentWorld::addActor(ActObj* actor) {
    m_actors.push_back(actor);
}

Board StudentWorld::getBoard() const{
    return bd;
}

Player* StudentWorld::getPeach() const{
    return m_peach;
}

Player* StudentWorld::getYoshi() const{
    return m_yoshi;
}


int StudentWorld::init()
{
	startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    ostringstream oss;
    oss << assetPath() << "board0" << getBoardNumber() << ".txt";
    string board_file = oss.str();
    Board::LoadResult result = bd.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found)
        cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
        Board::GridEntry ge;
        for (int i = 0; i < BOARD_WIDTH; i++) {
            for (int j = 0; j < BOARD_HEIGHT; j++) {
                ge = bd.getContentsOf(i, j);
                switch (ge) {
                case Board::empty:
                    break;
                case Board::player:
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, true));
                    m_peach = new Player(this, IID_PEACH, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 1);
                    m_yoshi = new Player(this, IID_YOSHI, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 2);
                    break;
                case Board::blue_coin_square:
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, true));
                    break;
                case Board::red_coin_square:
                    m_actors.push_back(new CoinSquare(this, IID_RED_COIN_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, false));
                    break;
                case Board::star_square:
                    m_actors.push_back(new StarSquare(this, IID_STAR_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Board::up_dir_square:
                    m_actors.push_back(new dirSquare(this, IID_DIR_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 90));
                    break;
                case Board::down_dir_square:
                    m_actors.push_back(new dirSquare(this, IID_DIR_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 270));
                    break;
                case Board::right_dir_square:
                    m_actors.push_back(new dirSquare(this, IID_DIR_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 0));
                    break;
                case Board::left_dir_square:
                    m_actors.push_back(new dirSquare(this, IID_DIR_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, 180));
                    break;
                case Board::bank_square:
                    m_actors.push_back(new bankSquare(this, IID_BANK_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Board::event_square:
                    m_actors.push_back(new eventSquare(this, IID_EVENT_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    break;
                case Board::bowser:
                    m_actors.push_back(new bowser(this, IID_BOWSER, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, true));
                    break;
                case Board::boo:
                    m_actors.push_back(new boo(this, IID_BOO, i * SPRITE_WIDTH, j * SPRITE_HEIGHT));
                    m_actors.push_back(new CoinSquare(this, IID_BLUE_COIN_SQUARE, i * SPRITE_WIDTH, j * SPRITE_HEIGHT, true));
                    break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit ESC.
    // Notice that the return value GWSTATUS_NOT_IMPLEMENTED will cause our framework to end the game.
    m_peach->doSomething();
    m_yoshi->doSomething(); 
    vector<Actor*>::iterator p; 
    p = m_actors.begin(); 
    while (p != m_actors.end()) { 
        if ((*p)->alive()) {
            (*p)->doSomething();
        } 
        p++;
    }  

    p = m_actors.begin();
    while (p != m_actors.end()) {
        if (!(*p)->alive()) {
            delete* p;
            p = m_actors.erase(p);
        }
        else
            p++;
    }

    ostringstream oss;
    oss << "P1 Roll: " << getPeach()->roll() << " Stars: " << getPeach()->stars(0) << " $$: " << getPeach()->coins(0) << " " << getPeach()->printVor() << " | Time: " << timeRemaining() << 
    " | Bank: " << get_bank_coins() << " | P2 ROll: " << getYoshi()->roll() << " Stars: " << getYoshi()->stars(0) << " $$: " << getYoshi()->coins(0) << " " << getYoshi()->printVor();
    string status = oss.str();
    setGameStatText(status);

    if (timeRemaining() == 0) {
        playSound(SOUND_GAME_FINISHED);
        if (getPeach()->stars(0) > getYoshi()->stars(0)) {
            setFinalScore(getPeach()->stars(0), getPeach()->coins(0));
            return GWSTATUS_PEACH_WON;
        }
        else if (getPeach()->stars(0) < getYoshi()->stars(0)) {
            setFinalScore(getYoshi()->stars(0), getYoshi()->coins(0));
            return GWSTATUS_YOSHI_WON;
        }
        else if (getPeach()->coins(0) > getYoshi()->coins(0)) {
            setFinalScore(getPeach()->stars(0), getPeach()->coins(0));
            return GWSTATUS_PEACH_WON;
        }
        else if (getPeach()->coins(0) < getYoshi()->coins(0)) {
            setFinalScore(getYoshi()->stars(0), getYoshi()->coins(0));
            return GWSTATUS_YOSHI_WON;
        }
        else {
            switch (randInt(1, 2)) {
            case 1:
                setFinalScore(getYoshi()->stars(0), getYoshi()->coins(0));
                return GWSTATUS_YOSHI_WON;
            case 2:
                setFinalScore(getPeach()->stars(0), getPeach()->coins(0));
                return GWSTATUS_PEACH_WON;
            }
        }

    }
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    vector<Actor*>::iterator p;
    p = m_actors.begin();
    while (p != m_actors.end()) {
            delete* p;
            p = m_actors.erase(p);
    }
    delete m_peach;
    delete m_yoshi;
}

// get # of coins in the bank
int StudentWorld::get_bank_coins() const
{
    return balance;
}

// add coins to the bank (when player passes over a bank square)
void StudentWorld::deposit_bank_coins(int coins)
{
    balance += coins;
}

// reset # of coins in the bank to zero
void StudentWorld::reset_bank_coins()
{
    balance = 0;
}

Actor* StudentWorld::getObj(int x, int y) const {
    for (int i = 0; i < m_actors.size(); i++) {
        if (x == m_actors[i]->getX() && y == m_actors[i]->getY() && m_actors[i]->depth() == 1 && m_actors[i]->alive())
            return m_actors[i];
    }
    return nullptr;
}

Actor* StudentWorld::isDirSq(int x, int y) {
    for (int i = 0; i < m_actors.size(); i++) {
        if (x == m_actors[i]->getX() && y == m_actors[i]->getY() && m_actors[i]->changesDir())
            return m_actors[i];
    }
    return nullptr;
}

Actor* StudentWorld::checkOverlap(int x, int y) {  //overlap check
    for (int i = 0; i < m_actors.size(); i++) {
        if (m_actors[i]->canBeHit() && x + 16 > m_actors[i]->getX() && y + 16 > m_actors[i]->getY() && y < m_actors[i]->getY() + 16 && x < m_actors[i]->getX() + 16)
            return m_actors[i];
    }
    return nullptr;
}

Actor* StudentWorld::getEnemy(int x, int y) const {
    for (int i = 0; i < m_actors.size(); i++) {
        if (x == m_actors[i]->getX() && y == m_actors[i]->getY() && m_actors[i]->canBeHit())
            return m_actors[i];
    }
    return nullptr;
}

