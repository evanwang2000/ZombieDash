#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <vector>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Penelope;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool loadThisLevel(std::string level);
    ~StudentWorld();
    void activateOnAppropriateActors(Actor* actor);
    bool isAgentMovementBlockedAt(int x, int y, Actor* a);
    bool isFlameBlockedAt(double x, double y) const;
    void addActor(Actor* a);
    bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) const;
    bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) const;
    bool isZombieVomitTriggerAt(double x, double y) const;
    bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);
    void recordCitizenGone();
    int numCitizens();
    void finishedLevel();
    void updateGameText();
    bool aInRangeOfB(double ax, double bx, double ay, double by, double distance) const;
    double getEuclidianDistance(double ax, double bx, double ay, double by) const;
    double getXCenter(double x) const;
    double getYCenter(double y) const;
private:
    bool levelCompleted = false;
    int currentLevel = 2;
    Penelope* penelope = nullptr;
    std::vector<Actor*> actors;
    int citizenCount = 0;
    bool levelExists = true;
    bool validLevel = true;
};

#endif // STUDENTWORLD_H_
