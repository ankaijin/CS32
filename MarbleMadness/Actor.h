#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class Actor : public GraphObject
{
  public:
    virtual bool isObstacle() const;
    virtual bool isMarble() const;
    virtual bool isPit() const;
    virtual void doSomething() {} // should this be pure virtual?
    void changeHP(int howMuch); // should be protected but is not working
    int getHP() const;
    
  protected:
    Actor(int IID, StudentWorld* sw, int x, int y, int dir);
    StudentWorld* getWorld() const;
    
  private:
    StudentWorld* m_world;
    int m_HP;
};

class Player : public Actor
{
  public:
    Player(StudentWorld* sw, int x, int y);
    int getPeas() const;
    void doSomething();
    
  private:
    void move(int dir);
    int numPeas;
};

class Wall : public Actor
{
  public:
    Wall(StudentWorld* sw, int x, int y, int IID);
    bool isObstacle() const;
};

class Marble : public Actor
{
  public:
    Marble(StudentWorld* sw, int x, int y);
    bool isMarble() const;
};

class Pit : public Wall
{
  public:
    Pit(StudentWorld* sw, int x, int y);
    bool isPit() const;
    void doSomething();
};

class Item : public Actor
{
  public:
    Item(StudentWorld* sw, int x, int y, int IID);
};

class Crystal : public Item
{
  public:
    Crystal(StudentWorld* sw, int x, int y);
    void doSomething();
};

class ExtraLife : public Item
{
  public:
    ExtraLife(StudentWorld*sw, int x, int y);
    void doSomething();
};

class RestoreHealth : public Item
{
  public:
    RestoreHealth(StudentWorld* sw, int x, int y);
    void doSomething();
};

// maybe declare these inline function inside of their classes

inline int Actor::getHP() const
{
    return m_HP;
}

inline void Actor::changeHP(int howMuch)
{
    m_HP += howMuch;
}

inline StudentWorld* Actor::getWorld() const
{
    return m_world;
}

inline bool Actor::isObstacle() const
{
    return false;
}

inline bool Actor::isMarble() const
{
    return false;
}
inline bool Actor::isPit() const
{
    return false;
}

inline int Player::getPeas() const
{
    return numPeas;
}

inline bool Wall::isObstacle() const
{
    return true;
}

inline bool Marble::isMarble() const
{
    return true;
}

inline bool Pit::isPit() const
{
    return true;
}

#endif // ACTOR_H_
