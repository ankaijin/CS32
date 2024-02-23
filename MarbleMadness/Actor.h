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
    virtual void doSomething() {} // WEIZHEN WANG WOULD DISAPPROVE
    virtual void push(int x, int y) {}
    virtual void makeVisible() {}
    virtual bool damage();
    void changeHP(int howMuch); // should be protected but is not working
    int getHP() const;
    
  protected:
    Actor(int IID, StudentWorld* sw, int x, int y, int dir);
    void getXY(int& x, int& y, int dir);
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
    void addPeas();
    void doSomething();
    bool damage();
    
  private:
    void move(int dir);
    int numPeas;
};

class Wall : public Actor
{
  public:
    Wall(StudentWorld* sw, int x, int y);
    bool isObstacle() const;
    bool damage();
};

class Marble : public Actor // make a push method for the player to push the marble
{
  public:
    Marble(StudentWorld* sw, int x, int y);
    void push(int x, int y);
    bool isMarble() const;
    bool damage();
};

class Pit : public Actor
{
  public:
    Pit(StudentWorld* sw, int x, int y);
    bool isObstacle() const;
    bool isPit() const;
    void doSomething();
};

class Item : public Actor
{
  public:
    Item(StudentWorld* sw, int x, int y, int IID, int increaseScoreBy);
  protected:
    bool action();
  private:
    int plusScore;
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

class RestoreAmmo : public Item
{
  public:
    RestoreAmmo(StudentWorld* sw, int x, int y);
    void doSomething();
};

class Pea : public Actor
{
  public:
    Pea(StudentWorld* sw, int x, int y, int dir);
    void doSomething();
};

class Exit : public Actor
{
  public:
    Exit(StudentWorld* sw, int x, int y);
    void doSomething();
    void makeVisible();
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

inline bool Actor::damage()
{
    // ADD LATER
    return false;
}

inline int Player::getPeas() const
{
    return numPeas;
}

inline void Player::addPeas()
{
    numPeas += 20;
}

inline bool Wall::isObstacle() const
{
    return true;
}

inline bool Marble::isMarble() const
{
    return true;
}

inline bool Pit::isObstacle() const
{
    return true;
}

inline bool Pit::isPit() const
{
    return true;
}

inline void Exit::makeVisible()
{
    setVisible(true);
}

#endif // ACTOR_H_
