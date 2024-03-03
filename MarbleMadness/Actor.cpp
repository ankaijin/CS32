#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

Actor::Actor(int IID, StudentWorld* sw, int x, int y, int dir)
 : GraphObject(IID, x, y, dir), m_world(sw)
{
}

void Actor::getXY(int& x, int& y, int dir)
{
    switch(dir)
    {
        case GraphObject::left:
            x = -1;
            y = 0;
            break;
        case GraphObject::right:
            x = 1;
            y = 0;
            break;
        case GraphObject::up:
            y = 1;
            x = 0;
            break;
        case GraphObject::down:
            y = -1;
            x = 0;
            break;
    }
}

Player::Player(StudentWorld* sw, int x, int y)
 : Actor(IID_PLAYER, sw, x, y, GraphObject::right)
{
    changeHP(20);   // change back to 20
    numPeas = 20;
}

void Player::move(int dir)
{
    int x = 0, y = 0;
    getXY(x, y, dir);
    
    setDirection(dir);    // make avatar face in the proper direction
    bool marble = false;    // not a marble if there is no actor at that position
    Actor* adjacentActor1 = nullptr;
    if (getWorld()->noObstacles(getX() + x, getY() + y))
    {
        adjacentActor1 = getWorld()->atPos(getX() + x, getY() + y);
        if (adjacentActor1 != nullptr)
            marble = adjacentActor1->isMarble();
        
        if (marble)
            adjacentActor1->push(x, y);
        else
            moveTo(getX() + x, getY() + y);   // moveTo() that space
    }
    // don't move if there is an obstacle
}

void Player::doSomething()
{
    if (getHP() <= 0)
        return;
    
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch(ch)
        {
            case KEY_PRESS_LEFT:
                move(GraphObject::left);    // make avatar face left
                break;
            case KEY_PRESS_RIGHT:
                move(GraphObject::right);    // make avatar face left
                break;
            case KEY_PRESS_UP:
                move(GraphObject::up);    // make avatar face left
                break;
            case KEY_PRESS_DOWN:
                move(GraphObject::down);    // make avatar face left
                break;
            case KEY_PRESS_SPACE:   
            {
                // fire a pea
                // create new pea one block in front of the direction the player is facing
                // decrease numPeas by 1
                if (numPeas > 0)
                {
                    int x = 0, y = 0;
                    getXY(x, y, getDirection());
                    getWorld()->createPea(getX() + x, getY() + y, getDirection());  // call createPea()
                    numPeas--;
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                }
                break;
            }
            case KEY_PRESS_ESCAPE:  // abort
                changeHP(-20);
                break;
        }
    }
}

bool Player::damage()
{
    changeHP(-2);
    if (getHP() > 0)
        getWorld()->playSound(SOUND_PLAYER_IMPACT);
    else
        getWorld()->playSound(SOUND_PLAYER_DIE);    // also set player's state to dead
    return true;
}

Wall::Wall(StudentWorld* sw, int x, int y, int IID)
 : Actor(IID, sw, x, y, GraphObject::none)
{
    changeHP(1);    // will always be alive
}

Marble::Marble(StudentWorld* sw, int x, int y)
 : Actor(IID_MARBLE, sw, x, y, GraphObject::none)
{
    changeHP(10);
}

void Marble::push(int x, int y)
{
    Actor* adjacentActor2 = getWorld()->atPositionReverse(getX() + x, getY() + y, this);
    if (adjacentActor2 == nullptr || adjacentActor2->isPit())   // empty spot or pit
    {
        moveTo(getX() + x, getY() + y);
        getWorld()->getPlayer()->moveTo(getX() - x, getY() - y);
    }
}

bool Marble::damage()
{
    changeHP(-2);
    return true;
}

Pit::Pit(StudentWorld* sw, int x, int y)
 : Actor(IID_PIT, sw, x, y, GraphObject::none)
{
    changeHP(1);    // will be alive until filled by a marble
}

void Pit::doSomething()
{
    if (getHP() <= 0) return;
    // the following assumes that pea, marble, and pit will never be on the same spot
    Actor* marble = getWorld()->atPos(getX(), getY());
    if (!(marble->isMarble()))
        marble = nullptr;
    if (marble == nullptr)
    {
        marble = getWorld()->atPositionRev(getX(), getY());
        if (!(marble->isMarble()))
            marble = nullptr;
    }
    
    if (marble != nullptr)  // marble is on the pit
    {
        changeHP(-1);
        marble->changeHP(-10);
    }
}

Item::Item(StudentWorld* sw, int x, int y, int IID, int IncreaseScoreBy) // all items have no direction and 1 hp
 : Actor(IID, sw, x, y, GraphObject::none), plusScore(IncreaseScoreBy)
{
    changeHP(1);    // will stay alive until collected
}

bool Item::action()
{
    if (getHP() <= 0) return false;
    
    if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
    {
        getWorld()->increaseScore(plusScore);
        changeHP(-1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        return true;
    }
    
    return false;
}

Crystal::Crystal(StudentWorld* sw, int x, int y)
 : Item(sw, x, y, IID_CRYSTAL, 50)
{
}

void Crystal::doSomething()
{
    if (action())
        getWorld()->collectCrystal();
}

Goodie::Goodie(StudentWorld* sw, int x, int y, int IID, int increaseScoreBy)
 : Item(sw, x, y, IID, increaseScoreBy)
{
}

ExtraLife::ExtraLife(StudentWorld* sw, int x, int y)
 : Goodie(sw, x, y, IID_EXTRA_LIFE, 1000)
{
}

void ExtraLife::doSomething()
{
    if (action())
        getWorld()->incLives();
}

RestoreHealth::RestoreHealth(StudentWorld* sw, int x, int y)
 : Goodie(sw, x, y, IID_RESTORE_HEALTH, 500)
{
}

void RestoreHealth::doSomething()
{
    if (action())
    {
        int difference = 20 - (getWorld()->getPlayer()->getHP());
        getWorld()->getPlayer()->changeHP(difference);
    }
}

RestoreAmmo::RestoreAmmo(StudentWorld* sw, int x, int y)
 : Goodie(sw, x, y, IID_AMMO, 100)
{
}

void RestoreAmmo::doSomething()
{
    if (action())
        getWorld()->getPlayer()->addPeas();
}

Pea::Pea(StudentWorld* sw, int x, int y, int dir)
 : Actor(IID_PEA, sw, x, y, dir), justCreated(true)
{
    // add stuff?
    changeHP(1);
}

void Pea::doSomething() // Verified: pea hits robot when it is on top of newly created goodie
{
    if (justCreated)
    {   // for animation purposes
        justCreated = false;
        return;
    }
    
    if (getHP() <= 0) return;   // check if it's currently alive
    
    Actor* sameSquare = getWorld()->atPositionReverse(getX(), getY(), this);
    int x = 0, y = 0;

    if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
    {   // if player is on the same square
        changeHP(-1);
        getWorld()->getPlayer()->damage();
    }
    else if (sameSquare != nullptr)  // if there is an actor on the same square
    {
        if (sameSquare->damage())   // damage it appropriately
            changeHP(-1);   // destroy itself
        else
        {   // check for two objects on same square
            sameSquare = getWorld()->atPos(getX(), getY());
            if (sameSquare->damage())
                changeHP(-1);
            else
            {   // no damageable actor on the same square, move it forward
                getXY(x, y, getDirection());
                moveTo(getX() + x, getY() + y);
            }
        }
    }
    else    // no actor on the same square
    {       // move it one square
        getXY(x, y, getDirection());
        moveTo(getX() + x, getY() + y);
    }
    
    if (getHP() <= 0) return;   // if pea has died, leave this function
    
    if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
    {   // if player is on the same square
        changeHP(-1);
        getWorld()->getPlayer()->damage();
    }
    else
    {
        sameSquare = getWorld()->atPositionReverse(getX(), getY(), this);
        if (sameSquare != nullptr)
        {
            if (sameSquare->damage())
                changeHP(-1);
            else
            {
                sameSquare = getWorld()->atPos(getX(), getY());
                if (sameSquare->damage())
                    changeHP(-1);
            }
        }
    }
}

Exit::Exit(StudentWorld* sw, int x, int y)
 : Actor(IID_EXIT, sw, x, y, GraphObject::none)
{
    changeHP(1);    // always alive
    setVisible(false);
}

void Exit::doSomething()
{
    if (getWorld()->getNumCrystals() == 0)
    {
        if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
        {
            getWorld()->playSound(SOUND_FINISHED_LEVEL);
            getWorld()->increaseScore(2000);
            getWorld()->completeLevel();    // tell StudentWorld that player must also receive bonus points
        }
    }
}

Enemy::Enemy(StudentWorld* sw, int x, int y, int IID, int dir, int points)
 : Actor(IID, sw, x, y, dir), m_points(points), currTick(1), m_goodie(nullptr)
{
    ticks = (28 - getWorld()->getLevel()) / 4;
    if (ticks < 3)
        ticks = 3;
}

Enemy::~Enemy()
{
    if (m_goodie != nullptr)
        delete m_goodie;
}

bool Enemy::canRobotMove(int x, int y) const    // should be good
{
    bool marble = false;
    Actor* adjacentActor1 = nullptr;
    if (getWorld()->noObstacles(x, y))
    {
        adjacentActor1 = getWorld()->atPosition(x, y, this);
        if (adjacentActor1 != nullptr)
            marble = adjacentActor1->isMarble();
        
        if (!marble && (getWorld()->getPlayer()->getX() != x || getWorld()->getPlayer()->getY() != y))
            return true;
    }
    return false;
}

bool Enemy::shoot(int direction)
{
    switch(direction)
    {
        case GraphObject::right:
            if (!(getWorld()->findObstruction(getX(), getY(), getDirection())))
            {
                if (getWorld()->getPlayer()->getY() == getY() && getWorld()->getPlayer()->getX() > getX())
                {
                    getWorld()->createPea(getX() + 1, getY(), getDirection());  // call createPea()
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    addCurrTick(-(getTicks() - 1));
                    return true;
                }
            }
            break;
        case GraphObject::left:
            if (!(getWorld()->findObstruction(getX(), getY(), getDirection())))
            {
                if (getWorld()->getPlayer()->getY() == getY() && getWorld()->getPlayer()->getX() < getX())
                {
                    getWorld()->createPea(getX() - 1, getY(), getDirection());  // call createPea()
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    addCurrTick(-(getTicks() - 1));
                    return true;
                }
            }
            break;
        case GraphObject::up:
            if (!(getWorld()->findObstruction(getX(), getY(), getDirection())))
            {
                if (getWorld()->getPlayer()->getY() > getY() && getWorld()->getPlayer()->getX() == getX())
                {
                    getWorld()->createPea(getX(), getY() + 1, getDirection());  // call createPea()
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    addCurrTick(-(getTicks() - 1));
                    return true;
                }
            }
            break;
        case GraphObject::down:
            if (!(getWorld()->findObstruction(getX(), getY(), getDirection())))
            {
                if (getWorld()->getPlayer()->getY() < getY() && getWorld()->getPlayer()->getX() == getX())
                {
                    getWorld()->createPea(getX(), getY() - 1, getDirection());  // call createPea()
                    getWorld()->playSound(SOUND_ENEMY_FIRE);
                    addCurrTick(-(getTicks() - 1));
                    return true;
                }
            }
            break;
    }
    return false;
}

bool Enemy::damage()
{   // make sure to also spawn in the goodie in case the ThiefBot dies after
    changeHP(-2);
    if (getHP() > 0)
        getWorld()->playSound(SOUND_ROBOT_IMPACT);
    else
    {
        if (m_goodie != nullptr)
        {
            getWorld()->createGoodie(getX(), getY(), m_goodie);   // drop the goodie
            m_goodie = nullptr; // enemy no longer has a goodie
        }
        getWorld()->playSound(SOUND_ROBOT_DIE);    // also set robot's state to dead
        getWorld()->increaseScore(m_points); // also give player points here?
    }
    return true;
}

RageBot::RageBot(StudentWorld* sw, int x, int y, int dir)
 : Enemy(sw, x, y, IID_RAGEBOT, dir, 100)
{
    changeHP(10);
}

void RageBot::move(int dir)
{
    int x = 0, y = 0;
    getXY(x, y, dir);
    
    if (canRobotMove(getX() + x, getY() + y))
        moveTo(getX() + x, getY() + y);   // moveTo() that space
    else
        setDirection(getDirection() + 180);
    // don't move if there is an obstacle
}

void RageBot::doSomething()
{
    if (getHP() <= 0) return;
    
    if (getCurrTick() != getTicks())
    {
        addCurrTick(1);
        return;
    }
    
    if (!shoot(getDirection()))
    {
        move(getDirection());
        addCurrTick(-(getTicks() - 1));
    }
}

ThiefBot::ThiefBot(StudentWorld* sw, int x, int y, int IID)
 : Enemy(sw, x, y, IID, GraphObject::right, 10), currDistance(0)
{
    changeHP(5);
    distanceBeforeTurning = randInt(1, 6);
}

bool ThiefBot::stealGoodie()    // check this function and Enemy::damage()
{
    if (getGoodie() == nullptr)   // 3: the ThiefBot has never picked up a goodie before
    {
        Actor* sameSquare = getWorld()->atPosition(getX(), getY(), this);
        if (sameSquare != nullptr && !(sameSquare->canSteal())) // non-stealable item
            sameSquare = nullptr;
        // insert code to check for atPosReverse if needed
        if (sameSquare != nullptr)
        {
            // implement RNG here after verifying that goodie can be picked up
            int munch = randInt(1, 10);
            if (munch == 3)
            {
                setGoodie(sameSquare);    // track the type of goodie collected
                sameSquare->setVisible(false);
                getWorld()->eraseFromList(sameSquare);
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
                addCurrTick(-(getTicks() - 1)); // reset currTick
                return true;
            }
        }
    }
    return false;
}

void ThiefBot::move(int dir) // verified that this does the right thing
{
    int x = 0, y = 0;
    getXY(x, y, dir);

    if (currDistance < distanceBeforeTurning && canRobotMove(getX() + x, getY() + y))
    {
        // move forward if it can move
        moveTo(getX() + x, getY() + y);   // moveTo() that space
        incCurrDist();
    }
    else    // remember to reset currDist
    {
        distanceBeforeTurning = randInt(1, 6);
        int randomDirection = randInt(0, 3) * 90;
        for (int i = 0; i < 4; i++)
        {
            x = 0;
            y = 0; // I don't think I need this
            setDirection(randomDirection);
            getXY(x, y, getDirection());
            
            if (canRobotMove(getX() + x, getY() + y))
            {
                moveTo(getX() + x, getY() + y);   // moveTo() that space
                currDistance = 1;   // reset currDist
                return;
            }
            else
                randomDirection += 90;
        }
        currDistance = 0;
        setDirection(randomDirection);  // obstructions in all 4 directions
    }
}

void ThiefBot::doSomething()    // should test this behavior extensively
{
    if (getHP() <= 0) return;   // 1
    
    if (getCurrTick() != getTicks())    // 2
    {
        addCurrTick(1);
        return;
    }
    
    if (!stealGoodie())
    {
        move(getDirection()); // 4 & 5
        addCurrTick(-(getTicks() - 1)); // reset currTick
    }
}

MeanThiefBot::MeanThiefBot(StudentWorld* sw, int x, int y)
 : ThiefBot(sw, x, y, IID_MEAN_THIEFBOT)
{
    changeHP(3);    // MeanThiefBot has 3 more hp than regular ThiefBot
}

void MeanThiefBot::doSomething()
{
    if (getHP() <= 0) return;   // 1
    
    if (getCurrTick() != getTicks())    // 2
    {
        addCurrTick(1);
        return;
    }
    
    if (shoot(getDirection()))  // 3
        return;
        
    if (stealGoodie())
        return;
    
    move(getDirection());
    addCurrTick(-(getTicks() - 1)); // reset currTick
}

ThiefBotFactory::ThiefBotFactory(StudentWorld* sw, int x, int y, bool mean)
 : Wall(sw, x, y, IID_ROBOT_FACTORY), meanThiefBots(mean)
{
}

void ThiefBotFactory::doSomething()
{
    if((getWorld()->countThiefbots(getX(), getY()) < 3) && 
       getWorld()->atPositionRev(getX(), getY())->isThiefBot() == false)
    {
        int random = randInt(1, 50);
        if (random == 10)   // 1 in 50 chance
        {
            getWorld()->createThiefBot(getX(), getY(), meanThiefBots);
            getWorld()->playSound(SOUND_ROBOT_BORN);
        }
    }
}
