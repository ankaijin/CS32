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
    Actor* atPositionReverse(int x, int y, const Actor* t);
    Actor* atPosition(int x, int y, const Actor* t);
    Actor* atPos(int x, int y);
    Actor* atPositionRev(int x, int y);
    Player* getPlayer();
    int getBonus();
    void decreaseBonus();
    void collectCrystal();
    int getNumCrystals();
    void createPea(int x, int y, int dir);
    void createGoodie(int x, int y, Actor* goodie);
    void completeLevel();
    bool noObstacles(int x, int y);
    bool findObstruction(int x, int y, int dir);
    int countThiefbots(int x, int y);
    void createThiefBot(int x, int y, bool mean);
    void eraseFromList(Actor* target);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

  private:
    void setDisplayText();
    Player* m_player;
    std::list<Actor*> m_actors;
    int m_bonus;
    int numCrystals;
    bool levelCompleted;
    bool exitRevealed;
    int exitX;
    int exitY;
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

inline
int StudentWorld::getNumCrystals()
{
    return numCrystals;
}

inline
void StudentWorld::completeLevel()
{
    levelCompleted = true;
}

inline
void StudentWorld::eraseFromList(Actor* target) // I don't think this deletes the actual actor
{
    m_actors.remove(target);
}

#endif // STUDENTWORLD_H_
