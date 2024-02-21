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
: GameWorld(assetPath), m_bonus(1000), numCrystals(0)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

Actor* StudentWorld::atPosition(int x, int y)
{
    list<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it)->getX() == x && (*it)->getY() == y)
            return (*it);
    }
    return nullptr;
}

Actor* StudentWorld::atPositionReverse(int x, int y)
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

int StudentWorld::init()    // initializes level
{
    string curLevel = "level00.txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found || result == Level:: load_fail_bad_format)
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
                    break;
                case Level::horiz_ragebot:
                    break;
                case Level::vert_ragebot:
                    break;
                case Level::thiefbot_factory:
                    break;
                case Level::mean_thiefbot_factory:
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

    m_player->doSomething();
    
    for (Actor* currActor : m_actors)
    {
        if (currActor->getHP() > 0)
        {
            currActor->doSomething();
            if (m_player->getHP() <= 0)
                return GWSTATUS_PLAYER_DIED;
        }
    }
/*
for each of the actors in the game world
{
 if (actor[i] is still active/alive)
 {
 // ask each actor to do something (e.g. move)
 actor[i]->doSomething();
if (thePlayerDiedDuringThisTick())
return GWSTATUS_PLAYER_DIED;
if (thePlayerCompletedTheCurrentLevel())
{
increaseScoreAppropriately();
return GWSTATUS_FINISHED_LEVEL;
}
 }
}
 */
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
/*
// If the player has collected all of the crystals on the level, then we
// must expose the exit so the player can advance to the next level
if (numCrystals == 0)
exposeTheExitInTheMaze(); // make the exit Active
// return the proper result
if (thePlayerDiedDuringThisTick())
return GWSTATUS_PLAYER_DIED;
if (thePlayerCompletedTheCurrentLevel())
{
increaseScoreAppropriately();
return GWSTATUS_FINISHED_LEVEL;
}
// the player hasn’t completed the current level and hasn’t died, so
// continue playing the current level
*/
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
}
