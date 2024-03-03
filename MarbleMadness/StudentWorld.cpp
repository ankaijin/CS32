#include "StudentWorld.h"
#include "Actor.h"
#include "GameConstants.h"
#include "Level.h"  // needed to load in the level with init()
#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_bonus(1000), numCrystals(0), levelCompleted(false), exitRevealed(false)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

Actor* StudentWorld::atPositionReverse(int x, int y, const Actor* t)
{
    list<Actor*>::reverse_iterator it;
    for (it = m_actors.rbegin(); it != m_actors.rend(); it++)
    {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != t)
            return (*it);
    }
    return nullptr;
}

Actor* StudentWorld::atPosition(int x, int y, const Actor* t)
{
    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != t)
            return (*it);
    }
    return nullptr;
}

Actor* StudentWorld::atPos(int x, int y)
{
    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->getX() == x && (*it)->getY() == y)
            return (*it);
    }
    return nullptr;
}

Actor* StudentWorld::atPositionRev(int x, int y)
{
    list<Actor*>::reverse_iterator it;
    for (it = m_actors.rbegin() ; it != m_actors.rend(); it++)
    {
        if ((*it)->getX() == x && (*it)->getY() == y)
            return (*it);
    }
    return nullptr;
}

void StudentWorld::setDisplayText() // read up on stringstreams
{

    int score = getScore();
    int level = getLevel();
    int livesLeft = getLives();
    double healthPercent = (m_player->getHP() / 20.0) * 100;
    int peasLeft = m_player->getPeas();
    unsigned int bonus = getBonus();
    
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << score << "  ";
    oss << "Level: " << setw(2) << level << "  ";
    oss.fill(' ');
    oss << "Lives: " << setw(2) << livesLeft << "  ";
    oss << "Health: " << setw(3) << healthPercent << "%  ";
    oss << "Ammo: " << setw(3) << peasLeft << "  ";
    oss << "Bonus: " << setw(4) << bonus;

    // Next, create a string from your statistics, of the form:
    // Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34
    string s = oss.str();
    // Finally, update the display text at the top of the screen with your
    // newly created stats
    setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

void StudentWorld::createPea(int x, int y, int dir)    // CHECK
{
    m_actors.push_back(new Pea(this, x, y, dir));
}

void StudentWorld::createGoodie(int x, int y, Actor* goodie)
{
    // add implementation here
    m_actors.push_back(goodie);   // push the pointer back into the list
    goodie->moveTo(x, y); // update its position
    goodie->setVisible(true);   // set it back to visible
}

bool StudentWorld::noObstacles(int x, int y)   // return true if there is obstacle, false otherwise
{
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->getY() == y && (*it)->getX() == x && (*it)->isObstacle())
            return false;
        it++;
    }
    return true;
}

bool StudentWorld::findObstruction(int x, int y, int dir)
{
    int playerX = m_player->getX();
    int playerY = m_player->getY();
    list<Actor*>::iterator it = m_actors.begin();
    switch(dir)
    {
        case GraphObject::right:    // fix logic
            while (it != m_actors.end())
            {
                if ((*it)->getY() == y && (*it)->getX() > x && (*it)->getX() < playerX && !((*it)->robotFireAt()))
                    return true;
                else
                    it++;
            }
            break;
        case GraphObject::left:
            while (it != m_actors.end())
            {
                if ((*it)->getY() == y && (*it)->getX() < x && (*it)->getX() > playerX && !((*it)->robotFireAt()))
                    return true;
                else
                    it++;
            }
            break;
        case GraphObject::up:
            while (it != m_actors.end())
            {
                if ((*it)->getY() > y && (*it)->getY() < playerY && (*it)->getX() == x && !((*it)->robotFireAt()))
                    return true;
                else
                    it++;
            }
            break;
        case GraphObject::down:
            while (it != m_actors.end())
            {
                if ((*it)->getY() < y && (*it)->getY() > playerY && (*it)->getX() == x && !((*it)->robotFireAt()))
                    return true;
                else
                    it++;
            }
            break;
    }
    return false;
}

int StudentWorld::countThiefbots(int x, int y)
{
    int count = 0;
    
    list<Actor*>::iterator it = m_actors.begin();
    while (it != m_actors.end())
    {
        if ((*it)->countsInFactoryCensus())
            if ((*it)->getX() >= (x - 3) && (*it)->getX() <= (x + 3))
                if ((*it)->getY() >= (y - 3) && (*it)->getY() <= (y + 3))
                    count++;
        it++;
    }
    return count;
}

void StudentWorld::createThiefBot(int x, int y, bool mean)
{
    if (mean == false)
        m_actors.push_back(new ThiefBot(this, x, y, IID_THIEFBOT));
    else
        m_actors.push_back(new MeanThiefBot(this, x, y));
}

int StudentWorld::init()    // initializes level
{
    ostringstream oss;
    oss.fill('0');
    oss << setw(2) << getLevel(); // getLevel()
    Level lev(assetPath());
    string s = oss.str();
    string curLevel = "Level" + s + ".txt";
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found || getLevel() > 99)
        return GWSTATUS_PLAYER_WON;
    if (result == Level:: load_fail_bad_format)
        return -1; // something bad happened!
    
    // otherwise the load was successful and you can access the
    // contents of the level – here’s an example
    for (int column = 0; column < VIEW_WIDTH; column++)
    {
        for (int row = 0; row < VIEW_HEIGHT; row++)
        {
            Level::MazeEntry ge = lev.getContentsOf(column, row);
            switch(ge)
            {
                case Level::empty:
                    break;
                case Level::exit:
                    m_actors.push_back(new Exit(this, column, row));
                    exitX = column;
                    exitY = row;
                    break;
                case Level::horiz_ragebot:
                    m_actors.push_back(new RageBot(this, column, row, GraphObject::right));
                    break;
                case Level::vert_ragebot:
                    m_actors.push_back(new RageBot(this, column, row, GraphObject::down));
                    break;
                case Level::thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(this, column, row, false));
                    break;
                case Level::mean_thiefbot_factory:
                    m_actors.push_back(new ThiefBotFactory(this, column, row, true));
                    break;
                case Level::wall:
                    m_actors.push_back(new Wall(this, column, row, IID_WALL));
                    break;
                case Level::marble:
                    m_actors.push_back(new Marble(this, column, row));
                    break;
                case Level::pit:
                    m_actors.push_back(new Pit(this, column, row));
                    break;
                case Level::crystal:
                    m_actors.push_back(new Crystal(this, column, row));
                    numCrystals++;
                    break;
                case Level::restore_health:
                    m_actors.push_back(new RestoreHealth(this, column, row));
                    break;
                case Level::extra_life:
                    m_actors.push_back(new ExtraLife(this, column, row));
                    break;
                case Level::ammo:
                    m_actors.push_back(new RestoreAmmo(this, column, row));
                    break;
                case Level::player:
                    m_player = new Player(this, column, row);
                    break;
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()    // facilitates gameplay
{

// Update the Game Status Line
    setDisplayText(); // update the score/lives/level text at screen top
 // The term "actors" refers to all robots, the player, Goodies,
 // Marbles, Crystals, Pits, Peas, the exit, etc.
 // Give each actor a chance to do something
    
    for (Actor* currActor : m_actors)   // for each of the actors in the game world
    {
        if (currActor->getHP() > 0) //  if (actor[i] is still active/alive)
        {
            currActor->doSomething();   //  ask each actor to do something (e.g. move)
            if (m_player->getHP() <= 0) // if (thePlayerDiedDuringThisTick()) return GWSTATUS_PLAYER_DIED;
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
        }
    }
    
    m_player->doSomething();

// Remove newly-dead actors after each tick
 
    list<Actor*>::iterator it = m_actors.begin();   // delete dead game objects
    while (it != m_actors.end())
    {
        if ((*it)->getHP() <= 0)
        {
            delete (*it);
            it = m_actors.erase(it);
        }
        else
            it++;
    }
// Reduce the current bonus for the Level by one
    decreaseBonus();

// If the player has collected all of the crystals on the level, then we
// must expose the exit so the player can advance to the next level
    if (numCrystals == 0 && !exitRevealed)
    {
        exitRevealed = true;
        // exposeTheExitInTheMaze(), make the exit Active
        atPos(exitX, exitY)->makeVisible();
        playSound(SOUND_REVEAL_EXIT);
    }

// return the proper result
    if (m_player->getHP() <= 0)
    {
        decLives();
        return GWSTATUS_PLAYER_DIED;
    }
/*
 if (thePlayerCompletedTheCurrentLevel())
 {
    increaseScoreAppropriately();
    return GWSTATUS_FINISHED_LEVEL;
 }
 */
    if (levelCompleted)
    {
        increaseScore(getBonus());
        return GWSTATUS_FINISHED_LEVEL;
    }
// the player hasn’t completed the current level and hasn’t died, so
// continue playing the current level
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()    // deletes everything
{
    list<Actor*>::iterator it;    // use iterator to delete all items in Actor* list
    it = m_actors.begin();
    while (it != m_actors.end())
    {
        delete *it;
        it = m_actors.erase(it);  // erase returns the item following the item erased
    }
    
    if (m_player != nullptr)
        delete m_player;
    m_player = nullptr;
    
    // update game stats (MAKE SURE THIS DOES WHAT IT'S SUPPOSED TO)
    numCrystals = 0;
    exitRevealed = false;
    levelCompleted = false;
    m_bonus = 1000;
}
