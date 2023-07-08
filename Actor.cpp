#include "Actor.h"
#include "StudentWorld.h"

Actor::Actor(StudentWorld* sw, int imageID, int x, int y, int dir, int depth, double size)
	: GraphObject(imageID, x, y, dir, depth, size), m_world(sw), m_moveDir(0), m_alive(true), m_depth(depth)
{}

StudentWorld* Actor::world() {
	return m_world;
}

void Actor::changeDir(int d) {
	m_moveDir = d;
}

int Actor::moveDir() const {
	return m_moveDir;
}

Player* Actor::Peach() const{
	return m_world->getPeach();
}

Player* Actor::Yoshi() const{
	return m_world->getYoshi();
} 

int Actor::depth() const {
	return m_depth;
}

Player::Player(StudentWorld* sw, int imageID, int x, int y, int num)
	: Actor(sw, imageID, x, y, 0, 0, 1), ticks_to_move(0), m_waiting(true), m_num(num), m_roll(0), m_coins(0), m_new(true), m_stars(0), m_vor(false), m_hit(false)
{
}

bool Player::waiting() const {
	return m_waiting;
}

void Player::changeState(bool waiting) {
	m_waiting = waiting;
}

int Player::roll() const {
	return m_roll;
}

void Player::hasVortex(bool vor) {
	m_vor = vor;
}

bool Player::vor() const {
	return m_vor;
}

std::string Player::printVor() {
	if (vor())
		return "VOR";
	return "";
}
void Player::changeRoll(int roll) {
	m_roll = roll;
}

int Player::ticks() {
	return ticks_to_move;
}

void Player::changeTicks(int ticks) {
	ticks_to_move = ticks;
}

bool Player::fresh() const {
	return m_new;
}

void Player::changeNew(bool n) {
	m_new = n;
}


void Player::valid(bool valid) {
	m_valid = valid;
}

bool Player::canBeHit() {
	return m_hit;
}

void Player::changeHit(bool hit) {
	m_hit = hit;
}

bool Player::changesDir() {
	return false;
}

void Player::swap(Player* p) {		//swap for event square's teleport
	int x = getX();
	int y = getY();
	moveTo(p->getX(), p->getY());
	p->moveTo(x, y);
	int temp = roll();
	changeRoll(p->roll());
	p->changeRoll(temp);
	temp = moveDir();
	changeDir(p->moveDir());
	p->changeDir(temp);
	temp = getDirection();
	setDirection(p->getDirection());
	p->setDirection(temp);
	temp = ticks();
	changeTicks(p->ticks());
	p->changeTicks(temp);
	bool t = waiting();
	changeState(p->waiting());
	p->changeState(t);
}

void Player::changeCoins(int coin) {
	m_coins = coin;
}

void Player::changeStars(int star) {
	m_stars = star;
}

void Player::swapCoins(Player* p) {
	int temp = m_coins;
	changeCoins(p->coins(0));
	p->changeCoins(temp);
}

void Player::swapStars(Player* p) {		//swap for boo's swap
	int temp = m_stars;
	changeStars(p->stars(0));
	p->changeStars(temp);
}

void Player::move(int x, int y, int dir, int moveDir) {		//move player if possible
	if (world()->getBoard().getContentsOf(x, y) != Board::empty) {
		setDirection(dir);
		changeDir(moveDir);
		m_fork = false;
	}
}

void Player::update() {		//updating things
	ticks_to_move--;
	if (ticks_to_move % 8 == 0) {
		m_roll--;
		m_fork = true;		//every square, update to check if at fork or not
		m_new = true;		//every square, player is new
	}
	if (m_roll < 0)
		m_roll = 0;
}

void Player::doSomething() {
	int x = getX() / 16;
	int y = getY() / 16;
	if (m_waiting) {
		if (!m_valid && world()->isDirSq(x*16, y*16) == nullptr) {		//checking for valid direction when teleported
			if (world()->getBoard().getContentsOf(x, y + 1) != Board::empty) {
				changeDir(up);
				setDirection(0);
			}
			else if (world()->getBoard().getContentsOf(x, y - 1) != Board::empty) {
				changeDir(down);
				setDirection(0);
			}
			else if (world()->getBoard().getContentsOf(x + 1, y) != Board::empty) {
				changeDir(right);
				setDirection(0);
			}
			else {
				changeDir(left);
				setDirection(180);
			}
			m_valid = true;
		}
		switch (world()->getAction(m_num)) {
		case ACTION_NONE:
			break;
		case ACTION_ROLL: {
			int roll = randInt(1, 1);
			ticks_to_move = roll * 8;
			m_roll = roll;
			m_waiting = false;
			m_new = false;
			m_fork = true;
			if (world()->isDirSq(x, y) != nullptr)
				m_fork = false;
		}
			break;
		case ACTION_FIRE: {
			if (m_vor) {
				if (moveDir() == right)
					world()->addActor(new vortex(world(), IID_VORTEX, getX() + 16, getY(), moveDir()));
				else if (moveDir() == left)
					world()->addActor(new vortex(world(), IID_VORTEX, getX() - 16, getY(), moveDir()));
				else if (moveDir() == up)
					world()->addActor(new vortex(world(), IID_VORTEX, getX(), getY() + 16, moveDir()));
				else if (moveDir() == down)
					world()->addActor(new vortex(world(), IID_VORTEX, getX(), getY() - 16, moveDir()));
				world()->playSound(SOUND_PLAYER_FIRE);
				hasVortex(false);
			}
		}
			break;
		}
		if (m_waiting == true && world()->isDirSq(getX(), getY()) != nullptr) {
			m_new = false;
		}
		else if (m_waiting)
			m_new = true;
		if (!m_valid) {
			m_valid = true;
			m_new = true;
		}
	}
	else {
		int xnew = 0;
		int	ynew = 0;
		if (moveDir() == up) {
			getPositionInThisDirection(up, 16, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (fork(xnew, ynew - 1) && m_fork)  {
				switch (world()->getAction(m_num)) {
				case ACTION_NONE:
					break;
				case ACTION_RIGHT:
					move(x + 1, y, 0, 0);
					break;
				case ACTION_LEFT:
					move(x - 1, y, 180, 180);
					break;
				case ACTION_UP:
					move(x, y + 1, 0, 90);
					break;
				}
			}
			else if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew + 1, ynew - 1) != Board::empty) {
					setDirection(right);
					changeDir(right);
					moveTo(getX() + 2, getY());
				}
				else {
					setDirection(left);
					changeDir(left);
					moveTo(getX() - 2, getY());
				}
				update();
			}
			else {
				moveTo(getX(), getY() + 2);
				update();
			}
		}
		else if (moveDir() == right) {
			getPositionInThisDirection(right, 16, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (fork(xnew - 1, ynew) && m_fork)  {
				switch (world()->getAction(m_num)) {
				case ACTION_NONE:
					break;
				case ACTION_RIGHT:
					move(x + 1, y, 0, 0);
					break;
				case ACTION_UP:
					move(x, y + 1, 0, 90);
					break;
				case ACTION_DOWN:
					move(x, y - 1, 0, 270);
					break;
				}
			}
			else if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew - 1, ynew + 1) != Board::empty) {
					setDirection(right);
					changeDir(up);
					moveTo(getX(), getY() + 2);
				}
				else {
					setDirection(right);
					changeDir(down);
					moveTo(getX(), getY() - 2);
				}
				update();
			}
			else {
				moveTo(getX() + 2, getY());
				update();
			}
		}
		else if (moveDir() == down) {
			getPositionInThisDirection(down, 1, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (fork(xnew, ynew + 1) && m_fork)  {
				switch (world()->getAction(m_num)) {
				case ACTION_NONE:
					break;
				case ACTION_RIGHT:
					move(x + 1, y, 0, 0);
					break;
				case ACTION_LEFT:
					move(x - 1, y, 180, 180);
					break;
				case ACTION_DOWN:
					move(x, y - 1, 0, 270);
					break;
				}
			}
			else if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew + 1, ynew + 1) != Board::empty) {
					setDirection(right);
					changeDir(right);
					moveTo(getX() + 2, getY());
				}
				else {
					setDirection(left);
					changeDir(left);
					moveTo(getX() - 2, getY());
				}
				update();
			}
			else if (getY() == 0) {
				if (world()->getBoard().getContentsOf(getX() + 1, getY()) != Board::empty) {
					setDirection(right);
					changeDir(right);
					moveTo(getX() + 2, getY());
				}
				else {
					setDirection(left);
					changeDir(left);
					moveTo(getX() - 2, getY());
				}
				update();
			}
			else {
				moveTo(getX(), getY() - 2);
				update();
			}
		}
		else if (moveDir() == left) {
			getPositionInThisDirection(left, 1, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (fork(xnew + 1, ynew) && m_fork) {
				switch (world()->getAction(m_num)) {
				case ACTION_NONE:
					break;
				case ACTION_LEFT:
					move(x - 1, y, 180, 180);
					break;
				case ACTION_UP:
					move(x, y + 1, 0, 90);
					break;
				case ACTION_DOWN:
					move(x, y - 1, 0, 270);
					break;
				}
			}
			else if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew + 1, ynew + 1) != Board::empty) {
					setDirection(right);
					changeDir(up);
					moveTo(getX(), getY() + 2);
				}
				else {
					setDirection(right);
					changeDir(down);
					moveTo(getX(), getY() - 2);
				}
				update();
			}
			else if (getX() == 0) {
				if (world()->getBoard().getContentsOf(getX(), getY() + 1) != Board::empty) {
					setDirection(right);
					changeDir(up);
					moveTo(getX(), getY() + 2);
				}
				else {
					setDirection(right);
					changeDir(down);
					moveTo(getX(), getY() - 2);
				}
				update();
			}
			else {
				moveTo(getX() - 2, getY());
				update();
			}
		}
	}

	if (ticks_to_move == 0)
		m_waiting = true;
}

bool Player::fork(int x, int y) {
	int count = 0;
	if (world()->getBoard().getContentsOf(x, y) != Board::down_dir_square && world()->getBoard().getContentsOf(x, y) != Board::up_dir_square && world()->getBoard().getContentsOf(x, y) != Board::right_dir_square && world()->getBoard().getContentsOf(x, y) != Board::left_dir_square) {
		if (world()->getBoard().getContentsOf(x + 1, y) != Board::empty)
			count++;
		if (world()->getBoard().getContentsOf(x - 1, y) != Board::empty)
			count++;
		if (world()->getBoard().getContentsOf(x, y + 1) != Board::empty)
			count++;
		if (world()->getBoard().getContentsOf(x, y - 1) != Board::empty)
			count++;
	}
	if (count > 2)
		return true;
	return false;
}

void Player::changeFork(bool fork) {
	m_fork = fork;
}

int Player::coins(int n) {
	m_coins += n;
	if (m_coins < 0) {
		m_coins = 0;
	}
	return m_coins;
}

int Player::stars(int s) {
	m_stars += s;
	if (m_stars < 0) 
		m_stars = 0;
	return m_stars;
}

ActObj::ActObj(StudentWorld* sw, int imageID, int x, int y, int dir, int depth, bool hit, bool changesDir)
	: Actor(sw, imageID, x, y, dir, depth, 1), m_landed(false), m_hit(hit)
{
}

void Actor::dead(bool dead) {
	m_alive = dead;
}

Player* ActObj::landsOn(int x, int y) {
	if (x == Peach()->getX() && y == Peach()->getY() && Peach()->waiting() && Peach()->fresh())
		return Peach();
	else if (x == Yoshi()->getX() && y == Yoshi()->getY() && Yoshi()->waiting() && Yoshi()->fresh())
		return Yoshi();
	return nullptr;
}
Player* ActObj::movesOn(int x, int y) {
	if (x == Peach()->getX() && y == Peach()->getY() && Peach()->fresh())
		return Peach();
	else if (x == Yoshi()->getX() && y == Yoshi()->getY() && Yoshi()->fresh())
		return Yoshi();
	return nullptr;
}
Player* ActObj::enemyLands(int x, int y) {
	if (x == Peach()->getX() && y == Peach()->getY() && Peach()->waiting())
		return Peach();
	else if (x == Yoshi()->getX() && y == Yoshi()->getY() && Yoshi()->waiting())
		return Yoshi();
	return nullptr;
}


bool ActObj::landed() {
	return m_landed;
}

void ActObj::land(bool l) {
	m_landed = l;
}

bool ActObj::canBeHit() {
	return m_hit;
}


void ActObj::changeHit(bool hit) {
	m_hit = hit;
}

bool ActObj::changesDir() {
	return m_changesDir;
}


void ActObj::doSomething() {
	if (landsOn(getX(), getY()) != nullptr) {
		m_landed = true;
	}
	else if (landsOn(getX(), getY()) == nullptr)
		m_landed = false; 
	if (movesOn(getX(), getY()) != nullptr ) 
		action();
}
CoinSquare::CoinSquare(StudentWorld* sw, int imageID, int x, int y, bool good)
	: ActObj(sw, imageID, x, y, 0, 1, false, false), m_good(good)
{
}

bool Actor::alive() const {
	return m_alive;
}

bool CoinSquare::good() {
	return m_good;
}

void CoinSquare::action() { 
	if (!alive() || !landed())
		return;
	if (m_good) {
		landsOn(getX(), getY())->coins(3);
		world()->playSound(SOUND_GIVE_COIN);
	}
	else if (!m_good) {
		landsOn(getX(), getY())->coins(-3);
		world()->playSound(SOUND_TAKE_COIN);
	}
}

StarSquare::StarSquare(StudentWorld* sw, int imageID, int x, int y)
	: ActObj(sw, imageID, x, y, 0, 1, false, false)
{
}

void StarSquare::action() {
	if (!alive())
		return;
	if (movesOn(getX(), getY())->coins(0) < 20)
		return;
	else {
		movesOn(getX(), getY())->coins(-20);
		movesOn(getX(), getY())->stars(1);
		world()->playSound(SOUND_GIVE_STAR);
	}
}


dirSquare::dirSquare(StudentWorld* sw, int imageID, int x, int y, int dir)
	: ActObj(sw, imageID, x, y, dir, 1, false, true), forceDir(dir)
{}

void dirSquare::action() {
	if (!alive())
		return;
	movesOn(getX(), getY())->changeDir(forceDir);
	if (forceDir == left)
		movesOn(getX(), getY())->setDirection(180);
	else
		movesOn(getX(), getY())->setDirection(0);
	movesOn(getX(), getY())->changeFork(false);

}

bankSquare::bankSquare(StudentWorld* sw, int imageID, int x, int y)
	:ActObj(sw, imageID, x, y, 0, 1, false, false)
{}

void bankSquare::action() {
	if (!alive())
		return;
	if (!movesOn(getX(), getY())->waiting()) {
		if (movesOn(getX(), getY())->coins(0) < 5) {
			world()->deposit_bank_coins(movesOn(getX(), getY())->coins(0));
		}
		else if (movesOn(getX(), getY())->coins(0) >= 5) {
			world()->deposit_bank_coins(5);
		}
		movesOn(getX(), getY())->coins(-5);
		world()->playSound(SOUND_DEPOSIT_BANK);
	}
	else {
		movesOn(getX(), getY())->coins(world()->get_bank_coins());
		world()->reset_bank_coins();
		world()->playSound(SOUND_WITHDRAW_BANK);
	}
}

eventSquare::eventSquare(StudentWorld* sw, int imageID, int x, int y)
	: ActObj(sw, imageID, x, y, 0, 1, false, false)
{}



void eventSquare::action()
{
	if (!alive() || !landed())
		return;
	switch (randInt(3, 3)) {
	case 1: {
		int x = randInt(0, BOARD_WIDTH - 1);
		int y = randInt(0, BOARD_HEIGHT - 1);
		while (world()->getBoard().getContentsOf(x, y) == Board::empty) {
			x = randInt(0, BOARD_WIDTH - 1);
			y = randInt(0, BOARD_HEIGHT - 1);
		}
		landsOn(getX(), getY())->valid(false);
		landsOn(getX(), getY())->moveTo(x*16, y*16);
		world()->playSound(SOUND_PLAYER_TELEPORT);
	}
		  break;
	case 2: {
		int x = getX();
		int y = getY();
		if (landsOn(getX(), getY()) == Peach()) 
			Peach()->swap(Yoshi());
		else 
			Yoshi()->swap(Peach());
		world()->playSound(SOUND_PLAYER_TELEPORT);
		land(false);
	}
		break;
	case 3: {
		world()->playSound(SOUND_GIVE_VORTEX);
		landsOn(getX(), getY())->hasVortex(true);
	}
		break;
	}
}

vortex::vortex(StudentWorld* sw, int imageID, int x, int y, int fireDir)
	:ActObj(sw, imageID, x, y, 0, 0, false, false), fireDir(fireDir)
{}

void vortex::doSomething() {
	if (!alive())
		return;
	if (fireDir == right)
		moveTo(getX() + 2, getY());
	if (fireDir == left)
		moveTo(getX() - 2, getY());
	if (fireDir == up)
		moveTo(getX(), getY() + 2);
	if (fireDir == down)
		moveTo(getX(), getY() - 2);
	if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT) {
		dead(false);
	}
	if (world()->checkOverlap(getX(), getY()) != nullptr) {
		dead(false);
		world()->playSound(SOUND_HIT_BY_VORTEX);
		world()->checkOverlap(getX(), getY())->changeHit(false);
	}
}


dropSquare::dropSquare(StudentWorld* sw, int imageID, int x, int y) 
	: ActObj(sw, imageID, x, y, 0, 1, true, false)
{
}



void dropSquare::action() {
	if (!alive() || !landed())
		return;
	switch (randInt(1, 2)) {
	case 1: {
		landsOn(getX(), getY())->coins(-10);
	}
		  break;
	case 2: {
		if (landsOn(getX(), getY())->stars(0) == 0)
			break;
		landsOn(getX(), getY())->stars(-1);

	}
		  break;
	}
	world()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}

enemy::enemy(StudentWorld* sw, int imageID, int x, int y, int depth)
	: ActObj(sw, imageID, x, y, 0, 0, true, false), m_walking(false), m_counter(180)
{}

bool enemy::walking() const {
	return m_walking;
}

void enemy::changeWalking(bool walk) {
	m_walking = walk;
}

int enemy::counter() const {
	return m_counter;
}

void enemy::changeCounter(int c) {
	m_counter = c;
}

bool enemy::changeDirection(int x, int y) {
	switch (randInt(1, 4)) {
	case 1:
		if (world()->getBoard().getContentsOf(x, y + 1) != Board::empty) {
			changeDir(up);
			setDirection(right);
			return true;
		}
		break;
	case 2:
		if (world()->getBoard().getContentsOf(x, y - 1) != Board::empty) {
			changeDir(down);
			setDirection(right);
			return true;
		}
		break;
	case 3:
		if (world()->getBoard().getContentsOf(x + 1, y) != Board::empty) {
			changeDir(right);
			setDirection(right);
			return true;
		}
		break;
	case 4:
		if (world()->getBoard().getContentsOf(x - 1, y) != Board::empty) {
			changeDir(left);
			setDirection(left);
			return true;
		}
		break;
	}
	return false;
}

bool enemy::fork(int x, int y) {
	int count = 0;
	if (world()->getBoard().getContentsOf(x + 1, y) != Board::empty)
		count++;
	if (world()->getBoard().getContentsOf(x - 1, y) != Board::empty)
		count++;
	if (world()->getBoard().getContentsOf(x, y + 1) != Board::empty)
		count++;
	if (world()->getBoard().getContentsOf(x, y - 1) != Board::empty)
		count++;
	if (count > 2)
		return true;
	return false;
}

void enemy::move(int dir) {
	if (dir == up) {
		setDirection(right);
		changeDir(up);
		moveTo(getX(), getY() + 2);
	}
	if (dir == down) {
		setDirection(right);
		changeDir(down);
		moveTo(getX(), getY() - 2);
	}
	if (dir == right) {
		setDirection(right);
		changeDir(right);
		moveTo(getX() + 2, getY());
	}
	if (dir == left) {
		setDirection(left);
		changeDir(left);
		moveTo(getX() - 2, getY());
	}
	m_fork = false;
}

void enemy::doSomething() {
	int x = getX() / 16;
	int y = getY() / 16;
	if (!m_walking) {
		if (enemyLands(getX(), getY()) != nullptr && !landed()) {
			action();
			land(true);
		}
		m_counter--;
		if (m_counter == 0) {
			if (changeDirection(x, y)) {
				int squares_to_move = getRoll();
				ticks_to_move = squares_to_move * 8;
				m_walking = true;
				m_fork = true;
				land(false);
			}
		}
		if (m_counter < 0)
			m_counter += 2;
	}
	else {
		int xnew = 0;
		int	ynew = 0;
		if (moveDir() == up) {
			getPositionInThisDirection(up, 16, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew + 1, ynew - 1) != Board::empty) {
					move(right);
				}
				else {
					move(left);
				}
				ticks_to_move--;
			}
			else if (fork(xnew, ynew - 1) && m_fork) {
				switch (randInt(1, 3)) {
				case 1:
					if (world()->getBoard().getContentsOf(xnew + 1, ynew - 1) != Board::empty) {
						setDirection(right);
						changeDir(right);
						m_fork = false;
					}
					break;
				case 2:
					if (world()->getBoard().getContentsOf(xnew - 1, ynew - 1) != Board::empty) {
						setDirection(left);
						changeDir(left);
						m_fork = false;
					}
					break;
				case 3:
					if (world()->getBoard().getContentsOf(xnew, ynew) != Board::empty) {
						setDirection(right);
						changeDir(up);
						m_fork = false;
					}
					break;
				}
			}
			else {
				moveTo(getX(), getY() + 2);
				ticks_to_move--;
			}
		}
		else if (moveDir() == down) {
			getPositionInThisDirection(down, 1, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew + 1, ynew + 1) != Board::empty) {
					move(right);

				}
				else {
					move(left);
				}
				ticks_to_move--;
			}
			else if (getY() == 0) {
				if (world()->getBoard().getContentsOf(getX() + 1, getY()) != Board::empty) {
					move(right);
				}
				else {
					move(left);
				}
				ticks_to_move--;
			}
			else if (fork(xnew, ynew + 1) && m_fork) {
				switch (randInt(1, 3)) {
				case 1:
					if (world()->getBoard().getContentsOf(xnew + 1, ynew + 1) != Board::empty) {
						setDirection(right);
						changeDir(right);
						m_fork = false;
					}
					break;
				case 2:
					if (world()->getBoard().getContentsOf(xnew - 1, ynew + 1) != Board::empty) {
						setDirection(left);
						changeDir(left);
						m_fork = false;
					}
					break;
				case 3:
					if (world()->getBoard().getContentsOf(xnew, ynew) != Board::empty) {
						setDirection(right);
						changeDir(down);
						m_fork = false;
					}
					break;
				}
				m_fork = false;
			}
			else {
				moveTo(getX(), getY() - 2);
				ticks_to_move--;
			}
		}
		else if (moveDir() == right) {
			getPositionInThisDirection(right, 16, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew - 1, ynew + 1) != Board::empty) {
					move(up);
				}
				else {
					move(down);
				}
				ticks_to_move--;
			}
			else if (fork(xnew - 1, ynew) && m_fork) {
				switch (randInt(1, 3)) {
				case 1:
					if (world()->getBoard().getContentsOf(xnew - 1, ynew + 1) != Board::empty) {
						setDirection(right);
						changeDir(up);
						m_fork = false;
					}
					break;
				case 2:
					if (world()->getBoard().getContentsOf(xnew - 1, ynew - 1) != Board::empty) {
						setDirection(right);
						changeDir(down);
						m_fork = false;
					}
					break;
				case 3:
					if (world()->getBoard().getContentsOf(xnew, ynew) != Board::empty) {
						setDirection(right);
						changeDir(right);
						m_fork = false;
					}
					break;
				}
			}
			else {
				moveTo(getX() + 2, getY());
				ticks_to_move--;
			}
		}
		else if (moveDir() == left) {
			getPositionInThisDirection(left, 1, xnew, ynew);
			xnew /= 16;
			ynew /= 16;
			if (world()->getBoard().getContentsOf(xnew, ynew) == Board::empty) {
				if (world()->getBoard().getContentsOf(xnew + 1, ynew + 1) != Board::empty) {
					move(up);
				}
				else {
					move(down);
				}
				ticks_to_move--;
			}
			else if (getX() == 0) {
				if (world()->getBoard().getContentsOf(getX(), getY() + 1) != Board::empty) {
					move(up);
				}
				else {
					move(down);
				}
				ticks_to_move--;
			}
			else if (fork(xnew + 1, ynew) && m_fork) {
				switch (randInt(1, 3)) {
				case 1:
					if (world()->getBoard().getContentsOf(xnew + 1, ynew + 1) != Board::empty) {
						setDirection(right);
						changeDir(up);
						m_fork = false;
					}
					break;
				case 2:
					if (world()->getBoard().getContentsOf(xnew + 1, ynew - 1) != Board::empty) {
						setDirection(right);
						changeDir(down);
						m_fork = false;
					}
					break;
				case 3:
					if (world()->getBoard().getContentsOf(xnew, ynew) != Board::empty) {
						setDirection(left);
						changeDir(left);
						m_fork = false;
					}
					break;
				}
			}
			else {
				moveTo(getX() - 2, getY());
				ticks_to_move--;
			}
		}
		if (ticks_to_move == 0) {
			m_walking = false;
			m_counter = 180;
			drop();
		}
	}
	if (!canBeHit()) {
		changeHit(true);
		int x = randInt(0, BOARD_WIDTH - 1);
		int y = randInt(0, BOARD_HEIGHT - 1);
		while (world()->getBoard().getContentsOf(x, y) == Board::empty) {
			x = randInt(0, BOARD_WIDTH - 1);
			y = randInt(0, BOARD_HEIGHT - 1);
		}
		world()->getEnemy(getX(), getY())->moveTo(x * 16, y * 16);
		world()->getEnemy(getX(), getY())->setDirection(right);
		world()->getEnemy(getX(), getY())->changeDir(right);
		m_counter = 180;
		m_walking = false;
	}
}

void enemy::drop() {
	return;
}

bowser::bowser(StudentWorld* sw, int imageID, int x, int y)
	: enemy(sw, imageID, x, y, 0)
{}

void bowser::action() { 
	switch (randInt(1, 2)) {
	case 1:
		break;
	case 2:
		while (enemyLands(getX(), getY())->coins(0) != 0) {
			enemyLands(getX(), getY())->coins(-100);
		}
		while (enemyLands(getX(), getY())->stars(0) != 0) {
			enemyLands(getX(), getY())->stars(-100);
		}
		world()->playSound(SOUND_BOWSER_ACTIVATE);
		break;
	}
}

void bowser::drop() {
	switch (randInt(1, 4)) {
	case 1:
		world()->getObj(getX(), getY())->dead(false);
		world()->addActor(new dropSquare(world(), IID_DROPPING_SQUARE, getX(), getY()));
		world()->playSound(SOUND_DROPPING_SQUARE_CREATED);
		break;
	}
}

int bowser::getRoll() {
	return randInt(1, 10);
}

boo::boo(StudentWorld* sw, int imageID, int x, int y)
	: enemy(sw, imageID, x, y, 0)
{}

int boo::getRoll() {
	return randInt(1, 3);
}

void boo::action() {
		switch (randInt(1, 2)) {
		case 1:
			if (enemyLands(getX(), getY()) == Peach()) {
				Peach()->swapCoins(Yoshi());
			}
			else {
				Yoshi()->swapCoins(Peach());
			}
			break;
		case 2:
			if (enemyLands(getX(), getY()) == Peach()) {
				Peach()->swapStars(Yoshi());
			}
			else {
				Yoshi()->swapStars(Peach());
			}
			break;
		}
		world()->playSound(SOUND_BOO_ACTIVATE);
}
