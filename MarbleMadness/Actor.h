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
    virtual bool isThiefBot() const;
    virtual bool robotFireAt() const;
    virtual bool canSteal() const;
    virtual int typeOfGoodie() const;   // extra life: 1, restore health: 2, ammo: 3, default: 0
    virtual void doSomething() {}
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
    bool isObstacle() const;
    bool damage();
    
  private:
    void move(int dir);
    int numPeas;
};

class Wall : public Actor
{
  public:
    Wall(StudentWorld* sw, int x, int y, int IID);
    bool isObstacle() const;
    bool robotFireAt() const;
    bool damage();
};

class Marble : public Actor // make a push method for the player to push the marble
{
  public:
    Marble(StudentWorld* sw, int x, int y);
    void push(int x, int y);
    bool isMarble() const;
    bool robotFireAt() const;
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
  protected:
    Item(StudentWorld* sw, int x, int y, int IID, int increaseScoreBy);
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

class Goodie : public Item
{
  public:
    bool canSteal() const;
    
  protected:
    Goodie(StudentWorld* sw, int x, int y, int IID, int increaseScoreBy);
};

class ExtraLife : public Goodie
{
  public:
    ExtraLife(StudentWorld*sw, int x, int y);
    int typeOfGoodie() const;
    void doSomething();
};

class RestoreHealth : public Goodie
{
  public:
    RestoreHealth(StudentWorld* sw, int x, int y);
    int typeOfGoodie() const;
    void doSomething();
};

class RestoreAmmo : public Goodie
{
  public:
    RestoreAmmo(StudentWorld* sw, int x, int y);
    int typeOfGoodie() const;
    void doSomething();
};

class Pea : public Actor
{
  public:
    Pea(StudentWorld* sw, int x, int y, int dir);
    void doSomething();
    
  private:
    bool justCreated;
};

class Exit : public Actor
{
  public:
    Exit(StudentWorld* sw, int x, int y);
    void doSomething();
    void makeVisible();
};

class Enemy : public Actor
{
  public:
    bool isObstacle() const;
    bool robotFireAt() const;
    bool canRobotMove(int x, int y) const;
    int getCurrTick() const;
    int getTicks() const;
    void addCurrTick(int howMuch);
    int getGoodieType() const;
    void setGoodieType(int t);
    bool shoot(int direction);
    bool damage();
    
  protected:
    Enemy(StudentWorld* sw, int x, int y, int IID, int dir, int points);
    
  private:
    int m_points;
    int ticks;
    int currTick;
    int goodieType; // only ThiefBot will use this
};

class RageBot : public Enemy
{
  public:
    RageBot(StudentWorld* sw, int x, int y, int dir);
    void doSomething();
    
  private:
    void move(int dir);
};

class ThiefBot : public Enemy
{
  public:
    ThiefBot(StudentWorld* sw, int x, int y, int IID);
    bool isThiefBot() const;
    void incCurrDist();
    virtual void doSomething();
    
  protected:
    bool stealGoodie();
    void move(int dir);
    
  private:
    int currDistance;
    int distanceBeforeTurning;
};

class MeanThiefBot : public ThiefBot
{
  public:
    MeanThiefBot(StudentWorld* sw, int x, int y);
    void doSomething();
};

class ThiefBotFactory : public Wall
{
  public:
    ThiefBotFactory(StudentWorld* sw, int x, int y, bool mean);
    void doSomething();
  private:
    bool meanThiefBots;
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

inline bool Actor::isThiefBot() const
{
    return false;
}

inline bool Actor::damage()
{
    // ADD LATER
    return false;
}

inline bool Actor::robotFireAt() const
{
    return true;
}

inline bool Actor::canSteal() const
{
    return false;
}

inline int Actor::typeOfGoodie() const
{
    return 0;
}

inline int Player::getPeas() const
{
    return numPeas;
}

inline void Player::addPeas()
{
    numPeas += 20;
}

inline bool Player::isObstacle() const
{
    return true;
}

inline bool Wall::damage()
{
    return true;
}

inline bool Wall::robotFireAt() const
{
    return false;
}

inline bool Wall::isObstacle() const
{
    return true;
}

inline bool Marble::isMarble() const
{
    return true;
}

inline bool Marble::robotFireAt() const
{
    return false;
}

inline bool Pit::isObstacle() const
{
    return true;
}

inline bool Pit::isPit() const
{
    return true;
}

inline bool Goodie::canSteal() const
{
    return true;
}

inline int ExtraLife::typeOfGoodie() const
{
    return 1;
}

inline int RestoreHealth::typeOfGoodie() const
{
    return 2;
}

inline int RestoreAmmo::typeOfGoodie() const
{
    return 3;
}

inline void Exit::makeVisible()
{
    setVisible(true);
}

inline bool Enemy::isObstacle() const
{
    return true;
}

inline bool Enemy::robotFireAt() const
{
    return false;
}

inline int Enemy::getCurrTick() const
{
    return currTick;
}

inline int Enemy::getTicks() const
{
    return ticks;
}

inline void Enemy::addCurrTick(int howMuch)
{
    currTick += howMuch;
}

inline int Enemy::getGoodieType() const
{
    return goodieType;
}

inline void Enemy::setGoodieType(int t)
{
    goodieType = t;
}

inline bool ThiefBot::isThiefBot() const
{
    return true;
}

inline void ThiefBot::incCurrDist()
{
    currDistance++;
}
#endif // ACTOR_H_
