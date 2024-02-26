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
    Actor* adjacentActor1 = getWorld()->atPositionReverse(getX() + x, getY() + y, this);
    bool obstacle = false;  // not an obstacle if there is no actor at that position
    bool marble = false;    // not a marble if there is no actor at that position
    if (adjacentActor1 != nullptr)
    {
        obstacle = adjacentActor1->isObstacle();
        marble = adjacentActor1->isMarble();
    }
    
    if (marble)
        adjacentActor1->push(x, y);
    else if (!obstacle)
        moveTo(getX() + x, getY() + y);   // moveTo() that space
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
                int x = 0, y = 0;
                getXY(x, y, getDirection());
                getWorld()->createPea(getX() + x, getY() + y, getDirection());  // call createPea()
                numPeas--;
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                break;
            }
            case KEY_PRESS_ESCAPE:  // abort
                changeHP(-20);
                break;
            case KEY_PRESS_TAB:
                break;
            case KEY_PRESS_ENTER:
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
    if (adjacentActor2 == nullptr || adjacentActor2->isPit())
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
 : Actor(IID_PEA, sw, x, y, dir)
{
    // add stuff?
    changeHP(1);
}

void Pea::doSomething()
{
    if (getHP() <= 0) return;
    
    Actor* sameSquare = getWorld()->atPositionReverse(getX(), getY(), this);
    int x = 0, y = 0;
    // THE FOLLOWING LOGIC SHOULD BE FIXED WHEN MORE FEATURES ARE ADDED
    if (sameSquare != nullptr)  // if there is an actor on the same square
    {
        if (sameSquare->damage())   // damage it appropriately
        {
            changeHP(-1);   // destroy itself
        }
        else    // if the actor can't be damaged
        {       // move it forward
            getXY(x, y, getDirection());
            moveTo(getX() + x, getY() + y);
        }
    }
    else    // no actor on the same square
    {       // move it one square
        getXY(x, y, getDirection());
        moveTo(getX() + x, getY() + y);
    }
    
    if (getHP() <= 0) return;   // if pea has died, leave this function
    
    sameSquare = getWorld()->atPositionReverse(getX(), getY(), this);
    if (sameSquare != nullptr && sameSquare->damage())
        changeHP(-1);
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
 : Actor(IID, sw, x, y, dir), m_points(points), currTick(1), goodieType(-1)
{
    ticks = (28 - getWorld()->getLevel()) / 4;
    if (ticks < 3)
        ticks = 3;
}

bool Enemy::canRobotMove(int x, int y) const
{
    Actor* adjacentActor1 = getWorld()->atPos(x, y);
    bool obstacle = false;  // not an obstacle if there is no actor at that position
    bool marble = false;    // not a marble if there is no actor at that position
    if (adjacentActor1 != nullptr)
    {
        obstacle = adjacentActor1->isObstacle();
        marble = adjacentActor1->isMarble();
    }
    if (!obstacle && !marble)
    {
        if (getWorld()->getPlayer()->getX() != x || getWorld()->getPlayer()->getY() != y)
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
        if (goodieType != -1)
            getWorld()->createGoodie(getX(), getY(), goodieType);
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
    setGoodieType(0);   // Thiefbots will not have -1 as their goodieType
}

bool ThiefBot::stealGoodie()
{
    if (getGoodieType() == 0)   // 3: the ThiefBot has never picked up a goodie before
    {
        Actor* sameSquare = getWorld()->atPos(getX(), getY());
        if (!(sameSquare->canSteal()))
            sameSquare = nullptr;
        // insert code to check for atPosReverse if needed
        if (sameSquare != nullptr)
        {
            // implement RNG here after verifying that goodie can be picked up
            setGoodieType(sameSquare->typeOfGoodie());    // track the type of goodie collected
            sameSquare->changeHP(-1);   // delete the goodie collected at the end of the tick
            getWorld()->playSound(SOUND_ROBOT_MUNCH);
            addCurrTick(-(getTicks() - 1)); // reset currTick
            return true;
        }
    }
    return false;
}

void ThiefBot::move(int& currDist, int dir) // verify that this does the right thing
{
    int x = 0, y = 0;
    getXY(x, y, dir);

    if (currDist < distanceBeforeTurning && canRobotMove(getX() + x, getY() + y))
    {
        // move forward if it can move
        moveTo(getX() + x, getY() + y);   // moveTo() that space
        currDist++;
    }
    else    // remember to reset currDist
    {
        distanceBeforeTurning = randInt(1, 6);
        int randomDirection = randInt(0, 3) * 90;
        for (int i = 0; i < 4; i++)
        {
            // int x = 0, y = 0; I don't think I need this
            getXY(x, y, randomDirection);
            
            if (canRobotMove(getX() + x, getY() + y))
            {
                setDirection(randomDirection);
                moveTo(getX() + x, getY() + y);   // moveTo() that space
                currDist = 1;   // reset currDist
                return;
            }
            else
                randomDirection += 90;
        }
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
        move(currDistance, getDirection()); // 4 & 5
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
