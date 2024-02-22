#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <list>
#include <string>

class Actor;
class Player;
class Pea; // am I allowed to add this?

class StudentWorld : public GameWorld
{
  public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    Actor* atPosition(int x, int y);
    Actor* atPositionReverse(int x, int y);
    Player* getPlayer();
    int getBonus();
    void decreaseBonus();
    void collectCrystal();
    void createPea(int x, int y, int dir);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

  private:
    void setDisplayText();
    Player* m_player;
    std::list<Actor*> m_actors;
    int m_bonus;
    int numCrystals;
};

inline
Player* StudentWorld::getPlayer()
{
    return m_player;
}

inline
int StudentWorld::getBonus()
{
    return m_bonus;
}

inline
void StudentWorld::decreaseBonus()
{
    if (m_bonus > 0)
        m_bonus--;
}

inline
void StudentWorld::collectCrystal()
{
    numCrystals--;
}

#endif // STUDENTWORLD_H_
