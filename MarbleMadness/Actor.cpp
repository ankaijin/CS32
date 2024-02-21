#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"

Actor::Actor(int IID, StudentWorld* sw, int x, int y, int dir)
 : GraphObject(IID, x, y, dir), m_world(sw)
{
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
    switch(dir)
    {
        case GraphObject::left:
            x = -1;
            break;
        case GraphObject::right:
            x = 1;
            break;
        case GraphObject::up:
            y = 1;
            break;
        case GraphObject::down:
            y = -1;
            break;
    }
    
    setDirection(dir);    // make avatar face in the proper direction
    Actor* adjacentActor1 = getWorld()->atPosition(getX() + x, getY() + y);
    bool obstacle = false;  // not an obstacle if there is no actor at that position
    bool marble = false;    // not a marble if there is no actor at that position
    if (adjacentActor1 != nullptr)
    {
        obstacle = adjacentActor1->isObstacle();
        marble = adjacentActor1->isMarble();
    }
    
    if (marble) // fix unneeded code and check for edge cases
    {
        Actor* adjacentActor2 = getWorld()->atPosition(getX() + (2 * x), getY() + (2 * y));
        if (adjacentActor2 == nullptr || adjacentActor2->isPit())
        {
            adjacentActor1->moveTo(getX() + (2 * x), getY() + (2 * y));
            moveTo(getX() + x, getY() + y);
        }
    }
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
            case KEY_PRESS_SPACE:   // fire a pea
                break;
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

Pit::Pit(StudentWorld* sw, int x, int y)
 : Wall(sw, x, y, IID_PIT)
{    // will be alive until filled by a marble
}

void Pit::doSomething()
{
    if (getHP() <= 0) return;
    
    Actor* marble = getWorld()->atPosition(getX(), getY());
    if (!(marble->isMarble()))
        marble = nullptr;
    if (marble == nullptr)
    {
        marble = getWorld()->atPositionReverse(getX(), getY());
        if (!(marble->isMarble()))
            marble = nullptr;
    }
    
    if (marble != nullptr)  // marble is on the pit
    {
        changeHP(-1);
        marble->changeHP(-10);
    }
}

Item::Item(StudentWorld* sw, int x, int y, int IID) // all items have no direction and 1 hp
 : Actor(IID, sw, x, y, GraphObject::none)
{
    changeHP(1);    // will stay alive until collected
}

Crystal::Crystal(StudentWorld* sw, int x, int y)
 : Item(sw, x, y, IID_CRYSTAL)
{
}

void Crystal::doSomething()
{
    if (getHP() <= 0) return;
    
    if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
    {
        getWorld()->increaseScore(50);
        changeHP(-1);
        getWorld()->collectCrystal();
        getWorld()->playSound(SOUND_GOT_GOODIE);
    }
}

ExtraLife::ExtraLife(StudentWorld* sw, int x, int y)
 : Item(sw, x, y, IID_EXTRA_LIFE)
{
}

void ExtraLife::doSomething()
{
    if (getHP() <= 0) return;
    
    if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
    {
        getWorld()->increaseScore(1000);
        changeHP(-1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->incLives();
    }
}

RestoreHealth::RestoreHealth(StudentWorld* sw, int x, int y)
 : Item(sw, x, y, IID_RESTORE_HEALTH)
{
}

void RestoreHealth::doSomething()
{
    if (getHP() <= 0) return;
    
    if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
    {
        getWorld()->increaseScore(500);
        changeHP(-1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        int difference = 20 - (getWorld()->getPlayer()->getHP());
        getWorld()->getPlayer()->changeHP(difference);
    }
}

RestoreAmmo::RestoreAmmo(StudentWorld* sw, int x, int y)
 : Item(sw, x, y, IID_AMMO)
{
}

void RestoreAmmo::doSomething()
{
    if (getHP() <= 0) return;
    
    if (getWorld()->getPlayer()->getX() == getX() && getWorld()->getPlayer()->getY() == getY())
    {
        getWorld()->increaseScore(100);
        changeHP(-1);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->getPlayer()->addPeas();
    }
}
